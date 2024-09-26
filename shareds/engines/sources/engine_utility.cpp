#include <utility.h>
#include <stdafx.h>

std::wstring dxe::Guid::GetNewGuid()
{
	UUID uuid;
	RPC_WSTR uuidString = nullptr;

	HRESULT result_create = UuidCreate(&uuid);
	HRESULT result_change = UuidToStringW(&uuid, &uuidString);
	std::wstring wstr{ reinterpret_cast<wchar_t*>(uuidString) };
	RpcStringFreeW(&uuidString);
	return wstr;
}
