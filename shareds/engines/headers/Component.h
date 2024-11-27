#pragma once
#include "stdafx.h"
#include "EObject.h"
#include "IDelayedDestroy.h"
#include "RenderPacket.h"

namespace dxe
{
    class Component : public EObject, public IDelayedDestroy
    {
    public:
        Component();
        ~Component() override;
        Component(const Component& component);
        Component(Component&& component) noexcept;
        Component& operator=(const Component& component);
        Component& operator=(Component&& component) noexcept;
        bool operator<(const Component& other) const;
        bool operator==(const Component& other) const;

    public:
        void* Clone() override;
        void ReRef() override;

        std::weak_ptr<GameObject> gameObject;

    protected:
        bool _first = true;

    public:
        bool IsFirst();
        void FirstDisable();
        void Destroy() override;

        //Init <- �̰� Component�� GameObject�� ó�� �Ҽӵɶ�.
        // 
        virtual void Init();
        virtual void Start();
        virtual void Update();
        virtual void LateUpdate();
        virtual void OnEnable();
        virtual void OnDisable();
        virtual void OnDestroy();
        virtual void OnComponentDestroy();

        //Remove Cycle

        virtual void BeforeRendering();
        virtual void Rendering(const RenderPacket& renderPack);
        virtual void AfterRendering();

        //------------------
        //OnCollision(GameObject& other) // �ƴϸ� �ݸ�������

        //------------------
        //gameobject weak
        //transform
    };
}
