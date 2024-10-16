#pragma once
#include "stdafx.h"
#include "simple_mesh_LH.h"

namespace dxe
{
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

    class GraphicSetting
    {
    public:
        //��üȭ��, �׵θ� ����, â���
        //msaa enable
        //msaa level

        bool aaActive = true;
        AAType aaType = AAType::MSAA;
        WindowType windowType = WindowType::Windows;
        Viewport screenInfo = Viewport(0,0,1920,1080);
        DXGI_FORMAT screenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;


    public:
        
        int swapChain_BufferCount = 2;
        int CBV_REGISTER_COUNT = 5;
        int SRV_REGISTER_COUNT = 5;
        int REGISTER_COUNT = 10; // ���� �ΰ� ��ģ��
        


    public:
        //����̽� ���� ���
        bool msaaSupportAble = false;
        int msaaSupportMaxLevel = 1;
    };
}

