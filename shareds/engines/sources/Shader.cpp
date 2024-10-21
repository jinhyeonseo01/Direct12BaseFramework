#include "stdafx.h"
#include "Shader.h"

#include "GraphicManager.h"
#include "graphic_config.h"


std::shared_ptr<ShaderCode> Shader::Load(std::wstring path, std::string startPointName,
                                         std::string shaderVersion, std::vector<D3D_SHADER_MACRO>& shaderMacro)
{
    std::wstring shaderPath = path;

    std::wstring ext = std::filesystem::path(shaderPath).extension();

    std::shared_ptr<ShaderCode> shaderCode = std::make_shared<ShaderCode>();
    shaderCode->type = shaderVersion;

    if (ext == L".hlsl" || ext == L".HLSL")
    {
        uint32_t compileFlag = 0;
#ifdef _DEBUG
        compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        //compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL2;
        compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3; // 고급 메모리 접근 최적화, 명령어 재배열 및 코드 압축, Data Parallelization
#endif

        std::string functionName = startPointName;
        std::string version = shaderVersion;
        if (str::split(shaderVersion, "_").size() <= 1)
            version += "_5_0";

        /*버텍스 셰이더 : "vs_5_0", "vs_6_0" 등
        픽셀 셰이더 : "ps_5_0", "ps_6_0" 등
        지오메트리 셰이더 : "gs_5_0", "gs_6_0" 등
        헬퍼 셰이더 : "hs_5_0", "hs_6_0" 등
        도메인 셰이더 : "ds_5_0", "ds_6_0" 등
        컴퓨트 셰이더 : "cs_5_0", "cs_6_0" 등*/

        ComPtr<ID3DBlob> _shaderBlob;

        if (!DXSuccess(::D3DCompileFromFile(shaderPath.c_str(), shaderMacro.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE
            , functionName.c_str(), version.c_str(), compileFlag, 0,
            _shaderBlob.GetAddressOf(), shaderCode->_errorBlob.GetAddressOf()))) {
            Debug::log << "Shader Compile Failed\n" << std::string{ (char*)shaderCode->_errorBlob->GetBufferPointer(), shaderCode->_errorBlob->GetBufferSize() } << "\n";
            return nullptr;
        }
        else
        {
            shaderCode->_shaderByteCode.pShaderBytecode = _shaderBlob->GetBufferPointer();
            shaderCode->_shaderByteCode.BytecodeLength = _shaderBlob->GetBufferSize();
            shaderCode->_blob = _shaderBlob;
        }
    }
    if (ext == L".cso" || ext == L".CSO")
    {
        std::ifstream shaderFile(shaderPath, std::ios::binary | std::ios::ate);
        if (!shaderFile.is_open())
            Debug::log << "Shader Compile Failed\n" << "미리 컴파일 된 쉐이더 경로 실패\n";
        std::streamsize size = shaderFile.tellg();
        shaderFile.seekg(0, std::ios::beg);
        shaderCode->_shaderPrecompiledBuffer.resize(size);
        if (!shaderFile.read(shaderCode->_shaderPrecompiledBuffer.data(), size))
            Debug::log << "Shader Compile Failed\n" << "미리 컴파일 된 쉐이더 읽기 실패\n";
        
        shaderCode->_shaderByteCode.pShaderBytecode = shaderCode->_shaderPrecompiledBuffer.data();
        shaderCode->_shaderByteCode.BytecodeLength = shaderCode->_shaderPrecompiledBuffer.size();
    }

    return shaderCode;
}

std::shared_ptr<RootSignature> Shader::GetRootSignature()
{
    return  _rootSignature;
}

void Shader::Init()
{
    auto device = GraphicManager::instance->_device;

    _pipelineDesc.NumRenderTargets = 1;
    _pipelineDesc.RTVFormats[0] = GraphicManager::instance->setting.screenFormat;

    //_pipelineState->

    CD3DX12_RASTERIZER_DESC2 resterizerDesc {D3D12_DEFAULT};

    switch (_info.cullingType)
    {
    case CullingType::BACK:
        resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        resterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        break;
    case CullingType::FRONT:
        resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        resterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
        break;
    case CullingType::NONE:
        resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        resterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        break;
    case CullingType::WIREFRAME:
        resterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
        resterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        break;
    }
    _pipelineDesc.RasterizerState = resterizerDesc;



    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
    depthStencilDesc.DepthEnable = _info._zTest ? TRUE : FALSE;
    switch(_info._zComp)
    {
    case CompOper::Always: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; break;
    case CompOper::Less: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; break;
    case CompOper::LEqual: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
    case CompOper::Equal: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
    case CompOper::GEqual: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
    case CompOper::Greater: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER; break;
    case CompOper::NotEqual: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
    default: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
    }
    depthStencilDesc.DepthWriteMask = _info._zWrite ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL;

    _pipelineDesc.DepthStencilState = depthStencilDesc;

    for(int i=0;i<8;i++)
    {
        if((_info._blendTargetMask & (1 << i)) != 0)
        {
            D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = _pipelineDesc.BlendState.RenderTarget[i];
            blendDesc.BlendEnable = _info._renderQueueType == RenderQueueType::Transparent;
            blendDesc.LogicOpEnable = FALSE;
            if (blendDesc.BlendEnable)
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
        }
    }

    device->CreateGraphicsPipelineState(&_pipelineDesc, ComPtrIDAddr(_pipelineState));
}

void Shader::SetPipeline(ComPtr<ID3D12GraphicsCommandList4> command)
{
    command->SetPipelineState(_pipelineState.Get());
}

void Shader::Profile(std::wstring path, std::string startPoint)
{
    std::ifstream shaderFile(path);
    if (!shaderFile.is_open())
        Debug::log << "쉐이더 분석 실패 : "<<path << " 열지 못함\n";
    std::streamsize size = shaderFile.tellg();
    shaderFile.seekg(0, std::ios::beg);
    std::vector<std::string> shaderText;
    shaderText.reserve(1024);

    std::string str;
    while(shaderFile >> str) {
        shaderText.push_back(std::move(str));
    }

    {
        auto findInputStructLineIter = std::find_if(shaderText.begin(), shaderText.end(), [&](const std::string& line) {
            return str::contains(line, startPoint);
            });
        if (findInputStructLineIter == shaderText.end())
        {
            Debug::log << "쉐이더 분석 실패 : " << path << " 진입점이 없음.\n";
            return;
        }
        auto temp = str::split(*findInputStructLineIter, "(")[1];
        str::trim(temp);
        auto InputStruct = str::split(temp, " ")[0];

        Debug::log << InputStruct << "\n";
        auto findInputStructLineIter = std::find_if(shaderText.begin(), shaderText.end(), [&](const std::string& line) {
            return str::contains(line, InputStruct) && str::contains(line, "struct");
            });

    }
}

std::shared_ptr<Shader> Shader::Load(std::wstring path)
{
    std::wstring shaderPath = path;

    std::vector<D3D_SHADER_MACRO> shaderMacro
    {
        {"Test", "1"},
        {nullptr, nullptr}
    };
    auto shader = std::make_shared<Shader>();

    auto shaderCode = Load(path, "VS_Main", "vs", shaderMacro);
    shader->_shaderCodeTable.emplace(shaderCode->type, shaderCode);
    auto shaderPS = Load(path, "PS_Main", "ps", shaderMacro);
    shader->_shaderCodeTable.emplace(shaderCode->type, shaderCode);
    return shader;
}

