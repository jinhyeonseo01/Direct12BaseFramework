#pragma once
#include "stdafx.h"
#include "simple_mesh_LH.h"

namespace dxe
{
    enum class AAType
    {
        MSAA,
        TAA,// 구현 X
    };
    enum class WindowType
    {
        FullScreen,
        FullScreen_Noborder,
        Windows,// 구현 X
    };

    class GraphicSetting
    {
    public:
        //전체화면, 테두리 없음, 창모드
        //msaa enable
        //msaa level
        HWND hWnd;

        bool aaActive = true;
        AAType aaType = AAType::MSAA;
        WindowType windowType = WindowType::Windows;
        Viewport screenSize = Viewport(0,0,1920,1080);


    public:
        //디바이스 지원 목록
        bool msaaSupportAble = false;
        int msaaSupportMaxLevel = 1;
    };
}

