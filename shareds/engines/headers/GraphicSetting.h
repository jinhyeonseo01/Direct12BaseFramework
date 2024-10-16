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

        bool aaActive = true;
        AAType aaType = AAType::MSAA;
        WindowType windowType = WindowType::Windows;
        Viewport screenInfo = Viewport(0,0,1920,1080);
        DXGI_FORMAT screenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;


    public:
        
        int swapChain_BufferCount = 2;
        int CBV_REGISTER_COUNT = 5;
        int SRV_REGISTER_COUNT = 5;
        int REGISTER_COUNT = 10; // 위에 두개 합친거
        


    public:
        //디바이스 지원 목록
        bool msaaSupportAble = false;
        int msaaSupportMaxLevel = 1;
    };
}

