
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
		// Static Line�� ���� ����
		CloseWindow();

		if (this->_hWndAccelerator != nullptr)
			DestroyAcceleratorTable(this->_hWndAccelerator);

        this->_engineQuitFlag = true;

		if(_engineMainThread != nullptr)
			_engineMainThread->request_stop();
		_engineMainThread.release();

        resource->Release();
        graphic->Release();
		Debug::log << std::format("���� ���� (�̸�:{})", std::to_string(_titleName)) << "\n";
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
		// Static Line�� ���� �߰�
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
        //���� �����ؾ��� Init�� �����ϰ� ����Ǵ°� ����.
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

                // -- Input ����
                InputEvent _event;
				input->DataBeginUpdate();
				while (this->_engineInputDispatcher->_inputDispatcher.try_pop(_event)) { // Input ���
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
				{ // ������ ���� Wait
					if (true) // ����� busy waiting
					{
						//������ ���� ���߱�
						int contextSwitch_MS = std::max(0, (int)(1000 / this->targetFrame) - 2); //�ִ� ���� +-1�� ���� ������� 2��
						std::this_thread::sleep_for(std::chrono::milliseconds(contextSwitch_MS));
						while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - currentFrameStartTime).count() <= (pow(10, 6) / this->targetFrame));
					}
					if (false) // ������ busy waiting
					{
						while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - currentFrameStartTime).count() <= (pow(10, 6) / this->targetFrame));
					}
				}
			}


		}
		catch (const std::exception& e)
		{
			Debug::log << "���� �߻�: "<< std::format("Engine : {}, thread : {}", std::to_string(_titleName), GetCurrentThreadId())<<"\n" << e.what() << "\n";
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
			#pragma region CS ����
			//CS_OWNDC: �� �����찡 ������ ��ġ ���ؽ�Ʈ�� �������� �մϴ�
			//CS_DROPSHADOW â�� �׸��ڸ� �߰��մϴ�. 
			//CS_CLASSDC : ��� �����찡 ���ؽ�Ʈ�� �����ϵ��� �մϴ�.
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

			#pragma region WS ����
			/*
WS_OVERLAPPED ĸ��(Ÿ��Ʋ ��)�� �ִ� �������Դϴ�. WS_TILED�� �����մϴ�.
WS_POPUP �˾� �������Դϴ�.
WS_CHILD �ڽ� �������Դϴ�. �� ��Ÿ���� ���� ������� �θ� �������� Ŭ���̾�Ʈ ���� ���� ǥ�õ˴ϴ�.
WS_MINIMIZE �ּ�ȭ�� �������Դϴ�.
WS_VISIBLE ȭ�� ǥ��
WS_DISABLED ��Ȱ��ȭ
WS_CLIPSIBLINGS �ڽ� �������� ������ ���� ��ĥ ��, ��ġ�� ������ �ڽ� �����찡 Ŭ���մϴ�.
WS_CLIPCHILDREN �θ� �������� ������ �׸� ��, �ڽ� �������� ������ Ŭ���մϴ�. �ڽ� �����츦 �׸��� �ʽ��ϴ�.
WS_MAXIMIZE �ִ�ȭ�� �������Դϴ�.
WS_CAPTION Ÿ��Ʋ ��(ĸ��)�� ���(border)�� ���� �������Դϴ�. WS_BORDER | WS_DLGFRAME�� �����մϴ�.
WS_BORDER ����: �ܼ��� ��踦 ���� �������Դϴ�.
WS_DLGFRAME ����: Ÿ��Ʋ �ٴ� ������, ��踦 ���� �������Դϴ�.
WS_VSCROLL ����: ���� ��ũ�� �ٸ� ���� �������Դϴ�.
WS_HSCROLL ����: ���� ��ũ�� �ٸ� ���� �������Դϴ�.
WS_SYSMENU ����: �ý��� �޴��� ���� �������Դϴ�. WS_CAPTION�� �Բ� ����ؾ� �մϴ�.
WS_THICKFRAME ����: ũ�⸦ ������ �� �ִ� ��踦 ���� �������Դϴ�. WS_SIZEBOX�� �����մϴ�.
WS_GROUP ����: ������ �׷쿡�� ù ��° ��Ʈ���� �����մϴ�.
WS_TABSTOP ����: �� Ű�� ����Ͽ� ��Ŀ���� �̵��� �� �ִ� ��Ʈ���� �����մϴ�.
WS_MINIMIZEBOX ����: �ּ�ȭ ��ư�� ���� �������Դϴ�. WS_SYSMENU�� �Բ� ����ؾ� �մϴ�.
WS_MAXIMIZEBOX ����: �ִ�ȭ ��ư�� ���� �������Դϴ�. WS_SYSMENU�� �Բ� ����ؾ� �մϴ�.

WS_OVERLAPPEDWINDOW : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
WS_POPUPWINDOW : WS_POPUP | WS_BORDER | WS_SYSMENU
WS_CHILDWINDOW : WS_CHILD�� ����
*/


			// â��� WS_OVERLAPPEDWINDOW
			// â���(ũ������ X) WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE
			// �׵θ� ����(�̵� ����) | WS_POPUP | WS_CAPTION | WS_VISIBLE
			// �׵θ� ���� WS_POPUP | WS_VISIBLE // �̰ɷ� �ȵǸ� WS_POPUP
			// ��üȭ�� WS_POPUP | WS_MAXIMIZE | WS_VISIBLE // �ٵ� �̰� ������ �ȸ����� ��Ҵ���.
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
				Debug::log << "Engine HWND ���� ����\n";
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
			#pragma region SW ����
/*
 SW_HIDE : �����츦 ����ϴ�.
 SW_SHOWNORMAL : �����츦 ���� ���·� ǥ���մϴ� (�⺻ ũ��� ��ġ��).
 SW_SHOWMINIMIZED : �����츦 �ּ�ȭ�� ���·� ǥ���մϴ�.
 SW_SHOWMAXIMIZED : �����츦 �ִ�ȭ�� ���·� ǥ���մϴ�.
 SW_SHOWNOACTIVATE : �����츦 ����������, Ȱ��ȭ�� ���� �ʽ��ϴ�.
 SW_SHOW : �����츦 ���� ���·� ǥ���մϴ�.
 SW_MINIMIZE : �����츦 �ּ�ȭ�մϴ�.
 SW_MAXIMIZE : �����츦 �ִ�ȭ�մϴ�.
 SW_RESTORE : �����츦 �����Ͽ� ���� ũ��� ��ġ�� ǥ���մϴ�.
 */
#pragma endregion
			ShowWindow(this->_hWnd, SW_SHOWNORMAL);
			UpdateWindow(this->_hWnd);
		}
		else
			Debug::log << "wnd �� �� ����.\n";
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
				#pragma region �޼��� ����
			case WM_CREATE:
				break;
			case WM_SYSCHAR:
			case WM_LBUTTONDBLCLK: // ����Ŭ��
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case WM_DROPFILES: //���� �巡�� ����
			case WM_CUT:// �߶󳻱� ���
			case WM_COPY:// ���� ���
			case WM_PASTE:// �ٿ��ֱ� ���

			case WM_NCACTIVATE: //��Ŭ���̾�Ʈ ������ Ȱ��ȭ�� ��
			case WM_NCCALCSIZE: //��Ŭ���̾�Ʈ ������ ũ�Ⱑ ���� ��
			case WM_NCHITTEST: //��Ŭ���̾�Ʈ �������� ��Ʈ �׽�Ʈ�� ����� ��
			case WM_NCLBUTTONDBLCLK: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ����Ŭ���� ��
			case WM_NCRBUTTONDBLCLK: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ����Ŭ���� ��
			case WM_NCMBUTTONDBLCLK: //��Ŭ���̾�Ʈ �������� ��� ���콺 ��ư�� ����Ŭ���� ��
			case WM_NCMOUSEHOVER: //��Ŭ���̾�Ʈ �������� ���콺�� ȣ������ ��
			case WM_NCMOUSELEAVE: //��Ŭ���̾�Ʈ �������� ���콺�� ���� ��
				break;

			case WM_SHOWWINDOW: //�̰� ���״�� ���̴��� ����
			case WM_GETMINMAXINFO: //â ũ�� �ּ�/�ִ�ȭ ��û
			case WM_SIZE:
            case WM_MOVE:
			case WM_DISPLAYCHANGE: // ����ȭ�� �ػ� ���� / ����� ����
			case WM_COMMAND:
			case WM_CLOSE:
			case WM_DESTROY:
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			case WM_ACTIVATE: //�̰� ��Ŀ�� ����

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			case WM_NCLBUTTONDOWN: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ���� ��
			case WM_NCLBUTTONUP: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ���� ��
			case WM_NCRBUTTONDOWN: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ���� ��
			case WM_NCRBUTTONUP: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ���� ��
			case WM_NCMBUTTONDOWN: //��Ŭ���̾�Ʈ �������� ��� ���콺 ��ư�� ���� ��
			case WM_NCMBUTTONUP: //��Ŭ���̾�Ʈ �������� ��� ���콺 ��ư�� ���� ��
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
			case WM_CHAR: // 229 �� ������ (wchar_t)wParam
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
		case WM_MOVE:// ������ �̵���ų��
			break;
		case WM_CREATE: //������
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_CLOSE: //���� ��ư
		{
			break;
		}
		case WM_SETFOCUS: //��Ŀ�� �޾�����
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_KILLFOCUS: //��Ŀ�� �Ҿ�����
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_DESTROY: // � ���������� �ı��ɶ�
			//Debug::log << "����\n";
			//Engine::_engineList.erase(targetEngineIter);
			//PostQuitMessage(0); // �� �����忡�� ���� ���� ��� ������� ���μ����� ���� ��Ȯ�� WM_Quit�� ȣ���ϴ°���.
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
		case WM_SHOWWINDOW: //�̰� ���״�� ���̴��� ����
			break;
		case WM_GETMINMAXINFO: //â ũ�� �ּ�/�ִ�ȭ ��û
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
		case WM_DISPLAYCHANGE: // ����ȭ�� �ػ� ���� / ����� ����
			break;
		case WM_COMMAND:
			break;
		case WM_CLOSE:
            eventDesc.type = InputType::Event;
            eventDesc.event.isQuit = true;
			break;
		case WM_DESTROY:
            
            //this->Quit();
			//DeleteEngine(this->shared_from_this()); // â �ı��� ���� ����
			break;
		case WM_SETFOCUS:
			break;
		case WM_KILLFOCUS:
			break;
		case WM_ACTIVATE: //�̰� ��Ŀ�� ����
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
		case WM_NCLBUTTONDOWN: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ���� ��
		case WM_NCRBUTTONDOWN: //��Ŭ���̾�Ʈ �������� ���� ���콺 ��ư�� ���� ��
		case WM_NCMBUTTONDOWN: //��Ŭ���̾�Ʈ �������� ��� ���콺 ��ư�� ���� ��
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
		case WM_CHAR: // 229 �� ������ (wchar_t)wParam
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
