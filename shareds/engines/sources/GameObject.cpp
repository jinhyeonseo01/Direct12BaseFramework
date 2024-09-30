#include <stdafx.h>

#include <DXEngine.h>


GameObject::GameObject()
{

}

GameObject::GameObject(const GameObject& eObject)
{

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

GameObject::~GameObject()
{

}
