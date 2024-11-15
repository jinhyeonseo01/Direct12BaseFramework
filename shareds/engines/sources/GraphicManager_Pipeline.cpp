#include <stdafx.h>
#include <GraphicManager.h>

#include "CBuffer_struct.h"


void GraphicManager::CreateTextureHandlePool()
{
    _textureHandlePool = std::make_shared<ShaderResourcePool>();
    _textureHandlePool->Init(4096);
}

void GraphicManager::CreateCBufferPool()
{
    _cbufferPoolList.resize(_commandLists.size());
    for (int i = 0; i < _cbufferPoolList.size(); i++)
    {
        _cbufferPoolList[i] = std::make_shared<CBufferPool>();
        _cbufferPoolList[i]->_cbufferDescriptorHeapCount = 8000;
        _cbufferPoolList[i]->AddCBuffer("TransformParams", sizeof(TransformParams), 8000);
        _cbufferPoolList[i]->AddCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams), 8000);
        _cbufferPoolList[i]->AddCBuffer("BoneParams", sizeof(BoneParams), 128);

        _cbufferPoolList[i]->Init();
    }
}

void GraphicManager::CreateDescriptorTable()
{
    _descriptorTableList.resize(_commandLists.size());
    for (int i = 0; i < _descriptorTableList.size(); i++)
    {
        _descriptorTableList[i] = std::make_shared<DescriptorTable>();
        _descriptorTableList[i]->Init(_rootSignature->_ranges, 8000);
    }
}

void GraphicManager::RefreshRenderTargetGroups()
{
    this->_renderTargetGroupTable.clear();
    this->_swapChainRT.clear();

    std::shared_ptr<RenderTexture> depthStencilTexture = RenderTexture::Create(
        setting.depthStencilFormat, setting.screenInfo.width, setting.screenInfo.height,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE, ResourceState::DSV);

    // SwapChain Group
    {
        std::vector<std::shared_ptr<RenderTexture>> renderTargetTextureList;
        renderTargetTextureList.resize(_swapChainBuffers_Res.size());

        for (uint32_t i = 0; i < renderTargetTextureList.size(); ++i)
        {
            ComPtr<ID3D12Resource> resource = _swapChainBuffers_Res[i];
            renderTargetTextureList[i] = RenderTexture::Link(resource, setting.screenFormat, setting.screenInfo.width,
                                                             setting.screenInfo.height,
                                                             ResourceState::RTV, Vector4(0.7, 0.7, 0.7, 1));
        }
        this->_swapChainRT = renderTargetTextureList;

        auto rtGroup = std::make_shared<RenderTargetGroup>();
        rtGroup->Create(renderTargetTextureList, depthStencilTexture);
        rtGroup->SetViewport(setting.screenInfo.width, setting.screenInfo.height);

        this->_renderTargetGroupTable.emplace(static_cast<int>(RTGType::SwapChain), rtGroup);
    }
}

void GraphicManager::RenderPrepare()
{
    if (!_isRelease)
    {
        if (_refreshReserve)
            Refresh();

        CommandListFenceWait(_currentCommandListIndex);
        ClearCurrentCommand();
        ClearPipelineState();
        GetCurrentCBufferPool()->Reset();
        GetCurrentDescriptorTable()->Reset();


        GetCurrentCommandList()->SetGraphicsRootSignature(_rootSignature->GetRootSignature().Get());
        GetCurrentCommandList()->SetDescriptorHeaps(1, GetCurrentDescriptorTable()->_descriptorHeap.GetAddressOf());

        auto currentSwapChainRT = _swapChainRT[_swapChainIndex];
        ResourceBarrier(currentSwapChainRT->GetResource(), D3D12_RESOURCE_STATE_PRESENT,
                        D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetRenderTargetGroup(RTGType::SwapChain)->OMSetRenderTargets(1, _swapChainIndex);
        GetRenderTargetGroup(RTGType::SwapChain)->ClearRenderTargetView(_swapChainIndex);
        GetRenderTargetGroup(RTGType::SwapChain)->ClearDepthStencilView();


        // 여기서 미리 Camera와 환경세팅의 ConstantBuffer를 등록함.
        //
    }
}


void GraphicManager::RenderFinish()
{
    if (!_isRelease)
    {
        auto currentSwapChainRT = _swapChainRT[_swapChainIndex];
        ResourceBarrier(currentSwapChainRT->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET,
                        D3D12_RESOURCE_STATE_PRESENT);

        FinishAndExecuteCurrentCommand();
        SwapChainExecute();

        CommandListFenceAppend(_currentCommandListIndex);
        ChangeNextCommand();
    }
}

void GraphicManager::ClearPipelineState()
{
    currentShader = nullptr;
}
