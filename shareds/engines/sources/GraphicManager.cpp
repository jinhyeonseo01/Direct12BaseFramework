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

    CreateFactory();
    CreateAdapterAndOutputs();
    CreateDevice();

    _isRelease = false;
}

void GraphicManager::Refresh()
{

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
        if(DXSuccess(adapter1->QueryInterface(ComToIDPtr(adapter3))))
            _adapterList.push_back(adapter3);
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

void GraphicManager::CreateFactory()
{
    DXAssert(CreateDXGIFactory1(ComToIDPtr(_factory)));
}

void GraphicManager::CreateDevice()
{
    ComPtr<IDXGIAdapter1> selectAdapter = _adapterList.size() == 0 ? nullptr : _adapterList[0];
    ComPtr<ID3D12Device1> d;
    DXAssert(D3D12CreateDevice(selectAdapter.Get(), D3D_FEATURE_LEVEL_11_0, ComToIDPtr(d)));
    DXAssert(d->QueryInterface(ComToIDPtr(_device)));
    
}

GraphicManager::GraphicManager()
{
}

GraphicManager::~GraphicManager()
{
    Release();
}
