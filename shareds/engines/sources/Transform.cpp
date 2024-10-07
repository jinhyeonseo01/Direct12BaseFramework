#include "stdafx.h"
#include "Transform.h"

#include "GameObject.h"

void* Transform::Clone() const
{
	return Component::Clone();
}

void Transform::ReRef() const
{
	Component::ReRef();
}


Vector3 Transform::forward(const Vector3& dir)
{
	if(dir != Vector3::Zero)
	{
		_forward = dir;
		_forward.Normalize(_forward);
		_right = _up.Cross(_forward);
		_up = _forward.Cross(_right);
		/*worldRotation(Quaternion::CreateFromRotationMatrix(Matrix(
			_right.x, _up.x, _forward.x, 0.0f,  // 첫 번째 열 (_right)
			_right.y, _up.y, _forward.y, 0.0f,  // 두 번째 열 (_up)
			_right.z, _up.z, _forward.z, 0.0f,  // 세 번째 열 (_forward)
			0.0f, 0.0f, 0.0f, 1.0f   // 평행 이동 및 4번째 열
		)));
		*/
		worldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right,_up,_forward)));
		return _forward;
	}
	Quaternion quat = worldRotation();
	_right = Vector3::Transform(Vector3(1, 0, 0), quat);
	_up = Vector3::Transform(Vector3(0, 1, 0), quat);
	_forward = Vector3::Transform(Vector3(0, 0, 1), quat);
	return _forward;
}

Vector3 Transform::up(const Vector3& dir)
{
	if (dir != Vector3::Zero)
	{
		_up = dir;
		_up.Normalize();
		_right = _up.Cross(_forward);
		_forward = _right.Cross(_up);
		worldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));

		return _up;
	}
	//쿼터니언 기반으로 다시 원래값 받아와야함.
	Quaternion quat = worldRotation();
	_right = Vector3::Transform(Vector3(1, 0, 0), quat);
	_up = Vector3::Transform(Vector3(0, 1, 0), quat);
	_forward = Vector3::Transform(Vector3(0, 0, 1), quat);
	return _up;
}

Vector3 Transform::right(const Vector3& dir)
{
	if (dir != Vector3::Zero)
	{
		_right = dir;
		_right.Normalize(_right);
		_forward = _right.Cross(_up);
		_up = _forward.Cross(_right);
		worldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));
		return _right;
	}
	Quaternion quat = worldRotation();
	_right = Vector3::Transform(Vector3(1, 0, 0), quat);
	_up = Vector3::Transform(Vector3(0, 1, 0), quat);
	_forward = Vector3::Transform(Vector3(0, 0, 1), quat);
	return _right;
}

Vector3 Transform::localEuler()
{
	return localRotation.ToEuler();
}

const Vector3& Transform::localEuler(const Vector3& euler)
{
	localRotation = Quaternion::CreateFromYawPitchRoll(euler);
	return euler;
}

Vector3 Transform::worldPosition()
{
	auto parent = gameObject.lock()->parent.lock();
	if(parent)
	{
		Matrix mat;
		parent->transform->GetLocalToWorldMatrix(mat);
		return Vector3::Transform(localPosition, mat);
	}
	return localPosition;
}

const Vector3& Transform::worldPosition(const Vector3& worldPos)
{
	auto parent = gameObject.lock()->parent.lock();
	if (parent)
	{
		Matrix mat;
		parent->transform->GetLocalToWorldMatrix(mat);
		localPosition = Vector3::Transform(worldPos, mat.Invert());
		Debug::log <<"local"<< localPosition << "\n";
	}
	else
		localPosition = worldPos;
	return worldPos;
}

Vector3 Transform::worldScale()
{
	Vector3 totalScale = localScale;
	auto currentObj = gameObject.lock()->parent.lock();
	while (currentObj != nullptr)
	{
		totalScale = totalScale * currentObj->transform->localScale;
		currentObj = currentObj->parent.lock();
	}
	return totalScale;
}

const Vector3& Transform::worldScale(const Vector3& scale)
{
	auto parent = gameObject.lock()->parent.lock();
	if (parent)
	{
		Vector3 ws = parent->transform->worldScale();
		localScale = (ws / (ws * ws)) * scale;
	}
	else
		localScale = scale;
	return scale;
}

Quaternion Transform::worldRotation()
{
	Quaternion totalQuat = Quaternion::Identity;
	auto currentObj = gameObject.lock();
	while (currentObj != nullptr)
	{
		totalQuat = totalQuat * currentObj->transform->localRotation;
		currentObj = currentObj->parent.lock();
	}
	totalQuat.Normalize();
	return totalQuat;
}

const Quaternion& Transform::worldRotation(const Quaternion& quaternion)
{
	/*
	Quaternion result;
	localRotation.Inverse(result);
	result = result * worldRotation(); // 부모 쿼터니언
	result.Inverse(result);
	localRotation = quaternion * result;
	localRotation.Normalize();
	*/

	auto parent = gameObject.lock()->parent.lock();
	if (parent)
	{
		Quaternion result;
		parent->transform->worldRotation().Inverse(result);
		localRotation = quaternion * result;
		localRotation.Normalize();
	}
	else
		localRotation = quaternion;
	return quaternion;
}

bool Transform::GetLocalToWorldMatrix(Matrix& localToWorldMatrix)
{
	if (CheckNeedLocalToWorldUpdate())
	{
		auto root = gameObject.lock()->rootParent.lock();
		root->transform->TopDownLocalToWorldUpdate(Matrix::Identity);
		localToWorldMatrix = this->localToWorldMatrix;
		return isLocalToWorldChanged;
	}
	localToWorldMatrix = this->localToWorldMatrix;
	return isLocalToWorldChanged;
}

bool Transform::GetLocalSRTMatrix(Matrix& localSRT)
{
	isLocalSRTChanged = CheckNeedLocalSRTUpdate();

	if (isLocalSRTChanged)
	{
		_prevlocalPosition = localPosition;
		_prevLocalRotation = localRotation;
		_prevlocalScale = localScale;
		Matrix matScale = Matrix::CreateScale(localScale);
		Matrix matRotation = Matrix::CreateFromQuaternion(localRotation);
		Matrix matTranslation = Matrix::CreateTranslation(localPosition);
		localSRTMatrix = matScale * matRotation * matTranslation;
	}
	localSRT = localSRTMatrix;
	return isLocalSRTChanged;
}

bool Transform::CheckNeedLocalSRTUpdate() const
{
	return (_prevlocalPosition != localPosition)
		|| (_prevLocalRotation != localRotation)
		|| (_prevlocalScale != localScale);
}
bool Transform::CheckNeedLocalToWorldUpdate() const
{
	//bottom-up check 방식
	bool needUpdate = false;
	auto currentObj = gameObject.lock();
	while (currentObj != nullptr && (!needUpdate))
	{
		needUpdate |= currentObj->transform->CheckNeedLocalSRTUpdate();
		currentObj = currentObj->parent.lock();
	}
	return needUpdate;
}

void Transform::TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate)
{
	Matrix localSRT;
	bool isLocalUpdate = GetLocalSRTMatrix(localSRT);
	bool isFinalUpdate = isLocalUpdate || isParentUpdate;

	if (isFinalUpdate)
		localToWorldMatrix = localSRT * parentLocalToWorld;
	isLocalToWorldChanged = isFinalUpdate;

	auto& childs = gameObject.lock()->_childs;
	for (int i = 0; i < childs.size(); i++)
	{
		auto ptr = childs[i].lock();
		if (ptr)
			ptr->transform->TopDownLocalToWorldUpdate(localToWorldMatrix, isFinalUpdate);
	}
}

Vector3 Transform::LocalToWorld_Position(const Vector3& value)
{
	Matrix& mat = localToWorldMatrix;
	if (CheckNeedLocalToWorldUpdate())
		GetLocalToWorldMatrix(mat);

	Vector4 result = Vector4(value);
	result.w = 1;
	Vector4::Transform(result, mat, result);
	return Vector3(result);
}

Vector3 Transform::LocalToWorld_Direction(const Vector3& value)
{
	Matrix& mat = localToWorldMatrix;
	if (CheckNeedLocalToWorldUpdate())
		GetLocalToWorldMatrix(mat);

	Vector3 result;
	Vector3::Transform(value, mat, result);
	return result;
}

Quaternion Transform::LocalToWorld_Quaternion(const Quaternion& value)
{
	Matrix& mat = localToWorldMatrix;
	if (CheckNeedLocalToWorldUpdate())
		GetLocalToWorldMatrix(mat);
	//mat * value;
	return Quaternion();
}

Vector3 Transform::WorldToLocal_Position(const Vector3& value)
{
	return Vector3();
}

Vector3 Transform::WorldToLocal_Direction(const Vector3& value)
{
	return Vector3();
}

Quaternion Transform::WorldToLocal_Quaternion(const Quaternion& value)
{
	return Quaternion();
}

Transform::Transform()
{
	localPosition = Vector3(0, 0, 0);
	localScale = Vector3(1, 1, 1);
	localRotation = Quaternion::Identity;

	_prevlocalPosition = Vector3(-1, -1, -1);
	_prevlocalScale = Vector3(-1, -1, -1);
	_prevLocalRotation = Quaternion(-1,-1,-1,-1);

	localSRTMatrix = Matrix::Identity;
	localToWorldMatrix = Matrix::Identity;

	_forward = Vector3::Forward;
	_up = Vector3::Up;
	_right = Vector3::Right;
}

Transform::~Transform()
{

}
