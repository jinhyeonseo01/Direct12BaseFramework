﻿// Direct12BaseFramework.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

//#include "Resource.h"
#include <stdafx.h>
#include <DXEngine.h>

#include "Camera.h"
#include "graphic_config.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Scene.h"


#include "Project_Study1/Study1Scene.h"
#include "Project_Study2/Study2Scene.h"
#include "Project_Module/TestScene.h"
#include "Project_Study3/Study3Scene.h"

using namespace dxe;

void Initialize(HINSTANCE hInstance);
void Release();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    Initialize(hInstance);


    MSG msg{};
    //try
    //{
        {
            auto engine = std::make_shared<dxe::Engine>();
            engine->SetTitleName(L"Game");
            engine->SetHandleName(L"main");
            engine->SetWindowRect({400, 200, 1280, 720});
            engine->EngineInit();
            engine->VisualInit();
            engine->EngineRun();
            engine = nullptr;
        }

        //auto scene = SceneManager::CreateScene<Study1Scene>(L"Study1 Scene");
        //auto scene = SceneManager::CreateScene<Study2Scene>(L"Study2 Scene");
        auto scene = SceneManager::CreateScene<Study3Scene>(L"Study3 Scene");
        //auto scene = SceneManager::CreateScene<TestScene>(L"Test Scene");

        while (!dxe::Engine::GetEngineList().empty())
        {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                    Engine::DeleteEngineAll();
                if (dxe::Engine::GetEngineList().empty())
                    break;

                auto findEngine = dxe::Engine::FindEngine(msg.hwnd);
                if ((findEngine != nullptr) && (!TranslateAccelerator(msg.hwnd, findEngine->_hWndAccelerator, &msg)))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        if (Engine::GetEngineList().empty())
        {
            Debug::log << "모든 엔진이 제거됨\n" << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }
    //}
    //catch (const std::exception& e)
    //{
    //    Debug::log << "예외 발생: " << e.what() << "\n";
    //    if (GraphicManager::main)
    //        DXSuccess(GraphicManager::main->_device->GetDeviceRemovedReason());
    //    std::this_thread::sleep_for(std::chrono::seconds(5));
    //}
    Release();
    return (int)msg.wParam;
}

void Initialize(HINSTANCE hInstance)
{
    timeBeginPeriod(1);

    Debug::Console::CreateConsole(0, 0, 600, 900, true);
    dxe::Engine::SetWindowHInstance(hInstance);
    dxe::Engine::_processStartClock = std::chrono::steady_clock::now();
}

void Release()
{
    timeEndPeriod(1);
    GraphicManager::main->TotalFenceWaitImmediate();
    dxe::SceneManager::DeleteAll();
    dxe::Engine::DeleteEngineAll();
    Debug::Console::Close();
    ShowCursor(TRUE);
}
