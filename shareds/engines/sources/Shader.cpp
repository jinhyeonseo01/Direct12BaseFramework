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

    auto vertexSetInfo = GraphicManager::instance->vertexInfo;
    _inputElementDesc.resize(GraphicManager::instance->vertexInfo.propCount);
    
    for(int i=0;i< _inputElementDesc.size();i++)
    {
        D3D12_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = PropNameStrings[static_cast<int>(vertexSetInfo.props[i])].c_str();
        elementDesc.SemanticIndex = 0;
        switch (vertexSetInfo.propInfos[i].size) {
        case 1: elementDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
        case 2: elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
        case 3: elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
        case 4: elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
        }
        elementDesc.InputSlot = 0;
        elementDesc.AlignedByteOffset = vertexSetInfo.propInfos[i].byteOffset;
        elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;
        _inputElementDesc[i] = elementDesc;
    }
    _pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
#if _DEBUG
    _pipelineDesc.Flags |= D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
#endif

    _rootSignature = std::make_shared<RootSignature>();
    _rootSignature->Init();

    _pipelineDesc.pRootSignature = GetRootSignature()->_rootSignature.Get();

    _pipelineDesc.InputLayout = { _inputElementDesc.data(), static_cast<unsigned int>(_inputElementDesc.size())};

    _pipelineDesc.NumRenderTargets = 1;
    _pipelineDesc.RTVFormats[0] = GraphicManager::instance->setting.screenFormat;
    _pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
    _pipelineDesc.SampleDesc.Count = GraphicManager::instance->setting.GetMSAALevel();
    _pipelineDesc.SampleDesc.Quality = (GraphicManager::instance->setting.GetMSAAActive() ? 1 : 0);

    _pipelineDesc.DSVFormat = GraphicManager::instance->setting.depthStencilFormat;
    //_pipelineDesc.

    //Material, Animation, Object, Camera, Scene

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
    _pipelineDesc.RasterizerState.MultisampleEnable = GraphicManager::instance->setting.GetMSAAActive();
    _pipelineDesc.RasterizerState.FrontCounterClockwise = _info._wise == FrontWise::CCW ? TRUE : FALSE;
    _pipelineDesc.RasterizerState.DepthClipEnable = TRUE; // 깊이값 범위 밖 짤라냄.
    _pipelineDesc.RasterizerState.AntialiasedLineEnable = _info.cullingType == CullingType::WIREFRAME ? TRUE : FALSE;
    _pipelineDesc.RasterizerState.DepthBias = 0;
    _pipelineDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
    _pipelineDesc.RasterizerState.DepthBiasClamp = 0.0f;



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
    case CompOper::Never: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER; break;
    default: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
    }
    depthStencilDesc.DepthWriteMask = _info._zWrite ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL;


    depthStencilDesc.StencilEnable = _info._stencilTest ? TRUE : FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = _info._stencilFailOp;
    depthStencilDesc.FrontFace.StencilDepthFailOp = _info._stencilDepthFailOp;
    depthStencilDesc.FrontFace.StencilPassOp = _info._stencilPassOp;
    switch (_info._stencilComp)
    {
    case CompOper::Always: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS; break;
    case CompOper::Less: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS; break;
    case CompOper::LEqual: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
    case CompOper::Equal: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
    case CompOper::GEqual: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
    case CompOper::Greater: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER; break;
    case CompOper::NotEqual: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
    case CompOper::Never: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER; break;
    default: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
    }

    depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

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
                switch (_info._blendType[i])
                {
                case BlendType::AlphaBlend:
                    blendDesc.BlendEnable = TRUE;
                    blendDesc.LogicOpEnable = FALSE;
                    blendDesc.LogicOp = D3D12_LOGIC_OP_COPY;
                    blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
                    blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
                    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                    break;
                case BlendType::Add:
                    blendDesc.BlendEnable = TRUE;
                    blendDesc.LogicOpEnable = FALSE;
                    blendDesc.SrcBlend = D3D12_BLEND_ONE;
                    blendDesc.DestBlend = D3D12_BLEND_ONE;
                    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                    break;
                case BlendType::Multiple:
                    blendDesc.BlendEnable = TRUE;
                    blendDesc.LogicOpEnable = FALSE;
                    blendDesc.SrcBlend = D3D12_BLEND_DEST_COLOR;
                    blendDesc.DestBlend = D3D12_BLEND_ZERO;
                    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                    break;
                case BlendType::ColorDodge:
                    blendDesc.BlendEnable = TRUE;
                    blendDesc.LogicOpEnable = FALSE;
                    blendDesc.SrcBlend = D3D12_BLEND_ONE;
                    blendDesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
                    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                    break;
                case BlendType::Subtract:
                    blendDesc.BlendEnable = TRUE;
                    blendDesc.LogicOpEnable = FALSE;
                    blendDesc.SrcBlend = D3D12_BLEND_ONE;
                    blendDesc.DestBlend = D3D12_BLEND_ONE;
                    blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
                    break;
                case BlendType::Screen:
                    blendDesc.BlendEnable = TRUE;
                    blendDesc.LogicOpEnable = FALSE;
                    blendDesc.SrcBlend = D3D12_BLEND_ONE;
                    blendDesc.DestBlend = D3D12_BLEND_INV_DEST_COLOR;
                    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                    break;
                }
            }
            else
            {
                blendDesc.BlendEnable = FALSE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.LogicOp = D3D12_LOGIC_OP_COPY;
                blendDesc.SrcBlend = D3D12_BLEND_ONE;
                blendDesc.DestBlend = D3D12_BLEND_ZERO;
            }
        }
    }

    _pipelineDesc.VS = _shaderCodeTable["vs"]->_shaderByteCode;
    _pipelineDesc.PS = _shaderCodeTable["ps"]->_shaderByteCode;

    device->CreateGraphicsPipelineState(&_pipelineDesc, ComPtrIDAddr(_pipelineState));

    Debug::log << "쉐이더 생성 완료\n";


    {
        ID3D12ShaderReflection* pReflector = nullptr;
        HRESULT hr = D3DReflect(_shaderCodeTable["vs"]->_shaderByteCode.pShaderBytecode, _shaderCodeTable["vs"]->_shaderByteCode.BytecodeLength, IID_ID3D12ShaderReflection, (void**)&pReflector);

        if (SUCCEEDED(hr)) {
            D3D12_SHADER_DESC shaderDesc;
            pReflector->GetDesc(&shaderDesc);
            Debug::log << "Number of constant buffers: " << shaderDesc.ConstantBuffers << "\n";
            // 상수 버퍼(CB)의 레지스터 정보를 읽어오기
            for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++) {
                ID3D12ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(i);
                D3D12_SHADER_BUFFER_DESC bufferDesc;
                pConstantBuffer->GetDesc(&bufferDesc);
                Debug::log << "Constant Buffer " << i << ": " << bufferDesc.Name
                    << ", Variables: " << bufferDesc.Variables
                    << ", Size: " << bufferDesc.Size << "\n";

                for (UINT j = 0; j < bufferDesc.Variables; j++) {
                    ID3D12ShaderReflectionVariable* pVar = pConstantBuffer->GetVariableByIndex(j);
                    D3D12_SHADER_VARIABLE_DESC varDesc;
                    pVar->GetDesc(&varDesc);
                    Debug::log << "  Variable " << j << ": " << varDesc.Name
                        << ", StartOffset: " << varDesc.StartOffset
                        << ", Size: " << varDesc.Size << "\n";
                }
            }


            for (UINT i = 0; i < shaderDesc.BoundResources; i++) {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc;
                pReflector->GetResourceBindingDesc(i, &bindDesc);
                Debug::log << "Resource " << i << ": " << bindDesc.Name
                    << ", Type: " << bindDesc.Type
                    << ", BindPoint: " << bindDesc.BindPoint << "\n";
            }
        }
    }
    {
        ID3D12ShaderReflection* pReflector = nullptr;
        HRESULT hr = D3DReflect(_shaderCodeTable["ps"]->_shaderByteCode.pShaderBytecode, _shaderCodeTable["ps"]->_shaderByteCode.BytecodeLength, IID_ID3D12ShaderReflection, (void**)&pReflector);

        if (SUCCEEDED(hr)) {
            D3D12_SHADER_DESC shaderDesc;
            pReflector->GetDesc(&shaderDesc);
            Debug::log << "Number of constant buffers: " << shaderDesc.ConstantBuffers << "\n";
            // 상수 버퍼(CB)의 레지스터 정보를 읽어오기
            for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++) {
                ID3D12ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(i);
                D3D12_SHADER_BUFFER_DESC bufferDesc;
                pConstantBuffer->GetDesc(&bufferDesc);
                Debug::log << "Constant Buffer " << i << ": " << bufferDesc.Name
                    << ", Variables: " << bufferDesc.Variables
                    << ", Size: " << bufferDesc.Size << "\n";

                for (UINT j = 0; j < bufferDesc.Variables; j++) {
                    ID3D12ShaderReflectionVariable* pVar = pConstantBuffer->GetVariableByIndex(j);
                    D3D12_SHADER_VARIABLE_DESC varDesc;
                    pVar->GetDesc(&varDesc);
                    Debug::log << "  Variable " << j << ": " << varDesc.Name
                        << ", StartOffset: " << varDesc.StartOffset
                        << ", Size: " << varDesc.Size << "\n";
                }
            }


            for (UINT i = 0; i < shaderDesc.BoundResources; i++) {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc;
                pReflector->GetResourceBindingDesc(i, &bindDesc);
                Debug::log << "Resource " << i << ": " << bindDesc.Name
                    << ", Type: " << bindDesc.Type
                    << ", BindPoint: " << bindDesc.BindPoint << "\n";
            }
        }
    }

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
        auto findStartIter = std::find_if(shaderText.begin(), shaderText.end(), [&](const std::string& line) {
            return str::contains(line, startPoint);
            });
        if (findStartIter == shaderText.end())
        {
            Debug::log << "쉐이더 분석 실패 : " << path << " 진입점이 없음.\n";
            return;
        }
        auto temp = str::split(*findStartIter, "(")[1];
        str::trim(temp);
        auto inputStruct = str::split(temp, " ")[0];

        {
            auto findInputStructLineIter = std::find_if(shaderText.begin(), shaderText.end(), [&](const std::string& line) {
                return str::contains(line, inputStruct) && str::contains(line, "struct");
                });
            if (findInputStructLineIter == shaderText.end())
            {
                Debug::log << "쉐이더 분석 실패 : " << path << " Struct가 없음.\n";
                return;
            }
            ShaderCodeInfo codeInfo;
            codeInfo._structData.emplace("vertexData", std::unordered_map<std::string, std::string>{});
            while (true)
            {
                if (str::contains(*findInputStructLineIter, "}"))
                    break;

                if (str::contains(*findInputStructLineIter, ":"))
                {
                    auto contexts = str::split(*findInputStructLineIter, ":");
                    auto leftContext = contexts[0];
                    str::trim(leftContext);
                    auto rightContext = contexts[1];
                    str::trim(rightContext);
                    auto names = str::split(leftContext, " ");
                    auto name = names[names.size() - 1];
                    auto type = names[names.size() - 2];
                    auto attris = str::split(rightContext, ";");
                    auto attri = attris[attris.size() - 1];
                    str::trim(name);
                    str::trim(type);
                    str::trim(attri);


                }
            }
            codeInfo._structData;
        }
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

    auto shaderVS = Load(path, "VS_Main", "vs", shaderMacro);
    shader->_shaderCodeTable.emplace(shaderVS->type, shaderVS);
    auto shaderPS = Load(path, "PS_Main", "ps", shaderMacro);
    shader->_shaderCodeTable.emplace(shaderPS->type, shaderPS);
    return shader;
}

