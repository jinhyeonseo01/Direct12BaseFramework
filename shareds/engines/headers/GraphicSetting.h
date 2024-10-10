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
        HWND hWnd;

        bool aaActive = true;
        AAType aaType = AAType::MSAA;
        WindowType windowType = WindowType::Windows;
        Viewport screenSize = Viewport(0,0,1920,1080);


    public:
        //����̽� ���� ���
        bool msaaSupportAble = false;
        int msaaSupportMaxLevel = 1;
    };
}

