#pragma once

#include <stdafx.h>
#include <input_module.h>


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

		DirectX::SimpleMath::Viewport _windowRect;
		bool _windowFullScreen = false;

		bool isOpenWindow = false;
		bool isActiveWindow = false;

	public:
		std::chrono::time_point<std::chrono::steady_clock> _engineStartClock{};
		std::unique_ptr<std::jthread> _engineMainThread{nullptr};
		std::unique_ptr<Input> _engineInput;

		static HINSTANCE SetWindowHInstance(HINSTANCE hInstance) { return Engine::_processInstance = hInstance; }
		HWND SetWindowHWnd(HWND hWnd) { return _hWnd = hWnd; }
		HWND GetWindowHWnd() { return _hWnd; }
		void SetTitleName(std::wstring name);
		void SetHandleName(std::wstring name);

		void OpenWindow();
		void EnableWindow();
		void DisableWindow();
		void CloseWindow();

		static LRESULT __stdcall WindowStaticCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void WindowLocalCallback(WinEvent& winEvent);

		DirectX::SimpleMath::Viewport* GetWindowRect();
		DirectX::SimpleMath::Viewport* SetWindowRect(DirectX::SimpleMath::Viewport& windowRect);
		

		// Engine Setting ----------------------------------------------------
	public:

		Engine();
		virtual ~Engine();

		virtual void ThreadExecute(std::stop_token token);
		std::shared_ptr<Engine> BaseInitialize();

		virtual std::shared_ptr<Engine> Initialize();
		virtual std::shared_ptr<Engine> Reset();
		virtual void Release();

	};
}
