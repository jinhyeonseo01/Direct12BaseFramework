#include <stdafx.h>

#include <DXEngine.h>


GameObject::GameObject()
{
	this->name = L"obj_Nameless";
}

GameObject::GameObject(std::wstring name)
{
	this->name = name;
}

GameObject::GameObject(const GameObject& eObject)
{
	this->name = eObject.name;
	this->_sortingOrder = eObject._sortingOrder;
}

GameObject::GameObject(GameObject&& gameObject) noexcept
{

}

GameObject& GameObject::operator=(const GameObject& gameObject)
{
	if (this == &gameObject)
		return *this;
	return *this;
}

GameObject& GameObject::operator=(GameObject&& gameObject) noexcept
{
	if (this == &gameObject)
		return *this;
	return *this;
}

bool dxe::GameObject::operator<(const GameObject& other) const
{
	return this->_sortingOrder < other._sortingOrder;
}

GameObject::~GameObject()
{

}

bool GameObject::RemoveAtComponent(int index)
{
	if (index >= 0 && index < this->_components.size())
	{
		this->_components.erase(this->_components.begin() + index);
	}
	return false;
}

void dxe::GameObject::Destroy()
{
	_destroy = true;
}

void* GameObject::Clone() const
{
	return EObject::Clone();
}

void GameObject::ReRef() const
{
	EObject::ReRef();
}

bool GameObject::AddChild(std::shared_ptr<GameObject> obj)
{
	auto thisObj = std::dynamic_pointer_cast<GameObject>(shared_from_this());
	
	if ((!this->parent.expired()) && this->parent.lock() != nullptr)
	{
		auto prevParent = obj->parent.lock();
		if (prevParent == thisObj)
			return false;
		prevParent->RemoveChild(obj);
		obj->parent = thisObj;
		if (this->rootParent.lock() == nullptr || this->rootParent.expired())
			obj->rootParent = thisObj;
		return true;
	}
	//2024-10-04
}

bool GameObject::RemoveChild(std::shared_ptr<GameObject> obj)
{
}

bool GameObject::RemoveAtChild(int index)
{
}

bool GameObject::SetParent(std::shared_ptr<GameObject> obj)
{
}
