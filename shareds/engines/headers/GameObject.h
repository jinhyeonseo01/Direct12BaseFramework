#pragma once
#include "IDelayedDestroy.h"
#include "Component.h"

namespace dxe
{
	class GameObject : public dxe::EObject, public IDelayedDestroy
	{
	public:
		GameObject();
		GameObject(std::wstring name);
		GameObject(const GameObject& eObject);
		GameObject(GameObject&& gameObject) noexcept;
		GameObject& operator=(const GameObject& gameObject);
		GameObject& operator=(GameObject&& gameObject) noexcept;
		
		bool operator<(const GameObject& other) const;

		virtual ~GameObject();

		std::shared_ptr<GameObject> Init();


	public:
		__int64 _sortingOrder = 0;
		std::weak_ptr<Scene> scene;
		std::wstring name = L"none";

		std::vector<std::shared_ptr<Component>> _components;
		std::shared_ptr<Transform> transform; // �̰� �� ����Ʈ�� �ߺ� ����.

		//template<class T, class = std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		//GetComponentsWithChilds(std::vector<std::shared_ptr<T>> vec)

		template<class T, class = std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::shared_ptr<T> GetComponent()
		{
			for (int i = 0; i < _components.size(); i++)
			{
				auto ptr = std::dynamic_pointer_cast<T>(_components[i]);
				if (ptr != nullptr)
					return ptr;
			}
			return nullptr;
		}
		template<class T, class = std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		int GetComponents(std::vector<std::shared_ptr<T>>& vec)
		{
			int count = 0;
			for(int i=0;i< _components.size();i++)
			{
				auto ptr = std::dynamic_pointer_cast<T>(_components[i]);
				if(ptr != nullptr)
				{
					vec.push_back(ptr);
					++count;
				}
			}
			return count;
		}

		template<class T, class = std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::shared_ptr<T> AddComponent()
		{
			auto component = std::make_shared<T>();
			auto componentCast = std::dynamic_pointer_cast<Component>(component);
			componentCast->gameObject = std::dynamic_pointer_cast<GameObject>(this->shared_from_this());
			_components.push_back(componentCast);
			return component;
		}

		template<class T, class = class std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component)
		{
			auto componentCast = std::dynamic_pointer_cast<Component>(component);
			auto iter = std::ranges::find(_components, componentCast);
			if(iter == this->_components.end())
			{
				componentCast->gameObject = std::dynamic_pointer_cast<GameObject>(this->shared_from_this());
				this->_components.push_back(componentCast);
			}
			else
			{
				return std::shared_ptr<T>(nullptr);
			}
			return component;
		}

		template<class T, class = class std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::vector<std::shared_ptr<Component>>::iterator RemoveComponent(const std::shared_ptr<T>& component)
		{
			auto iter = std::ranges::find(_components, component);
			if (iter != this->_components.end())
			{
				auto next = this->_components.erase(iter);
				return next;
			}
			
			return iter;
		}
		bool RemoveAtComponent(int index);

	public:
		void Destroy() override;

	public:
		void* Clone() const override;
		void ReRef() const override;

		std::weak_ptr<GameObject> parent;
		std::weak_ptr<GameObject> rootParent;
		std::vector<std::weak_ptr<GameObject>> _childs;
		
		std::shared_ptr<GameObject> GetChild(int index);
		std::shared_ptr<GameObject> GetChildByName(const std::wstring& name);
		int GetChilds(std::vector<std::shared_ptr<GameObject>>& vec);
		int GetChildsByName(std::vector<std::shared_ptr<GameObject>>& vec, const std::wstring& name);
		int GetChildsAll(std::vector<std::shared_ptr<GameObject>>& vec);
		int GetChildsAllByName(std::vector<std::shared_ptr<GameObject>>& vec, const std::wstring& name);

		//bool AddChild(std::shared_ptr<GameObject> obj); // ��ȯ���� �˻� �ؾ���. root parent �������� �ڵ� �ʿ��ҵ�.
		bool AddChild(const std::shared_ptr<GameObject>& obj);
		bool RemoveChild(const std::shared_ptr<GameObject>& obj);
		bool RemoveAtChild(int index);
		bool SetParent(const std::shared_ptr<GameObject>& nextParentObj);

		bool IsInParent(const std::shared_ptr<GameObject>& obj);

	public:

		bool _active_self_prev = false;
		bool _active_self = true;
		bool _active_total_prev = true;
		bool _active_total = true;

		bool GetActive();//_active_total
		bool GetActiveSelf(); // _active_self
		bool SetActiveSelf(bool _active); //_active_self
	protected:
		void ActiveUpdateChain(bool _active_total);
	public:
		void Debug(int depth = 0);
	};

}

