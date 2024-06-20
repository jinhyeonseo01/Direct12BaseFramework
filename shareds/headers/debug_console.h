#pragma once
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>
#include <string_view>
#include <format> // c++20

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

#include "convert_string.h"

namespace Debug
{

	class Console
	{
	protected:
		std::atomic_bool active{ false };
		std::wstringstream oss;

	public:
		//static Console cout;

		Console();
		virtual ~Console();

		bool IsActive();
		bool SetActive(bool active);
		bool ClearContext();

		static Console& CreateConsole(int offsetX = 0, int offsetY = 0, int width = 600, int hegiht = 800);
		static void Close();

		std::atomic_bool runningAsyncWriteThread{false};
		std::thread asyncWriteThread;
		std::queue<std::wstring> wstringQueue;
		std::mutex m;

		std::wstring TextPop();
		int TextCount();
		void TextQueueClear();
		void TextPush(std::wstring& str);

		void AsyncWriteFunction()
		{
			while (runningAsyncWriteThread.load())
			{
				std::wstring str;
				int safeCount = 0;
				while (!(str = TextPop()).empty() && safeCount <= 10'000) {
					WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					++safeCount;
				}
				if (TextCount() >= 100'000)
					TextQueueClear();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		template <typename T>
		Console& operator<<(const T& value) {
			if (this->active)
			{
				if constexpr (std::is_same_v<std::decay_t<T>, char*> ||
					std::is_same_v<std::decay_t<T>, const char*>) {//(std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
					std::wstring str = std::to_wstring((const char*)value, (const char*)value + std::strlen(value));
					//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					TextPush(str);
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					std::wstring str = std::to_wstring(value);
					//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					TextPush(str);
				}
				else if constexpr (std::is_same_v<T, bool>) {
					std::wstring str{ value ? L"true" : L"false" };
					//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
					TextPush(str);
				}
				else {
					std::wstring str;
					if constexpr (std::is_pointer_v<T>)
					{
						if (value == nullptr || value == NULL)
						{
							str = L"null";
							//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
							TextPush(str);
							return *this;
						}
						const unsigned long long magicValue = 0xCCCCCCCCCCCCCCCC;
						if ((unsigned long long)value == magicValue)
						{
							str = L"초기화 안된 주소";
							//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
							TextPush(str);
							return *this;
						}
					}
					if constexpr (Convert::has_to_wstring_v<T>)
					{
						str = std::to_wstring(value);
						//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						TextPush(str);
						return *this;
					}
					if constexpr (Convert::is_wstringstreamable_v<T>)
					{
						oss.clear();
						oss.str(std::wstring());
						oss << value;
						str = oss.str();
						//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						TextPush(str);
						return *this;
					}
					else
					{
						str = std::to_wstring(std::format("출력불가:{}", typeid(T).name()));
						//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), (const void*)str.c_str(), (DWORD)str.size(), 0, 0);
						TextPush(str);
						return *this;
					}
				}
			}
			return *this;
		}
		
		std::string GetContextAll();
	};

	extern Console log;
}