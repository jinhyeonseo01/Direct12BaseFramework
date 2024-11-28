#pragma once
#include "stdafx.h"
#include "Component.h"

namespace dxe
{
    class Transform : public Component
    {
    public:
        Transform();
        ~Transform() override;

    public:
        void* Clone() override;
        void ReRef() override;

    protected:
        Vector3 _prevLocalPosition;
        Vector3 _prevLocalScale;
        Quaternion _prevLocalRotation;

    public:
        Vector3 localPosition;
        Vector3 localScale;
        Quaternion localRotation;

        Vector3 _forward;
        Vector3 _up;
        Vector3 _right;

        Vector3 forward(const Vector3& dir = Vector3::Zero);
        Vector3 up(const Vector3& dir = Vector3::Zero);
        Vector3 right(const Vector3& dir = Vector3::Zero);

        Vector3 localEuler();
        const Vector3& localEuler(const Vector3& euler);

        Vector3 worldPosition();
        const Vector3& worldPosition(const Vector3& worldPos);
        Vector3 worldScale();
        const Vector3& worldScale(const Vector3& worldScale);
        Quaternion worldRotation();
        const Quaternion& worldRotation(const Quaternion& quaternion);

        Matrix _prevLocalSRTMatrix = Matrix::Identity;
        Matrix localSRTMatrix = Matrix::Identity; // prev랑 비교후 갱신/ 갱신시 islocal머시기 true 아니면 false
        Matrix localToWorldMatrix = Matrix::Identity;
        bool GetLocalToWorldMatrix(Matrix& localToWorldMatrix);
        bool GetLocalToWorldMatrix_BottomUp(Matrix& localToWorld);
        bool GetLocalSRTMatrix(Matrix& localSRT);
        bool SetLocalSRTMatrix(Matrix& localSRT);
        bool CheckLocalSRTUpdate() const;
        bool CheckLocalMatrixUpdate() const;
        bool CheckLocalToWorldMatrixUpdate() const;
        void TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate = false);
        bool BottomUpLocalToWorldUpdate();

        void LookUp(const Vector3& dir, const Vector3& up);

        bool isLocalSRTChanged = true; //이거 활성화시 시 월드매트릭스 갱신.isLocalToWorldChanged 이거 활성화
        bool isLocalToWorldChanged = true; //부모가 local 업데이트 or 부모 world 변경시 이거 true.worldtrs변경.
        bool needLocalToWorldChanged = true; // 부모가 업데이트 됬을때 내가 변경되어야함을 표기, 위에꺼랑은 역할이 조금 다른게. 위에껀 자기 기준이라, 전 프레임이랑 같으면 바뀌는데, 이건 내가 바뀌기 전까지 안꺼짐

        Vector3 LocalToWorld_Position(const Vector3& value);
        Vector3 LocalToWorld_Direction(const Vector3& value);
        Quaternion LocalToWorld_Quaternion(const Quaternion& value);

        Vector3 WorldToLocal_Position(const Vector3& value);
        Vector3 WorldToLocal_Direction(const Vector3& value);
        Quaternion WorldToLocal_Quaternion(const Quaternion& value);
        void Start() override;
        void Update() override;
        void LateUpdate() override;
        void OnEnable() override;
        void OnDisable() override;
        void OnDestroy() override;
        void Init() override;
        void OnComponentDestroy() override;
        void BeforeRendering() override;
        void Rendering(const RenderPacket& renderPack) override;
        void AfterRendering() override;
    };
}
