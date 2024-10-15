#include "stdafx.h"
#include "Texture.h"
#include <filesystem>

#include "GraphicManager.h"
#include "graphic_config.h"



Texture::Texture()
{

}

Texture::~Texture()
{

}

std::shared_ptr<Texture> Texture::Create(DXGI_FORMAT format, uint32_t width, uint32_t height,
                                         const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, ResourceState state, Vector4 clearColor)
{
    auto texture = std::make_shared<Texture>();
    auto device = GraphicManager::instance->_device;

    texture->_state = state;

    texture->_clearColor[0] = clearColor.x;
    texture->_clearColor[1] = clearColor.y;
    texture->_clearColor[2] = clearColor.z;
    texture->_clearColor[3] = clearColor.w;

    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;
    D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr; // Optimized clear value pointer

    if (state == ResourceState::DSV)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        resourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
        pOptimizedClearValue = &optimizedClearValue;
    }
    else if (state == ResourceState::RTV)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        resourceStates = D3D12_RESOURCE_STATE_RENDER_TARGET;
        float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
        pOptimizedClearValue = &optimizedClearValue;
    }
    else if (state == ResourceState::RTVSRV)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        //모든 쉐이더에서 쓸 수 있음을 의미함
        resourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
        pOptimizedClearValue = &optimizedClearValue;
    }

    DXAssert(device->CreateCommittedResource(
        &heapProperty,
        heapFlags,
        &desc,
        resourceStates,
        pOptimizedClearValue,
        ComPtrIDAddr(texture->_resource)));

    texture->CreateFromResource(texture->_resource, format);

    return texture;
}

std::shared_ptr<Texture> Texture::Load(const std::wstring& path)
{
    //상혁님 코드 참조
    auto texture = std::make_shared<Texture>();
    auto device = GraphicManager::instance->_device;

    std::wstring ext = std::filesystem::path(path).extension();

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, texture->_image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(path.c_str(), nullptr, texture->_image);
    else // png, jpg, jpeg, bmp
        ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, texture->_image);

    DXAssert(CreateTexture(device.Get(), texture->_image.GetMetadata(), &texture->_resource));

    std::vector<D3D12_SUBRESOURCE_DATA> subResources;
    DXAssert(PrepareUpload(device.Get(), texture->_image.GetImages(), texture->_image.GetImageCount(), texture->_image.GetMetadata(), subResources));

    int bufferSize = ::GetRequiredIntermediateSize(texture->_resource.Get(), 0, static_cast<uint32_t>(subResources.size()));

    ComPtr<ID3D12Resource> textureUploadHeap;
    DXAssert(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // 업로드 힙
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ, // 읽기 상태로 전환
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf())));

    auto resourceCommandList = GraphicManager::instance->GetResourceCommandList();

    // 텍스쳐용 리소스 생성
    DXAssert(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(
            texture->_image.GetMetadata().format,
            texture->_image.GetMetadata().width,
            texture->_image.GetMetadata().height,
            texture->_image.GetMetadata().arraySize,
            texture->_image.GetMetadata().mipLevels),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        ComPtrIDAddr(texture->_resource)));


    // 리소스 배리어: 복사 작업 전
    //리소스 배리어를 통해 상태를 복사 대기상태로
    resourceCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        texture->_resource.Get(),
        D3D12_RESOURCE_STATE_GENERIC_READ,  // 초기 상태에서
        D3D12_RESOURCE_STATE_COPY_DEST));   // 복사 대상으로 전환
    //메모리 복사
    ::UpdateSubresources(resourceCommandList.Get(),
        texture->_resource.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(subResources.size()),
        subResources.data());
    //메모리 복사
    // 리소스 배리어: 복사 작업 후 쉐이더 리소스로 세팅
    resourceCommandList.Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        texture->_resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,  // 복사 대상에서
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));  // 원하는 최종 상태로 전환

    GraphicManager::instance->SetResource();


    texture->_SRV_CPUHandle = GraphicManager::instance->TextureDescriptorHandleAlloc();
    
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = texture->_image.GetMetadata().format;
    SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // D3D12_SRV_DIMENSION_TEXTURECUBE
    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SRVDesc.TextureCube.MipLevels = texture->_image.GetMetadata().mipLevels;
    SRVDesc.TextureCube.MostDetailedMip = 0;
    SRVDesc.TextureCube.ResourceMinLODClamp = 0.0f;

    texture->_SRV_ViewDesc = SRVDesc;

    device->CreateShaderResourceView(texture->_resource.Get(), &SRVDesc, texture->_SRV_CPUHandle);
}

void Texture::CreateFromResource(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format)
{
    auto device = GraphicManager::instance->_device;
    _resource = resource;

    if (_state == ResourceState::DSV)
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
        _DSV_ViewDesc = DSVDesc;

        device->CreateDepthStencilView(_resource.Get(), &DSVDesc, DSVHandle);
    }

    else if (_state == ResourceState::RTV)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DXAssert(device->CreateDescriptorHeap(&heapDesc, ComPtrIDAddr(_RTV_DescHeap)));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTV_DescHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_RENDER_TARGET_VIEW_DESC RTVDesc{};
        _RTV_ViewDesc = RTVDesc;
        device->CreateRenderTargetView(_resource.Get(), &_RTV_ViewDesc, rtvHeapBegin);
    }


    else if (_state == ResourceState::RTVSRV)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DXAssert(device->CreateDescriptorHeap(&heapDesc, ComPtrIDAddr(_RTV_DescHeap)));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTV_DescHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_RENDER_TARGET_VIEW_DESC RTVDesc{};
        _RTV_ViewDesc = RTVDesc;
        device->CreateRenderTargetView(_resource.Get(), &_RTV_ViewDesc, rtvHeapBegin);



        GraphicManager::instance->TextureDescriptorHandleAlloc(&_SRV_CPUHandle);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;
        _SRV_ViewDesc = srvDesc;

        device->CreateShaderResourceView(_resource.Get(), &_SRV_ViewDesc, _SRV_CPUHandle);

    }

}
