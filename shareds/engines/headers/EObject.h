#pragma once

#include <stdafx.h>
#include <IClone.h>

#include <IType.h>

namespace dxe
{
	class EObject : public std::enable_shared_from_this<EObject>, public dxe::IClone, public IType
	{
	public:
		static std::unordered_map<std::wstring, std::weak_ptr<EObject>> _EObjectTable;
		static std::unordered_map<std::wstring, std::wstring> _CloneGuidTable;
		
		//template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		template<class T, class = std::enable_if_t<std::is_convertible_v<T*, EObject*>>>
		static bool AddObject(std::shared_ptr<T> object)
		{
			if (object == nullptr)
				return false;
			if (!_EObjectTable.contains(object->guid))
			{
				_EObjectTable.insert(std::make_pair(object->guid, std::weak_ptr<EObject>{ std::dynamic_pointer_cast<EObject>(object) }));
				return true;
			}
			return false;
		}
		//template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		template<class T, class = std::enable_if_t<std::is_convertible_v<T*, EObject*>>>
		static bool RemoveObject(std::shared_ptr<T> object)
		{
			if (object == nullptr)
				return false;
			if (_EObjectTable.contains(object->guid))
			{
				_EObjectTable.erase(object->guid);
				return true;
			}
			return false;
		}
		static bool RemoveGuid(const std::wstring& guid)
		{
			auto iter = _EObjectTable.find(guid);
			if (iter != _EObjectTable.end())
			{
				_EObjectTable.erase(iter);
				return true;
			}
			return false;
		}
		static bool ContainsByGuid(std::wstring& guid)
		{
			if (_EObjectTable.contains(guid))
			{
				if (_EObjectTable[guid].expired())
				{
					_EObjectTable.erase(guid);
					return false;
				}
				return true;
			}
			return false;
		}
		template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		static std::shared_ptr<T> FindObjectByType()
		{
			for (auto it = _EObjectTable.begin(); it != _EObjectTable.end();)
			{
				auto current = *it;
				if (!current.second.expired())
				{
					auto ptr = std::dynamic_pointer_cast<T>(current.second.lock());
					if (ptr != nullptr)
						return ptr;
					++it;
				}
				else
					it = _EObjectTable.erase(it);
			}
			return nullptr;
		}
		template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		static bool FindObjectsByType(std::vector<std::shared_ptr<T>>& vec)
		{
			bool tf = false;
			for (auto it = _EObjectTable.begin(); it != _EObjectTable.end();)
			{
				auto current = *it;
				if (!current.second.expired())
				{
					auto ptr = std::dynamic_pointer_cast<T>(current.second.lock());
					if (ptr != nullptr)
					{
						vec.push_back(ptr);
						tf = true;
					}
					++it;
				}
				else
					it = _EObjectTable.erase(it);
			}
			return tf;
		}
		template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		static std::shared_ptr<T> FindObjectByGuid(std::wstring& guid)
		{
			if (_EObjectTable.contains(guid))
			{
				auto current = _EObjectTable[guid];
				if (!current.expired())
				{
					auto ptr = std::dynamic_pointer_cast<T>(current.lock());
					return ptr;
				}
				else
				{
					_EObjectTable.erase(guid);
				}
			}
			return nullptr;
		}

		static void ClearCloneTable()
		{
			_CloneGuidTable.clear();
		}
		template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		static void AddClone(const T& obj, T& cloneObj)
		{
			if ((EObject)obj != (EObject)cloneObj)
			{
				AddObject(cloneObj);
				if (_CloneGuidTable.contains(cloneObj->guid))
				{
					_CloneGuidTable.insert(std::make_pair(obj->guid, cloneObj->guid));
				}
			}
		}

	public:
		std::wstring guid = L"";
		std::wstring type = L"";

		EObject();
		EObject(const std::wstring& guid);
		EObject(const EObject& eObject);
		EObject(EObject&& eObject) noexcept;
		EObject& operator=(const EObject& eObject);
		EObject& operator=(EObject&& eObject) noexcept;
		bool operator==(const EObject& other) const;
		bool operator<(const EObject& other) const;

		virtual ~EObject();

		void SetGUID(const std::wstring& str);
		std::wstring GetGUID() const;

		virtual void* Clone() const override;
		virtual void ReRef() const override;
	};
}

