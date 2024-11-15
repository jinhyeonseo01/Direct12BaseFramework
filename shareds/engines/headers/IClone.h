#pragma once


namespace dxe
{
    class IClone
    {
    public:
        virtual void* Clone() = 0;
        virtual void ReRef() = 0;

        // ���� �Ҹ��� (�ʼ��� �ƴ����� �����)
        virtual ~IClone() = default;
    };
}
