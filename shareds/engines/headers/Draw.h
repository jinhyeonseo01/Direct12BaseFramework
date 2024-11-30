#pragma once
#include <stdafx.h>


namespace dxe
{
    class Draw
    {
    public:
        static void Release();
        static void Ray(const Ray& ray, const Color& color = Color(0,0,0, 1));
        static void Line(const Vector3& start, const Vector3& end, const Color& color = Color(0, 0, 0, 1));
    };
}

