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

		Matrix localTrsMatrix = Matrix::Identity; // prev�� ���� ����/ ���Ž� islocal�ӽñ� true �ƴϸ� false
		Matrix worldTrsMatrix = Matrix::Identity;
		Matrix& GetLocalToWorldMatrix();
		Matrix& GetLocalSRTMatrix();
		bool islocaltrschanged = true; //�̰� Ȱ��ȭ�� �� �����Ʈ���� ����.isworldtrschanged �̰� Ȱ��ȭ
		bool isworldtrschanged = true; //�θ� local ������Ʈ or �θ� world ����� �̰� true.worldtrs����.
	};
}



// localTrsMatrix // prev�� ���� ����/ ���Ž� islocal�ӽñ� true �ƴϸ� false
// worldTrsMatrix
// bool islocaltrschanged �̰� Ȱ��ȭ�� �� �����Ʈ���� ����. isworldtrschanged �̰� Ȱ��ȭ
// bool isworldtrschanged �θ� local ������Ʈ or �θ� world ����� �̰� true. worldtrs����.


// func LocalToWorld_Position
// func LocalToWorld_Direction

// getForward
// getRight
// getUp

// setForward Right�� Cross�� Up ����
// setRight Forward�� Cross�� Up ����
// setUp Farward�� Cross

// LookAt(target, upvector)