#include <stdafx.h>
#include <GraphicManager.h>

#include "CBuffer_struct.h"
#include "ResourceManager.h"


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

    {
        float shadowSize = 2048;
        std::shared_ptr<RenderTexture> shadowMapRenderTexture = RenderTexture::Create(DXGI_FORMAT_R32_FLOAT,//DXGI_FORMAT_D32_FLOAT
            shadowSize, shadowSize,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, ResourceState::RTV);

        std::shared_ptr<RenderTexture> shadowDepthTexture = RenderTexture::Create(DXGI_FORMAT_D32_FLOAT,//DXGI_FORMAT_D32_FLOAT
            shadowSize, shadowSize,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, ResourceState::DSV);

        _shadowMap = Texture::Link(shadowDepthTexture, //shadowDepthTexture shadowMapRenderTexture
            DXGI_FORMAT_R32_FLOAT, ResourceState::RT_SRV);

        auto rtGroup = std::make_shared<RenderTargetGroup>();
        rtGroup->Create({ shadowMapRenderTexture }, shadowDepthTexture);//shadowMapRenderTexture
        rtGroup->SetViewport(shadowSize, shadowSize);

        this->_renderTargetGroupTable.emplace(static_cast<int>(RTGType::Shadow), rtGroup);
    }
}

void GraphicManager::CustomInit()
{
    ShaderInfo info;
    auto shadowShader = ResourceManager::main->LoadShader(L"directional_shadow.hlsl", L"directional_shadow", 
        GetRenderTargetGroup(RTGType::Shadow)->_renderTargetTextureList);
    info.cullingType = CullingType::NONE;
    info._zWrite = true;
    info._zTest = true;
    info._stencilTest = false;
    //info._depthOnly = true;
    info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    shadowShader->SetMSAADisable();
    shadowShader->SetDepthStencil(GetRenderTargetGroup(RTGType::Shadow)->_depthStencilTexture);
    shadowShader->SetShaderSetting(info);
    shadowShader->Init();

    _shadowMaterial = std::make_shared<Material>();
    _shadowMaterial->shader = shadowShader;
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
