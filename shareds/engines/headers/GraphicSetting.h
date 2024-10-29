#pragma once
#include "stdafx.h"
#include "simple_mesh_LH.h"

namespace dxe
{

    enum class ResourceState
    {
        RTV,
        DSV,
        SRV,
        RT_SRV,
    };


    enum class AAType
    {
        MSAA,
        TAA,// ���� X
    };
    enum class WindowType
    {
        FullScreen,
        FullScreen_Noborder,
        Windows,// ���� X
    };
    enum class FrameSync
    {
        VSync,
        GSync,
        NonSync
    };

    class GraphicSetting
    {
    public:
        //��üȭ��, �׵θ� ����, â���
        //msaa enable
        //msaa level

        bool aaActive = true;
        AAType aaType = AAType::MSAA;
        int msaaLevel = 2;

        WindowType windowType = WindowType::Windows;
        FrameSync syncType = FrameSync::VSync;
        Viewport screenInfo = Viewport(0,0,1920,1080);
        DXGI_FORMAT screenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        //DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


    public:
        
        int swapChain_BufferCount = 3;

        int GetMSAALevel();
        BOOL GetMSAAActive();

    public:
        //����̽� ���� ���
        bool msaaSupportAble = false;
        int msaaSupportMaxLevel = 1;
        int maxMonitorFrame = 60;
        long long int maxGPUMemory = 0;
        std::wstring GPUAdapterName;
    };


    struct TransformParams
    {
        Matrix WorldMatrix = Matrix::Identity;
        Matrix ViewMatrix = Matrix::Identity;
        Matrix ProjectionMatrix = Matrix::Identity;
    };

}

