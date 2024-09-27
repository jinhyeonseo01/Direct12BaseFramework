#include <stdafx.h>

#include <engine_utility.h>

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

std::wstring dxe::Guid::ConvertGuid(const GUID& guid)
{
	RPC_WSTR uuidString = nullptr;
	HRESULT result_change = UuidToStringW(&guid, &uuidString);
	std::wstring wstr{ reinterpret_cast<wchar_t*>(uuidString) };
	RpcStringFreeW(&uuidString);
	return wstr;
}

GUID dxe::Guid::ConvertGuid(const std::wstring& guid)
{
	UUID guid2;
	UuidFromStringW((RPC_WSTR)(guid.c_str()), reinterpret_cast<UUID*>(&guid2));
	Debug::log << guid2.Data1 << "\n";
	Debug::log << guid2.Data2 << "\n";
	Debug::log << guid2.Data3 << "\n";
	Debug::log << guid2.Data4 << "\n";
	return guid2;
}