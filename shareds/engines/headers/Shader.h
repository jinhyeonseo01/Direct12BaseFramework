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
        Always
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

    class ShaderInfo
    {
    public:
        bool _zTest = true;
        bool _zWrite = true;
        CompOper _zComp = CompOper::LEqual;
        bool _stencilTest = false;
        int _stencilIndex = 0;
        CompOper _stencilComp = CompOper::LEqual;
        CullingType cullingType;

        uint8_t _blendTargetMask = 0b00000001;

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

        ComPtr<ID3D12PipelineState>			_pipelineState;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};

        ShaderInfo _info;

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

