#include <stdafx.h>
#include <GraphicManager.h>


void GraphicManager::RefreshRenderTargetGroups()
{

    _renderTargetGroupList.clear();

    std::shared_ptr<Texture> depthStencilTexture = Texture::Create(DXGI_FORMAT_D32_FLOAT_S8X24_UINT, setting.screenInfo.width, setting.screenInfo.height,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE, ResourceState::DSV);

    // SwapChain Group
    {
        std::vector<std::shared_ptr<Texture>> renderTargetTextureList;
        renderTargetTextureList.resize(_swapChainBuffers_Res.size());

        for (uint32_t i = 0; i < renderTargetTextureList.size(); ++i)
        {
            ComPtr<ID3D12Resource> resource = _swapChainBuffers_Res[i];
            renderTargetTextureList[i] = std::make_shared<Texture>();
            renderTargetTextureList[i]->SetState(ResourceState::RTV);
            renderTargetTextureList[i]->SetClearColor(Vector4(0.5, 0.5, 1, 1));
            renderTargetTextureList[i]->CreateFromResource(resource, setting.screenFormat);
        }
        this->_swapChainRT = renderTargetTextureList;

        std::shared_ptr<RenderTargetGroup> rtGroup = std::make_shared<RenderTargetGroup>();
        rtGroup->Create(renderTargetTextureList, depthStencilTexture);
        rtGroup->SetViewport(setting.screenInfo.width, setting.screenInfo.height);

        this->_renderTargetGroupList.push_back(rtGroup);
    }

}

void GraphicManager::RenderPrepare()
{
    if (_refrashReserve)
        Refresh();

    FanceWaitSync(_currentCommandListIndex);
    ClearCurrentCommand();
    auto currentSwapChainRT = _swapChainRT[_swapChainIndex];

    ResourceBarrier(currentSwapChainRT->GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _renderTargetGroupList->ClearRenderTargetView(_swapChainIndex);
    _renderTargetGroupList->ClearDepthStencilView();

    // 여기서 미리 Camera와 환경세팅의 ConstantBuffer를 등록함.
    // 
}



void GraphicManager::RenderFinish()
{
    auto currentSwapChainRT = _swapChainRT[_swapChainIndex];
    ResourceBarrier(currentSwapChainRT->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    FinishAndExecuteCurrentCommand();
    SwapChainExecute();

    FanceAppend(_currentCommandListIndex);
    ChangeNextCommand();
}