#pragma once

namespace dxe
{
	class IDelayedDestroy
	{
	public:
		bool _destroy = false;

		bool IsDestroy() const;
		virtual void Destroy();

		// 가상 소멸자 (필수는 아니지만 권장됨)
		IDelayedDestroy();
		virtual ~IDelayedDestroy();
	};

}
