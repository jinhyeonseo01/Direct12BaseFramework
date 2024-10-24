#include "stdafx.h"
#include "Transform.h"

#include "GameObject.h"

void* Transform::Clone()
{
	Component::Clone();

    auto thisObject = std::dynamic_pointer_cast<Transform>(this->shared_from_this());
    auto cloneObject = std::make_shared<Transform>()->MakeInit<Transform>();
    AddClone(thisObject, cloneObject);

    cloneObject->gameObject = thisObject->gameObject;
    cloneObject->_first = thisObject->_first;
    {
        cloneObject->localPosition = thisObject->localPosition;
        cloneObject->localRotation = thisObject->localRotation;
        cloneObject->localScale = thisObject->localScale;
        cloneObject->localSRTMatrix = thisObject->localSRTMatrix;
        cloneObject->localToWorldMatrix = thisObject->localToWorldMatrix;

        cloneObject->_prevLocalPosition = thisObject->_prevLocalPosition;
        cloneObject->_prevLocalRotation = thisObject->_prevLocalRotation;
        cloneObject->_prevLocalScale = thisObject->_prevLocalScale;
        cloneObject->_prevLocalSRTMatrix = thisObject->_prevLocalSRTMatrix;

        cloneObject->isLocalSRTChanged = thisObject->isLocalSRTChanged;
        cloneObject->isLocalToWorldChanged = thisObject->isLocalToWorldChanged;
        cloneObject->_right = thisObject->_right;
        cloneObject->_up = thisObject->_up;
        cloneObject->_forward = thisObject->_forward;
    }

    return cloneObject.get();
}

void Transform::ReRef()
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
		_prevLocalPosition = localPosition;
		_prevLocalRotation = localRotation;
		_prevLocalScale = localScale;
		Matrix matScale = Matrix::CreateScale(localScale);
		Matrix matRotation = Matrix::CreateFromQuaternion(localRotation);
		Matrix matTranslation = Matrix::CreateTranslation(localPosition);
        _prevLocalSRTMatrix = localSRTMatrix = matScale * matRotation * matTranslation;
	}
    if(CheckNeedLocalChangedUpdate())
    {
        _prevLocalSRTMatrix = localSRTMatrix;
        isLocalSRTChanged = true;
    }
	localSRT = localSRTMatrix;
	return isLocalSRTChanged;
}

bool Transform::SetLocalSRTMatrix(Matrix& localSRT)
{
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    // 행렬을 위치, 회전, 스케일로 분해
    if(localSRT.Decompose(scale, rotation, position))
    {
        localSRTMatrix = localSRT;
        localScale = scale;
        localRotation = rotation;
        localPosition = position;
        _prevLocalPosition = localPosition;
        _prevLocalRotation = localRotation;
        _prevLocalScale = localScale;
        return true;
    }
    return false;
}

bool Transform::CheckNeedLocalSRTUpdate() const
{
	return (_prevLocalPosition != localPosition)
		|| (_prevLocalRotation != localRotation)
		|| (_prevLocalScale != localScale);
}

bool Transform::CheckNeedLocalChangedUpdate() const
{
    return CheckNeedLocalSRTUpdate() || (_prevLocalSRTMatrix != localToWorldMatrix);
}

bool Transform::CheckNeedLocalToWorldUpdate() const
{
	//bottom-up check 방식
	bool needUpdate = false;
	auto currentObj = gameObject.lock();
	while (currentObj != nullptr && (!needUpdate))
	{
		needUpdate |= currentObj->transform->CheckNeedLocalChangedUpdate();
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

	return Vector3::Transform(value, mat);
}

Vector3 Transform::LocalToWorld_Direction(const Vector3& value)
{
	Matrix& mat = localToWorldMatrix;
	if (CheckNeedLocalToWorldUpdate())
		GetLocalToWorldMatrix(mat);

	return Vector3::TransformNormal(value, mat);
}

Quaternion Transform::LocalToWorld_Quaternion(const Quaternion& value)
{
	return value * worldRotation();
}

Vector3 Transform::WorldToLocal_Position(const Vector3& value)
{
	Matrix mat = localToWorldMatrix;
	if (CheckNeedLocalToWorldUpdate())
		GetLocalToWorldMatrix(mat);
	mat.Invert(mat);
	return Vector3::Transform(value, mat);
}

Vector3 Transform::WorldToLocal_Direction(const Vector3& value)
{
	Matrix mat = localToWorldMatrix;
	if (CheckNeedLocalToWorldUpdate())
		GetLocalToWorldMatrix(mat);
	mat.Invert(mat);
	Vector3 vec = Vector3::TransformNormal(value, mat);
	vec.Normalize(vec);
	return vec;
}

Quaternion Transform::WorldToLocal_Quaternion(const Quaternion& value)
{
	Quaternion result;
	worldRotation().Inverse(result);
	result = value * result;
	result.Normalize();
	return result;
}

void Transform::Start()
{
	Component::Start();
}

void Transform::Update()
{
	Component::Update();
}

void Transform::LateUpdate()
{
	Component::LateUpdate();
}

void Transform::OnEnable()
{
	Component::OnEnable();
}

void Transform::OnDisable()
{
	Component::OnDisable();
}

void Transform::OnDestroy()
{
	Component::OnDestroy();
}

void Transform::Init()
{
	Component::Init();
}

void Transform::OnComponentDestroy()
{
	Component::OnComponentDestroy();
}

Transform::Transform()
{
	localPosition = Vector3(0, 0, 0);
	localScale = Vector3(1, 1, 1);
	localRotation = Quaternion::Identity;

	_prevLocalPosition = Vector3(-1, -1, -1);
	_prevLocalScale = Vector3(-1, -1, -1);
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
