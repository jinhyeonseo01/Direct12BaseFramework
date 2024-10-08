#pragma once
#include <ostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
#include <string_view>
#include <format> // c++20
#include <array>

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

#include <convert_total.h>

namespace Debug
{
	class Console
	{
	protected:
		std::atomic_bool active{ false };
		std::wstringstream oss;
		std::array<wchar_t, 65536> consoleInputBuffer{ L'\0' };

	public:
		//static Console cout;

		Console();
		virtual ~Console();

		bool IsActive();
		bool SetActive(bool active);
		bool ClearContext();

		static Console& CreateConsole(int offsetX = 0, int offsetY = 0, int width = 600, int hegiht = 800, bool record = false);
		static void Close();

		std::atomic_bool runningAsyncWriteThread{true};
		std::atomic_bool record{ false };
		std::thread asyncWriteThread;
		std::queue<std::wstring> wstringQueue;
		std::mutex m;
		std::wofstream debugLogOutPut;
		
		std::atomic_bool waiting{ false };

		std::wstring TextPop();
		int TextCount();
		void TextQueueClear();
		void TextPush(std::wstring& str);

		void AsyncWriteFunction()
		{
			bool isRecord = record.load();
			if (isRecord)
				debugLogOutPut.open("Debug_Log.txt");
			while (runningAsyncWriteThread.load())
			{
				if (!waiting.load())
				{
					std::wstring str;
					int safeCount = 0;

					while (!(str = TextPop()).empty() && safeCount <= 10'000) {
						if (isRecord)
							debugLogOutPut << str;
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						++safeCount;
					}
					if (TextCount() >= 100'000)
						TextQueueClear();
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			if (isRecord)
				debugLogOutPut.close();
		}

		template <typename T>
		Console& operator<<(const T& value) {
			if (this->active)
			{
				if constexpr (std::is_same_v<std::decay_t<T>, char*> ||
					std::is_same_v<std::decay_t<T>, const char*>) {//(std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
					std::wstring str = std::to_wstring((const char*)value, (const char*)value + std::strlen(value));
					if (!runningAsyncWriteThread.load())
					{
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					}
					else
						TextPush(str);
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					std::wstring str = std::to_wstring(value);
					if (!runningAsyncWriteThread.load())
					{
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					}
					else
						TextPush(str);
				}
				else if constexpr (std::is_same_v<T, std::wstring>) {
					std::wstring str = value;
					if (!runningAsyncWriteThread.load())
					{
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					}
					else
						TextPush(str);
				}
				else if constexpr (std::is_same_v<T, bool>) {
					std::wstring str{ value ? L"true" : L"false" };
					if (!runningAsyncWriteThread.load())
					{
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					}
					else
						TextPush(str);
				}
				else {
					std::wstring str;
					if constexpr (std::is_pointer_v<T>)
					{
						if (value == nullptr || value == NULL)
						{
							str = L"null";
							if (!runningAsyncWriteThread.load())
							{
								WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
							}
							else
								TextPush(str);
							return *this;
						}
						const unsigned long long magicValue = 0xCCCCCCCCCCCCCCCC;
						if ((unsigned long long)value == magicValue)
						{
							str = L"초기화 안된 주소";
							if (!runningAsyncWriteThread.load())
							{
								WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
							}
							else
								TextPush(str);
							return *this;
						}
					}
					if constexpr (has_to_wstring_v<T>)
					{
						str = std::to_wstring(value);
						if (!runningAsyncWriteThread.load())
						{
							WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						}
						else
							TextPush(str);
						return *this;
					}
					if constexpr (Convert::is_wstringstreamable_v<T>)
					{
						oss.clear();
						oss.str(std::wstring());
						oss << value;
						str = oss.str();
						if (!runningAsyncWriteThread.load())
						{
							WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						}
						else
							TextPush(str);
						return *this;
					}
					else
					{
						str = std::to_wstring(std::format("출력불가:{}", typeid(T).name()));
						if (!runningAsyncWriteThread.load())
						{
							WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						}
						else
							TextPush(str);
						return *this;
					}
				}
			}
			return *this;
		}

		Console& operator>>(std::wstring& value)
		{
			waiting.store(true);

			SetForegroundWindow(GetConsoleWindow());
			ShowWindow(GetConsoleWindow(), SW_SHOW);

			DWORD textSize = 0;
			ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), consoleInputBuffer.data(), consoleInputBuffer.size(), &textSize, nullptr);
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			while (textSize != 0 && (consoleInputBuffer[textSize - 1] == (wchar_t)0xA || consoleInputBuffer[textSize - 1] == (wchar_t)0xD))
				textSize--;
			consoleInputBuffer[textSize] = L'\0';
			if (textSize != 0) {
				std::wstring text{ consoleInputBuffer.begin(), consoleInputBuffer.begin() + textSize };
				auto b = text.find_first_not_of(L" \t\n\r");
				auto e = text.find_last_not_of(L" \t\n\r");
				text = text.substr(b, e - b + 1);
				value = text;
			}
			
			waiting.store(false);
			return *this;
		}
		Console& operator>>(std::string& value)
		{
			std::wstring original;
			(*this) >> original;
			value = std::to_string(original);
			return *this;
		}
		
		std::string GetContextAll();
	};

	extern Console log;
}