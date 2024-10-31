// Direct12BaseFramework.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

//#include "Resource.h"
#include <stdafx.h>
#include <DXEngine.h>

#include "Camera.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"


using namespace dxe;

void Initialize(HINSTANCE hInstance);
void Release();



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    Initialize(hInstance);


    MSG msg{};
    try
    {

        {
            std::shared_ptr<dxe::Engine> engine = std::make_shared<dxe::Engine>();
            engine->SetTitleName(L"Game");
            engine->SetHandleName(L"main");
            engine->EngineInit();
            engine->VisualInit();
            engine->EngineRun();
            engine = nullptr;
        }

        {
            auto scene = SceneManager::CreateScene(L"Test Scene");

            auto cameraObj = scene->CreateGameObject(L"Camera");
            cameraObj->transform->worldPosition(Vector3(0, 0, -0.5f));
            cameraObj->AddComponent<Camera>();

            auto obj1 = scene->CreateGameObject(L"Render Test");
            obj1->transform->worldPosition(Vector3(0.5f, 0, 0.1f));
            obj1->transform->localRotation = Quaternion::CreateFromYawPitchRoll(Vector3(0, 45, 0) * D2R);
            auto meshRender = obj1->AddComponent<MeshRenderer>();

        }
        ResourceManager::main->LoadModel(L"Ellen.fbx", L"Ellen");
        ResourceManager::main->LoadModel(L"Kind.fbx", L"Kind");
        //ResourceManager::main->WaitAll();

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
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
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
    GraphicManager::instance->WaitSync();
    dxe::Engine::DeleteEngineAll();
    dxe::SceneManager::DeleteAll();
    Debug::Console::Close();
}
