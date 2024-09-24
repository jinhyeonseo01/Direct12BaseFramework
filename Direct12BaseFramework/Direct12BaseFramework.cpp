// Direct12BaseFramework.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

//#include "Resource.h"
#include <stdafx.h>
#include <dx_engine.h>

using namespace dxe;

void Initialize(HINSTANCE hInstance);
void Release();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    Initialize(hInstance);
    MSG msg;
    try
    {
        std::shared_ptr<dxe::Engine> engine = std::make_shared<dxe::Engine>();
        engine->SetTitleName(L"Game");
        engine->SetHandleName(L"main");
        engine->BaseInitialize();
        engine->Initialize();
        engine = nullptr;

        std::shared_ptr<dxe::Engine> engine2 = std::make_shared<dxe::Engine>();
        //engine2->BaseInitialize();
        //engine2->Initialize();
        engine2 = nullptr;

        while (!dxe::Engine::GetEngineList().empty())
        {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (dxe::Engine::GetEngineList().empty()) break;
                auto findEngine = dxe::Engine::FindEngine(msg.hwnd);
                if ((findEngine != nullptr) && (!TranslateAccelerator(msg.hwnd, findEngine->_hWndAccelerator, &msg)))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                if (msg.message == WM_QUIT)
                    Engine::DeleteEngineAll();
            }
            //Frame
        }
    }
    catch (const std::runtime_error& e)
    {
        Debug::log << "예외 발생: " << e.what() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    Release();
    return (int) msg.wParam;
}

void Initialize(HINSTANCE hInstance)
{
    timeBeginPeriod(1);

    Debug::Console::CreateConsole(0,0,600,900,true);
    dxe::Engine::SetWindowHInstance(hInstance);
    dxe::Engine::_processStartClock = std::chrono::steady_clock::now();
}

void Release()
{
    timeEndPeriod(1);

    dxe::Engine::DeleteEngineAll();
    Debug::Console::Close();
}
