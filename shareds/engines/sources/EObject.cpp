#include "EObject.h"

namespace dxe
{
	EObject::EObject()
	{

	}

	EObject::EObject(std::wstring guid)
	{

	}

	std::wstring EObject::SetGUID(std::wstring str)
	{
		UUID uuid;
		RPC_WSTR uuidString = nullptr;

		HRESULT result_create = UuidCreate(&uuid);
		HRESULT result_change = UuidToStringW(&uuid, &uuidString);
		std::wstring wstr{ reinterpret_cast<wchar_t*>(uuidString) };
		RpcStringFreeW(&uuidString);
		Debug::log << wstr << "\n";

		return wstr;
	}
}