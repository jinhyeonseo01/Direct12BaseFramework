#include "stdafx.h"
#include "engine_utility.h"
#include "IClone.h"
#include "EObject.h"


namespace dxe
{
    std::unordered_map<std::wstring, std::weak_ptr<EObject>> EObject::_EObjectTable{};
    std::unordered_map<std::wstring, std::wstring> EObject::_CloneGuidTable{};
    std::vector<std::shared_ptr<EObject>> EObject::_TempLifeCycle{};


    EObject::EObject()
    {
        this->SetGUID(dxe::Guid::GetNewGuid());
    }

    EObject::~EObject()
    {
        RemoveGuid(this->guid);
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
        auto prevGuid = this->guid;
        bool alreadyGuid = this->guid == L"" ? false : ContainsByGuid(this->guid);

        this->guid = str;
        if (alreadyGuid)
        {
            RemoveGuid(prevGuid);
            AddObject(this->shared_from_this());
        }
    }

    std::wstring EObject::GetGUID() const
    {
        return this->guid;
    }

    bool EObject::operator==(const EObject& other) const
    {
        return other.guid == this->guid;
    }

    bool EObject::operator<(const EObject& other) const
    {
        return other.guid < this->guid;
    }

    void* EObject::Clone()
    {
        return nullptr;
    }

    void EObject::ReRef()
    {
    }


    void EObject::ClearCloneTable()
    {
        _CloneGuidTable.clear();
        _TempLifeCycle.clear();
    }
}
