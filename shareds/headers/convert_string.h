#pragma once

#include <string>
#include <string_view>
#include <Windows.h>

namespace Convert
{
	template <typename, typename T>
	struct has_example : std::false_type {};

	template <typename T>
	struct has_example<
		T,
		std::void_t<decltype(std::to_wstring(std::declval<T>()))>
	> : std::true_type {};

	// 헬퍼 변수 템플릿
	template <typename T>
	inline constexpr bool has_to_wstring_v = has_example<T, void>::value;

	//----------------------------------------

	template <typename, typename T, typename = void>
	struct is_ostreamable : std::false_type {};

	// 특수화 템플릿: T가 std::ostream에 출력 가능한 경우
	template <typename T>
	struct is_ostreamable<
		std::ostream,
		T,
		std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>
	> : std::true_type {};

	// 헬퍼 변수 템플릿
	template <typename T>
	inline constexpr bool is_ostreamable_v = is_ostreamable<std::ostream, T>::value;

	//----------------------------------------

	template <typename, typename T, typename = void>
	struct is_wstringstreamable : std::false_type {};

	// 특수화 템플릿: T가 std::ostream에 출력 가능한 경우
	template <typename T>
	struct is_wstringstreamable<
		std::wstringstream,
		T,
		std::void_t<decltype(std::declval<std::wstringstream&>() << std::declval<T>())>
	> : std::true_type {};

	// 헬퍼 변수 템플릿
	template <typename T>
	inline constexpr bool is_wstringstreamable_v = is_wstringstreamable<std::wstringstream, T>::value;


	inline std::wstring to_wstring(const std::string& str, UINT codePage = CP_THREAD_ACP)
	{
		if (str.empty())
			return std::wstring();
		int required = ::MultiByteToWideChar(codePage, 0, str.data(), (int)str.size(), NULL, 0);
		//if (0 == required)
		//	return std::wstring();

		std::wstring str2;
		str2.resize(required);

		int converted = ::MultiByteToWideChar(codePage, 0, str.data(), (int)str.size(), str2.data(), (int)str2.capacity());
		//if (0 == converted)
		//	return std::wstring();
		return str2;
	}
	inline std::wstring to_wstring(const char* _first, const char* _end, UINT codePage = CP_THREAD_ACP)
	{
		int required = ::MultiByteToWideChar(codePage, 0, _first, (int)(_end - _first), NULL, 0);
		//if (0 == required)
		//	return std::wstring();

		std::wstring str2;
		str2.resize(required);

		int converted = ::MultiByteToWideChar(codePage, 0, _first, (int)(_end - _first), str2.data(), (int)str2.capacity());
		//if (0 == converted)
		//	return std::wstring();
		return str2;
	}
	inline std::wstring to_wstring(const char* _first, UINT codePage = CP_THREAD_ACP)
	{
		int size = std::strlen(_first);
		int required = ::MultiByteToWideChar(codePage, 0, _first, size, NULL, 0);
		//if (0 == required)
		//	return std::wstring();

		std::wstring str2;
		str2.resize(required);

		int converted = ::MultiByteToWideChar(codePage, 0, _first, size, str2.data(), (int)str2.capacity());
		//if (0 == converted)
		//	return std::wstring();
		return str2;
	}


	inline std::string to_string(const std::wstring& str, UINT codePage = CP_THREAD_ACP)
	{
		if (str.empty())
			return std::string();

		int required = ::WideCharToMultiByte(codePage, 0, str.data(), (int)str.size(), NULL, 0, NULL, NULL);
		//if (0 == required)
		//	return std::string();

		std::string str2;
		str2.resize(required);

		int converted = ::WideCharToMultiByte(codePage, 0, str.data(), (int)str.size(), str2.data(), (int)str2.capacity(), NULL, NULL);
		//if (0 == converted)
		//	return std::string();

		return str2;
	}

	inline std::string to_string(const wchar_t* _first, const wchar_t* _end, UINT codePage = CP_THREAD_ACP)
	{
		int required = ::WideCharToMultiByte(codePage, 0, _first, (int)(_end - _first), NULL, 0, NULL, NULL);
		//if (0 == required)
		//	return std::string();

		std::string str2;
		str2.resize(required);

		int converted = ::WideCharToMultiByte(codePage, 0, _first, (int)(_end - _first), str2.data(), (int)str2.capacity(), NULL, NULL);
		//if (0 == converted)
		//	return std::string();

		return str2;
	}
	inline std::string to_string(const wchar_t* _first, UINT codePage = CP_THREAD_ACP)
	{
		int size = std::wcslen(_first);
		int required = ::WideCharToMultiByte(codePage, 0, _first, size, NULL, 0, NULL, NULL);
		//if (0 == required)
		//	return std::string();

		std::string str2;
		str2.resize(required);

		int converted = ::WideCharToMultiByte(codePage, 0, _first, size, str2.data(), (int)str2.capacity(), NULL, NULL);
		//if (0 == converted)
		//	return std::string();

		return str2;
	}
}




namespace std
{
	inline std::wstring to_wstring(const std::string& str, UINT codePage = CP_THREAD_ACP)
	{
		if (str.empty())
			return std::wstring();
		int required = ::MultiByteToWideChar(codePage, 0, str.data(), (int)str.size(), NULL, 0);
		//if (0 == required)
		//	return std::wstring();

		std::wstring str2;
		str2.resize(required);

		int converted = ::MultiByteToWideChar(codePage, 0, str.data(), (int)str.size(), str2.data(), (int)str2.capacity());
		//if (0 == converted)
		//	return std::wstring();
		return str2;
	}
	inline std::wstring to_wstring(const char* _first, const char* _end, UINT codePage = CP_THREAD_ACP)
	{
		int required = ::MultiByteToWideChar(codePage, 0, _first, (int)(_end - _first), NULL, 0);
		//if (0 == required)
		//	return std::wstring();

		std::wstring str2;
		str2.resize(required);

		int converted = ::MultiByteToWideChar(codePage, 0, _first, (int)(_end - _first), str2.data(), (int)str2.capacity());
		//if (0 == converted)
		//	return std::wstring();
		return str2;
	}
	inline std::wstring to_wstring(const char* _first, UINT codePage = CP_THREAD_ACP)
	{
		int size = std::strlen(_first);
		int required = ::MultiByteToWideChar(codePage, 0, _first, size, NULL, 0);
		//if (0 == required)
		//	return std::wstring();

		std::wstring str2;
		str2.resize(required);

		int converted = ::MultiByteToWideChar(codePage, 0, _first, size, str2.data(), (int)str2.capacity());
		//if (0 == converted)
		//	return std::wstring();
		return str2;
	}


	inline std::string to_string(const std::wstring& str, UINT codePage = CP_THREAD_ACP)
	{
		if (str.empty())
			return std::string();

		int required = ::WideCharToMultiByte(codePage, 0, str.data(), (int)str.size(), NULL, 0, NULL, NULL);
		//if (0 == required)
		//	return std::string();

		std::string str2;
		str2.resize(required);

		int converted = ::WideCharToMultiByte(codePage, 0, str.data(), (int)str.size(), str2.data(), (int)str2.capacity(), NULL, NULL);
		//if (0 == converted)
		//	return std::string();

		return str2;
	}

	inline std::string to_string(const wchar_t* _first, const wchar_t* _end, UINT codePage = CP_THREAD_ACP)
	{
		int required = ::WideCharToMultiByte(codePage, 0, _first, (int)(_end - _first), NULL, 0, NULL, NULL);
		//if (0 == required)
		//	return std::string();

		std::string str2;
		str2.resize(required);

		int converted = ::WideCharToMultiByte(codePage, 0, _first, (int)(_end - _first), str2.data(), (int)str2.capacity(), NULL, NULL);
		//if (0 == converted)
		//	return std::string();

		return str2;
	}
	inline std::string to_string(const wchar_t* _first, UINT codePage = CP_THREAD_ACP)
	{
		int size = std::wcslen(_first);
		int required = ::WideCharToMultiByte(codePage, 0, _first, size, NULL, 0, NULL, NULL);
		//if (0 == required)
		//	return std::string();

		std::string str2;
		str2.resize(required);

		int converted = ::WideCharToMultiByte(codePage, 0, _first, size, str2.data(), (int)str2.capacity(), NULL, NULL);
		//if (0 == converted)
		//	return std::string();

		return str2;
	}
}