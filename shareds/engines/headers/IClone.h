#pragma once


namespace dxe
{
    class IClone
    {
    public:
        virtual void* Clone() = 0;
        virtual void ReRef() = 0;

        // 가상 소멸자 (필수는 아니지만 권장됨)
        virtual ~IClone() = default;
    };
}
