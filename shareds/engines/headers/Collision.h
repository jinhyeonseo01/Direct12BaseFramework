#pragma once


namespace dxe
{
    enum class CollisionType
    {
        Box,
        Sphere,
        Capsule,
        Mesh,
    };

    class Collision
    {
    public:
        Collision();
        Collision(CollisionType type);
        virtual ~Collision();

        CollisionType type = CollisionType::Box;
        BoundingBox _aabb;

        BoundingSphere sphereBound;
        BoundingOrientedBox boxBound;

        void SetSphere(Vector3 center, float radius);
        void SetBox(Vector3 center, Vector3 size);


        BoundingBox GetWorldBB(BoundingBox box, const Matrix& localToWorldMatrix = Matrix::Identity);
        BoundingOrientedBox GetWorldBB(BoundingOrientedBox box, const Matrix& localToWorldMatrix = Matrix::Identity);
        BoundingSphere GetWorldBB(BoundingSphere sphere, const Matrix& localToWorldMatrix = Matrix::Identity);

        bool Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay, float& dis, Vector3& normal);
        bool Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay);
        bool Intersects(std::shared_ptr<Transform> trans, std::shared_ptr<Transform> other, const Collision& collision);

    };

}
