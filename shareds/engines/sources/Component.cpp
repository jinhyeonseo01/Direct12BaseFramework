#include "stdafx.h"
#include <Component.h>
#include <EObject.h>
#include <GameObject.h>


namespace dxe
{

	Component::Component()
	{

	}
	Component::~Component()
	{

	}

	Component::Component(const Component& component)
	{
		this->gameObject = component.gameObject;
	}

	Component::Component(Component&& component) noexcept
	{
		this->gameObject = component.gameObject;
	}

	Component& Component::operator=(const Component& component)
	{
		if (this == &component)
			return *this;
		this->gameObject = component.gameObject;
		return *this;
	}

	Component& Component::operator=(Component&& component) noexcept
	{
		if (this == &component)
			return *this;

		this->gameObject = component.gameObject;

		return *this;
	}

	bool Component::operator<(const Component& other) const
	{
		return EObject::operator<(other);
	}

	bool Component::operator==(const Component& other) const
	{
		return EObject::operator==(other);
	}

	void* Component::Clone()
    {
		return EObject::Clone();
	}

	void Component::ReRef()
    {
		EObject::ReRef();
        ChangePtrToClone(gameObject);
	}

	bool Component::IsFirst()
	{
		return _first;
	}

	void Component::FirstDisable()
	{
		_first = false;
	}

	void Component::Destroy()
	{
		IDelayedDestroy::Destroy();
	}

	void Component::Init()
	{
	}

	void Component::Start()
	{
	}

	void Component::Update()
	{
	}

	void Component::LateUpdate()
	{
	}

	void Component::OnEnable()
	{
	}

	void Component::OnDisable()
	{
	}

	void Component::OnDestroy()
	{
	}

	void Component::OnComponentDestroy()
	{
		
	}

    void Component::BeforeRendering()
    {
    }

    void Component::Rendering()
    {
    }

    void Component::AfterRendering()
    {
    }
}
