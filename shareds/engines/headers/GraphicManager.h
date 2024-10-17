#pragma once

#include <stdafx.h>

#include "GraphicSetting.h"
#include "RenderTargetGroup.h"
#include "Texture.h"

namespace dxe
{
    class GraphicManager : public std::enable_shared_from_this<GraphicManager>
    {
    public:
        static GraphicManager* instance;
        GraphicSetting setting;
        std::weak_ptr<Engine> _engine;
        HWND hWnd;

        void SetHWnd(const HWND& hWnd);

    public: // Direct

        bool _refreshReserve = false;//예약


        ComPtr<IDXGIFactory4> _factory;
        ComPtr<ID3D12Device4> _device;
        ComPtr<IDXGISwapChain3> _swapChain;

        ComPtr<ID3D12Fence> _fence;
        HANDLE _fenceEvent;
        int _fenceValue = 0;


        int bestAdapterIndex = -1;
        std::vector<ComPtr<IDXGIAdapter3>> _adapterList; // 그래픽카드
        std::vector<ComPtr<IDXGIOutput4>> _outputList; // 모니터

    public:
        ComPtr<ID3D12CommandQueue> _commandQueue;

        int commandListCount = 4;
        std::vector<ComPtr<ID3D12CommandAllocator>> _commandAllocators;
        std::vector<ComPtr<ID3D12GraphicsCommandList4>> _commandLists;
        std::vector<ComPtr<ID3D12Fence>> _commandListFences;
        std::vector<HANDLE> _commandListFenceEvents;
        std::vector<int> _commandListFenceValue;


        ComPtr<ID3D12CommandAllocator> _resourceCommandAllocator;
        ComPtr<ID3D12GraphicsCommandList4> _resourceCommandList;
        
    public:
        std::vector<ComPtr<ID3D12Resource>> _swapChainBuffers_Res;
        std::vector<std::shared_ptr<Texture>> _swapChainRT;
        int _swapChainIndex = 0;

        std::unordered_map<, std::shared_ptr<RenderTargetGroup>> _renderTargetGroupList;

    public://DescriptorHeap
        int _textureDescriptorSize = 0;
        int _textureDescriptorHeapCount = 0;
        int _textureDescriptorHeapIndex = 0;
        std::vector<bool> _textureDescriptorHeapAllocator;
        ComPtr<ID3D12DescriptorHeap> _textureDescriptorHeap;

        uint32_t TextureIndexAlloc();
        D3D12_CPU_DESCRIPTOR_HANDLE TextureDescriptorHandleAlloc(D3D12_CPU_DESCRIPTOR_HANDLE* handle = nullptr);
        void TextureDescriptorHandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

    public:
        bool _isRelease = false;
    public:

        void Init();
        void Refresh();//D3D12GetDebugInterface

        void Release();

        void CreateAdapterAndOutputs();
        void CreateSwapChain();
        void ExecuteCurrentCommand();
        void SwapChainExecute();
        void CreateFactory();
        void CreateDevice();
        void CreateFences();
        void RefreshRenderTargetGroups();
        void CreateCommandQueueListAlloc();
        void CreateDescriptorHeap();
        void CreateTextureHeap();

        void RefreshSwapChain();
        void RefreshRequest();

        void WaitSync();
        void SetResource();

        void FanceAppend(int index);
        void FanceWaitSync(int index);


        void ChangeNextCommand();
        void ClearCurrentCommand();
        void FinishAndExecuteCurrentCommand();
        void FinishCurrentCommand();

        void SetScreenInfo(Viewport viewInfo);

        void ResourceBarrier(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, bool isResource = false);

        // 렌더링 준비
        void RenderPrepare();
        // 렌더링 마무리
        void RenderFinish();


        ComPtr<ID3D12GraphicsCommandList4> GetResourceCommandList();
        ComPtr<ID3D12CommandAllocator> GetResourceCommandAllocator();

        ComPtr<ID3D12GraphicsCommandList4> GetCurrentCommandList();
        ComPtr<ID3D12CommandAllocator> GetCurrentCommandAllocator();
        ComPtr<ID3D12CommandQueue> GetCommandQueue();

        int _currentCommandListIndex = 0;

        GraphicManager();
        virtual ~GraphicManager();
    };

}

