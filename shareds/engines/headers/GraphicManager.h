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
        HWND hWnd;

        void SetHwnd(const HWND& hwnd);

    public: // Direct

        bool _refrashReserve = false;//예약


        ComPtr<IDXGIFactory4> _factory;
        ComPtr<ID3D12Device4> _device;
        ComPtr<IDXGISwapChain3> _swapChain;

        int bestAdapterIndex = -1;
        std::vector<ComPtr<IDXGIAdapter3>> _adapterList; // 그래픽카드
        std::vector<ComPtr<IDXGIOutput4>> _outputList; // 모니터

    public:
        ComPtr<ID3D12CommandQueue> _commandQueue;
        std::vector<ComPtr<ID3D12CommandAllocator>> _commandAllocators;
        std::vector<ComPtr<ID3D12GraphicsCommandList4>> _commandLists;

        ComPtr<ID3D12CommandAllocator> _resourceCommandAllocator;
        ComPtr<ID3D12GraphicsCommandList4> _resourceCommandList;

    public:
        std::vector<ComPtr<ID3D12Resource2>> _swapChainBuffers_Res;

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


        void RefreshSwapChain();


        void SetScreenInfo(Viewport viewInfo);

        GraphicManager();
        virtual ~GraphicManager();
    };

}

