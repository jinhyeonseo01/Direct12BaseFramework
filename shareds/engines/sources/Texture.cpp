#include "stdafx.h"
#include "Texture.h"
#include <filesystem>

#include "GraphicManager.h"
#include "graphic_config.h"

void Texture::Load(const std::wstring& path)
{
    //상혁님 코드 참조
    std::wstring ext = std::filesystem::path(path).extension();
    auto device = GraphicManager::instance->_device;

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(path.c_str(), nullptr, _image);
    else // png, jpg, jpeg, bmp
        ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    DXAssert(CreateTexture(device.Get(), _image.GetMetadata(), &_resource));

    std::vector<D3D12_SUBRESOURCE_DATA> subResources;
    DXAssert(PrepareUpload(device.Get(), _image.GetImages(), _image.GetImageCount(), _image.GetMetadata(), subResources));

    int bufferSize = ::GetRequiredIntermediateSize(_resource.Get(), 0, static_cast<uint32_t>(subResources.size()));

    ComPtr<ID3D12Resource> textureUploadHeap;
    DXAssert(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // 업로드 힙
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ, // 읽기 상태로 전환
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf())));



    // 텍스쳐용 리소스 생성
    DXAssert(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(
            _image.GetMetadata().format,
            _image.GetMetadata().width,
            _image.GetMetadata().height,
            _image.GetMetadata().arraySize,
            _image.GetMetadata().mipLevels),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        ComPtrIDAddr(_resource)));


    // 리소스 배리어: 복사 작업 전
    //리소스 배리어를 통해 상태를 복사 대기상태로
    GraphicManager::instance->_resourceCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(),
        D3D12_RESOURCE_STATE_GENERIC_READ,  // 초기 상태에서
        D3D12_RESOURCE_STATE_COPY_DEST));   // 복사 대상으로 전환
    //메모리 복사
    ::UpdateSubresources(GraphicManager::instance->_resourceCommandList.Get(),
        _resource.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(subResources.size()),
        subResources.data());
    //메모리 복사
    // 리소스 배리어: 복사 작업 후 쉐이더 리소스로 세팅
    GraphicManager::instance->_resourceCommandList.Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,  // 복사 대상에서
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));  // 원하는 최종 상태로 전환

    GraphicManager::instance->SetResource();


    auto heapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = heapsize;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

    HRESULT hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

    D3D12_CPU_DESCRIPTOR_HANDLE TextureHeap::AllocDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE * handle)
    {
        assert(_currentIndex < _heapSize);

        int32 index = Alloc();

        if (index == -1) {
            throw std::runtime_error("No free descriptor handles available");
        }

        CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_srvHeap->GetCPUDescriptorHandleForHeapStart(), index, _offset);

        *handle = DescriptorHandle;
        _currentIndex++;

        return *handle;
    }

}
