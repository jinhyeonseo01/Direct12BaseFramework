// Direct12BaseFramework.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

//#include "Resource.h"
#include <stdafx.h>
#include <DXEngine.h>

#include <GameObject.h>
#include <Component.h>
#include <Transform.h>

#include "Scene.h"
#include "SceneManager.h"

using namespace dxe;

void Initialize(HINSTANCE hInstance);
void Release();



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    Initialize(hInstance);


    auto scene = SceneManager::CreateScene(L"Test Scene");
    auto obj1 = scene->CreateGameObject(L"Hello");
    auto obj2 = scene->CreateGameObject(L"Hello2");
    auto obj3 = scene->CreateGameObject(L"Hello3");
    auto obj4 = scene->CreateGameObject(L"Hello4");
    Debug::log << SceneManager::_sceneList.size() << "\n";
    for (auto& b : SceneManager::_sceneList)
        Debug::log << b->name << "\n";
    //for (auto& a : scene->_gameObjectList)
        //Debug::log << a->name << "\n";
    obj2->SetParent(obj1);
    obj3->SetParent(obj2);
    obj4->SetParent(obj1);

    obj1->Debug();


    MSG msg{};
    try
    {

        {
            std::shared_ptr<dxe::Engine> engine = std::make_shared<dxe::Engine>();
            engine->SetTitleName(L"Game");
            engine->SetHandleName(L"main");
            engine->BaseInitialize();
            engine->Initialize();
            engine = nullptr;
        }

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
        if(Engine::GetEngineList().empty())
        {
            Debug::log << "모든 엔진이 제거됨\n" << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    catch (const std::exception& e)
    {
        Debug::log << "예외 발생: " << e.what() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
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

    dxe::Engine::DeleteEngineAll();
    Debug::Console::Close();
}
