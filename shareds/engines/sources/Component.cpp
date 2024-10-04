#include <Component.h>


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

	void* Component::Clone() const
	{
		return EObject::Clone();
	}

	void Component::ReRef() const
	{
		EObject::ReRef();
	}
}
