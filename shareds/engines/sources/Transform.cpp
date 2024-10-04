#include "stdafx.h"
#include "Transform.h"

void* Transform::Clone() const
{
	return Component::Clone();
}

void Transform::ReRef() const
{
	Component::ReRef();
}


Vector3 Transform::localEuler()
{
	return _localEulerAngles;
}

void Transform::localEuler(const Vector3& euler)
{
	_localEulerAngles = euler;
	_localRotation = Quaternion::CreateFromYawPitchRoll(euler);
}

Quaternion Transform::localRotation()
{
	_localRotation = Quaternion::CreateFromYawPitchRoll(_localEulerAngles);
	return _localRotation;
}

void Transform::localRotation(const Quaternion& quat)
{
	_localRotation = quat;
	ToEuler(_localRotation, _localEulerAngles);
}

Vector3 Transform::worldPosition()
{
	return Vector3::Zero;
}

Vector3 Transform::worldScale()
{
	return Vector3::Zero;
}

Vector3 Transform::worldRotation()
{
	return Vector3::Zero;
}

Matrix& Transform::GetLocalToWorldMatrix()
{
	Transform::GetLocalSRTMatrix();
	if(islocaltrschanged)
	{
		if (!gameObject.expired())
		{
			gameObject.lock()->
		}
	}

	return localTrsMatrix;
}

Matrix& Transform::GetLocalSRTMatrix()
{
	bool reCalc = false;
	if (_prevlocalPosition != localPosition)
	{
		_prevlocalPosition = localPosition;
		reCalc = true;
	}
	if (_prevLocalEulerAngles != _localEulerAngles)
	{
		_prevLocalEulerAngles = _localEulerAngles;
		reCalc = true;
	}
	if (_prevlocalScale != localScale)
	{
		_prevlocalScale = localScale;
		reCalc = true;
	}

	if (reCalc)
	{
		Matrix matScale = Matrix::CreateScale(localScale);
		Matrix matRotation = Matrix::CreateFromQuaternion(localRotation());
		Matrix matTranslation = Matrix::CreateTranslation(localPosition);
		localTrsMatrix = matScale * matRotation * matTranslation;
		islocaltrschanged = true;
	}
	else
	{
		islocaltrschanged = false;
	}
	return localTrsMatrix;
}

Transform::Transform()
{
	localPosition = Vector3(0, 0, 0);
	localScale = Vector3(1, 1, 1);
	_localRotation = Quaternion::Identity;

	_prevlocalPosition = Vector3(-1, -1, -1);
	_prevlocalScale = Vector3(-1, -1, -1);
	_prevlocalRotation = Quaternion(-1, -1, -1, -1);

	Matrix matScale = Matrix::CreateScale(localScale);
	Matrix matRotation = Matrix::CreateFromQuaternion(_localRotation);
	Matrix matTranslation = Matrix::CreateTranslation(localPosition);
	
}

Transform::~Transform()
{

}
