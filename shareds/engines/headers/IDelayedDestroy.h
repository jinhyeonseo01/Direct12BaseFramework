#pragma once

namespace dxe
{
	class IDelayedDestroy
	{
	public:
		bool _destroy = false;

		bool IsDestroy() const;
		virtual void Destroy();

		// ���� �Ҹ��� (�ʼ��� �ƴ����� �����)
		IDelayedDestroy();
		virtual ~IDelayedDestroy();
	};

}
