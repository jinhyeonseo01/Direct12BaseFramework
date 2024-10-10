#pragma once

#include <stdafx.h>

#include "GraphicSetting.h"

namespace dxe
{
    class GraphicManager : public std::enable_shared_from_this<GraphicManager>
    {
    public:
        static GraphicManager* instance;
        GraphicSetting setting;

        std::weak_ptr<Engine> _engine;

        ComPtr<IDXGIFactory4> _factory;
        ComPtr<ID3D12Device4> _device;
        ComPtr<IDXGISwapChain3> _swapChain;

        std::vector<ComPtr<IDXGIAdapter3>> _adapterList; // 그래픽카드
        std::vector<ComPtr<IDXGIOutput4>> _outputList; // 모니터

    public:
        ComPtr<ID3D12CommandAllocator> _commandAllocator;
        ComPtr<ID3D12CommandQueue> _commandQueue;
        ComPtr<ID3D12GraphicsCommandList4> _commandList;

    public:

    public:
        bool _isRelease = false;
    public:

        void Init();
        void Refresh();//D3D12GetDebugInterface

        void Release();

        void CreateAdapterAndOutputs();
        void CreateSwapChain();
        void CreateFactory();
        void CreateDevice();
        void CreateCommandQueueListAlloc();

        GraphicManager();
        virtual ~GraphicManager();
    };

}

