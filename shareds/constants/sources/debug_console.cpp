#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <memory>
#include <Windows.h>

#include <thread>

#include "debug_console.h"


Debug::Console Debug::log = {};


namespace Debug
{
    int Console::debugDeltaTime = 10;

	Console::Console()
	{
		
		
	}
	Console::~Console()
	{
		Close();
	}
	//std::thread asyncWriteThread;
	//std::queue<std::wstring> wstringQueue;
	std::wstring Console::TextPop()
	{
		std::wstring str;
		m.lock();
		if (!Debug::log.wstringQueue.empty())
		{
			str = Debug::log.wstringQueue.front();
			Debug::log.wstringQueue.pop();
		}
		m.unlock();
		return str;
	}

	void Console::TextPush(std::wstring& str)
	{
		m.lock();
		Debug::log.wstringQueue.push(str);
		m.unlock();
	}

	int Console::TextCount()
	{
		int count;
		m.lock();
		count = Debug::log.wstringQueue.size();
		m.unlock();
		return count;
	}

	void Console::TextQueueClear()
	{
		m.lock();
		while(!Debug::log.wstringQueue.empty())
			Debug::log.wstringQueue.pop();
		m.unlock();
		std::wstring str{ L"Debug Console Clear : 너무 많은 부채\n" };
		TextPush(str);
	}

	Console& Console::CreateConsole(int offsetX, int offsetY, int width, int hegiht, bool record)
	{
		if (!log.active.load())
		{
			AllocConsole();
			SetConsoleTitle(L"콘솔창");
			
			log.record.store(record);
			log.active.store(true);
			log.runningAsyncWriteThread.store(true);

			if (log.runningAsyncWriteThread.load())
			{
				log.asyncWriteThread = std::thread{ []() {
					log.AsyncWriteFunction();
				} };
				//log.asyncWriteThread.detach();
			}
		}
		//HWND_TOP
		SetWindowPos(GetConsoleWindow(), HWND_NOTOPMOST, offsetX, offsetY, width, hegiht, SWP_SHOWWINDOW|
			SWP_ASYNCWINDOWPOS);
		return log;
	}
	void Console::Close()
	{
		if (log.active.load())
		{
			log.active.store(false);
			log.runningAsyncWriteThread.store(false);
            log.asyncWriteThread.join();
			FreeConsole();
		}
	}

	bool Console::IsActive()
	{
		return this->active.load();
	}

	bool Console::SetActive(bool active)
	{
		this->active.store(active);
		if (this->active.load())
		{

		}
		else
		{

		}

		return this->active.load();
	}

	bool Console::ClearContext()
	{
		if (!this->active)
			return false;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

		if (hConsole == INVALID_HANDLE_VALUE)
			return false;
		if (!GetConsoleScreenBufferInfo(hConsole, &bufferInfo))
			return false;

		COORD topLeft = { 0, 0 };
		DWORD charsWritten;
		DWORD conSize = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;

		if (!FillConsoleOutputCharacterW(hConsole, L' ', conSize, topLeft, &charsWritten))
			return false;
		if (!SetConsoleCursorPosition(hConsole, topLeft))
			return false;

		return true;
	}


	std::string Console::GetContextAll()
	{
		std::string context{""};

		return context;
	}
}

/*
SWP_ASYNCWINDOWPOS:
작업이 비동기적으로 수행됩니다. 이 플래그가 설정된 경우, SetWindowPos 함수는 모든 요청된 작업이 완료되기 전에 반환될 수 있습니다.
SWP_DEFERERASE:
창이 지워지지 않습니다. 이 플래그는 창이 클라이언트 영역을 다시 그리지 않도록 합니다.
SWP_DRAWFRAME:
창의 프레임을 다시 그립니다. 이 플래그는 SWP_FRAMECHANGED와 동일합니다.
SWP_FRAMECHANGED:
창의 스타일을 변경하고 창의 프레임을 다시 그립니다. 창의 스타일을 변경한 후 이 플래그를 설정해야 합니다.
SWP_HIDEWINDOW:
창을 숨깁니다.
SWP_NOACTIVATE:
창을 활성화하지 않습니다. 창의 위치와 크기는 변경되지만 활성 창은 변경되지 않습니다.
SWP_NOCOPYBITS:
비트 블록 전송을 사용하여 창을 다시 그리지 않습니다. 일반적으로 창의 일부를 다른 영역으로 이동할 때 사용합니다.
SWP_NOMOVE:
창의 위치를 변경하지 않습니다. 창의 크기만 변경됩니다.
SWP_NOOWNERZORDER:
소유 창의 Z 순서를 변경하지 않습니다.
SWP_NOREDRAW:
창을 다시 그리지 않습니다. 클라이언트 영역의 콘텐츠는 그대로 유지됩니다.
SWP_NOREPOSITION:
SWP_NOOWNERZORDER와 동일합니다.
SWP_NOSENDCHANGING:
창이 변경될 때 WM_WINDOWPOSCHANGING 메시지를 보내지 않습니다.
SWP_NOSIZE:
창의 크기를 변경하지 않습니다. 창의 위치만 변경됩니다.
SWP_NOZORDER:
창의 Z 순서를 변경하지 않습니다.
SWP_SHOWWINDOW:
창을 표시합니다
*/