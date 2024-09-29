#pragma once

namespace dxe
{
	class GameObject : public dxe::EObject, public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject();
		GameObject(const GameObject& eObject);
		GameObject(GameObject&& gameObject) noexcept;
		GameObject& operator=(const GameObject& gameObject);
		GameObject& operator=(GameObject&& gameObject) noexcept;

		virtual ~GameObject();


	public:
		//std::vector<Component> _components(8);
		//std::shared_ptr transform; // 이건 위 리스트와 중복 참조.
		//GetComponent
		//GetComponents
		//GetComponentsWithChild
		//AddComponent
		//RemoveComponent
		//RemoveAtComponent(index)
	public:
		//weak.parent = nullptr
		//weak.rootparent = nullptr
		//std::vector<weak.GameObject> _childs(16);
		
		//GetChild(index)
		//GetChildByName(index)
		//GetChilds(vector)
		//GetChildsByName(index)
		//GetChildsAll(vector)
		//GetChildsAllByName(index)

		//AddChild(index) // 순환참조 검사 해야함. root parent 가져오는 코드 필요할듯.
		//RemoveChild(index)

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

