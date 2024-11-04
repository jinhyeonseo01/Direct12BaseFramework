
#include <stdafx.h>
#include <input_module.h>
#include <DXEngine.h>
#include <Input.h>

#include "ResourceManager.h"
#include "Scene.h"
#include "SceneManager.h"

namespace dxe
{
	int Engine::_mainEngineIndex = -1;
	std::vector<std::shared_ptr<Engine>> Engine::_engineList{};
	std::chrono::time_point<std::chrono::steady_clock> Engine::_processStartClock{};

	HINSTANCE Engine::_processInstance = nullptr;
	std::vector<ACCEL> Engine::globalSpecialKey{
		ACCEL{ FVIRTKEY | FCONTROL, 'N', 1 }
	};

	Engine::Engine()
	{
		Engine::_engineList.reserve(32);
		this->_hWndACCELs.reserve(16);
		this->_windowRect.width = 1920;
		this->_windowRect.height = 1080;

	}

	Engine::~Engine()
	{
		this->Release();
	}

	std::shared_ptr<Engine> Engine::GetEngine(int index)
	{
		if (Engine::_engineList.size() == 0)
			return nullptr;
		if (index == -1)
		{
			Engine::_mainEngineIndex = std::clamp(Engine::_mainEngineIndex, 0, static_cast<int>(Engine::_engineList.size() - 1));
			return Engine::_engineList[Engine::_mainEngineIndex];
		}
		else
		{
			index = std::clamp(index, 0, static_cast<int>(Engine::_engineList.size() - 1));
			return Engine::_engineList[index];
		}
		return nullptr;
	}
	int Engine::SetMainEngine(int index)
	{
		return Engine::_mainEngineIndex = (index = std::clamp(index, 0, static_cast<int>(Engine::_engineList.size() - 1)));
	}

    std::shared_ptr<Engine> Engine::GetMainEngine()
    {
        return GetEngine(_mainEngineIndex);
    }

    std::shared_ptr<Engine> Engine::AppendEngine(std::shared_ptr<Engine> engine)
	{
		auto findEngine = std::ranges::find(Engine::_engineList, engine);
		if (findEngine == Engine::_engineList.end())
			Engine::_engineList.emplace_back(engine);
		else
			engine = *findEngine;
		return engine;
	}
	bool Engine::DeleteEngine(int index)
	{
		if (index >= 0 && index < Engine::_engineList.size())
		{
			Engine::_mainEngineIndex -= index <= Engine::_mainEngineIndex ? 1 : 0;
			Engine::_mainEngineIndex = std::clamp(Engine::_mainEngineIndex, 0, static_cast<int>(Engine::_engineList.size() - 1));
			Engine::_engineList.erase(Engine::_engineList.begin() + index);
			if (Engine::_engineList.empty()) Engine::_mainEngineIndex = -1;
			return true;
		}
		return false;
	}
	bool Engine::DeleteEngine(std::shared_ptr<Engine> engine)
	{
		//Engine::_engineList.erase();
		int index = std::distance(Engine::_engineList.begin(),
			std::find_if(Engine::_engineList.begin(), Engine::_engineList.end(), [engine](const std::shared_ptr<Engine>& eng) {
				return eng == engine;
			}));
		return Engine::DeleteEngine(index);
	}
	void Engine::DeleteEngineAll()
	{
		for (int i = 0, size = Engine::_engineList.size(); i < size; ++i)
			Engine::_engineList.erase(Engine::_engineList.begin());
		Engine::_mainEngineIndex = -1;
	}
	std::shared_ptr<Engine> Engine::FindEngine(std::wstring handleName)
	{
		auto findEngine = std::find_if(Engine::_engineList.begin(), Engine::_engineList.end(), [handleName](const std::shared_ptr<Engine> engine) {
			return engine->_handleName == handleName;
			});
		if (findEngine == Engine::_engineList.end())
			return nullptr;
		return *findEngine;
	}
	std::shared_ptr<Engine> Engine::FindEngine(HWND& hWnd)
	{
		auto findEngine = std::find_if(Engine::_engineList.begin(), Engine::_engineList.end(), [hWnd](const std::shared_ptr<Engine> engine) {
			return engine->_hWnd == hWnd;
			});
		if (findEngine == Engine::_engineList.end())
			return nullptr;
		return *findEngine;
	}

    void Engine::SetCursorHide(bool hide)
    {
        cursorHide = hide;
        if (cursorHide)
            ShowCursor(cursorHide ? FALSE : TRUE);
    }

    bool Engine::GetCursorHide() const
    {
        return cursorHide;
    }

    void Engine::Release()
	{
		// Static Line에 엔진 제거
		CloseWindow();

		if (this->_hWndAccelerator != nullptr)
			DestroyAcceleratorTable(this->_hWndAccelerator);

        this->_engineQuitFlag = true;

		if(_engineMainThread != nullptr)
			_engineMainThread->request_stop();
		_engineMainThread.release();

        resource->Release();
        graphic->Release();
		Debug::log << std::format("엔진 종료 (이름:{})", std::to_string(_titleName)) << "\n";
	}

	std::shared_ptr<Engine> Engine::Reset()
	{
		SetWindowTextW(this->_hWnd, this->_titleName.data());

		return this->shared_from_this();
	}

    std::shared_ptr<Engine> Engine::EngineInit()
	{
		this->_engineStartClock = std::chrono::steady_clock::now();
		this->_engineInputDispatcher = std::make_unique<InputDispatcher>(this->shared_from_this());
		this->input = std::make_unique<Input>();
        this->graphic = std::make_shared<GraphicManager>();
        this->resource = std::make_shared<ResourceManager>();

		AppendEngine(this->shared_from_this());
		DebugInit();

        this->_engineQuitFlag = false;

		return this->shared_from_this();
	}

	std::shared_ptr<Engine> Engine::VisualInit()
	{
		// Static Line에 엔진 추가
		OpenWindow();
        graphic->_engine = this->shared_from_this();
        graphic->SetHWnd(GetWindowHWnd());
        graphic->SetScreenInfo(GetWindowRect());
	    graphic->Init();

        resource->Init();

		return this->shared_from_this();
	}

    void Engine::EngineRun()
    {
        this->_engineMainThread = std::make_unique<std::jthread>(std::bind(&Engine::ThreadExecute, this, std::placeholders::_1));
        this->_engineMainThread->detach();
        SetThreadPriority(this->_engineMainThread->native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
    }

    void Engine::ThreadExecute(std::stop_token token)
	{
        //이후 동작해야할 Init를 무시하고 실행되는걸 방지.
		try
		{
			auto prevTime = std::chrono::steady_clock::now() - std::chrono::milliseconds(10);
			while (!token.stop_requested()) //Frame Loop
			{
				auto currentFrameStartTime = std::chrono::steady_clock::now();

				this->deltaTime = std::min(std::chrono::duration_cast<std::chrono::microseconds>(currentFrameStartTime - prevTime).count() / 1000000.0, deltaTimeLimitValue);
				this->deltaTimeLimit = this->deltaTime >= this->deltaTimeLimitValue;
				this->currentFrame = 1.0 / this->deltaTime;

				UpdateTitleName(std::to_wstring(std::format("{} ({}:{:0.2f})", std::to_string(_titleName), "Fps", this->currentFrame)));

                // -- Input 동작
                InputEvent _event;
				input->DataBeginUpdate();
				while (this->_engineInputDispatcher->_inputDispatcher.try_pop(_event)) { // Input 등록
					input->DataUpdate(_event);

                    if (_event.type == InputType::Event && _event.event.isQuit)
                        Quit();
				}
				DebugPipeline();

                if (this->_engineQuitFlag)
                    break;

				auto logicPipelineStartTime = std::chrono::steady_clock::now();
				LogicPipeline();
				auto logicPipelineEndTime = std::chrono::steady_clock::now();

				auto renderingPipelineStartTime = logicPipelineEndTime;
				RenderingPipeline();
				auto renderingPipelineEndTime = std::chrono::steady_clock::now();
				auto logicPipelineDeltaTime = logicPipelineEndTime - logicPipelineStartTime;
				auto renderingPipelineDeltaTime = renderingPipelineEndTime - renderingPipelineStartTime;


				prevTime = currentFrameStartTime;
				if (this->isFrameLock)
				{ // 고정밀 스핀 Wait
					if (true) // 완충된 busy waiting
					{
						//사전에 조금 멈추기
						int contextSwitch_MS = std::max(0, (int)(1000 / this->targetFrame) - 2); //최대 오차 +-1에 대한 베리어로 2값
						std::this_thread::sleep_for(std::chrono::milliseconds(contextSwitch_MS));
						while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - currentFrameStartTime).count() <= (pow(10, 6) / this->targetFrame));
					}
					if (false) // 고정밀 busy waiting
					{
						while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - currentFrameStartTime).count() <= (pow(10, 6) / this->targetFrame));
					}
				}
			}


		}
		catch (const std::exception& e)
		{
			Debug::log << "예외 발생: "<< std::format("Engine : {}, thread : {}", std::to_string(_titleName), GetCurrentThreadId())<<"\n" << e.what() << "\n";
            Debug::log.Call();
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
        DeleteEngine(this->shared_from_this());
	}

	void Engine::OpenWindow()
	{
		if (!this->isOpenWindow)
		{
			this->isOpenWindow = true;


			this->_hWndWNDCLASSEXW.cbSize = sizeof(WNDCLASSEX);
			#pragma region CS 종류
			//CS_OWNDC: 각 윈도우가 고유한 장치 컨텍스트를 가지도록 합니다
			//CS_DROPSHADOW 창에 그림자를 추가합니다. 
			//CS_CLASSDC : 모든 윈도우가 컨텍스트를 공유하도록 합니다.
#pragma endregion
			this->_hWndWNDCLASSEXW.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			this->_hWndWNDCLASSEXW.lpfnWndProc = Engine::WindowStaticCallback;
			this->_hWndWNDCLASSEXW.cbClsExtra = 0;
			this->_hWndWNDCLASSEXW.cbWndExtra = 0;
			this->_hWndWNDCLASSEXW.hInstance = this->_processInstance;
			this->_hWndWNDCLASSEXW.hIcon = (HICON)LoadImageW(this->_processInstance, _iconPath.data(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			this->_hWndWNDCLASSEXW.hIconSm = (HICON)LoadImageW(this->_processInstance, _iconSmallPath.data(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			this->_hWndWNDCLASSEXW.hCursor = LoadCursor(nullptr, IDC_ARROW);
			this->_hWndWNDCLASSEXW.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			this->_hWndWNDCLASSEXW.lpszMenuName = NULL;
			this->_hWndWNDCLASSEXW.lpszClassName = this->_handleName.data();

			this->_hWndATOM = RegisterClassExW(&this->_hWndWNDCLASSEXW);

			#pragma region WS 종류
			/*
WS_OVERLAPPED 캡션(타이틀 바)이 있는 윈도우입니다. WS_TILED와 동일합니다.
WS_POPUP 팝업 윈도우입니다.
WS_CHILD 자식 윈도우입니다. 이 스타일을 가진 윈도우는 부모 윈도우의 클라이언트 영역 내에 표시됩니다.
WS_MINIMIZE 최소화된 윈도우입니다.
WS_VISIBLE 화면 표시
WS_DISABLED 비활성화
WS_CLIPSIBLINGS 자식 윈도우의 영역이 서로 겹칠 때, 겹치는 영역을 자식 윈도우가 클립합니다.
WS_CLIPCHILDREN 부모 윈도우의 영역을 그릴 때, 자식 윈도우의 영역을 클립합니다. 자식 윈도우를 그리지 않습니다.
WS_MAXIMIZE 최대화된 윈도우입니다.
WS_CAPTION 타이틀 바(캡션)와 경계(border)를 가진 윈도우입니다. WS_BORDER | WS_DLGFRAME과 동일합니다.
WS_BORDER 설명: 단순한 경계를 가진 윈도우입니다.
WS_DLGFRAME 설명: 타이틀 바는 없지만, 경계를 가진 윈도우입니다.
WS_VSCROLL 설명: 수직 스크롤 바를 가진 윈도우입니다.
WS_HSCROLL 설명: 수평 스크롤 바를 가진 윈도우입니다.
WS_SYSMENU 설명: 시스템 메뉴를 가진 윈도우입니다. WS_CAPTION과 함께 사용해야 합니다.
WS_THICKFRAME 설명: 크기를 조절할 수 있는 경계를 가진 윈도우입니다. WS_SIZEBOX와 동일합니다.
WS_GROUP 설명: 윈도우 그룹에서 첫 번째 컨트롤을 지정합니다.
WS_TABSTOP 설명: 탭 키를 사용하여 포커스를 이동할 수 있는 컨트롤을 지정합니다.
WS_MINIMIZEBOX 설명: 최소화 버튼을 가진 윈도우입니다. WS_SYSMENU와 함께 사용해야 합니다.
WS_MAXIMIZEBOX 설명: 최대화 버튼을 가진 윈도우입니다. WS_SYSMENU와 함께 사용해야 합니다.

WS_OVERLAPPEDWINDOW : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
WS_POPUPWINDOW : WS_POPUP | WS_BORDER | WS_SYSMENU
WS_CHILDWINDOW : WS_CHILD랑 동일
*/


			// 창모드 WS_OVERLAPPEDWINDOW
			// 창모드(크기조절 X) WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE
			// 테두리 없음(이동 가능) | WS_POPUP | WS_CAPTION | WS_VISIBLE
			// 테두리 없음 WS_POPUP | WS_VISIBLE // 이걸로 안되면 WS_POPUP
			// 전체화면 WS_POPUP | WS_MAXIMIZE | WS_VISIBLE // 근데 이거 사이즈 안맞으면 축소당함.
#pragma endregion

            int windowSettingFlag = WS_OVERLAPPED;
            switch (graphic->setting.windowType)
            {
            case WindowType::Windows:
                windowSettingFlag = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
                break;
            case WindowType::FullScreen_Noborder:
                windowSettingFlag = WS_POPUP | WS_VISIBLE;
                break;
            case WindowType::FullScreen:
                windowSettingFlag = WS_POPUP | WS_MAXIMIZE | WS_VISIBLE;
                    break;
            }
            
			HWND hWnd = CreateWindowW(_handleName.data(), _titleName.data(),
                windowSettingFlag,
                graphic->setting.screenInfo.x, graphic->setting.screenInfo.y,
                graphic->setting.screenInfo.width, graphic->setting.screenInfo.height, //CW_USEDEFAULT //this->_windowRect.width, this->_windowRect.height
				nullptr, nullptr,
				Engine::_processInstance, nullptr);
			for(auto& speckey : globalSpecialKey)
				_hWndACCELs.push_back(speckey);
			_hWndACCELs.push_back(ACCEL{ FVIRTKEY | FCONTROL, 'N', 1 });
			_hWndACCELs.push_back(ACCEL{ FVIRTKEY | FCONTROL, 'O', 2 });
			_hWndACCELs.push_back(ACCEL{ FVIRTKEY | FCONTROL, 'S', 3 });
			
			if (this->_hWndAccelerator != nullptr)
				DestroyAcceleratorTable(this->_hWndAccelerator);
			this->_hWndAccelerator = CreateAcceleratorTable(_hWndACCELs.data(), _hWndACCELs.size());

			if (!hWnd)
				Debug::log << "Engine HWND 생성 실패\n";
			else
			{
				SetWindowHWnd(hWnd);
			}


			this->EnableWindow();
		}
	}
	void Engine::CloseWindow()
	{
		if (this->isOpenWindow)
		{
			this->DisableWindow();
		    this->graphic->Release();
            this->graphic = nullptr;
			this->isOpenWindow = false;

            DestroyWindow(this->_hWnd);
			this->_hWnd = nullptr;
		}
	}

    void Engine::Quit()
    {
        this->_engineQuitFlag = true;
    }

    void Engine::EnableWindow()
	{
		if (this->isOpenWindow && !this->isActiveWindow)
		{
			this->isActiveWindow = true;
			#pragma region SW 설명
/*
 SW_HIDE : 윈도우를 숨깁니다.
 SW_SHOWNORMAL : 윈도우를 보통 상태로 표시합니다 (기본 크기와 위치로).
 SW_SHOWMINIMIZED : 윈도우를 최소화된 상태로 표시합니다.
 SW_SHOWMAXIMIZED : 윈도우를 최대화된 상태로 표시합니다.
 SW_SHOWNOACTIVATE : 윈도우를 보여주지만, 활성화는 하지 않습니다.
 SW_SHOW : 윈도우를 현재 상태로 표시합니다.
 SW_MINIMIZE : 윈도우를 최소화합니다.
 SW_MAXIMIZE : 윈도우를 최대화합니다.
 SW_RESTORE : 윈도우를 복구하여 이전 크기와 위치로 표시합니다.
 */
#pragma endregion
			ShowWindow(this->_hWnd, SW_SHOWNORMAL);
			UpdateWindow(this->_hWnd);
		}
		else
			Debug::log << "wnd 열 수 없음.\n";
	}
	void Engine::DisableWindow()
	{
		if (this->isOpenWindow && this->isActiveWindow)
		{
			ShowWindow(this->_hWnd, SW_HIDE);
			this->isActiveWindow = false;
		}
	}

    DirectX::SimpleMath::Viewport Engine::GetWindowRect()
	{
		return (this->_windowRect);
	}

    DirectX::SimpleMath::Viewport Engine::SetWindowRect(const DirectX::SimpleMath::Viewport& windowRect)
	{
		this->_windowRect = windowRect;
        if(this->graphic != nullptr)
	        this->graphic->SetScreenInfo(windowRect);
		return this->GetWindowRect();
	}

	void Engine::SetTitleName(const std::wstring& name)
	{
		this->_titleName = name;
		Engine::UpdateTitleName(this->_titleName);
	}

	void Engine::UpdateTitleName(const std::wstring& name)
	{
		if (this->_hWnd != nullptr)
			SetWindowTextW(this->_hWnd, name.data());
	}

	void Engine::SetHandleName(const std::wstring& name)
	{
		this->_handleName = name;
	}

	LRESULT __stdcall Engine::WindowStaticCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		std::shared_ptr<Engine> targetEngine = Engine::FindEngine(hWnd);
		std::chrono::time_point<std::chrono::steady_clock> timeClock = std::chrono::steady_clock::now();

		WinEvent winEvent;
		winEvent.hWnd = hWnd;
		winEvent.message = message;
		winEvent.wParam = wParam;
		winEvent.lParam = lParam;
		winEvent.time = timeClock;
		
		if (targetEngine != nullptr)
		{
			switch (winEvent.message)
			{
				#pragma region 메세지 종류
			case WM_CREATE:
				break;
			case WM_SYSCHAR:
			case WM_LBUTTONDBLCLK: // 더블클릭
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case WM_DROPFILES: //파일 드래그 관련
			case WM_CUT:// 잘라내기 명령
			case WM_COPY:// 복사 명령
			case WM_PASTE:// 붙여넣기 명령

			case WM_NCACTIVATE: //비클라이언트 영역이 활성화될 때
			case WM_NCCALCSIZE: //비클라이언트 영역의 크기가 계산될 때
			case WM_NCHITTEST: //비클라이언트 영역에서 히트 테스트가 수행될 때
			case WM_NCLBUTTONDBLCLK: //비클라이언트 영역에서 왼쪽 마우스 버튼이 더블클릭될 때
			case WM_NCRBUTTONDBLCLK: //비클라이언트 영역에서 왼쪽 마우스 버튼이 더블클릭될 때
			case WM_NCMBUTTONDBLCLK: //비클라이언트 영역에서 가운데 마우스 버튼이 더블클릭될 때
			case WM_NCMOUSEHOVER: //비클라이언트 영역에서 마우스가 호버링할 때
			case WM_NCMOUSELEAVE: //비클라이언트 영역에서 마우스가 떠날 때
				break;

			case WM_SHOWWINDOW: //이건 말그대로 보이는지 여부
			case WM_GETMINMAXINFO: //창 크기 최소/최대화 요청
			case WM_SIZE:
            case WM_MOVE:
			case WM_DISPLAYCHANGE: // 바탕화면 해상도 변경 / 모니터 변경
			case WM_COMMAND:
			case WM_CLOSE:
			case WM_DESTROY:
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			case WM_ACTIVATE: //이건 포커스 관련

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			case WM_NCLBUTTONDOWN: //비클라이언트 영역에서 왼쪽 마우스 버튼이 눌릴 때
			case WM_NCLBUTTONUP: //비클라이언트 영역에서 왼쪽 마우스 버튼이 놓일 때
			case WM_NCRBUTTONDOWN: //비클라이언트 영역에서 왼쪽 마우스 버튼이 눌릴 때
			case WM_NCRBUTTONUP: //비클라이언트 영역에서 왼쪽 마우스 버튼이 놓일 때
			case WM_NCMBUTTONDOWN: //비클라이언트 영역에서 가운데 마우스 버튼이 눌릴 때
			case WM_NCMBUTTONUP: //비클라이언트 영역에서 가운데 마우스 버튼이 놓일 때
			case WM_NCXBUTTONDOWN:
			case WM_NCXBUTTONUP:

			case WM_NCMOUSEMOVE:
			case WM_MOUSEMOVE:
			case WM_MOUSEWHEEL:
			case WM_MOUSEHOVER:
			case WM_MOUSELEAVE:

			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR: // 229 로 들어오면 (wchar_t)wParam
#pragma endregion
				targetEngine->WindowLocalCallback(winEvent);
			default:
				break;
			}
		}
		switch (message)
		{
		case WM_COMMAND:
		{
			
			int wmId = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);

			if (wmId == 0)
				break;
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			EndPaint(hWnd, &ps);
		}
		break;
		case WM_MOVE:// 윈도우 이동시킬때
			break;
		case WM_CREATE: //생성때
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_CLOSE: //종료 버튼
		{
			break;
		}
		case WM_SETFOCUS: //포커스 받았을때
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_KILLFOCUS: //포커스 잃었을때
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_DESTROY: // 어떤 이유에서든 파괴될때
			//Debug::log << "삭제\n";
			//Engine::_engineList.erase(targetEngineIter);
			//PostQuitMessage(0); // 그 스레드에서 실행 중인 모든 윈도우와 프로세스가 종료 정확힌 WM_Quit을 호출하는거임.
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

    
	void Engine::WindowLocalCallback(WinEvent& winEvent)
	{
		InputEvent eventDesc;
		std::memset(&eventDesc, 0, sizeof(InputEvent));
        eventDesc.type = InputType::Event;

		switch (winEvent.message)
		{
		case WM_SHOWWINDOW: //이건 말그대로 보이는지 여부
			break;
		case WM_GETMINMAXINFO: //창 크기 최소/최대화 요청
			break;
        case WM_SIZE:
        {
            UINT width = LOWORD(winEvent.lParam);
            UINT height = HIWORD(winEvent.lParam);
            auto rect = GetWindowRect();
            rect.width = width;
            rect.height = height;
            SetWindowRect(rect);
            break;
        }
        case WM_MOVE:
        {
            UINT x = LOWORD(winEvent.lParam);
            UINT y = HIWORD(winEvent.lParam);
            auto rect = GetWindowRect();
            rect.x = x;
            rect.y = y;
            SetWindowRect(rect);
            break;
        }
		case WM_DISPLAYCHANGE: // 바탕화면 해상도 변경 / 모니터 변경
			break;
		case WM_COMMAND:
			break;
		case WM_CLOSE:
            eventDesc.type = InputType::Event;
            eventDesc.event.isQuit = true;
			break;
		case WM_DESTROY:
            
            //this->Quit();
			//DeleteEngine(this->shared_from_this()); // 창 파괴시 엔진 제거
			break;
		case WM_SETFOCUS:
			break;
		case WM_KILLFOCUS:
			break;
		case WM_ACTIVATE: //이건 포커스 관련
			break;


		case WM_NCLBUTTONUP:
		case WM_NCRBUTTONUP:
		case WM_NCMBUTTONUP:
		case WM_NCXBUTTONUP:
		{
			break;
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.isCtrl = winEvent.wParam & MK_CONTROL;
			eventDesc.mouse.isShift = winEvent.wParam & MK_SHIFT;
			eventDesc.mouse.isUp = true;
			eventDesc.mouse.isInClient = false;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
			switch (winEvent.message)
			{
			case WM_NCLBUTTONUP:eventDesc.keyCode = KeyCode::LeftMouse; break;
			case WM_NCRBUTTONUP:eventDesc.keyCode = KeyCode::RightMouse; break;
			case WM_NCMBUTTONUP:eventDesc.keyCode = KeyCode::CenterMouse; break;
			}
			if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X1Mouse;
			if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X2Mouse;

			//ReleaseCapture();
			break;
		}
		case WM_NCLBUTTONDOWN: //비클라이언트 영역에서 왼쪽 마우스 버튼이 눌릴 때
		case WM_NCRBUTTONDOWN: //비클라이언트 영역에서 왼쪽 마우스 버튼이 눌릴 때
		case WM_NCMBUTTONDOWN: //비클라이언트 영역에서 가운데 마우스 버튼이 눌릴 때
		case WM_NCXBUTTONDOWN:
		{
			break;
			if (winEvent.wParam == HTCAPTION) {
				DefWindowProc(winEvent.hWnd, winEvent.message, winEvent.wParam, winEvent.lParam);
				break;
			}
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.isCtrl = winEvent.wParam & MK_CONTROL;
			eventDesc.mouse.isShift = winEvent.wParam & MK_SHIFT;
			eventDesc.mouse.isDown = true;
			eventDesc.mouse.isInClient = false;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
			switch (winEvent.message)
			{
			case WM_NCLBUTTONDOWN:eventDesc.keyCode = KeyCode::LeftMouse; break;
			case WM_NCRBUTTONDOWN:eventDesc.keyCode = KeyCode::RightMouse; break;
			case WM_NCMBUTTONDOWN:eventDesc.keyCode = KeyCode::CenterMouse; break;
			default:
				if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X1Mouse;
				if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X2Mouse;
				break;
			}

			//SetCapture(winEvent.hWnd);
			break;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.isCtrl = winEvent.wParam & MK_CONTROL;
			eventDesc.mouse.isShift = winEvent.wParam & MK_SHIFT;
			eventDesc.mouse.isUp = true;
			eventDesc.mouse.isInClient = true;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
			switch (winEvent.message)
			{
			case WM_LBUTTONUP:eventDesc.keyCode = KeyCode::LeftMouse; break;
			case WM_RBUTTONUP:eventDesc.keyCode = KeyCode::RightMouse; break;
			case WM_MBUTTONUP:eventDesc.keyCode = KeyCode::CenterMouse; break;
			default:
				if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X1Mouse;
				if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X2Mouse;
				break;
			}
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.isCtrl = winEvent.wParam & MK_CONTROL;
			eventDesc.mouse.isShift = winEvent.wParam & MK_SHIFT;
			eventDesc.mouse.isDown = true;
			eventDesc.mouse.isInClient = true;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
			switch (winEvent.message)
			{
			case WM_LBUTTONDOWN:eventDesc.keyCode = KeyCode::LeftMouse; break;
			case WM_RBUTTONDOWN:eventDesc.keyCode = KeyCode::RightMouse; break;
			case WM_MBUTTONDOWN:eventDesc.keyCode = KeyCode::CenterMouse; break;
			default:
				if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X1Mouse;
				if (winEvent.wParam & MK_XBUTTON1)	eventDesc.keyCode = KeyCode::X2Mouse;
				break;
			}
			break;
		}
		case WM_NCMOUSEMOVE:
		{
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
			eventDesc.mouse.isInClient = false;
			break;
		}
		case WM_MOUSEMOVE:
		{
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
			eventDesc.mouse.isInClient = true;
            if(!isInClientMouse)
            {
                isInClientMouse = true;
                if (cursorHide)
                    ShowCursor(FALSE);
                else
                    ShowCursor(TRUE);

                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = winEvent.hWnd;
                TrackMouseEvent(&tme);
            }
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int deltaZ = (winEvent.wParam >> 16) & 0xffff;
			char sign = std::sign(deltaZ);
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.posX = 0;
			eventDesc.mouse.posY = deltaZ;
			eventDesc.mouse.isDown = sign < 0;
			eventDesc.mouse.isUp = sign > 0;
			eventDesc.mouse.isCtrl = winEvent.wParam & MK_CONTROL;
			eventDesc.mouse.isShift = winEvent.wParam & MK_SHIFT;
			eventDesc.keyCode = KeyCode::CenterMouse;
			break;
		}
		case WM_MOUSEHOVER:
		{
			eventDesc.type = InputType::Mouse;
			eventDesc.mouse.posX = static_cast<float>(LOWORD(winEvent.lParam));
			eventDesc.mouse.posY = static_cast<float>(HIWORD(winEvent.lParam));
            
			break;
		}
		case WM_MOUSELEAVE:
		{
            isInClientMouse = false;
            if(cursorHide)
                ShowCursor(TRUE);
			break;
		}
		case WM_KEYUP:
		case WM_KEYDOWN:
		{
			short repeat = winEvent.lParam & 0xFFFF;
			short scanCode = (winEvent.lParam >> 16) & 0xFF;
			short extendedKey = (winEvent.lParam >> 16) & 0xFF;
			bool isAlt = (winEvent.lParam >> 29) & 0x1;
			bool isFirst = !((winEvent.lParam >> 30) & 0x1);
			bool isDown = !((winEvent.lParam >> 31) & 0x1);
			bool isUp = !isDown;

			std::memset(&eventDesc, 0, sizeof(InputEvent));
			eventDesc.type = InputType::Keyboard;
			eventDesc.keyCode = winEvent.wParam;
			eventDesc.keyboard.repeat = repeat;
			eventDesc.keyboard.scanCode = scanCode;
			eventDesc.keyboard.extendedKey = extendedKey;
			eventDesc.keyboard.isFirst = isFirst;
			eventDesc.keyboard.isAlt = isAlt;
			eventDesc.keyboard.isDown = isDown;
			eventDesc.keyboard.isUp = isUp;
			break;
		}
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			if (winEvent.wParam != 18)
				break;
			short repeat = winEvent.lParam & 0xFFFF;
			short scanCode = (winEvent.lParam >> 16) & 0xFF;
			short extendedKey = (winEvent.lParam >> 16) & 0xFF;
			bool isAlt = (winEvent.lParam >> 29) & 0x1;
			bool isFirst = !((winEvent.lParam >> 30) & 0x1);
			bool isDown = !((winEvent.lParam >> 31) & 0x1);
			bool isUp = !isDown;

			std::memset(&eventDesc, 0, sizeof(InputEvent));
			eventDesc.type = InputType::Keyboard;
			eventDesc.keyCode = winEvent.wParam;
			eventDesc.keyboard.repeat = repeat;
			eventDesc.keyboard.scanCode = scanCode;
			eventDesc.keyboard.extendedKey = extendedKey;
			eventDesc.keyboard.isFirst = isFirst;
			eventDesc.keyboard.isAlt = isAlt;
			eventDesc.keyboard.isDown = isDown;
			eventDesc.keyboard.isUp = isUp;
			break;
		}
		case WM_CHAR: // 229 로 들어오면 (wchar_t)wParam
			//Debug::log << (wchar_t)winEvent.wParam << "\n\n";
			break;
		default:
			break;
		}
		switch (winEvent.message)
		{
		case WM_KEYDOWN:
			//Debug::log << winEvent.wParam << "\n";
		default:
			break;
		}
		this->_engineInputDispatcher->_inputDispatcher.push(eventDesc);
	}
}
