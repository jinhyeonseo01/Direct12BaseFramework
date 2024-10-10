#include "stdafx.h"
#include "GraphicManager.h"
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

    graphic->setting.hWnd

    CreateFactory();
    CreateAdapterAndOutputs();
    CreateDevice();
    CreateCommandQueueListAlloc();
    // 여기까진 정적

    Refresh();

    _isRelease = false;
}

void GraphicManager::Refresh()
{
    //리소스 힙 디스크립트 힙
    CreateSwapChain();
    //뷰 생성
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
            if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // 소프트웨어로 연결된 디바이스면 쳐내는거 같음. 그래픽카드 같은 하드웨어만
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
        throw std::exception("Direct 12를 지원하지 않는 그래픽 어뎁터");

    DXAssert(selectDevice->QueryInterface(ComToIDPtr(_device)));

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

    DXAssert(_device->CreateCommandQueue(ComToPtr(commandQueueDesc), ComToIDPtr(_commandQueue)));
    DXAssert(_device->CreateCommandAllocator(commandType, ComToIDPtr(_commandAllocator)));
    DXAssert(_device->CreateCommandList(0, commandType, _commandAllocator.Get(), nullptr, ComToIDPtr(_commandList)));
    _commandList->Close();
    
}


GraphicManager::GraphicManager()
{

}

GraphicManager::~GraphicManager()
{
    Release();
}
