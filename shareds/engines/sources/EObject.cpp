
#include "engine_utility.h"
#include "IClone.h"
#include "EObject.h"


namespace dxe
{
	std::unordered_map<std::wstring, std::weak_ptr<EObject>> EObject::_EObjectTable(8192);
	std::unordered_map<std::wstring, std::wstring> EObject::_CloneGuidTable(8192);

	EObject::EObject()
	{
		this->SetGUID(dxe::Guid::GetNewGuid());
	}
	EObject::~EObject()
	{
		
	}

	EObject::EObject(const std::wstring& guid)
	{
		this->SetGUID(guid);
	}

	EObject::EObject(const EObject& eObject)
	{
		this->SetGUID(eObject.GetGUID());
	}

	EObject::EObject(EObject&& eObject) noexcept
	{
		this->SetGUID(std::move(eObject.guid));
	}

	EObject& EObject::operator=(const EObject& eObject)
	{
		if (this == &eObject)
			return *this;

		this->SetGUID(eObject.GetGUID());

		return *this;
	}

	EObject& EObject::operator=(EObject&& eObject) noexcept
	{
		if (this == &eObject)
			return *this;
		
		this->SetGUID(std::move(eObject.guid));

		return *this;
	}


	void EObject::SetGUID(const std::wstring& str)
	{
		this->guid = dxe::Guid::GetNewGuid();
	}

	std::wstring EObject::GetGUID() const
	{
		return this->guid;
	}
}
bool EObject::operator==(const EObject& other)
{
	return other.guid == this->guid;
}

void* EObject::Clone() const
{
	return nullptr;
}

void EObject::ReRef() const
{
	
}