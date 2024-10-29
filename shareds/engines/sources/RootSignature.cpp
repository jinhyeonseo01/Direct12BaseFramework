#include "RootSignature.h"

#include "GraphicManager.h"
#include "graphic_config.h"

RootSignature::RootSignature()
{

}

RootSignature::~RootSignature()
{

}

ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature()
{
    return _rootSignature;
}

ComPtr<ID3D12RootSignature> RootSignature::GetComputeRootSignature()
{
    return _computeRootSignature;
}

void RootSignature::Init()
{
    // 루트 서명 설정
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    // 디스크립터 범위 설정
    unsigned int cbvRegister = 0;
    unsigned int srvRegister = 0;
    unsigned int uavRegister = 0;
    _ranges.push_back({ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 8, cbvRegister, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } );
    cbvRegister += _ranges[_ranges.size() - 1].NumDescriptors;

    _ranges.push_back({ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, srvRegister, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } );
    srvRegister += _ranges[_ranges.size() - 1].NumDescriptors;

    //_ranges.push_back({ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, uavRegister, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } );
    //uavRegister += _ranges[_ranges.size() - 1].NumDescriptors;

    // 루트 파라미터 설정
    // 직접32비트 꼽을때사용 (Constants)         =>        SetGraphicsRoot32BitConstants / SetGraphicsRoot32BitConstant
    // 서술자 꼽아줄때 사용  (Descriptor)        =>        SetGraphicsRootConstantBufferView /SetGraphicsRootShaderResourceView
    // 테이블 이용할떄 사용  (DescriptorTable)   =>        SetDescriptorHeaps / SetGraphicsRootDescriptorTable

    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    rootParameters.resize(2);
    //서술사뷰 사용
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 100; //b0
    rootParameters[0].Descriptor.RegisterSpace = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    //테이블 사용
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _ranges.size();
    rootParameters[1].DescriptorTable.pDescriptorRanges = _ranges.data();
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    
    rootSignatureDesc.NumParameters = rootParameters.size();
    rootSignatureDesc.pParameters = rootParameters.data();


    D3D12_STATIC_SAMPLER_DESC samplerDesc[5] = {};

    samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc[0].MipLODBias = 0;
    samplerDesc[0].MaxAnisotropy = 16;
    samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc[0].MinLOD = 0.0f;
    samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc[0].ShaderRegister = 0;
    samplerDesc[0].RegisterSpace = 0;
    samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    samplerDesc[1] = samplerDesc[0];
    samplerDesc[2] = samplerDesc[0];
    samplerDesc[3] = samplerDesc[0];
    samplerDesc[4] = samplerDesc[0];

    samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc[1].MinLOD = 0.0f;
    samplerDesc[1].MaxLOD = 0;
    samplerDesc[1].MaxAnisotropy = 16;
    samplerDesc[1].ShaderRegister = 1;

    samplerDesc[2].Filter = D3D12_FILTER_ANISOTROPIC;
    samplerDesc[2].MinLOD = 0.0f;
    samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc[2].MaxAnisotropy = 4;
    samplerDesc[2].ShaderRegister = 2;

    samplerDesc[3].Filter = D3D12_FILTER_ANISOTROPIC;
    samplerDesc[3].MinLOD = 0.0f;
    samplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc[3].MaxAnisotropy = 8;
    samplerDesc[3].ShaderRegister = 3;

    samplerDesc[4].Filter = D3D12_FILTER_ANISOTROPIC;
    samplerDesc[4].MinLOD = 0.0f;
    samplerDesc[4].MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc[4].MaxAnisotropy = 16;
    samplerDesc[4].ShaderRegister = 4;


    rootSignatureDesc.NumStaticSamplers = 5;
    rootSignatureDesc.pStaticSamplers = samplerDesc;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
 // SO 쓰겠다는 명시

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    GraphicManager::instance->_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), ComPtrIDAddr(_rootSignature));


    InitCompute();
}

void RootSignature::InitCompute()
{
    D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = { CD3DX12_STATIC_SAMPLER_DESC() };

    samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc[0].MipLODBias = 0;
    samplerDesc[0].MaxAnisotropy = 0;
    samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc[0].MinLOD = 0.0f;
    samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;

    samplerDesc[0].ShaderRegister = 0;
    samplerDesc[0].RegisterSpace = 0;
    samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // 디스크립터 범위 설정
    D3D12_DESCRIPTOR_RANGE ranges[2] =
    {
        { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 7, 1, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }, // b1 ~ b7
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } //  t0 ~t8
        // { D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } //  u0 ~u1
    };

    // 루트 파라미터 설정
    // 직접32비트 꼽을때사용 (Constants)         =>        SetGraphicsRoot32BitConstants / SetGraphicsRoot32BitConstant
    // 서술자 꼽아줄때 사용  (Descriptor)        =>        SetGraphicsRootConstantBufferView /SetGraphicsRootShaderResourceView
    // 테이블 이용할떄 사용  (DescriptorTable)   =>        SetDescriptorHeaps / SetGraphicsRootDescriptorTable

    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    rootParameters.resize(2);
    //서술사뷰 사용
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0; //b0
    rootParameters[0].Descriptor.RegisterSpace = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    //테이블 사용
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(ranges);
    rootParameters[1].DescriptorTable.pDescriptorRanges = ranges;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // 루트 서명 설정
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    rootSignatureDesc.NumParameters = rootParameters.size();
    rootSignatureDesc.pParameters = rootParameters.data();
    rootSignatureDesc.NumStaticSamplers = 1;
    rootSignatureDesc.pStaticSamplers = samplerDesc;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
    // SO 쓰겠다는 명시

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    GraphicManager::instance->_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), ComPtrIDAddr(_computeRootSignature));
}
