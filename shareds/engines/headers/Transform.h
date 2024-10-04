#pragma once
#include "stdafx.h"
#include "Component.h"

namespace dxe
{
	class Transform : public dxe::Component
	{
	public:
		Transform();
		virtual ~Transform();
	public:
		void* Clone() const override;
		void ReRef() const override;
	public:
		Vector3 localPosition;
		Vector3 localScale;
		Vector3 _localEulerAngles;
		Quaternion _localRotation;

		Vector3 _prevlocalPosition;
		Vector3 _prevlocalScale;
		Vector3 _prevLocalEulerAngles;

		Vector3 localEuler();
		void localEuler(const Vector3& euler);
		Quaternion localRotation();
		void localRotation(const Quaternion& quat);

		Vector3 worldPosition();
		Vector3 worldScale();
		Vector3 worldRotation();

		Matrix localTrsMatrix = Matrix::Identity; // prev랑 비교후 갱신/ 갱신시 islocal머시기 true 아니면 false
		Matrix worldTrsMatrix = Matrix::Identity;
		Matrix& GetLocalToWorldMatrix();
		Matrix& GetLocalSRTMatrix();
		bool islocaltrschanged = true; //이거 활성화시 시 월드매트릭스 갱신.isworldtrschanged 이거 활성화
		bool isworldtrschanged = true; //부모가 local 업데이트 or 부모 world 변경시 이거 true.worldtrs변경.
	};
}



// localTrsMatrix // prev랑 비교후 갱신/ 갱신시 islocal머시기 true 아니면 false
// worldTrsMatrix
// bool islocaltrschanged 이거 활성화시 시 월드매트릭스 갱신. isworldtrschanged 이거 활성화
// bool isworldtrschanged 부모가 local 업데이트 or 부모 world 변경시 이거 true. worldtrs변경.


// func LocalToWorld_Position
// func LocalToWorld_Direction

// getForward
// getRight
// getUp

// setForward Right랑 Cross후 Up 갱신
// setRight Forward랑 Cross후 Up 갱신
// setUp Farward랑 Cross

// LookAt(target, upvector)