#pragma once
#include "IDelayedDestroy.h"

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


	public:
		__int64 _sortingOrder = 0;
		std::weak_ptr<Scene> scene;
		std::wstring name = L"none";

		std::vector<std::shared_ptr<Component>> _components;

		//std::shared_ptr transform; // 이건 위 리스트와 중복 참조.
		//GetComponent()
		//GetComponents
		//GetComponentsWithChild

		template<class T, class = class std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::shared_ptr<T> AddComponent()
		{
			auto component = std::make_shared<T>();
			_components.push_back(component);
			return component;
		}

		template<class T, class = class std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::shared_ptr<T> AddComponent(std::shared_ptr<T> component)
		{
			auto iter = std::ranges::find(_components, component);
			if(iter == this->_components.end())
			{
				this->_components.push_back(component);
			}
			else
			{
				return std::shared_ptr<T>(nullptr);
			}
			return component;
		}

		template<class T, class = class std::enable_if_t<std::is_convertible_v<T*, Component*>>>
		std::vector<std::shared_ptr<Component>>::iterator RemoveComponent(std::shared_ptr<T> component)
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

	private:
		void Destroy() override;

	public:
		void* Clone() const override;
		void ReRef() const override;
		
		std::weak_ptr<GameObject> parent;
		std::weak_ptr<GameObject> rootParent;
		std::vector<std::weak_ptr<GameObject>> _childs;
		
		//GetChild(index)
		//GetChildByName(index)
		//GetChilds(vector)
		//GetChildsByName(index)
		//GetChildsAll(vector)
		//GetChildsAllByName(index)

		bool AddChild(std::shared_ptr<GameObject> obj); // 순환참조 검사 해야함. root parent 가져오는 코드 필요할듯.
		bool RemoveChild(std::shared_ptr<GameObject> obj);
		bool RemoveAtChild(int index);
		bool SetParent(std::shared_ptr<GameObject> obj);

		//GetParent
		//GetRootParent

	public:
		//static Find
		//static Find
		//static Finds
	public:

		//bool _active_self
		//bool _active_total
		//GetActive(_active_total)
		//GetActiveSelf(_active_self)
		//SetActiveSelf(_active_self)
	};

}

