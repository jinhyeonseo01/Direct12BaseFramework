#pragma once
#include "stdafx.h"
#include "simple_mesh_LH.h"

namespace dxe
{
    enum class ResourceState : uint32_t
    {
        RTV = 0x01,
        DSV = 0x02,
        SRV = 0x03,
        RT_SRV = 0x04,
        UAV = 0x05
    };


    enum class AAType
    {
        MSAA,
        TAA, // 구현 X
    };

    enum class WindowType
    {
        FullScreen,
        FullScreen_Noborder,
        Windows, // 구현 X
    };

    enum class FrameSync
    {
        VSync,
        GSync,
        NonSync
    };

    enum class DebugLevel
    {
        fast,
        normal,
        full
    };

    class GraphicSetting
    {
    public:
        //전체화면, 테두리 없음, 창모드
        DebugLevel debugLevel = DebugLevel::normal;


        bool aaActive = true;
        AAType aaType = AAType::MSAA;
        int msaaLevel = 4;

        WindowType windowType = WindowType::Windows;
        FrameSync syncType = FrameSync::GSync;
        Viewport screenInfo = Viewport(0, 0, 1920, 1080);
        DXGI_FORMAT screenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        //DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        std::wstring shaderRootPath = L"./resources/shaders/";
        //std::wstring shaderRootPath = L"./shaders/";

        std::vector<D3D_SHADER_MACRO> shaderMacro
        {
            {"Test", "1"},
            {nullptr, nullptr}
        };

    public:

    public:
        int swapChain_BufferCount = 3;

        int GetMSAACount();
        int GetMSAAQuality();
        BOOL GetMSAAActive();

    public:
        //디바이스 지원 목록
        bool msaaSupportAble = false;
        int msaaSupportMaxLevel = 1;
        int maxMonitorFrame = 60;
        long long int maxGPUMemory = 0;
        std::wstring GPUAdapterName;
    };
}
