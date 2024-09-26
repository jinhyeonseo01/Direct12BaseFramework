#pragma once

#include <string>

namespace dxe
{
	class Guid
	{
	public:
		static std::wstring GetNewGuid();
		static std::wstring ConvertGuid(const GUID& guid);
		static GUID ConvertGuid(const std::wstring& guid);
	};
}
