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
		std::wstring str{ L"Debug Console Clear : �ʹ� ���� ��ä\n" };
		TextPush(str);
	}

	Console& Console::CreateConsole(int offsetX, int offsetY, int width, int hegiht, bool record)
	{
		if (!log.active.load())
		{
			AllocConsole();
			SetConsoleTitle(L"�ܼ�â");
			
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
�۾��� �񵿱������� ����˴ϴ�. �� �÷��װ� ������ ���, SetWindowPos �Լ��� ��� ��û�� �۾��� �Ϸ�Ǳ� ���� ��ȯ�� �� �ֽ��ϴ�.
SWP_DEFERERASE:
â�� �������� �ʽ��ϴ�. �� �÷��״� â�� Ŭ���̾�Ʈ ������ �ٽ� �׸��� �ʵ��� �մϴ�.
SWP_DRAWFRAME:
â�� �������� �ٽ� �׸��ϴ�. �� �÷��״� SWP_FRAMECHANGED�� �����մϴ�.
SWP_FRAMECHANGED:
â�� ��Ÿ���� �����ϰ� â�� �������� �ٽ� �׸��ϴ�. â�� ��Ÿ���� ������ �� �� �÷��׸� �����ؾ� �մϴ�.
SWP_HIDEWINDOW:
â�� ����ϴ�.
SWP_NOACTIVATE:
â�� Ȱ��ȭ���� �ʽ��ϴ�. â�� ��ġ�� ũ��� ��������� Ȱ�� â�� ������� �ʽ��ϴ�.
SWP_NOCOPYBITS:
��Ʈ ��� ������ ����Ͽ� â�� �ٽ� �׸��� �ʽ��ϴ�. �Ϲ������� â�� �Ϻθ� �ٸ� �������� �̵��� �� ����մϴ�.
SWP_NOMOVE:
â�� ��ġ�� �������� �ʽ��ϴ�. â�� ũ�⸸ ����˴ϴ�.
SWP_NOOWNERZORDER:
���� â�� Z ������ �������� �ʽ��ϴ�.
SWP_NOREDRAW:
â�� �ٽ� �׸��� �ʽ��ϴ�. Ŭ���̾�Ʈ ������ �������� �״�� �����˴ϴ�.
SWP_NOREPOSITION:
SWP_NOOWNERZORDER�� �����մϴ�.
SWP_NOSENDCHANGING:
â�� ����� �� WM_WINDOWPOSCHANGING �޽����� ������ �ʽ��ϴ�.
SWP_NOSIZE:
â�� ũ�⸦ �������� �ʽ��ϴ�. â�� ��ġ�� ����˴ϴ�.
SWP_NOZORDER:
â�� Z ������ �������� �ʽ��ϴ�.
SWP_SHOWWINDOW:
â�� ǥ���մϴ�
*/