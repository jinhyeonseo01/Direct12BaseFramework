#include "stdafx.h"
#include "GraphicManager.h"

#include "DXEngine.h"
#include "graphic_config.h"


void GraphicManager::Init()
{
#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug6> _d3dDebug;
        DXSuccess(D3D12GetDebugInterface(ComToIDPtr(_d3dDebug)));
        _d3dDebug->EnableDebugLayer();
    }
    //OutputDebugString
#endif

    CreateFactory();
    CreateAdapterAndOutputs();
    CreateDevice();
    CreateCommandQueueListAlloc();
    // ������� ����

    Refresh();

    _isRelease = false;
}

void GraphicManager::Refresh()
{
    //���ҽ� �� ��ũ��Ʈ ��
    CreateSwapChain();
    //�� ����
}

void GraphicManager::Release()
{
    if (!_isRelease)
    {
#ifdef _DEBUG
        {
            ComPtr<IDXGIDebug1> _d3dDebug;
            DXSuccess(DXGIGetDebugInterface1(0, ComToIDPtr(_d3dDebug)));
            _d3dDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
        }
#endif
    }
    _isRelease = true;
}

void GraphicManager::CreateAdapterAndOutputs()
{
#ifdef _DEBUG
    DXSuccess(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, ComToIDPtr(_factory)));
#else
    DXSuccess(CreateDXGIFactory2(0, ComIDPtr(_factory)));
#endif
    ComPtr<IDXGIAdapter1> adapter1;
    ComPtr<IDXGIAdapter3> adapter3;
    ComPtr<IDXGIOutput> output;
    ComPtr<IDXGIOutput4> output4;

    for(int index = 0; _factory->EnumAdapters1(index, ComToPtr(adapter1)) != DXGI_ERROR_NOT_FOUND;++index)
    {
        DXGI_ADAPTER_DESC2 adapterDesc;
        if (DXSuccess(adapter1->QueryInterface(ComToIDPtr(adapter3))))
        {
            adapter3->GetDesc2(&adapterDesc);
            if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // ����Ʈ����� ����� ����̽��� �ĳ��°� ����. �׷���ī�� ���� �ϵ���
                continue;
            _adapterList.push_back(adapter3);
        }
    }
    

    for (int i = 0; i < _adapterList.size(); i++)
    {
        for (int index = 0; _adapterList[i]->EnumOutputs(index, ComToPtr(output)) != DXGI_ERROR_NOT_FOUND; ++index)
        {
            if (DXSuccess(output->QueryInterface(ComToIDPtr(output4))))
                _outputList.push_back(output4);
        }
    }
}

void GraphicManager::CreateSwapChain()
{
    BOOL bFullScreenState = FALSE;
    //m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
    //m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);


    dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
    dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
    dxgiSwapChainDesc.Flags = 0;
#else
    dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif



    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    {
        swapChainDesc.Width = setting.screenSize.width;
        swapChainDesc.Height = setting.screenSize.height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        //msaa rendering
        swapChainDesc.SampleDesc.Count = 1;//�ƴϸ� �� �ȿ��� �� �۰�
        swapChainDesc.SampleDesc.Quality = 0;
        if (setting.aaActive && setting.aaType == AAType::MSAA)
        {
            swapChainDesc.SampleDesc.Count = setting.msaaSupportMaxLevel;//�ƴϸ� �� �ȿ��� �� �۰�
            swapChainDesc.SampleDesc.Quality = setting.msaaSupportMaxLevel - 1;
        }

        //swapChainDesc.BufferDesc.RefreshRate.Numerator = m_uiRefreshRate;
        //swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
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
    }

    DEVMODEW dm;
    dm.dmSize = sizeof(DEVMODEW);
    EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &dm, 0);

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc = {};
    dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = _engine.lock()->isFrameLock ? _engine.lock()->targetFrame : dm.dmDisplayFrequency;
    dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
    dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    dxgiSwapChainFullScreenDesc.Windowed = setting.windowType == WindowType::FullScreen;

    ComPtr<IDXGISwapChain1> swapChain1 = nullptr;
    DXAssert(_factory->CreateSwapChainForHwnd(_cmdQueue.Get(), _hwnd, &swapChainDesc, &dxgiSwapChainFullScreenDesc, nullptr, &swapChain1));
    DXAssert(swapChain1->QueryInterface(ComToIDPtr(_swapChain)));

    int m_nSwapChainBufferIndex = _swapChain->GetCurrentBackBufferIndex();

}

void GraphicManager::CreateFactory()
{
    DXAssert(CreateDXGIFactory1(ComToIDPtr(_factory)));
}

void GraphicManager::CreateDevice()
{
    ComPtr<IDXGIAdapter1> selectAdapter = _adapterList.size() == 0 ? nullptr : _adapterList[0];
    ComPtr<ID3D12Device1> selectDevice;
    DXSuccess(D3D12CreateDevice(selectAdapter.Get(), D3D_FEATURE_LEVEL_12_0, ComToIDPtr(selectDevice)));
    if ((!selectDevice) && (!DXSuccess(D3D12CreateDevice(selectAdapter.Get(), D3D_FEATURE_LEVEL_11_0, ComToIDPtr(selectDevice)))))
        throw std::exception("Direct 12�� �������� �ʴ� �׷��� ���");

    DXAssert(selectDevice->QueryInterface(ComToIDPtr(_device)));

    // MSAA ������?
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

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {}; //memset 0�̶� ���� ȿ��
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = commandType;
    commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // D3D12_COMMAND_QUEUE_PRIORITY_High�� �δ°� ���� ������?
    commandQueueDesc.NodeMask = 0; // ���� ���� ��͸� Ȱ���Ҷ� �����ɷ� �����.

    DXAssert(_device->CreateCommandQueue(ComToPtr(commandQueueDesc), ComToIDPtr(_commandQueue)));
    DXAssert(_device->CreateCommandAllocator(commandType, ComToIDPtr(_commandAllocator)));
    DXAssert(_device->CreateCommandList(0, commandType, _commandAllocator.Get(), nullptr, ComToIDPtr(_commandList)));
    _commandList->Close();
    
}

void GraphicManager::SetScreenSize(Viewport viewInfo)
{
    setting.screenSize = viewInfo;
}


GraphicManager::GraphicManager()
{

}

GraphicManager::~GraphicManager()
{
    Release();
}
