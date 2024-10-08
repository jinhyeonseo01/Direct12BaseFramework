#include "IDelayedDestroy.h"

namespace dxe
{
	bool IDelayedDestroy::IsDestroy() const
	{
		return _destroy;
	}

	void IDelayedDestroy::Destroy()
	{
		_destroy = true;
	}

	IDelayedDestroy::~IDelayedDestroy() = default;


	IDelayedDestroy::IDelayedDestroy() : _destroy(false)
	{

	}
}
