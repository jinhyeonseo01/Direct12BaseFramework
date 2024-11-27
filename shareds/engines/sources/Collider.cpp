#include "stdafx.h"
#include "Collider.h"



BoundingBox Collider::GetAABBBound()
{
    DirectX::XMFLOAT3 corners[8];
    aabbBox.GetCorners(corners);

    BoundingOrientedBox oriented_box;

    switch (type)
    {
    case ColliderType::Box:
    {
        DirectX::XMFLOAT3 minPoint = corners[0];
        DirectX::XMFLOAT3 maxPoint = corners[0];

        for (int i = 1; i < 8; ++i) {
            minPoint.x = std::min(minPoint.x, corners[i].x);
            minPoint.y = std::min(minPoint.y, corners[i].y);
            minPoint.z = std::min(minPoint.z, corners[i].z);

            maxPoint.x = std::max(maxPoint.x, corners[i].x);
            maxPoint.y = std::max(maxPoint.y, corners[i].y);
            maxPoint.z = std::max(maxPoint.z, corners[i].z);
        }

        aabbBox.Center = DirectX::XMFLOAT3(
            (minPoint.x + maxPoint.x) / 2,
            (minPoint.y + maxPoint.y) / 2,
            (minPoint.z + maxPoint.z) / 2
        );
        aabbBox.Extents = DirectX::XMFLOAT3(
            (maxPoint.x - minPoint.x) / 2,
            (maxPoint.y - minPoint.y) / 2,
            (maxPoint.z - minPoint.z) / 2
        );
        break;
    }
    case ColliderType::Sphere:
    {
        break;
    }
    }

    return aabbBox;
}

Collider::~Collider()
{
}

void* Collider::Clone()
{
    return Component::Clone();
}

void Collider::ReRef()
{
    Component::ReRef();
}

void Collider::Destroy()
{
    Component::Destroy();
}

void Collider::Init()
{
    Component::Init();
    //aabbBox.GetCorners()
}

void Collider::Start()
{
    Component::Start();
}

void Collider::Update()
{
    Component::Update();
}

void Collider::LateUpdate()
{
    Component::LateUpdate();
}

void Collider::OnEnable()
{
    Component::OnEnable();
}

void Collider::OnDisable()
{
    Component::OnDisable();
}

void Collider::OnDestroy()
{
    Component::OnDestroy();
}

void Collider::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void Collider::BeforeRendering()
{
    Component::BeforeRendering();
}

void Collider::Rendering(const RenderPacket& renderPack)
{
    Component::Rendering(renderPack);
}

void Collider::AfterRendering()
{
    Component::AfterRendering();
}
