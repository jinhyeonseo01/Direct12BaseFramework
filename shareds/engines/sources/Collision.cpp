#include "stdafx.h"
#include "Collision.h"

#include "Transform.h"


Collision::Collision()
{
    type = CollisionType::Box;
    SetBox(Vector3(0, 0, 0), Vector3(1, 1, 1));
}

Collision::Collision(CollisionType type)
{
    this->type = type;
}

Collision::~Collision()
{

}


void Collision::SetSphere(Vector3 center, float radius)
{
    sphereBound = BoundingSphere(center, radius);
}

void Collision::SetBox(Vector3 center, Vector3 size)
{
    boxBound = BoundingOrientedBox(center, size, Quaternion::Identity);
}

BoundingBox Collision::GetWorldBB(BoundingBox box, const Matrix& localToWorldMatrix)
{
    BoundingBox worldBox;
    box.Transform(worldBox, localToWorldMatrix);
    return worldBox;
}

BoundingOrientedBox Collision::GetWorldBB(BoundingOrientedBox box, const Matrix& localToWorldMatrix)
{
    BoundingOrientedBox worldBox;
    box.Transform(worldBox, localToWorldMatrix);
    return worldBox;
}

BoundingSphere Collision::GetWorldBB(BoundingSphere sphere, const Matrix& localToWorldMatrix)
{
    BoundingSphere worldBox;
    sphere.Transform(worldBox, localToWorldMatrix);
    return worldBox;
}

bool Collision::Intersects(std::shared_ptr<Transform> trans, std::shared_ptr<Transform> other, const Collision& collision)
{
    BoundingOrientedBox boxMy = boxBound;
    BoundingSphere sphereMy = sphereBound;
    BoundingOrientedBox boxOther = collision.boxBound;
    BoundingSphere sphereOther = collision.sphereBound;
    Matrix localToWorld = Matrix::Identity;
    Matrix localToWorldOther = Matrix::Identity;

    if (trans != nullptr)
        trans->GetLocalToWorldMatrix(localToWorld);
    if (other != nullptr)
        other->GetLocalToWorldMatrix(localToWorldOther);

    if (type == CollisionType::Box)
        boxMy = GetWorldBB(boxMy, localToWorld);
    if (type == CollisionType::Sphere)
        sphereMy = GetWorldBB(sphereMy, localToWorld);
    
    if (collision.type == CollisionType::Box)
        return boxMy.Intersects(GetWorldBB(boxOther, localToWorldOther));
    if (collision.type == CollisionType::Sphere)
        return sphereMy.Intersects(GetWorldBB(sphereOther, localToWorldOther));
}

bool Collision::Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay, float& dis, Vector3& normal)
{
    if (type == CollisionType::Box)
    {
        BoundingOrientedBox worldOBB;
        BoundingOrientedBox localOBB = boxBound;
        Matrix localToWorld = Matrix::Identity;
        if (trans != nullptr)
        {
            trans->GetLocalToWorldMatrix_BottomUp(localToWorld);
            localOBB.Transform(worldOBB, localToWorld);
        }
        bool hit = worldOBB.Intersects(worldRay.position, worldRay.direction, dis);
        if (hit && dis >= 0)
        {
            auto worldHitPoint = worldRay.position + worldRay.direction * dis;

            Matrix worldToLocal;
            localToWorld.Invert(worldToLocal);
            Matrix scaleMatrix = Matrix::CreateScale(localOBB.Extents);
            Matrix transMatrix = Matrix::CreateTranslation(localOBB.Center);
            worldToLocal = worldToLocal * ((scaleMatrix * transMatrix).Invert());

            auto localHitPoint = Vector3::Transform(worldHitPoint, worldToLocal);
            localHitPoint.Normalize(localHitPoint);

            XMVECTOR normals[6] = {
                XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), // +X
                XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), // -X
                XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), // +Y
                XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), // -Y
                XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), // +Z
                XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f) // -Z
            };

            float maxDot = -FLT_MAX;
            for (int i = 0; i < 6; ++i)
            {
                float dotProduct = localHitPoint.Dot(normals[i]);
                if (dotProduct > maxDot)
                {
                    maxDot = dotProduct;
                    normal = normals[i];
                }
            }
            normal.Normalize(normal);
            normal = Vector3::TransformNormal(normal, localToWorld);
        }

        return hit;
    }
    if (type == CollisionType::Sphere)
    {
        BoundingSphere worldOBB;
        BoundingSphere localOBB = sphereBound;
        Matrix localToWorld = Matrix::Identity;
        if (trans != nullptr)
        {
            trans->GetLocalToWorldMatrix_BottomUp(localToWorld);
            localOBB.Transform(worldOBB, localToWorld);
        }
        bool hit = worldOBB.Intersects(worldRay.position, worldRay.direction, dis);
        if (hit && dis >= 0)
        {
            auto worldHitPoint = worldRay.position + worldRay.direction * dis;
            Vector3 worldPos = worldOBB.Center;
            (worldHitPoint - worldPos).Normalize(normal);
        }
    }
    return false;
}

bool Collision::Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay)
{
    float dis = std::numeric_limits<float>::max();
    if (type == CollisionType::Box)
    {
        BoundingOrientedBox worldOBB;
        BoundingOrientedBox localOBB = boxBound;
        Matrix localToWorld = Matrix::Identity;
        if (trans != nullptr)
        {
            trans->GetLocalToWorldMatrix_BottomUp(localToWorld);
            localOBB.Transform(worldOBB, localToWorld);
        }
        return worldOBB.Intersects(worldRay.position, worldRay.direction, dis);
    }
    if (type == CollisionType::Sphere)
    {
        BoundingSphere worldOBB;
        BoundingSphere localOBB = sphereBound;
        Matrix localToWorld = Matrix::Identity;
        if (trans != nullptr)
        {
            trans->GetLocalToWorldMatrix_BottomUp(localToWorld);
            localOBB.Transform(worldOBB, localToWorld);
        }
        return worldOBB.Intersects(worldRay.position, worldRay.direction, dis);
    }
    return false;
}
