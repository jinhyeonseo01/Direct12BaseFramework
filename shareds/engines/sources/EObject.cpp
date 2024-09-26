#include "EObject.h"
#include "engine_utility.h"

namespace dxe
{
	EObject::EObject()
	{
		this->SetGUID(dxe::Guid::GetNewGuid());
	}

	EObject::EObject(std::wstring guid)
	{
		this->SetGUID(guid);
	}

	std::wstring EObject::SetGUID(std::wstring str)
	{

	}
}