#include "stdafx.h"
#include "RenderTexture.h"

#include "GraphicManager.h"
#include "graphic_config.h"

RenderTexture::RenderTexture()
{
}

RenderTexture::~RenderTexture()
{
}


void RenderTexture::SetState(ResourceState state)
{
    _state = state;
}

std::shared_ptr<RenderTexture> RenderTexture::Create(DXGI_FORMAT format, uint32_t width, uint32_t height,
                                                     const D3D12_HEAP_PROPERTIES& heapProperty,
                                                     D3D12_HEAP_FLAGS heapFlags, ResourceState state,
                                                     Vector4 clearColor)
{
    auto texture = std::make_shared<RenderTexture>();
    auto device = GraphicManager::main->_device;

    texture->SetState(state);
    texture->SetClearColor(clearColor);
    texture->SetSize(Vector2(width, height));
    texture->SetFormat(format);

    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;
    D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr; // Optimized clear value pointer

    if ((static_cast<unsigned int>(state) & static_cast<unsigned int>(ResourceState::DSV)) != 0)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Format = format;
        resourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        //DXGI_FORMAT_R32_TYPELESS
        //desc.Format = DXGI_FORMAT_R32_TYPELESS;
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
    }
    if ((static_cast<unsigned int>(state) & static_cast<unsigned int>(ResourceState::DSV_R32TL)) != 0)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        //DXGI_FORMAT_R32_TYPELESS
        //desc.Format = DXGI_FORMAT_R32_TYPELESS;
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
    }
    else if ((static_cast<unsigned int>(state) & static_cast<unsigned int>(ResourceState::RTV)) != 0)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        //desc.SampleDesc.Count = GraphicManager::main->setting.GetMSAACount();
        //desc.SampleDesc.Quality = GraphicManager::main->setting.GetMSAAQuality();
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.Format = format;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceStates = D3D12_RESOURCE_STATE_RENDER_TARGET;// D3D12_RESOURCE_STATE_RENDER_TARGET| DXGI_FORMAT_R32_FLOAT
        float arrFloat[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
    }

    DXAssert(device->CreateCommittedResource(
        &heapProperty,
        heapFlags,
        &desc,
        resourceStates,
        &optimizedClearValue,//&optimizedClearValue
        ComPtrIDAddr(texture->_resource))); //ComPtrIDAddr(texture->_resource)
    texture->CreateFromResource(texture->_resource, format);
    return texture;
}

std::shared_ptr<RenderTexture> RenderTexture::Link(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format, uint32_t width,
                                                   uint32_t height, ResourceState state, Vector4 clearColor)
{
    auto texture = std::make_shared<RenderTexture>();

    texture->SetFormat(format);
    texture->SetState(state);
    texture->SetSize(Vector2(static_cast<float>(width), static_cast<float>(height)));
    texture->SetClearColor(clearColor);
    texture->CreateFromResource(resource, format);
    return texture;
}

void RenderTexture::CreateFromResource(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format)
{
    auto device = GraphicManager::main->_device;
    _resource = resource;
    SetFormat(format);

    if ((static_cast<unsigned int>(_state) & static_cast<unsigned int>(ResourceState::DSV)) != 0 ||
        (static_cast<unsigned int>(_state) & static_cast<unsigned int>(ResourceState::DSV_R32TL)) != 0)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DXAssert(device->CreateDescriptorHeap(&heapDesc, ComPtrIDAddr(_DSV_DescHeap)));

        D3D12_CPU_DESCRIPTOR_HANDLE DSVHandle = _DSV_DescHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
        DSVDesc.Format = format;
        DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
        DSVDesc.Texture2D.MipSlice = 0;

        _DSV_ViewDesc = DSVDesc;

        device->CreateDepthStencilView(_resource.Get(), &DSVDesc, DSVHandle);
    }
    else if ((static_cast<unsigned int>(_state) & static_cast<unsigned int>(ResourceState::RTV)) != 0)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DXAssert(device->CreateDescriptorHeap(&heapDesc, ComPtrIDAddr(_RTV_DescHeap)));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTV_DescHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_RENDER_TARGET_VIEW_DESC RTVDesc{};
        RTVDesc.Format = format; // 리소스의 포맷
        RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 텍스처 2D
        RTVDesc.Texture2D.MipSlice = 0; // MIP 레벨 0
        RTVDesc.Texture2D.PlaneSlice = 0; // 평면 슬라이스 0

        _RTV_ViewDesc = RTVDesc;
        device->CreateRenderTargetView(_resource.Get(), nullptr, rtvHeapBegin);
    }
}
