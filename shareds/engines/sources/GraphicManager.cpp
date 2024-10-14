#include "stdafx.h"
#include "GraphicManager.h"

#include "DXEngine.h"
#include "graphic_config.h"
#include "Texture.h"


void GraphicManager::SetHwnd(const HWND& hwnd)
{
    this->hWnd = hwnd;
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
    CreateDescriptorHeap();
    // 여기까진 정적

    Refresh();

    _isRelease = false;
}

void GraphicManager::Refresh()
{
    //리소스 힙 디스크립트 힙
    RefreshSwapChain();
    CreateFences();
    CreateRenderTargetViews();
    //뷰 생성
}

void GraphicManager::Release()
{
    if (!_isRelease)
    {
#ifdef _DEBUG
        {
            ComPtr<IDXGIDebug1> _d3dDebug;
            DXSuccess(DXGIGetDebugInterface1(0, ComPtrIDAddr(_d3dDebug)));
            _d3dDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
        }
#endif
    }
    _isRelease = true;
}

void GraphicManager::CreateAdapterAndOutputs()
{
#ifdef _DEBUG
    DXSuccess(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, ComPtrIDAddr(_factory)));
#else
    DXSuccess(CreateDXGIFactory2(0, ComIDPtr(_factory)));
#endif
    ComPtr<IDXGIAdapter1> adapter1;
    ComPtr<IDXGIAdapter3> adapter3;
    ComPtr<IDXGIOutput> output;
    ComPtr<IDXGIOutput4> output4;

    int maxMemory = -1;
    for(int index = 0; _factory->EnumAdapters1(index, ComPtrAddr(adapter1)) != DXGI_ERROR_NOT_FOUND;++index)
    {
        DXGI_ADAPTER_DESC2 adapterDesc;
        if (DXSuccess(adapter1->QueryInterface(ComPtrIDAddr(adapter3))))
        {
            adapter3->GetDesc2(&adapterDesc);
            if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // 소프트웨어로 연결된 디바이스면 쳐내는거 같음. 그래픽카드 같은 하드웨어만
                continue;
            _adapterList.push_back(adapter3);
            if (adapterDesc.DedicatedVideoMemory > maxMemory) {
                bestAdapterIndex = index;
                maxMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory);
            }

        }
    }
    Debug::log << maxMemory << "\n";

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
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        //msaa rendering
        swapChainDesc.SampleDesc.Count = 1;//아니면 이 안에서 더 작게
        swapChainDesc.SampleDesc.Quality = 0;
        if (setting.aaActive && setting.aaType == AAType::MSAA)
        {
            swapChainDesc.SampleDesc.Count = setting.msaaSupportMaxLevel;//아니면 이 안에서 더 작게
            swapChainDesc.SampleDesc.Quality = setting.msaaSupportMaxLevel - 1;
        }

        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 해당 버퍼를 렌더타겟 용도로 쓰겠다
        swapChainDesc.BufferCount = setting.swapChain_BufferCount;

        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//flip 방식에
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        //DXGI_ALPHA_MODE_IGNORE 알파 폐기
        //DXGI_ALPHA_MODE_UNSPECIFIED 알파안씀
        swapChainDesc.Flags = 
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
            DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // 화면 테더링. 현상 허용하는거인듯. 모니터랑 주사율 안맞을때 쓴다나봄. 이거 안하면 vsync켜짐
    }

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullDesc = {};
    {
        DEVMODEW dm = {};
        dm.dmSize = sizeof(DEVMODEW);
        int frequency = 60;
        if (EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &dm, 0))
            if (_engine.lock())
                frequency = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : dm.dmDisplayFrequency;

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
    for(int i=0;i<setting.swapChain_BufferCount;i++)
        _swapChain->GetBuffer(i, ComPtrIDAddr(_swapChainBuffers_Res[i]));

    _swapChainIndex = _swapChain->GetCurrentBackBufferIndex();

}

void GraphicManager::RefreshSwapChain()
{
    _swapChain->SetFullscreenState(setting.windowType == WindowType::FullScreen, nullptr);

    DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
    _swapChain->GetDesc1(&dxgiSwapChainDesc);
    _swapChain->ResizeBuffers(setting.swapChain_BufferCount, setting.screenInfo.width, setting.screenInfo.height, dxgiSwapChainDesc.Format, dxgiSwapChainDesc.Flags);
    _swapChainBuffers_Res.resize(setting.swapChain_BufferCount);
    for (int i = 0; i < setting.swapChain_BufferCount; i++)
        _swapChain->GetBuffer(i, ComPtrIDAddr(_swapChainBuffers_Res[i]));


    DXGI_MODE_DESC dxgiDesc1;
    dxgiDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiDesc1.Width = setting.screenInfo.width;
    dxgiDesc1.Height = setting.screenInfo.height;

    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODEW);
    int frequency = 60;
    if (EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &dm, 0))
        if (_engine.lock())
            frequency = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : dm.dmDisplayFrequency;

    dxgiDesc1.RefreshRate.Numerator = frequency;
    dxgiDesc1.RefreshRate.Denominator = 1;
    dxgiDesc1.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    dxgiDesc1.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    _swapChain->ResizeTarget(&dxgiDesc1);

    _swapChainIndex = _swapChain->GetCurrentBackBufferIndex();
}

void GraphicManager::WaitSync()
{
    _fenceValue++;
    _commandQueue->Signal(_fences.Get(), _fenceValue);

    if (_fences->GetCompletedValue() < _fenceValue)
    {
        _fences->SetEventOnCompletion(_fenceValue, _fenceEvent);

        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void GraphicManager::SetResource()
{
    _resourceCommandList->Close();

    ID3D12CommandList* cmdListArr[] = { _resourceCommandList.Get() };
    _commandQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    WaitSync(); //Gpu에게 작업 맡긴 뒤에 Lock 검

    _resourceCommandAllocator.Reset();
    _resourceCommandList->Reset(_resourceCommandAllocator.Get(), nullptr);
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
        throw std::exception("Direct 12를 지원하지 않는 그래픽 어뎁터");

    DXAssert(selectDevice->QueryInterface(ComPtrIDAddr(_device)));

    // MSAA 지원함?
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
    //_swapChainFences.resize(_swapChainBuffers_Res.size());
    //for(int i=0;i< _swapChainFences.size();i++)
    {
        DXAssert(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, ComPtrIDAddr(_fences)));
        _fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    
}

void GraphicManager::CreateCommandQueueListAlloc()
{
    D3D12_COMMAND_LIST_TYPE commandType = D3D12_COMMAND_LIST_TYPE_DIRECT;

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {}; //memset 0이랑 같은 효과
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = commandType;
    commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // D3D12_COMMAND_QUEUE_PRIORITY_High로 두는게 좋지 않을까?
    commandQueueDesc.NodeMask = 0; // 기억상 여러 어뎁터를 활용할때 쓰던걸로 기억함.

    DXAssert(_device->CreateCommandQueue(ComPtrAddr(commandQueueDesc), ComPtrIDAddr(_commandQueue)));

    for (int i = 0; i < 3; i++)
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


void GraphicManager::CreateRenderTargetViews()
{
    /*
     *
     *	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
     **/


    std::shared_ptr<Texture> _depthStencil = Texture::Create( DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
        setting.screenInfo.width, setting.screenInfo.height,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE, ResourceState::DSV);
}

void GraphicManager::SetScreenInfo(Viewport viewInfo)
{
    if( setting.screenInfo.width != viewInfo.width ||
        setting.screenInfo.height != viewInfo.height)
        _refrashReserve = true;

    setting.screenInfo = viewInfo;
}


GraphicManager::GraphicManager()
{

}

GraphicManager::~GraphicManager()
{
    Release();
}
