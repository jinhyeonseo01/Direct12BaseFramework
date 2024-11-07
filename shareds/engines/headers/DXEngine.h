#pragma once

#include <stdafx.h>
#include <EObject.h>
#include <GameObject.h>
#include <Component.h>
#include <Transform.h>
#include <input_module.h>
#include <engine_utility.h>
#include <GameObject.h>
#include <Component.h>
#include <Transform.h>
#include <Scene.h>
#include <SceneManager.h>
#include <Vertex.h>
#include <Input.h>

#include "GraphicManager.h"


namespace dxe
{

	class Engine : public std::enable_shared_from_this<Engine>
	{
		// Global Setting ----------------------------------------------------
	protected:
		static std::vector<std::shared_ptr<Engine>> _engineList;
		static int _mainEngineIndex;

	public:
		static HINSTANCE _processInstance;
		static std::chrono::time_point<std::chrono::steady_clock> _processStartClock;
		static std::vector<ACCEL> globalSpecialKey;

	public:
		static std::shared_ptr<Engine> GetEngine(int index = -1);
		static int SetMainEngine(int index);
        static std::shared_ptr<Engine> GetMainEngine();
		static std::shared_ptr<Engine> AppendEngine(std::shared_ptr<Engine> engine);
		static bool DeleteEngine(int index);
		static bool DeleteEngine(std::shared_ptr<Engine> engine);
		static void DeleteEngineAll();
		static std::shared_ptr<Engine> FindEngine(std::wstring handleName);
		static std::shared_ptr<Engine> FindEngine(HWND& hWnd);
		static std::vector<std::shared_ptr<Engine>>& GetEngineList() { return _engineList; };

	
		// Window Setting ----------------------------------------------------
	public:
		HWND _hWnd = nullptr;
		WNDCLASSEXW _hWndWNDCLASSEXW;
		ATOM _hWndATOM;
		std::vector<ACCEL> _hWndACCELs;
		HACCEL _hWndAccelerator = nullptr;

		std::wstring _titleName{ L"title" };
		std::wstring _handleName{ L"handle" };
		std::wstring _iconPath{ L"configs/Icons/MainIcon.ico"};
		std::wstring _iconSmallPath{ L"configs/Icons/SmallIcon.ico" };

		DirectX::SimpleMath::Viewport _windowRect = Viewport(0,0,1280,720);
		bool _windowFullScreen = false;

		bool isOpenWindow = false;
		bool isActiveWindow = false;
        bool isInClientMouse = false;

        bool cursorHide = false;
        void SetCursorHide(bool hide);
        bool GetCursorHide() const;

	public:
        std::shared_ptr<GraphicManager> graphic;
        std::shared_ptr<ResourceManager> resource;

	public: //Engine Field
		std::chrono::time_point<std::chrono::steady_clock> _engineStartClock{};
		std::unique_ptr<std::jthread> _engineMainThread{nullptr};
		std::unique_ptr<InputDispatcher> _engineInputDispatcher;
		std::unique_ptr<Input> input;

        bool _engineQuitFlag = false;

		bool isFrameLock = true;
		double targetFrame = 165;
		double currentFrame = 144;
		double deltaTime = 0.016;

		bool deltaTimeLimit = false;
        double deltaTimeLimitValue = 0.33333;

		static HINSTANCE SetWindowHInstance(HINSTANCE hInstance) { return Engine::_processInstance = hInstance; }
		HWND SetWindowHWnd(HWND hWnd) { return _hWnd = hWnd; }
		HWND GetWindowHWnd() { return _hWnd; }
		void SetTitleName(const std::wstring& name);
		void UpdateTitleName(const std::wstring& name);
		void SetHandleName(const std::wstring& name);

		void OpenWindow();
		void EnableWindow();
		void DisableWindow();
		void CloseWindow();

        void Quit();

		static LRESULT __stdcall WindowStaticCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void WindowLocalCallback(WinEvent& winEvent);

        DirectX::SimpleMath::Viewport GetWindowRect();
        DirectX::SimpleMath::Viewport SetWindowRect(const DirectX::SimpleMath::Viewport& windowRect);
		

		// Engine Setting ----------------------------------------------------
	public:

		Engine();
		virtual ~Engine();

		virtual void ThreadExecute(std::stop_token token);
        virtual std::shared_ptr<Engine> EngineInit();
		virtual std::shared_ptr<Engine> VisualInit();
        virtual void EngineRun();
		virtual std::shared_ptr<Engine> Reset();
		virtual void Release();

	public:
		virtual void LogicPipeline();
		virtual void RenderingPipeline();

	protected:
		virtual void DebugInit();
		virtual void DebugPipeline();

	    std::vector<DebugCommand> _debugCommandList;
		bool _debugCommandMode = false;

	    double _debugFrameTimer = 0;
        bool _debugFrameTimerActive = false;
	};

	class DebugCommand
	{
	public:
		std::string command;
		std::wstring detail;
		std::function<void(std::vector<std::wstring>&)> func;
		bool operator==(const std::string& command) const;
		bool operator==(const std::wstring& command) const;
	};
}
