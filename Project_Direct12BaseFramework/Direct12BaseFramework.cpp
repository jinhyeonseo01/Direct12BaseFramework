// Direct12BaseFramework.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

//#include "Resource.h"
#include <stdafx.h>
#include <DXEngine.h>

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
	

    auto scene = SceneManager::CreateScene(L"Test Scene");
    auto obj1 = scene->CreateGameObject(L"Hello");
    auto obj2 = scene->CreateGameObject(L"Hello2");
    auto obj3 = scene->CreateGameObject(L"Hello3");
    auto obj4 = scene->CreateGameObject(L"Hello4");
    Debug::log << SceneManager::_sceneList.size() << "\n";
    for (auto& b : SceneManager::_sceneList)
        Debug::log << b->name << "\n";
	//for (auto& a : scene->_gameObjectList)
        //Debug::log << a->names << "\n";
    obj2->SetParent(obj1);
    obj3->SetParent(obj2);
	obj4->SetParent(obj1);

    obj1->Debug();
    std::vector<std::shared_ptr<GameObject>> vec;
    obj1->GetChildsAllByName(vec, L"Hello4");
    Debug::log << vec.size() << "\n";

    obj1->Debug();


    float* b = new float[100*9];
    int offset = 0;
    std::vector<Vertex> vertexs;
    vertexs.reserve(100);
    for(int i=0;i<100;i++)
    {
        Vertex v;
        v.position = Vector3(i, i, i);
        v.normal = Vector3(i, i, i);
        v.uvs.emplace_back(i, i, i);
        v.uvs.emplace_back(i, i, i);
        vertexs.push_back(v);
    }
    std::vector<VertexProp> selector = {
        VertexProp::pos,
        VertexProp::normal,
        VertexProp::uv
    };
    for (auto& a : vertexs)
        a.WriteBuffer(b, offset, selector);
    for (int i = 0; i < 900; i++)
        Debug::log << b;


    MSG msg{};
    Sleep(50000);
    try
    {
        
        {
            std::shared_ptr<dxe::Engine> engine = std::make_shared<dxe::Engine>();
            engine->SetTitleName(L"Game");
            engine->SetHandleName(L"main");
            engine->EngineInit();
            engine->VisualInit();
            engine = nullptr;

            std::shared_ptr<dxe::Engine> engine2 = std::make_shared<dxe::Engine>();
            engine->SetTitleName(L"Gamea2");
            engine->SetHandleName(L"maina2");
            engine2->EngineInit();
            engine2->VisualInit();
            engine2 = nullptr;
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
        //if(Engine::GetEngineList().empty())
        {
            Debug::log << "모든 엔진이 제거됨\n" << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            throw;
        }
    }
    catch (const std::exception& e)
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
    SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
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
