#include "stdafx.h"
#include "GraphicManager.h"

#include "DXEngine.h"
#include "graphic_config.h"
#include "RenderTargetGroup.h"
#include "Texture.h"
#include "RootSignature.h"



GraphicManager* GraphicManager::instance = nullptr;


void GraphicManager::SetHWnd(const HWND& hWnd)
{
    this->hWnd = hWnd;
}

uint32_t GraphicManager::TextureIndexAlloc()
{
    for (int i = 0; i < _textureDescriptorHeapAllocator.size(); ++i)
    {
        if (!_textureDescriptorHeapAllocator[i])
        {
            _textureDescriptorHeapAllocator[i] = true;
            return i;
        }
    }
    return -1;  // No available slots
}
D3D12_CPU_DESCRIPTOR_HANDLE GraphicManager::TextureDescriptorHandleAlloc(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{
    unsigned int index = TextureIndexAlloc();
    assert(index != -1);

    CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, _textureDescriptorSize);
    if(handle != nullptr)
        *handle = DescriptorHandle;
    return DescriptorHandle;
}

void GraphicManager::TextureDescriptorHandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE start = _textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t index = (uint32_t)(handle.ptr - start.ptr) / _textureDescriptorSize;
    assert(index >= 0);
    _textureDescriptorHeapAllocator[index] = false;
}

std::shared_ptr<CBufferTable> GraphicManager::GetCurrentCBufferTable()
{
    if (_currentCbufferTableIndex != _currentCommandListIndex)
    {
        _currentCbufferTableIndex = _currentCommandListIndex;
        _cbufferTableList[_currentCommandListIndex]->Reset();
    }
    return _cbufferTableList[_currentCommandListIndex];
}

void GraphicManager::Init()
{
    if (instance == nullptr)
        instance = this;
#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug6> _d3dDebug;
        DXSuccess(D3D12GetDebugInterface(ComPtrIDAddr(_d3dDebug)));
        _d3dDebug->EnableDebugLayer();
    }
    //OutputDebugString
#endif

    CreateFactory();
    CreateAdapterAndOutputs();
    CreateDevice();
    CreateCommandQueueListAlloc();
    CreateSwapChain();
    CreateFences();
    CreateRootSignature();
    CreateDescriptorHeap();
    // ������� ����

    Refresh();

    InitShader();

    _isRelease = false;
}

void GraphicManager::Refresh()
{
    //���ҽ� �� ��ũ��Ʈ ��
    WaitSync();
    RefreshSwapChain();
    RefreshRenderTargetGroups();

    _refreshReserve = false;
    //�� ����
}

void GraphicManager::Release()
{
    if (!_isRelease)
    {
        WaitSync();

#ifdef _DEBUG
        {
            ComPtr<IDXGIDebug1> _d3dDebug;
            DXSuccess(DXGIGetDebugInterface1(0, ComPtrIDAddr(_d3dDebug)));
            _d3dDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
        }
#endif

        _rootSignature.reset();
        _renderTargetGroupTable.clear();

        _swapChainBuffers_Res.clear();
        _swapChainRT.clear();
        _factory.Reset();
        _device.Reset();
        _swapChain.Reset();
        _fence.Reset();
        _adapterList.clear();
        _outputList.clear();
        _commandQueue.Reset();
        _commandAllocators.clear();
        _commandLists.clear();
        _commandListFences.clear();

        for (auto& c : _commandListFenceEvents)
            CloseHandle(c);
        _commandListFenceEvents.clear();

        _resourceCommandAllocator.Reset();
        _resourceCommandList.Reset();
    }
    _isRelease = true;
}

void GraphicManager::CreateAdapterAndOutputs()
{
#ifdef _DEBUG
    DXSuccess(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, ComPtrIDAddr(_factory)));
#else
    DXSuccess(CreateDXGIFactory2(0, ComPtrIDAddr(_factory)));
#endif
    ComPtr<IDXGIAdapter1> adapter1;
    ComPtr<IDXGIAdapter3> adapter3;
    ComPtr<IDXGIOutput> output;
    ComPtr<IDXGIOutput4> output4;

    unsigned __int64 maxMemory = 0;
    for(int index = 0; _factory->EnumAdapters1(index, ComPtrAddr(adapter1)) != DXGI_ERROR_NOT_FOUND;++index)
    {
        DXGI_ADAPTER_DESC2 adapterDesc;
        if (DXSuccess(adapter1->QueryInterface(ComPtrIDAddr(adapter3))))
        {
            adapter3->GetDesc2(&adapterDesc);
            if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // ����Ʈ����� ����� ����̽��� �ĳ��°� ����. �׷���ī�� ���� �ϵ���
                continue;
            _adapterList.push_back(adapter3);

            unsigned __int64 currentMemory = adapterDesc.DedicatedVideoMemory;
            if (currentMemory > maxMemory) {
                bestAdapterIndex = index;
                setting.maxGPUMemory = static_cast<long long int>(maxMemory = currentMemory);
                setting.GPUAdapterName = std::wstring(adapterDesc.Description);
            }
        }
    }
    DEVMODEW currentMoniterInfo = {};
    currentMoniterInfo.dmSize = sizeof(DEVMODEW);
    if (EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &currentMoniterInfo, 0))
        setting.maxMonitorFrame = currentMoniterInfo.dmDisplayFrequency;

    int maxMonitorFrame = 60;
    int maxGPUMemory = 0;
    std::wstring GPUAdapterName;
    for (int i = 0; i < _adapterList.size(); i++)
    {
        for (int index = 0; _adapterList[i]->EnumOutputs(index, ComPtrAddr(output)) != DXGI_ERROR_NOT_FOUND; ++index)
        {
            if (DXSuccess(output->QueryInterface(ComPtrIDAddr(output4))))
                _outputList.push_back(output4);
        }
    }
}

void GraphicManager::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    {
        swapChainDesc.Width = setting.screenInfo.width;
        swapChainDesc.Height = setting.screenInfo.height;
        swapChainDesc.Format = setting.screenFormat;

        //msaa rendering
        swapChainDesc.SampleDesc.Count = 1;//�ƴϸ� �� �ȿ��� �� �۰�
        swapChainDesc.SampleDesc.Quality = 0;
        if (setting.GetMSAAActive())
        {
            swapChainDesc.SampleDesc.Count = setting.GetMSAALevel();//�ƴϸ� �� �ȿ��� �� �۰�
            swapChainDesc.SampleDesc.Quality = 1;
        }

        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �ش� ���۸� ����Ÿ�� �뵵�� ���ڴ�
        swapChainDesc.BufferCount = setting.swapChain_BufferCount;

        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//flip ��Ŀ�
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        //DXGI_ALPHA_MODE_IGNORE ���� ���
        //DXGI_ALPHA_MODE_UNSPECIFIED ���ľȾ�
        swapChainDesc.Flags = 
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
            DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // ȭ�� �״���. ���� ����ϴ°��ε�. ����Ͷ� �ֻ��� �ȸ����� ���ٳ���. �̰� ���ϸ� vsync����
        //DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
    }

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullDesc = {};
    {
        DEVMODEW currentMonitorSetting = {};
        currentMonitorSetting.dmSize = sizeof(DEVMODEW);
        if (EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &currentMonitorSetting, 0))
            setting.maxMonitorFrame = currentMonitorSetting.dmDisplayFrequency;

        int frequency = 60;
        frequency = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : setting.maxMonitorFrame;

        swapChainFullDesc.RefreshRate.Numerator = frequency;
        swapChainFullDesc.RefreshRate.Denominator = 1;
        swapChainFullDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainFullDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainFullDesc.Windowed = setting.windowType == WindowType::FullScreen;
    }

    ComPtr<IDXGISwapChain1> swapChain1 = nullptr;
    DXAssert(_factory->CreateSwapChainForHwnd(_commandQueue.Get(), hWnd, &swapChainDesc, swapChainFullDesc.Windowed ? &swapChainFullDesc : nullptr, nullptr, &swapChain1));
    DXAssert(swapChain1->QueryInterface(ComPtrIDAddr(_swapChain)));


    _swapChainBuffers_Res.resize(setting.swapChain_BufferCount);
    ComPtr<ID3D12Resource> buffer;
    for (int i = 0; i < setting.swapChain_BufferCount; i++) {
        _swapChain->GetBuffer(i, ComPtrIDAddr(_swapChainBuffers_Res[i]));
    }

    _swapChainIndex = _swapChain->GetCurrentBackBufferIndex();

}

void GraphicManager::RefreshSwapChain()
{
    BOOL bFullScreenState = FALSE;
    _swapChain->GetFullscreenState(&bFullScreenState, NULL);
    Debug::log << bFullScreenState << (int)setting.windowType << "\n";
    if (bFullScreenState != (setting.windowType == WindowType::FullScreen))
    {
        {
            _swapChain->SetFullscreenState(setting.windowType == WindowType::FullScreen, nullptr);
        }

        { // ���÷����� ��带 �ٲٴ°�
            DXGI_MODE_DESC dxgiDesc1 = {};
            DEVMODEW currentMonitorSetting = {};
            currentMonitorSetting.dmSize = sizeof(DEVMODEW);
            if (EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &currentMonitorSetting, 0))
                setting.maxMonitorFrame = currentMonitorSetting.dmDisplayFrequency;

            int frequency = 60;
            frequency = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : setting.maxMonitorFrame;

            dxgiDesc1.RefreshRate.Numerator = frequency;
            dxgiDesc1.RefreshRate.Denominator = 1;
            dxgiDesc1.Format = setting.screenFormat;
            dxgiDesc1.Width = setting.screenInfo.width;
            dxgiDesc1.Height = setting.screenInfo.height;
            dxgiDesc1.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            dxgiDesc1.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            _swapChain->ResizeTarget(&dxgiDesc1); //�̰� ���÷��� ��带 �ٲٴ°���
        }

        WaitSync();

        { // ����ü���� ���۸� �ٲٴ°�
            DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc = {};
            _swapChain->GetDesc1(&dxgiSwapChainDesc);
            _swapChain->ResizeBuffers(setting.swapChain_BufferCount, setting.screenInfo.width, setting.screenInfo.height, setting.screenFormat, dxgiSwapChainDesc.Flags);
        }

        { //���� ���Ҵ�
            _swapChainBuffers_Res.clear();
            _swapChainBuffers_Res.resize(setting.swapChain_BufferCount);
            for (int i = 0; i < setting.swapChain_BufferCount; i++)
                _swapChain->GetBuffer(i, ComPtrIDAddr(_swapChainBuffers_Res[i]));
            _swapChainIndex = _swapChain->GetCurrentBackBufferIndex();
        }
    }
}

void GraphicManager::RefreshRequest()
{
    _refreshReserve = true;
}

void GraphicManager::WaitSync()
{
    _fenceValue++;
    _commandQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue)
    {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void GraphicManager::SetResource()
{
    auto resourceCommandList = GetResourceCommandList();
    resourceCommandList->Close();

    std::array<ID3D12CommandList*, 1> commandListArray{ resourceCommandList.Get() };
    GetCommandQueue()->ExecuteCommandLists(commandListArray.size(), commandListArray.data());

    WaitSync(); //Gpu���� �۾� �ñ� �ڿ� Lock ��

    GetResourceCommandAllocator()->Reset();
    GetResourceCommandList()->Reset(GetResourceCommandAllocator().Get(), nullptr);
}

void GraphicManager::FanceAppend(int index)
{
    _commandListFenceValue[index] += 1;
    _commandQueue->Signal(_commandListFences[index].Get(), _commandListFenceValue[index]);
}

void GraphicManager::FanceWaitSync(int index)
{
    if (_commandListFences[index]->GetCompletedValue() < _commandListFenceValue[index])
    {
        _commandListFences[index]->SetEventOnCompletion(_commandListFenceValue[index], _commandListFenceEvents[index]);
        ::WaitForSingleObject(_commandListFenceEvents[index], INFINITE);
    }
}

void GraphicManager::ChangeNextCommand()
{
    _currentCommandListIndex = (_currentCommandListIndex+1) % _commandLists.size();
}

void GraphicManager::ClearCurrentCommand()
{
    auto list = GetCurrentCommandList();
    auto allocator = GetCurrentCommandAllocator();
    // �̹� ���� �⵿ֱ�� ����� list�� �������ֽð��
    allocator->Reset();
    list->Reset(allocator.Get(), nullptr);
}

void GraphicManager::FinishAndExecuteCurrentCommand()
{
    FinishCurrentCommand();
    ExecuteCurrentCommand();
}
void GraphicManager::FinishCurrentCommand()
{
    GetCurrentCommandList()->Close();
}
void GraphicManager::ExecuteCurrentCommand()
{
    ID3D12CommandList* commandListArray[] = { GetCurrentCommandList().Get() };
    _commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
}
void GraphicManager::SwapChainExecute()
{
    switch (setting.syncType)
    {
    case FrameSync::VSync:
        _swapChain->Present(1, 0);
        break;
    case FrameSync::GSync:
        _swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
        break;
    case FrameSync::NonSync:
        _swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING | DXGI_PRESENT_DO_NOT_WAIT);
        break;
    }
    //_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING); // GSync
    //_swapChain->Present(1, 0); // VSync
    //_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING | DXGI_PRESENT_DO_NOT_WAIT); // NonSync

    /*
     SyncInterval (UINT):

    0: VSync ���� ��� ������ ǥ�� (Ƽ� �߻� ����).
    1: �� ���� VSync ��� �� ������ ǥ�� (�Ϲ����� VSync ���).
    n (n > 1): n���� VSync �ֱ� �� ������ ǥ�� (������ ����Ʈ ����)

    DXGI_PRESENT_NONE (0): �⺻��
    DXGI_PRESENT_ALLOW_TEARING  �񵿱� ���������̼�, ƿ��(Tearing)�� ���, DXGI 1.3
    DXGI_PRESENT_RESTART ��ü ������ �������� �ʱ�ȭ
    DXGI_PRESENT_TEST �������� ������ ǥ������ �ʰ� ���������̼��� �������� �׽�Ʈ
     **/
    _swapChainIndex = _swapChain->GetCurrentBackBufferIndex();
}

void GraphicManager::CreateFactory()
{
    DXAssert(CreateDXGIFactory1(ComPtrIDAddr(_factory)));
}

void GraphicManager::CreateDevice()
{
    ComPtr<IDXGIAdapter1> selectAdapter = _adapterList.size() == 0 ? nullptr : _adapterList[0];
    if (bestAdapterIndex != -1)
        selectAdapter = _adapterList[bestAdapterIndex];
    ComPtr<ID3D12Device1> selectDevice;
    DXSuccess(D3D12CreateDevice(selectAdapter.Get(), D3D_FEATURE_LEVEL_12_0, ComPtrIDAddr(selectDevice)));
    if ((!selectDevice) && (!DXSuccess(D3D12CreateDevice(selectAdapter.Get(), D3D_FEATURE_LEVEL_11_0, ComPtrIDAddr(selectDevice)))))
        throw std::exception("Direct 12�� �������� �ʴ� �׷��� ���");

    DXAssert(selectDevice->QueryInterface(ComPtrIDAddr(_device)));

    // MSAA ������?
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevel;
    msaaQualityLevel.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    msaaQualityLevel.SampleCount = 4;
    msaaQualityLevel.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msaaQualityLevel.NumQualityLevels = 0;
    DXSuccess(_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

    setting.msaaSupportAble = msaaQualityLevel.NumQualityLevels > 1;
    setting.msaaSupportMaxLevel = msaaQualityLevel.NumQualityLevels;

}

void GraphicManager::CreateFences()
{
    { // ���ҽ���
        DXAssert(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, ComPtrIDAddr(_fence)));
        _fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    {
        int size = static_cast<int>(_commandLists.size());

        _commandListFences.clear();
        _commandListFences.resize(size);
        _commandListFenceEvents.clear();
        _commandListFenceEvents.resize(size);
        _commandListFenceValue.clear();
        _commandListFenceValue.resize(size);

        for(int i=0;i< size;++i)
        {
            DXAssert(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, ComPtrIDAddr(_commandListFences[i])));
            _commandListFenceEvents[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
            _commandListFenceValue[i] = 0;
        }
    }
}

void GraphicManager::InitShader()
{
    std::vector<VertexProp> props{
        VertexProp::pos,
        VertexProp::normal,
        VertexProp::tangent,
        VertexProp::color,
        VertexProp::uv0,
        VertexProp::uv1,
        VertexProp::uv2,
        VertexProp::uv3,
        VertexProp::uv4,
        VertexProp::uv5,
        VertexProp::uv6,
        VertexProp::uv7,
        VertexProp::bone_ids,
        VertexProp::bone_weights,
        //BLENDWEIGHT
        //BLENDINDICES
    };
    vertexInfo = Vertex::GetSelectorInfo(props);



    {//Test
        std::shared_ptr<Shader> shader =  Shader::Load(L"../shareds/engines/shaders/forward.hlsl");
        shader->Init();
    }
}


void GraphicManager::CreateCommandQueueListAlloc()
{
    D3D12_COMMAND_LIST_TYPE commandType = D3D12_COMMAND_LIST_TYPE_DIRECT;

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {}; //memset 0�̶� ���� ȿ��
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = commandType;
    commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // D3D12_COMMAND_QUEUE_PRIORITY_High�� �δ°� ���� ������?
    commandQueueDesc.NodeMask = 0; // ���� ���� ��͸� Ȱ���Ҷ� �����ɷ� �����.

    DXAssert(_device->CreateCommandQueue(ComPtrAddr(commandQueueDesc), ComPtrIDAddr(_commandQueue)));

    
    for (int i = 0; i < commandListCount; i++)
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12GraphicsCommandList4> commandList4;

        DXAssert(_device->CreateCommandAllocator(commandType, ComPtrIDAddr(commandAllocator)));
        DXAssert(_device->CreateCommandList(0, commandType, commandAllocator.Get(), nullptr, ComPtrIDAddr(commandList)));
        DXAssert(commandList->QueryInterface(ComPtrIDAddr(commandList4)));
        commandList4->Close();

        _commandAllocators.push_back(commandAllocator);
        _commandLists.push_back(commandList4);
    }

    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12GraphicsCommandList4> commandList4;

        DXAssert(_device->CreateCommandAllocator(commandType, ComPtrIDAddr(commandAllocator)));
        DXAssert(_device->CreateCommandList(0, commandType, commandAllocator.Get(), nullptr, ComPtrIDAddr(commandList)));
        DXAssert(commandList->QueryInterface(ComPtrIDAddr(commandList4)));
        commandList4->Close();

        _resourceCommandAllocator = commandAllocator;
        _resourceCommandList = commandList4;
    }
}

void GraphicManager::CreateDescriptorHeap()
{
    CreateTextureHeap();
    CreateCBufferHeap();
}

void GraphicManager::CreateTextureHeap()
{
    _textureDescriptorHeapCount = 2048;
    _textureDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    _textureDescriptorHeapAllocator.resize(_textureDescriptorHeapCount, false);

    D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
    SRV_HeapDesc.NumDescriptors = _textureDescriptorHeapCount;
    SRV_HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    SRV_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

    DXAssert(_device->CreateDescriptorHeap(&SRV_HeapDesc, IID_PPV_ARGS(&_textureDescriptorHeap)));
}

void GraphicManager::CreateCBufferHeap()
{
    _cbufferDescriptorHeapCount = 4096;

    _cbufferTableList.reserve(_commandLists.size());
    for(int i=0;i<_commandLists.size();i++)
    {
        _cbufferTableList.push_back(std::make_shared<CBufferTable>());
        _cbufferTableList[i]->_cbufferDescriptorHeapCount = _cbufferDescriptorHeapCount;
        _cbufferTableList[i]->Init();
    }
}


void GraphicManager::SetScreenInfo(Viewport viewInfo)
{
    if( setting.screenInfo.width != viewInfo.width ||
        setting.screenInfo.height != viewInfo.height)
        _refreshReserve = true;

    setting.screenInfo = viewInfo;
}

void GraphicManager::ResourceBarrier(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after, bool isResource)
{
    ComPtr<ID3D12GraphicsCommandList4> list = GetCurrentCommandList();
    if (isResource)
        list = GetResourceCommandList();
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), before, after);
    list->ResourceBarrier(1, &barrier);
}


ComPtr<ID3D12GraphicsCommandList4> GraphicManager::GetResourceCommandList()
{
    return _resourceCommandList;
}

ComPtr<ID3D12CommandAllocator> GraphicManager::GetResourceCommandAllocator()
{
    return _resourceCommandAllocator;
}

ComPtr<ID3D12GraphicsCommandList4> GraphicManager::GetCurrentCommandList()
{
    return _commandLists[_currentCommandListIndex];
}

ComPtr<ID3D12CommandAllocator> GraphicManager::GetCurrentCommandAllocator()
{
    return _commandAllocators[_currentCommandListIndex];
}

ComPtr<ID3D12CommandQueue> GraphicManager::GetCommandQueue()
{
    return _commandQueue;
}

std::shared_ptr<RenderTargetGroup> GraphicManager::GetRenderTargetGroup(const RTGType& type)
{
    if (_renderTargetGroupTable.contains(static_cast<int>(type)))
        return _renderTargetGroupTable[static_cast<int>(type)];
    return nullptr;
}


GraphicManager::GraphicManager()
{

}

GraphicManager::~GraphicManager()
{
    Release();
}
