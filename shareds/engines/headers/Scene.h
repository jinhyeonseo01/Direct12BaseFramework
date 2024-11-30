#pragma once
#include <stdafx.h>

#include "CBuffer_struct.h"
#include "Collider.h"
#include "EObject.h"
#include "Model.h"
#include "RenderPacket.h"

namespace dxe
{
    class Scene : public EObject
    {
    public:
        Scene();
        Scene(std::wstring name);
        Scene(const Scene& scene);
        Scene(Scene&& scene) noexcept;
        Scene& operator=(const Scene& scene);
        Scene& operator=(Scene&& scene) noexcept;
        ~Scene() override;

    public:
        void SetName(std::wstring name);
        void Reset(std::shared_ptr<Scene> prevScene);
        virtual void Init();
        virtual void Update();
        virtual void RenderingBegin();
        virtual void RenderingEnd();

    public:
        std::wstring name;

        std::vector<std::shared_ptr<GameObject>> _gameObjectList;
        std::vector<std::shared_ptr<GameObject>> _destroyObject;

        std::vector<RenderPacket> _renderPacketList;

        CameraParams _cameraParams;

        void AddRenderPacket(const RenderPacket& renderPacket);

        static void LoadJsonObject(Scene& scene, std::wstring json);

    public:
        std::shared_ptr<GameObject> CreateGameObject();
        std::shared_ptr<GameObject> CreateGameObject(std::wstring name);
        std::shared_ptr<GameObject> CreateGameObjects(const std::shared_ptr<Model>& model, ModelNode* node = nullptr);

        bool AddGameObject(std::shared_ptr<GameObject> gameObject);
        bool RemoveGameObject(std::shared_ptr<GameObject> gameObject);
        bool RemoveAtGameObject(int index);

        std::shared_ptr<GameObject> Find(std::wstring name, bool includeDestroy = false);
        int Finds(std::wstring name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy = false);

    public:
        std::vector<std::weak_ptr<Collider>> colliderTable;
        bool RayCast(const Vector3& worldPos, const Vector3& worldDirection, float& hitDistance, Vector3& hitNormal);

    public:
        void Debug();
    };
}
