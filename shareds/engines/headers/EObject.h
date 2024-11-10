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
        static std::vector<std::shared_ptr<EObject>> _TempLifeCycle;
		
		//template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::shaderType>
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
		//template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::shaderType>
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
		static bool ContainsByGuid(const std::wstring& guid)
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
		static std::shared_ptr<T> FindObjectByGuid(const std::wstring& guid)
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

		template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
		static bool AddClone(const std::shared_ptr<T>& originalObj, std::shared_ptr<T>& cloneObj)
		{
            auto originalEObject = std::dynamic_pointer_cast<EObject>(originalObj);
            auto cloneEObject = std::dynamic_pointer_cast<EObject>(cloneObj);
			if (originalEObject != cloneEObject) {
				if (!_CloneGuidTable.contains(cloneObj->guid)) {
					_CloneGuidTable.insert(std::make_pair(originalObj->guid, cloneObj->guid));

                    _TempLifeCycle.push_back(originalEObject);
                    _TempLifeCycle.push_back(cloneEObject);
                    return true;
				}
			}
            return false;
		}

        static void ClearCloneTable();

        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static std::shared_ptr<T> FindCloneByGuid(const std::wstring& guid)
        {
            if (!_CloneGuidTable.contains(guid))
                return nullptr;
		    const std::wstring& cloneGuid = _CloneGuidTable[guid];
            if (_EObjectTable.contains(cloneGuid))
            {
                auto current = _EObjectTable[cloneGuid];
                if (!current.expired())
                {
                    auto ptr = std::dynamic_pointer_cast<T>(current.lock());
                    return ptr;
                }
                else
                {
                    _EObjectTable.erase(cloneGuid);
                }
            }
            return nullptr;
        }

        /// <summary>
        /// 재참조 처리를 수행할 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static bool ReRefChain(std::weak_ptr<T>& other)
        {
            if (other.lock() && ContainsByGuid(other.lock()->GetGUID()) && (FindCloneByGuid<T>(other.lock()->GetGUID()) == nullptr)) {
                other.lock()->ReRef();
                return true;
            }
            return false;
        }
        /// <summary>
        /// 재참조 처리를 수행할 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static bool ReRefChain(std::shared_ptr<T>& other)
        {
            if (ContainsByGuid(other->GetGUID()) && FindCloneByGuid<T>(other->GetGUID()) == nullptr) {
                other->ReRef();
                return true;
            }
            return false;
        }
        /// <summary>
        /// 재참조 처리를 수행할 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static void ReRefChain(std::vector<std::weak_ptr<T>>& others)
        {
            for (auto& other : others)
                ReRefChain(other);
        }
        /// <summary>
        /// 재참조 처리를 수행할 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static void ReRefChain(std::vector<std::shared_ptr<T>>& others)
        {
            for (auto& other : others)
                ReRefChain(other);
        }
        /// <summary>
        /// 깊은 복사 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static bool CloneChain(std::weak_ptr<T>& other)
		{
            if (other.lock() && (ContainsByGuid(other.lock()->GetGUID()) && FindCloneByGuid<T>(other.lock()->GetGUID()) == nullptr)) {
                other.lock()->Clone();
                return true;
            }
            return false;
		}
        /// <summary>
        /// 깊은 복사 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static bool CloneChain(std::shared_ptr<T>& other)
        {
            if (ContainsByGuid(other->GetGUID()) && FindCloneByGuid<T>(other->GetGUID()) == nullptr) {
                other->Clone();
                return true;
            }
            return false;
        }
        /// <summary>
        /// 깊은 복사 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static void CloneChain(std::vector<std::weak_ptr<T>>& others)
        {
            for (auto& other : others)
                CloneChain(other);
        }
        /// <summary>
        /// 깊은 복사 대상 추가
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static void CloneChain(std::vector<std::shared_ptr<T>>& others)
        {
            for (auto& other : others)
                CloneChain(other);
        }

        /// <summary>
        /// 재참조 처리 수행
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_base_of<EObject, T>::value>::type>
        static bool ChangePtrToClone(std::weak_ptr<T>& other)
        {
            std::shared_ptr<T> cloneObject;
            if (other.lock() && (ContainsByGuid(other.lock()->GetGUID()) && (cloneObject = FindCloneByGuid<T>(other.lock()->GetGUID())) != nullptr)) {
                other = cloneObject;
                return true;
            }
            return false;
        }
        /// <summary>
        /// 재참조 처리 수행
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static bool ChangePtrToClone(std::shared_ptr<T>& other)
        {
            std::shared_ptr<T> cloneObject;
            if (ContainsByGuid(other->GetGUID()) && (cloneObject = FindCloneByGuid<T>(other->GetGUID())) != nullptr) {
                other = cloneObject;
                return true;
            }
            return false;
        }
        /// <summary>
        /// 재참조 처리 수행
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static void ChangePtrToClone(std::vector<std::weak_ptr<T>>& others)
        {
            for (int i = 0; i < others.size(); i++)
            {
                auto& other = others[i];
                ChangePtrToClone(other);
            }
        }
        /// <summary>
        /// 재참조 처리 수행
        /// </summary>
        /// <typeparam names="T"></typeparam>
        /// <typeparam names=""></typeparam>
        /// <param names="others"></param>
        template<class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
        static void ChangePtrToClone(std::vector<std::shared_ptr<T>>& others)
        {
            for (int i = 0; i < others.size(); i++)
            {
                auto& other = others[i];
                ChangePtrToClone(other);
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

		virtual void* Clone() override;
		virtual void ReRef() override;

        template<class T, class = std::enable_if_t<std::is_base_of_v<EObject, T>>>
        std::shared_ptr<T> MakeInit()
        {
            AddObject(this->shared_from_this());
            return GetThis<T>();
        }
        template<class T, class = std::enable_if_t<std::is_base_of_v<EObject, T>>>
        std::shared_ptr<T> GetThis()
        {
            return std::dynamic_pointer_cast<T>(this->shared_from_this());
        }
	};
}

