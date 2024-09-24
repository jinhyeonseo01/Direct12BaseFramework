#pragma once

#include <stdafx.h>

namespace dxe
{
	class EObject
	{
	public:
		std::wstring guid;
		EObject();
		EObject(std::wstring guid);
		std::wstring SetGUID(std::wstring str);
	};
}

