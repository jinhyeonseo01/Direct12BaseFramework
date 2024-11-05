#include "stdafx.h"
#include "Texture.h"

#include "GraphicManager.h"
#include "graphic_config.h"



Texture::Texture()
{
    _SRV_CPUHandle.ptr = 0;
}

Texture::~Texture()
{
    if(_SRV_CPUHandle.ptr != 0)
        GraphicManager::main->_textureHandlePool->HandleFree(_SRV_CPUHandle);
}

void Texture::SetState(ResourceState state)
{
    _state = state;
}

void Texture::SetName(const std::wstring& name)
{
    this->name = name;
}

std::shared_ptr<Texture> Texture::Create(DXGI_FORMAT format, uint32_t width, uint32_t height,
                                         const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, Vector4 clearColor)
{
    auto texture = std::make_shared<Texture>();
    auto device = GraphicManager::main->_device;

    texture->SetState(ResourceState::SRV);
    texture->SetClearColor(clearColor);
    texture->SetSize(Vector2(width, height));



    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;
    D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr; // Optimized clear value pointer
    /*
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
    }*/
    /*
    if (state == ResourceState::RT_SRV)
    {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        //��� ���̴����� �� �� ������ �ǹ���
        resourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
        pOptimizedClearValue = &optimizedClearValue;
    }
    */
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    //��� ���̴����� �� �� ������ �ǹ���
    resourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;
    float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
    optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
    pOptimizedClearValue = &optimizedClearValue;
    DXAssert(device->CreateCommittedResource(
        &heapProperty,
        heapFlags,
        &desc,
        resourceStates,
        pOptimizedClearValue,
        IID_PPV_ARGS(&(texture->_resource)))); //ComPtrIDAddr(texture->_resource)
    texture->CreateFromResource(texture->_resource, format);

    return texture;
}

std::shared_ptr<Texture> Texture::Load(const std::wstring& path, bool createMipMap)
{
    //������ �ڵ� ����
    auto texture = std::make_shared<Texture>();
    auto device = GraphicManager::main->_device;

    std::wstring ext = std::filesystem::path(path).extension();
    std::wstring fileName = std::filesystem::path(path).filename();
    Debug::log << "�ؽ��� �ε� ���� : " << fileName << "\n";
    HRESULT loadSuccess;
    if (ext == L".dds" || ext == L".DDS")
        loadSuccess = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, texture->_image);
    else if (ext == L".tga" || ext == L".TGA")
        loadSuccess = LoadFromTGAFile(path.c_str(), nullptr, texture->_image);
    else if (ext == L".hdr" || ext == L".HDR")
        loadSuccess = LoadFromHDRFile(path.c_str(), nullptr, texture->_image);
    else // png, jpg, jpeg, bmp
        loadSuccess = LoadFromWICFile(path.c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, texture->_image);

    if (!DXSuccess(loadSuccess))
    {
        Debug::log << "�ش� ��ο� �ؽ��� ����\n"<<path<<"\n";
        return nullptr;
    }
    if (!(ext == L".dds" || ext == L".DDS") && createMipMap)
    {
        ScratchImage mipmapImage;
        //TEX_FILTER_LINEAR ����
        //TEX_FILTER_CUBIC 3������
        //TEX_FILTER_FANT ����þ� ���漺 ����
        //TEX_FILTER_POINT
        GenerateMipMaps(
            texture->_image.GetImages(),          // �ҽ� �̹��� �迭
            texture->_image.GetImageCount(),      // �ҽ� �̹��� ����
            texture->_image.GetMetadata(),        // �ҽ� �̹��� ��Ÿ������
            TEX_FILTER_CUBIC, // ���� �ɼ� (�⺻�� ���) // CUBIC
            0, mipmapImage);
        texture->_image.Release();
        texture->_image = std::move(mipmapImage);
    }
    /*ScratchImage flippedImage;
    DXAssert(FlipRotate(texture->_image.GetImages(), texture->_image.GetImageCount(), texture->_image.GetMetadata(),
        TEX_FR_FLIP_VERTICAL, flippedImage));
    texture->_image = std::move(flippedImage);*/

    DXAssert(CreateTexture(device.Get(), texture->_image.GetMetadata(), &texture->_resource));

    std::vector<D3D12_SUBRESOURCE_DATA> subResources;
    DXAssert(PrepareUpload(device.Get(), texture->_image.GetImages(), texture->_image.GetImageCount(), texture->_image.GetMetadata(), subResources));

    int bufferSize = ::GetRequiredIntermediateSize(texture->_resource.Get(), 0, static_cast<uint32_t>(subResources.size()));

    ComPtr<ID3D12Resource> textureUploadHeap;

    D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    // 2. CD3DX12_RESOURCE_DESC::Buffer�� �ν��Ͻ��� ��������� ����
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    // 3. CreateCommittedResource ȣ�� �� ��������� ������ lvalue���� �Ѱ���
    DXAssert(device->CreateCommittedResource(
        &uploadHeapProperties,       // ���ε� ��
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, // �б� ���·� ��ȯ
        nullptr, // clear value�� �����Ƿ� null
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf())
    ));

    auto resourceCommandList = GraphicManager::main->GetResourceCommandList();

    // �ؽ��Ŀ� ���ҽ� ����
    D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    // 2. �ؽ�ó�� ��Ÿ������ ����
    const auto& metadata = texture->_image.GetMetadata();

    // 3. CD3DX12_RESOURCE_DESC::Tex2D�� �ν��Ͻ��� ��������� ����
    D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        metadata.format,
        metadata.width,
        metadata.height,
        metadata.arraySize,
        metadata.mipLevels
    );

    // 4. CreateCommittedResource ȣ�� �� �ν��Ͻ��� �Ѱ���
    DXAssert(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, // clear value�� �����Ƿ� null
        IID_PPV_ARGS(texture->_resource.GetAddressOf())
    ));


    // ���ҽ� �踮��: ���� �۾� ��
    //���ҽ� �踮� ���� ���¸� ���� �����·�
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture->_resource.Get(),
        D3D12_RESOURCE_STATE_GENERIC_READ,  // �ʱ� ���¿���
        D3D12_RESOURCE_STATE_COPY_DEST);
    resourceCommandList->ResourceBarrier(1, &barrier);   // ���� ������� ��ȯ
    //�޸� ����
    ::UpdateSubresources(resourceCommandList.Get(),
        texture->_resource.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(subResources.size()),
        subResources.data());
    //�޸� ����
    // ���ҽ� �踮��: ���� �۾� �� ���̴� ���ҽ��� ����
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture->_resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,  // ���� ��󿡼�
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    resourceCommandList.Get()->ResourceBarrier(1, &barrier);  // ���ϴ� ���� ���·� ��ȯ

    GraphicManager::main->ResourceSet();

    texture->_SRV_CPUHandle = GraphicManager::main->_textureHandlePool->HandleAlloc();
    
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = metadata.format;
    SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // D3D12_SRV_DIMENSION_TEXTURECUBE
    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SRVDesc.TextureCube.MipLevels = metadata.mipLevels;
    SRVDesc.TextureCube.MostDetailedMip = 0;
    SRVDesc.TextureCube.ResourceMinLODClamp = 0.0f;

    texture->_SRV_ViewDesc = SRVDesc;
    texture->mipLevels = metadata.mipLevels;
    texture->SetSize(Vector2(metadata.width, metadata.height));
    texture->imageFormat = texture->_image.GetMetadata().format;

    device->CreateShaderResourceView(texture->_resource.Get(), &SRVDesc, texture->_SRV_CPUHandle);
    Debug::log << "�ؽ��� �ε� ���� : " << fileName << "\n";
    return texture;
}

std::shared_ptr<Texture> Texture::Link(std::shared_ptr<RenderTexture> renderTexture, DXGI_FORMAT format)
{
    auto texture = std::make_shared<Texture>();

    texture->_resource = renderTexture->GetResource();
    texture->mipLevels = 1;
    texture->SetState(ResourceState::RT_SRV);
    texture->SetSize(renderTexture->_size);
    texture->CreateFromResource(texture->_resource, format);
    return texture;
}

std::shared_ptr<Texture> Texture::Link(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format, uint32_t width, uint32_t height, int mipLevels)
{
    auto texture = std::make_shared<Texture>();

    texture->SetState(ResourceState::RT_SRV);
    texture->SetSize(Vector2(width, height));
    texture->CreateFromResource(resource, format);
    texture->mipLevels = std::max(mipLevels, 1);
    return texture;
}

Vector4 Texture::GetPixel(int x, int y, int channal) const
{
    int pixelSize = BitsPerPixel(imageFormat);
    if (x < 0 || x >= static_cast<int>(_size.x)
        || y < 0 || y >= static_cast<int>(_size.y))
        return Vector4(0, 0, 0, 0);
    int index = (y * static_cast<int>(_size.x) + x) * (pixelSize / 8);
    unsigned char* currentPixel = &(_image.GetPixels()[index]);
    Vector4 color = Vector4(0,0,0,0);
    unsigned long long int data = 0;
    for (int i = 0; i < channal; i++)
    {
        data = 0;
        std::memcpy((void*)&data, currentPixel + i * channal, (pixelSize / channal / 8));
        (&color.x)[i] = data / (float)(std::pow(2, pixelSize));
    }
    return color;
}

void Texture::CreateFromResource(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format)
{
    auto device = GraphicManager::main->_device;
    _resource = resource;
    /*
    if (_state == ResourceState::DSV)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DXAssert(device->CreateDescriptors(&heapDesc, ComPtrIDAddr(_DSV_DescHeap)));

        D3D12_CPU_DESCRIPTOR_HANDLE DSVHandle = _DSV_DescHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
        DSVDesc.Format = format;
        DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
        DSVDesc.Texture2D.MipSlice = 0;

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
        DXAssert(device->CreateDescriptors(&heapDesc, ComPtrIDAddr(_RTV_DescHeap)));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTV_DescHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_RENDER_TARGET_VIEW_DESC RTVDesc{};
        RTVDesc.Format = format; // ���ҽ��� ����
        RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // �ؽ�ó 2D
        RTVDesc.Texture2D.MipSlice = 0; // MIP ���� 0
        RTVDesc.Texture2D.PlaneSlice = 0; // ��� �����̽� 0

        _RTV_ViewDesc = RTVDesc;
        device->CreateRenderTargetView(_resource.Get(), nullptr, rtvHeapBegin);
    }*/
    if (_state == ResourceState::RT_SRV || _state == ResourceState::SRV)
    {
        _SRV_CPUHandle = GraphicManager::main->_textureHandlePool->HandleAlloc();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = mipLevels;
        _SRV_ViewDesc = srvDesc;

        device->CreateShaderResourceView(_resource.Get(), &_SRV_ViewDesc, _SRV_CPUHandle);
    }
}
