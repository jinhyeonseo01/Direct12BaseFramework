#include "stdafx.h"
#include "GraphicManager.h"

#include "DXEngine.h"
#include "graphic_config.h"


void GraphicManager::SetHwnd(const HWND& hwnd)
{
    this->hWnd = hwnd;
}

void GraphicManager::Init()
{
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
    // 여기까진 정적

    Refresh();

    _isRelease = false;
}

void GraphicManager::Refresh()
{
    //리소스 힙 디스크립트 힙
    RefreshSwapChain();
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
        DEVMODEW dm;
        dm.dmSize = sizeof(DEVMODEW);
        EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &dm, 0);

        swapChainFullDesc.RefreshRate.Numerator = 60;
        if(_engine.lock())
        swapChainFullDesc.RefreshRate.Numerator = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : dm.dmDisplayFrequency;
        swapChainFullDesc.RefreshRate.Denominator = 1;
        swapChainFullDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainFullDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainFullDesc.Windowed = setting.windowType == WindowType::FullScreen;
    }

    ComPtr<IDXGISwapChain1> swapChain1 = nullptr;
    DXAssert(_factory->CreateSwapChainForHwnd(_commandQueue.Get(), hWnd, &swapChainDesc, &swapChainFullDesc, nullptr, &swapChain1));
    DXAssert(swapChain1->QueryInterface(ComPtrIDAddr(_swapChain)));
    _swapChainBuffers_Res.resize(setting.swapChain_BufferCount);
    for(int i=0;i<setting.swapChain_BufferCount;i++)
        _swapChain->GetBuffer(i, ComPtrIDAddr(_swapChainBuffers_Res[i]));


    int m_nSwapChainBufferIndex = _swapChain->GetCurrentBackBufferIndex();

}

void GraphicManager::RefreshSwapChain()
{
    _swapChain->SetFullscreenState(setting.windowType == WindowType::FullScreen, NULL);

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

    DEVMODEW moniterSetting;
    moniterSetting.dmSize = sizeof(DEVMODEW);
    EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &moniterSetting, 0);
    dxgiDesc1.RefreshRate.Numerator = 60;
    if (_engine.lock())
        dxgiDesc1.RefreshRate.Numerator = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : moniterSetting.dmDisplayFrequency;
    dxgiDesc1.RefreshRate.Denominator = 1;
    dxgiDesc1.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    dxgiDesc1.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    _swapChain->ResizeTarget(&dxgiDesc1);


    //m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
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

    //hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
    //for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

    //m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
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
