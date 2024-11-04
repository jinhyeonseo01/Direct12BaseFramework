#include "stdafx.h"
#include "Shader.h"

#include "GraphicManager.h"
#include "graphic_config.h"


std::shared_ptr<ShaderCode> Shader::Load(std::wstring path, std::string endPointName,
                                         std::string shaderVersion, std::vector<D3D_SHADER_MACRO>& shaderMacro)
{
    std::wstring shaderPath = path;

    std::wstring ext = std::filesystem::path(shaderPath).extension();

    std::shared_ptr<ShaderCode> shaderCode = std::make_shared<ShaderCode>();
    shaderCode->shaderType = shaderVersion;
    shaderCode->endPointName = endPointName;

    if (ext == L".hlsl" || ext == L".HLSL")
    {
        uint32_t compileFlag = 0;
#ifdef _DEBUG
        compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        //compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL2;
        compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3; // 고급 메모리 접근 최적화, 명령어 재배열 및 코드 압축, Data Parallelization
#endif

        std::string functionName = shaderCode->endPointName;
        std::string version = shaderCode->shaderType;
        if (str::split(version, "_").size() <= 1)
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

ShaderRegisterInfo ShaderProfileInfo::GetRegisterByName(const std::string& name)
{
    if (_nameToRegisterTable.contains(name))
        return _nameToRegisterTable[name];
    return ShaderRegisterInfo{};
}

ShaderCBufferInfo ShaderProfileInfo::GetCBufferByName(const std::string& name)
{
    if (_nameToCBufferTable.contains(name))
        return _nameToCBufferTable[name];
    return ShaderCBufferInfo{};
}

std::shared_ptr<RootSignature> Shader::GetRootSignature()
{
    return  _rootSignature;
}


void Shader::Init()
{
    auto device = GraphicManager::main->_device;

    _pipelineDesc = {};

    auto vertexSetInfo = GraphicManager::main->vertexInfo_Full;
    _inputElementDesc.resize(GraphicManager::main->vertexInfo_Full.propCount);

    for (int i = 0; i < _inputElementDesc.size(); i++)
    {
        D3D12_INPUT_ELEMENT_DESC elementDesc = {};
        //char* c = new char[strlen(PropNameStrings[static_cast<int>(vertexSetInfo.props[i])].c_str()) + 1];
        //std::memcpy(c, PropNameStrings[static_cast<int>(vertexSetInfo.props[i])].c_str(), strlen(PropNameStrings[static_cast<int>(vertexSetInfo.props[i])].c_str()) + 1);
        //elementDesc.SemanticName = c;
        std::string name = PropNameStrings[static_cast<int>(vertexSetInfo.props[i])].c_str();
        elementDesc.SemanticName = PropNameStrings[static_cast<int>(vertexSetInfo.props[i])].c_str();
        elementDesc.SemanticIndex = vertexSetInfo.propInfos[i].index;
        elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
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

    //_rootSignature = std::make_shared<RootSignature>();
    //_rootSignature->Init();
    //_pipelineDesc.pRootSignature = GetRootSignature()->_rootSignature.Get();

    _pipelineDesc.pRootSignature = GraphicManager::main->_rootSignature->_rootSignature.Get();

    _pipelineDesc.InputLayout = { _inputElementDesc.data(), static_cast<unsigned int>(_inputElementDesc.size()) };

    _pipelineDesc.RTVFormats[0] = GraphicManager::main->setting.screenFormat;
    _pipelineDesc.NumRenderTargets = std::max(std::min(8, renderTargetCount), 1);
    for (int i = 0; i < _pipelineDesc.NumRenderTargets; i++)
        _pipelineDesc.RTVFormats[i] = RTVForamts[i];

    _pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    _pipelineDesc.SampleMask = UINT_MAX;
    _pipelineDesc.SampleDesc.Count = GraphicManager::main->setting.GetMSAACount();
    _pipelineDesc.SampleDesc.Quality = GraphicManager::main->setting.GetMSAAQuality();
    if (isMsaaDisable)
    {
        _pipelineDesc.SampleDesc.Count = 1;
        _pipelineDesc.SampleDesc.Quality = 0;
    }
    _pipelineDesc.DSVFormat = GraphicManager::main->setting.depthStencilFormat;
    //_pipelineDesc.

    //Material, Animation, Object, Camera, Scene

    //_pipelineState->

    //CD3DX12_RASTERIZER_DESC2 resterizerDesc {D3D12_DEFAULT};
    CD3DX12_RASTERIZER_DESC resterizerDesc{ D3D12_DEFAULT };

    resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    resterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
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
    _pipelineDesc.RasterizerState.MultisampleEnable = GraphicManager::main->setting.GetMSAAActive();
    _pipelineDesc.RasterizerState.FrontCounterClockwise = _info._wise == FrontWise::CCW ? TRUE : FALSE;
    _pipelineDesc.RasterizerState.DepthClipEnable = TRUE; // 깊이값 범위 밖 짤라냄.
    _pipelineDesc.RasterizerState.AntialiasedLineEnable = _info.cullingType == CullingType::WIREFRAME ? TRUE : FALSE;
    _pipelineDesc.RasterizerState.DepthBias = 0;
    _pipelineDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
    _pipelineDesc.RasterizerState.DepthBiasClamp = 0.0f;



    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
    depthStencilDesc.DepthEnable = _info._zTest ? TRUE : FALSE;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    switch (_info._zComp)
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
    depthStencilDesc.DepthWriteMask = _info._zWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;


    depthStencilDesc.StencilEnable = _info._stencilTest ? TRUE : FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = _info._stencilFailOp;
    depthStencilDesc.FrontFace.StencilDepthFailOp = _info._stencilDepthFailOp;
    depthStencilDesc.FrontFace.StencilPassOp = _info._stencilPassOp;
    depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
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

    _pipelineDesc.BlendState.AlphaToCoverageEnable = FALSE; // 필요에 따라 TRUE로 설정
    _pipelineDesc.BlendState.IndependentBlendEnable = TRUE; // 여러 렌더 타겟에 대해 다른 블렌딩 설정을 원할 경우 TRUE로 설정



    for (int i = 0; i < 8; i++)
    {
        D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = _pipelineDesc.BlendState.RenderTarget[i];
        blendDesc.BlendEnable = FALSE;
        blendDesc.LogicOpEnable = FALSE;
        blendDesc.SrcBlend = D3D12_BLEND_ONE;
        blendDesc.DestBlend = D3D12_BLEND_ZERO;

        blendDesc.BlendEnable = _info._renderQueueType == RenderQueueType::Transparent ? TRUE : FALSE;
        blendDesc.LogicOpEnable = FALSE;
        blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

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

    _pipelineDesc.VS = _shaderCodeTable["vs"]->_shaderByteCode;
    _pipelineDesc.PS = _shaderCodeTable["ps"]->_shaderByteCode;

    DXAssert(device->CreateGraphicsPipelineState(&_pipelineDesc, ComPtrIDAddr(_pipelineState)));

    Debug::log << "쉐이더 생성 완료\n";

}

void Shader::SetShaderSetting(const ShaderInfo& info)
{
    _info = info;
}

void Shader::SetRenderTargets(std::vector<std::shared_ptr<RenderTexture>> rts)
{
    renderTargetCount = rts.size();
    for(int i=0;i<std::min(8, renderTargetCount);i++)
        RTVForamts[i] = rts[i]->format;
}

void Shader::SetMSAADisable()
{
    isMsaaDisable = true;
}


void Shader::SetPipeline(ComPtr<ID3D12GraphicsCommandList4> command)
{
    if (GraphicManager::main->currentShader != this)
    {
        GraphicManager::main->currentShader = this;
        command->SetPipelineState(_pipelineState.Get());
    }
}


void Shader::Profile()
{
    for (auto& codePair : _shaderCodeTable)
    {
        auto& code = codePair.second;
        ComPtr<ID3D12ShaderReflection> pReflector = nullptr;
        HRESULT hr = D3DReflect(code->_shaderByteCode.pShaderBytecode, code->_shaderByteCode.BytecodeLength, IID_ID3D12ShaderReflection, reinterpret_cast<void**>(pReflector.GetAddressOf()));

        if (DXSuccess(hr))
        {
            D3D12_SHADER_DESC shaderDesc;
            pReflector->GetDesc(&shaderDesc);

            ShaderCBufferInfo cbufferInfo;
            ShaderCBufferPropertyInfo cbufferPropertyInfo;

            for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++) {
                ID3D12ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(i);
                D3D12_SHADER_BUFFER_DESC bufferDesc;
                pConstantBuffer->GetDesc(&bufferDesc);
                if(bufferDesc.Type != D3D_CT_CBUFFER)
                    continue;

                cbufferInfo.name = std::string(bufferDesc.Name);
                cbufferInfo.index = bufferDesc.Variables;
                cbufferInfo.bufferByteSize = static_cast<int>(bufferDesc.Size);

                for (UINT j = 0; j < bufferDesc.Variables; j++) {
                    ID3D12ShaderReflectionVariable* pVar = pConstantBuffer->GetVariableByIndex(j);

                    D3D12_SHADER_VARIABLE_DESC varDesc;
                    pVar->GetDesc(&varDesc);
                    ID3D12ShaderReflectionType* pType = pVar->GetType();
                    D3D12_SHADER_TYPE_DESC typeDesc;
                    pType->GetDesc(&typeDesc);
                    
                    switch (typeDesc.Class)
                    {
                    case D3D_SVC_SCALAR:
                        cbufferPropertyInfo._className = "scalar";
                        break;
                    case D3D_SVC_VECTOR:
                        cbufferPropertyInfo._className = "vector";
                        break;
                    case D3D_SVC_MATRIX_ROWS:
                        cbufferPropertyInfo._className = "matrix";
                        break;
                    case D3D_SVC_MATRIX_COLUMNS:
                        cbufferPropertyInfo._className = "matrix";
                        break;
                    case D3D_SVC_OBJECT:
                        cbufferPropertyInfo._className = "object";
                        break;
                    case D3D_SVC_STRUCT:
                        cbufferPropertyInfo._className = "struct";
                        break;
                    }
                    cbufferPropertyInfo.elementType = "none";
                    switch (typeDesc.Type)
                    {
                    case D3D_SVT_FLOAT:
                        cbufferPropertyInfo.elementType = "float";
                        break;
                    case D3D_SVT_INT:
                        cbufferPropertyInfo.elementType = "int";
                        break;
                    case D3D_SVT_UINT:
                        cbufferPropertyInfo.elementType = "uint";
                        break;
                    }
                    cbufferPropertyInfo.index = static_cast<int>(i);
                    cbufferPropertyInfo._name = std::string(varDesc.Name);
                    cbufferPropertyInfo.byteOffset = varDesc.StartOffset;
                    cbufferPropertyInfo.byteSize = varDesc.Size;
                    cbufferPropertyInfo.elementCount = static_cast<int>(typeDesc.Elements);
                    cbufferPropertyInfo.rowCount = static_cast<int>(typeDesc.Rows);
                    cbufferPropertyInfo.colCount = static_cast<int>(typeDesc.Columns);

                    cbufferInfo.propertys.push_back(cbufferPropertyInfo);
                }

                //cbufferInfo.propertys.push_back();
                
                if (!_profileInfo._nameToCBufferTable.contains(cbufferInfo.name))
                {
                    _profileInfo._nameToCBufferTable.emplace(cbufferInfo.name, cbufferInfo);
                    _profileInfo.cbuffers.push_back(cbufferInfo);
                    CBufferPool::CBufferRegister(cbufferInfo, 1);
                }
                
            }


            ShaderStructPropertyInfo structPropertyInfo;
            if (!_profileInfo._typeToStructTable.contains(codePair.first))
                _profileInfo._typeToStructTable.emplace(codePair.first, ShaderStructInfo{});
            _profileInfo._typeToStructTable[codePair.first].count = shaderDesc.InputParameters;

            for (UINT i = 0; i < shaderDesc.InputParameters; i++) {

                D3D12_SIGNATURE_PARAMETER_DESC desc;
                pReflector->GetInputParameterDesc(i, &desc);

                structPropertyInfo.elementType = "float";
                switch (desc.ComponentType) {
                case D3D_REGISTER_COMPONENT_UINT32:
                    structPropertyInfo.elementType = "uint";
                    break;
                case D3D_REGISTER_COMPONENT_SINT32:
                    structPropertyInfo.elementType = "int";
                    break;
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    structPropertyInfo.elementType = "float";
                    break;
                    break;
                default:
                    Debug::log << "Unknown" << "\n";
                    break;
                }
                structPropertyInfo.elementTypeRange = 0;
                for(int j=0;j<8;j++)
                    structPropertyInfo.elementTypeRange += (desc.Mask & (1 << j)) ? 1 : 0;
                structPropertyInfo.semantic = std::string(desc.SemanticName);
                structPropertyInfo.semanticIndex = static_cast<int>(desc.SemanticIndex);
                structPropertyInfo.registerIndex = static_cast<int>(desc.Register);

                _profileInfo._typeToStructTable[codePair.first].propertys.push_back(structPropertyInfo);
            }


            ShaderRegisterInfo registerInfo;

            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            for (UINT i = 0; i < shaderDesc.BoundResources; i++)
            {
                pReflector->GetResourceBindingDesc(i, &bindDesc);
                registerInfo.registerIndex = static_cast<int>(bindDesc.BindPoint);
                registerInfo.registerCount = static_cast<int>(bindDesc.BindCount);
                registerInfo.name = std::string(bindDesc.Name);
                registerInfo.elementType = "float";
                switch (bindDesc.ReturnType) {
                case D3D_RETURN_TYPE_UINT:
                    registerInfo.elementType = "uint";
                    break;
                case D3D_RETURN_TYPE_SINT:
                    registerInfo.elementType = "int";
                    break;
                case D3D_RETURN_TYPE_FLOAT:
                    registerInfo.elementType = "float";
                    break;
                case D3D_RETURN_TYPE_UNORM:
                    registerInfo.elementType = "float";
                    break;
                case 0:
                    // SRV가 아닐경우
                    break;
                default:
                    Debug::log << "Unknown Return Type: " << bindDesc.ReturnType << "\n";
                    break;
                }
                registerInfo.space = static_cast<int>(bindDesc.Space);
                registerInfo.numSample = static_cast<int>(bindDesc.NumSamples);
                registerInfo.registerType = 'b';
                switch (bindDesc.Type) {
                case D3D_SIT_CBUFFER:
                    registerInfo.registerType = 'b';
                    break;
                case D3D_SIT_BYTEADDRESS:
                case D3D_SIT_STRUCTURED:
                case D3D_SIT_TEXTURE:
                case D3D_SIT_TBUFFER:
                    registerInfo.registerType = 't';
                    break;
                case D3D_SIT_UAV_RWSTRUCTURED:
                case D3D_SIT_UAV_RWBYTEADDRESS:
                case D3D_SIT_UAV_APPEND_STRUCTURED:
                case D3D_SIT_UAV_CONSUME_STRUCTURED:
                case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                case D3D_SIT_UAV_RWTYPED:
                    registerInfo.registerType = 'u';
                    break;
                case D3D_SIT_SAMPLER:
                    registerInfo.registerType = 's';
                    break;
                default:
                    Debug::log << "Unknown Type : " << bindDesc.Type << "\n";
                    break;
                }
                registerInfo.bufferType = "none";
                switch (bindDesc.Dimension) {
                case D3D_SRV_DIMENSION_BUFFER:
                    registerInfo.bufferType = "buffer";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE1D:
                    registerInfo.bufferType = "texture1D";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
                    registerInfo.bufferType = "texture1DArray";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2D:
                    registerInfo.bufferType = "texture2D";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
                    registerInfo.bufferType = "texture2DArray";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2DMS:
                    registerInfo.bufferType = "texture2DMS";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
                    registerInfo.bufferType = "texture2DMSArray";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE3D:
                    registerInfo.bufferType = "texture3D";
                    break;
                case D3D_SRV_DIMENSION_TEXTURECUBE:
                    registerInfo.bufferType = "textureCube";
                    break;
                case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
                    registerInfo.bufferType = "textureCubeArray";
                    break;
                }
                registerInfo.registerTypeString = registerInfo.registerType + std::to_string(registerInfo.registerIndex);
                str::trim(registerInfo.registerTypeString);

                if (!_profileInfo._nameToRegisterTable.contains(registerInfo.name))
                {
                    _profileInfo._nameToRegisterTable.emplace(registerInfo.name, registerInfo);
                    _profileInfo.registers.push_back(registerInfo);
                }
            }
        }
    }
}

std::shared_ptr<Shader> Shader::Load(std::wstring path)
{
    std::wstring shaderPath = GraphicManager::main->setting.shaderRootPath + path;

    auto shader = std::make_shared<Shader>();

    auto shaderVS = Load(shaderPath, "VS_Main", "vs", GraphicManager::main->setting.shaderMacro);
    auto shaderPS = Load(shaderPath, "PS_Main", "ps", GraphicManager::main->setting.shaderMacro);

    shader->_shaderCodeTable.emplace(shaderVS->shaderType, shaderVS);
    shader->_shaderCodeTable.emplace(shaderPS->shaderType, shaderPS);

    shader->Profile();

    return shader;
}

