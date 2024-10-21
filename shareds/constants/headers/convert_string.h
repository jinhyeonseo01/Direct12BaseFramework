#pragma once

#include <string>
#include <string_view>
#include <iterator>
#include <concepts>
#include <functional>

#include <Windows.h>

namespace Convert
{
	template <typename T>
	concept InputIterator = requires(T it) {
		{ ++it } -> std::same_as<T&>;       // ���� ���� ������
		{ *it } -> std::same_as<typename std::iterator_traits<T>::reference>; // ������ ������
		{ it == it } -> std::same_as<bool>; // ���� �� ������
	};


	//----------------------------------------

	template <typename, typename T, typename = void>
	struct is_ostreamable : std::false_type {};

	// Ư��ȭ ���ø�: T�� std::ostream�� ��� ������ ���
	template <typename T>
	struct is_ostreamable<
		std::ostream,
		T,
		std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>
	> : std::true_type {};

	// ���� ���� ���ø�
	template <typename T>
	inline constexpr bool is_ostreamable_v = is_ostreamable<std::ostream, T>::value;

	//----------------------------------------

	template <typename, typename T, typename = void>
	struct is_wstringstreamable : std::false_type {};

	// Ư��ȭ ���ø�: T�� std::ostream�� ��� ������ ���
	template <typename T>
	struct is_wstringstreamable<
		std::wstringstream,
		T,
		std::void_t<decltype(std::declval<std::wstringstream&>() << std::declval<T>())>
	> : std::true_type {};

	// ���� ���� ���ø�
	template <typename T>
	inline constexpr bool is_wstringstreamable_v = is_wstringstreamable<std::wstringstream, T>::value;


	inline std::wstring to_wstring(const std::string& str, UINT codePage = CP_THREAD_ACP) noexcept
	{
		if (str.empty())
			return {};
		std::wstring wstr;
		wstr.resize(MultiByteToWideChar(codePage, 0,str.data(),static_cast<int>(str.size()),nullptr, 0));
		MultiByteToWideChar(codePage, 0,str.data(),static_cast<int>(str.size()), wstr.data(), wstr.size());
		return std::move(wstr);
	}
	inline std::wstring to_wstring(const char* _first, const char* _end, UINT codePage = CP_THREAD_ACP) noexcept
	{
        if (_first == _end)
            return {};
		std::wstring str2;
		str2.resize(MultiByteToWideChar(codePage, 0, _first, static_cast<int>(_end - _first), nullptr, 0));
		MultiByteToWideChar(codePage, 0, _first, static_cast<int>(_end - _first), str2.data(), str2.size());
		return std::move(str2);
	}
	inline std::wstring to_wstring(const char* _first, UINT codePage = CP_THREAD_ACP) noexcept
	{
		int size = std::strlen(_first);
        if (size == 0)
            return{};
		std::wstring str2;
		str2.resize(MultiByteToWideChar(codePage, 0, _first, size, nullptr, 0));
		int converted = ::MultiByteToWideChar(codePage, 0, _first, size, str2.data(), static_cast<int>(str2.capacity()));
        return std::move(str2);
	}


	inline std::string to_string(const std::wstring& str, UINT codePage = CP_THREAD_ACP) noexcept
	{
		if (str.empty())
			return {};
		std::string str2;
		str2.resize(WideCharToMultiByte(codePage, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr));
		WideCharToMultiByte(codePage, 0, str.data(), static_cast<int>(str.size()), str2.data(), static_cast<int>(str2.capacity()), nullptr, nullptr);
        return std::move(str2);
	}

	inline std::string to_string(const wchar_t* _first, const wchar_t* _end, UINT codePage = CP_THREAD_ACP) noexcept
	{
		std::string str2;
		str2.resize(WideCharToMultiByte(codePage, 0, _first, static_cast<int>(_end - _first), nullptr, 0, nullptr, nullptr));
		WideCharToMultiByte(codePage, 0, _first, static_cast<int>(_end - _first), str2.data(), static_cast<int>(str2.capacity()), nullptr, nullptr);
        return std::move(str2);
	}
	inline std::string to_string(const wchar_t* _first, UINT codePage = CP_THREAD_ACP) noexcept
	{
		int size = std::wcslen(_first);
        if (size == 0)
            return {};
		std::string str2;
		str2.resize(WideCharToMultiByte(codePage, 0, _first, size, nullptr, 0, nullptr, nullptr));
	    WideCharToMultiByte(codePage, 0, _first, size, str2.data(), static_cast<int>(str2.capacity()), nullptr, nullptr);
        return std::move(str2);
	}
}




namespace std
{

    inline std::wstring to_wstring(const std::string& str, UINT codePage = CP_THREAD_ACP) noexcept
    {
        if (str.empty())
            return {};
        std::wstring wstr;
        wstr.resize(MultiByteToWideChar(codePage, 0, str.data(), static_cast<int>(str.size()), nullptr, 0));
        MultiByteToWideChar(codePage, 0, str.data(), static_cast<int>(str.size()), wstr.data(), wstr.size());
        return std::move(wstr);
    }
    inline std::wstring to_wstring(const char* _first, const char* _end, UINT codePage = CP_THREAD_ACP) noexcept
    {
        if (_first == _end)
            return {};
        std::wstring str2;
        str2.resize(MultiByteToWideChar(codePage, 0, _first, static_cast<int>(_end - _first), nullptr, 0));
        MultiByteToWideChar(codePage, 0, _first, static_cast<int>(_end - _first), str2.data(), str2.size());
        return std::move(str2);
    }
    inline std::wstring to_wstring(const char* _first, UINT codePage = CP_THREAD_ACP) noexcept
    {
        int size = std::strlen(_first);
        if (size == 0)
            return{};
        std::wstring str2;
        str2.resize(MultiByteToWideChar(codePage, 0, _first, size, nullptr, 0));
        int converted = ::MultiByteToWideChar(codePage, 0, _first, size, str2.data(), static_cast<int>(str2.capacity()));
        return std::move(str2);
    }


    inline std::string to_string(const std::wstring& str, UINT codePage = CP_THREAD_ACP) noexcept
    {
        if (str.empty())
            return {};
        std::string str2;
        str2.resize(WideCharToMultiByte(codePage, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr));
        WideCharToMultiByte(codePage, 0, str.data(), static_cast<int>(str.size()), str2.data(), static_cast<int>(str2.capacity()), nullptr, nullptr);
        return std::move(str2);
    }

    inline std::string to_string(const wchar_t* _first, const wchar_t* _end, UINT codePage = CP_THREAD_ACP) noexcept
    {
        std::string str2;
        str2.resize(WideCharToMultiByte(codePage, 0, _first, static_cast<int>(_end - _first), nullptr, 0, nullptr, nullptr));
        WideCharToMultiByte(codePage, 0, _first, static_cast<int>(_end - _first), str2.data(), static_cast<int>(str2.capacity()), nullptr, nullptr);
        return std::move(str2);
    }
    inline std::string to_string(const wchar_t* _first, UINT codePage = CP_THREAD_ACP) noexcept
    {
        int size = std::wcslen(_first);
        if (size == 0)
            return {};
        std::string str2;
        str2.resize(WideCharToMultiByte(codePage, 0, _first, size, nullptr, 0, nullptr, nullptr));
        WideCharToMultiByte(codePage, 0, _first, size, str2.data(), static_cast<int>(str2.capacity()), nullptr, nullptr);
        return std::move(str2);
    }

	template<class T, class Y>
	inline int split(const T& target, const T& c, Y& split)
	{
		split.clear();
		std::wstring str;
		auto prevPtr = target.begin();
		int count = 0;
		while (true)
		{
			auto currentPtr = std::search(prevPtr, target.end(), c.begin(), c.end());
			std::wstring s;
			split.emplace_back(prevPtr, currentPtr);
			if (currentPtr == target.end())
				break;
			if (count > 10000)
				break;
			
			prevPtr = currentPtr + std::distance(c.begin(), c.end());
		}
		return count;
	}
}