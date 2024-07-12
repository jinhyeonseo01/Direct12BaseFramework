#pragma once

#include <string.h>

#include "convert_string.h"
#include "convert_directx.h"
#include "convert_math.h"


template <typename, typename T>
struct has_to_wstring : std::false_type {};

template <typename T>
struct has_to_wstring<
	T,
	std::void_t<decltype(std::to_wstring(std::declval<T>()))>
> : std::true_type {};

// ���� ���� ���ø�
template <typename T>
inline constexpr bool has_to_wstring_v = has_to_wstring<T, void>::value;


template <typename T>
concept InputIterator = requires(T it) {
	{ ++it } -> std::same_as<T&>;       // ���� ���� ������
	{ *it } -> std::same_as<typename std::iterator_traits<T>::reference>; // ������ ������
	{ it == it } -> std::same_as<bool>; // ���� �� ������
};

namespace std
{
	template<InputIterator Iter>
	inline std::wstring to_wstring(Iter _First, Iter _End, UINT codePage = CP_THREAD_ACP)
	{
		wstringstream wss;
		wss << "[";
		for (; _First != _End; ++_First)
			wss << std::to_wstring(*_First) << ", ";
		wss << "]";
		return wss.str();
	}
}
