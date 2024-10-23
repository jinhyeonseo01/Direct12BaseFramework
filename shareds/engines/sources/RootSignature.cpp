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
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
    
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    samplerDesc.ShaderRegister = 0;
    samplerDesc.RegisterSpace = 0;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // 디스크립터 범위 설정
    D3D12_DESCRIPTOR_RANGE ranges[2] =
    {
        { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 4, 1, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }, // b1 ~ b4
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } //  t0 ~t4
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
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
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
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND } //  t0 ~t4
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
