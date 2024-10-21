#include "stdafx.h"
#include "Shader.h"

#include "GraphicManager.h"
#include "graphic_config.h"


std::shared_ptr<Shader> Shader::Load(std::wstring path, std::string startPointName, std::string shaderVersion, std::vector<D3D_SHADER_MACRO>& shaderMacro)
{
    std::wstring shaderPath = path;

    std::wstring ext = std::filesystem::path(shaderPath).extension();

    auto shader = std::make_shared<Shader>();
    D3D12_SHADER_BYTECODE shaderCode = {};


    if (ext == L".hlsl" || ext == L".HLSL")
    {
        uint32_t compileFlag = 0;
#ifdef _DEBUG
        compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        //compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL2;
        compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3; // ��� �޸� ���� ����ȭ, ��ɾ� ��迭 �� �ڵ� ����, Data Parallelization
#endif

        std::string functionName = startPointName;
        std::string version = shaderVersion;
        if (str::split(shaderVersion, "_").size() <= 1)
            version += "_5_0";

        /*���ؽ� ���̴� : "vs_5_0", "vs_6_0" ��
        �ȼ� ���̴� : "ps_5_0", "ps_6_0" ��
        ������Ʈ�� ���̴� : "gs_5_0", "gs_6_0" ��
        ���� ���̴� : "hs_5_0", "hs_6_0" ��
        ������ ���̴� : "ds_5_0", "ds_6_0" ��
        ��ǻƮ ���̴� : "cs_5_0", "cs_6_0" ��*/

        if (!DXSuccess(::D3DCompileFromFile(shaderPath.c_str(), shaderMacro.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE
            , functionName.c_str(), version.c_str(), compileFlag, 0,
            shader->_shaderBlob.GetAddressOf(), shader->_errorBlob.GetAddressOf()))) {
            Debug::log << "Shader Compile Failed\n" << std::string{ (char*)shader->_errorBlob->GetBufferPointer(), shader->_errorBlob->GetBufferSize() } << "\n";
            return nullptr;
        }
        else
        {
            shaderCode.pShaderBytecode = shader->_shaderBlob->GetBufferPointer();
            shaderCode.BytecodeLength = shader->_shaderBlob->GetBufferSize();
            shader->_shaderByteCode = shaderCode;
        }
    }
    if (ext == L".cso" || ext == L".CSO")
    {
        std::ifstream shaderFile(shaderPath, std::ios::binary | std::ios::ate);
        if (!shaderFile.is_open())
            Debug::log << "Shader Compile Failed\n" << "�̸� ������ �� ���̴� ��� ����\n";
        std::streamsize size = shaderFile.tellg();
        shaderFile.seekg(0, std::ios::beg);
        shader->_shaderPrecompiledBuffer.resize(size);
        if (!shaderFile.read(shader->_shaderPrecompiledBuffer.data(), size))
            Debug::log << "Shader Compile Failed\n" << "�̸� ������ �� ���̴� �б� ����\n";
        
        shaderCode.pShaderBytecode = shader->_shaderPrecompiledBuffer.data();
        shaderCode.BytecodeLength = shader->_shaderPrecompiledBuffer.size();
    }

    return shader;
}

std::shared_ptr<RootSignature> Shader::GetRootSignature()
{
    return  _rootSignature;
}

void Shader::Init()
{
    auto device = GraphicManager::instance->_device;

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
    depthStencilDesc.DepthEnable = _info._zTest;
    switch(_info._zComp)
    {
    case CompOper::Always: break;
    default: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
    }
    depthStencilDesc.DepthFunc =
    depthStencilDesc.DepthWriteMask = _info._zWrite ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL;

    _pipelineDesc.DepthStencilState = depthStencilDesc;


    D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = _pipelineDesc.BlendState.RenderTarget[0];
    blendDesc.BlendEnable = _info._renderQueueType == RenderQueueType::Transparent;
    blendDesc.LogicOpEnable = FALSE;
    if(blendDesc.BlendEnable)
    {
        blendDesc.BlendEnable = TRUE;
        blendDesc.LogicOpEnable = TRUE;
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    }
    else
    {
        blendDesc.SrcBlend = D3D12_BLEND_ONE;
        blendDesc.DestBlend = D3D12_BLEND_ZERO;
    }
     
    device->CreateGraphicsPipelineState(&_pipelineDesc, ComPtrIDAddr(_pipelineState));
}

void Shader::SetPipeline(ComPtr<ID3D12GraphicsCommandList4> command)
{
    command->SetPipelineState(_pipelineState.Get());
}

void Shader::Profile(std::wstring path)
{

}

void Shader::Load(std::wstring path)
{
    std::wstring shaderPath = path;

    std::vector<D3D_SHADER_MACRO> shaderMacro
    {
        {"Test", "1"},
        {nullptr, nullptr}
    };

    Load(path, "VS_Main", "vs", shaderMacro);
    Load(path, "PS_Main", "ps", shaderMacro);

}
