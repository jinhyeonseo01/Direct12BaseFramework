#pragma once
#include "stdafx.h"
#include "RootSignature.h"


namespace dxe
{
    enum class CompOper
    {
        Less,
        LEqual,
        Equal,
        GEqual,
        Greater,
        NotEqual,
        Always,
        Never
    };
    enum class CullingType
    {
        NONE,
        FRONT,
        BACK,
        WIREFRAME
    };
    enum class RenderQueueType
    {
        Opaque,
        AlphaTest,
        Transparent,
        Geometry
    };
    enum class FrontWise
    {
        CW,
        CCW
    };
    enum class BlendType
    {
        AlphaBlend, //FinalColor = SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
        Add, //FinalColor = SrcColor + DestColor
        Multiple, //FinalColor = SrcColor * DestColor
        ColorDodge, //FinalColor = DestColor / (1 - SrcColor)
        Subtract, //FinalColor = DestColor - SrcColor
        Screen, // FinalColor = 1 - (1 - SrcColor) * (1 - DestColor)
    };

    class ShaderCodeInfo
    {
    public:
        std::string _VSStructName;
        std::string _PSStructName;

        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _structData;
    };

    class ShaderInfo
    {
    public:
        bool _zTest = true;
        bool _zWrite = true;
        CompOper _zComp = CompOper::LEqual;
        bool _stencilTest = false;
        int _stencilIndex = 0;
        CompOper _stencilComp = CompOper::Always;
        D3D12_STENCIL_OP _stencilFailOp = D3D12_STENCIL_OP_KEEP;
        D3D12_STENCIL_OP _stencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        D3D12_STENCIL_OP _stencilPassOp = D3D12_STENCIL_OP_KEEP;

        CullingType cullingType;

        FrontWise _wise = FrontWise::CW;

        uint8_t _blendTargetMask = 0b00000001;
        BlendType _blendType[8] = { BlendType::AlphaBlend };

        /*
        BlendType _blendType;
        _src
        _dst
        */

        int _renderQueue = 2000;
        RenderQueueType _renderQueueType = RenderQueueType::Opaque;
    };

    class ShaderCode
    {
    public:
        std::string type;
        ComPtr<ID3DBlob> _blob;
        ComPtr<ID3DBlob> _errorBlob;
        std::vector<char> _shaderPrecompiledBuffer;
        D3D12_SHADER_BYTECODE _shaderByteCode = {};
    };
    class Shader
    {
    public:
        std::shared_ptr<RootSignature> _rootSignature;
        std::shared_ptr<RootSignature> GetRootSignature();

        std::unordered_map<std::string, std::shared_ptr<ShaderCode>> _shaderCodeTable;
        ShaderInfo _info;
    public:

        ComPtr<ID3D12PipelineState>			_pipelineState;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};
        std::vector<D3D12_INPUT_ELEMENT_DESC> _inputElementDesc;



        void Init();
        void SetPipeline(ComPtr<ID3D12GraphicsCommandList4> command);

        static void Profile(std::wstring path, std::string startPoint);
        static std::shared_ptr<Shader> Load(std::wstring path);
        static std::shared_ptr<ShaderCode> Load(
            std::wstring path, std::string startPointName,
            std::string shaderVersion,
            std::vector<D3D_SHADER_MACRO>& shaderMacro);
    };
} 

