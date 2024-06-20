#pragma once
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>
#include <string_view>
#include <format> // c++20

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>

#include <convert_string.h>


namespace std
{
	inline std::wstring to_wstring(const DirectX::XMFLOAT4X4& matrix, UINT codePage = CP_THREAD_ACP)
	{

		std::wstring str2{std::to_wstring(std::format("<{:.3}, {:.3}, {:.3}, {:.3}>\n<{:.3}, {:.3}, {:.3}, {:.3}>\n<{:.3}, {:.3}, {:.3}, {:.3}>\n<{:.3}, {:.3}, {:.3}, {:.3}>",
			matrix._11, matrix._12, matrix._13, matrix._14,
			matrix._21, matrix._22, matrix._23, matrix._24,
			matrix._31, matrix._32, matrix._33, matrix._34,
			matrix._41, matrix._42, matrix._43, matrix._44))};
		return str2;
	}
	inline std::wstring to_wstring(const DirectX::XMMATRIX& matrix, UINT codePage = CP_THREAD_ACP)
	{
		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, matrix);
		return std::to_wstring(mat);
	}
	
	inline std::wstring to_wstring(const DirectX::XMFLOAT4& vector, UINT codePage = CP_THREAD_ACP)
	{
		std::wstring str2{ std::to_wstring(std::format("<{:.3}, {:.3}, {:.3}, {:.3}>",
			vector.x, vector.y, vector.z, vector.w)) };
		return str2;
	}

	inline std::wstring to_wstring(const DirectX::XMFLOAT3& vector, UINT codePage = CP_THREAD_ACP)
	{
		std::wstring str2{ std::to_wstring(std::format("<{:.3}, {:.3}, {:.3}>",
			vector.x, vector.y, vector.z)) };
		return str2;
	}

	inline std::wstring to_wstring(const DirectX::XMFLOAT2& vector, UINT codePage = CP_THREAD_ACP)
	{
		std::wstring str2{ std::to_wstring(std::format("<{:.3}, {:.3}>",
			vector.x, vector.y)) };
		return str2;
	}

	inline std::wstring to_wstring(const DirectX::XMVECTOR& vector, UINT codePage = CP_THREAD_ACP)
	{
		DirectX::XMFLOAT4 vec;
		DirectX::XMStoreFloat4(&vec, vector);
		return std::to_wstring(vec);
	}
}