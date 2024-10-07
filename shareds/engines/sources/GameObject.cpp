#include <stdafx.h>

#include <DXEngine.h>


GameObject::GameObject()
{
	this->name = L"obj_Nameless";
}

GameObject::GameObject(std::wstring name)
{
	this->name = name;
}

GameObject::GameObject(const GameObject& eObject)
{
	this->name = eObject.name;
	this->_sortingOrder = eObject._sortingOrder;
}

GameObject::GameObject(GameObject&& gameObject) noexcept
{

}

GameObject& GameObject::operator=(const GameObject& gameObject)
{
	if (this == &gameObject)
		return *this;
	return *this;
}

GameObject& GameObject::operator=(GameObject&& gameObject) noexcept
{
	if (this == &gameObject)
		return *this;
	return *this;
}

bool dxe::GameObject::operator<(const GameObject& other) const
{
	return this->_sortingOrder < other._sortingOrder;
}

GameObject::~GameObject()
{
	_components.clear();
}

std::shared_ptr<GameObject> GameObject::Init()
{
	auto thisPtr = std::dynamic_pointer_cast<GameObject>(this->shared_from_this());
	rootParent = thisPtr;
	parent.reset();
	_childs.reserve(16);
	this->SetActiveSelf(true);
	this->transform = this->AddComponent<Transform>();
	return thisPtr;
}

bool GameObject::RemoveAtComponent(int index)
{
	if (index >= 0 && index < this->_components.size())
	{
		this->_components.erase(this->_components.begin() + index);
	}
	return false;
}

void dxe::GameObject::Destroy()
{
	_destroy = true;
	SetActiveSelf(false);
	for (int i = 0; i < _childs.size(); i++)
	{
		auto current = _childs[i].lock();
		if (current)
			current->Destroy();
	}
}

void* GameObject::Clone() const
{
	return EObject::Clone();
}

void GameObject::ReRef() const
{
	EObject::ReRef();
}

std::shared_ptr<GameObject> GameObject::GetChild(int index)
{
	if (index < 0 || index >= _childs.size())
		return nullptr;
	return _childs[index].lock();
}

std::shared_ptr<GameObject> GameObject::GetChildByName(const std::wstring& name)
{
	auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::weak_ptr<GameObject> obj)
		{
			if (!obj.lock())
				return false;
			return obj.lock()->name == name;
		});
	if (iter == _childs.end())
		return  nullptr;
	return iter->lock();
}

int GameObject::GetChilds(std::vector<std::shared_ptr<GameObject>>& vec)
{
	int count = 0;
	for (auto& c : _childs)
		if (c.lock())
		{
			vec.push_back(c.lock());
			++count;
		}
	return count;
}

int GameObject::GetChildsByName(std::vector<std::shared_ptr<GameObject>>& vec, const std::wstring& name)
{
	int count = 0;
	for (auto& c : _childs)
	{
		auto currentObj = c.lock();
		if (currentObj && currentObj->name == name)
		{
			vec.push_back(currentObj);
			++count;
		}
	}
	return count;
}

int GameObject::GetChildsAll(std::vector<std::shared_ptr<GameObject>>& vec)
{
	int count = 1;
	vec.push_back(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	
	for (auto& c : _childs)
	{
		auto currentObj = c.lock();
		if (currentObj)
		{
			count += currentObj->GetChildsAll(vec);
		}
	}
	return count;
}

int GameObject::GetChildsAllByName(std::vector<std::shared_ptr<GameObject>>& vec, const std::wstring& name)
{
	int count = 1;
	if(this->name == name)
		vec.push_back(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));

	for (auto& c : _childs)
	{
		auto currentObj = c.lock();
		if (currentObj)
		{
			count += currentObj->GetChildsAllByName(vec, name);
		}
	}
	return count;
}


bool GameObject::AddChild(const std::shared_ptr<GameObject>& obj)
{
	if (obj == nullptr)
		return false;
	auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::weak_ptr<GameObject>& element) {
		return element.lock() == obj;
	});
	if (iter == _childs.end()) {
		_childs.push_back(obj);
		return true;
	}
	return false;
}

bool GameObject::RemoveChild(const std::shared_ptr<GameObject>& obj)
{
	if (obj == nullptr)
		return false;

	auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::weak_ptr<GameObject>& element) {
		return element.lock() == obj;
		});
	if (iter != _childs.end()) {
		_childs.erase(iter);
		return true;
	}
	return false;
}

bool GameObject::RemoveAtChild(int index)
{
	if (index < 0 || index >= _childs.size())
		return false;
	_childs.erase(_childs.begin() + index);
	return true;
}

bool GameObject::IsInParent(const std::shared_ptr<GameObject>& obj)
{
	if (obj == nullptr || obj == this->shared_from_this())
		return false;
	auto parent = this->parent.lock();
	while(parent != nullptr)
	{
		if (parent == obj)
			return true;
		parent = parent->parent.lock();
	}
	return false;
}

bool GameObject::GetActive()
{
	return _active_total && _active_self;//이미 total에 반영되있긴 하지만 혹시 모르니 한번더
}

bool GameObject::GetActiveSelf()
{
	return _active_self;
}

bool GameObject::SetActiveSelf(bool _active)
{
	_active_self = _active;
	ActiveUpdateChain(_active_total);
	return _active;
}

void GameObject::ActiveUpdateChain(bool _active_total)
{
	this->_active_total = _active_total && _active_self;
	
	for (int i=0;i<_childs.size();i++)
	{
		auto current = _childs[i].lock();
		if (current && (!current->IsDestroy()))
			current->ActiveUpdateChain(this->_active_total);
	}
}

void GameObject::Debug(int depth)
{
	for (int i = 0; i < depth; i++)
		Debug::log << "    ";
	Debug::log << "- " << name << std::format(" (act_t{}, act_s{}, des{})", _active_total,_active_self,_destroy) << "\n";

	for (int i = 0; i < depth; i++)
		Debug::log << "    ";
	Debug::log << "  " << "Components" << "\n";

	for (int i = 0; i < _components.size(); i++)
	{
		for (int i = 0; i < depth; i++)
			Debug::log << "    ";
		Debug::log << "  : " << typeid(*_components[i].get()).name() << "\n";
	}

	for (int i = 0; i < depth; i++)
		Debug::log << "    ";
	Debug::log << "  " << "Childs" << "\n";
	for (int i = 0; i < _childs.size(); i++)
		if (_childs[i].lock())
			_childs[i].lock()->Debug(depth + 1);
}

bool GameObject::SetParent(const std::shared_ptr<GameObject>& nextParentObj)
{
	if (nextParentObj == nullptr)
		return false;
	if (nextParentObj.get() == this)
		return false;
	if(this->IsDestroy())
		return false;

	auto thisObj = std::dynamic_pointer_cast<GameObject>(shared_from_this());
	auto prevParentObj = this->parent.lock();

	if (nextParentObj != nullptr && nextParentObj->IsInParent(thisObj)) // step 1.
		return false;
	if(prevParentObj != nullptr) // step 2. 부모연결 끊고 최상단으로 올리기
	{
		prevParentObj->RemoveChild(thisObj);
		this->parent.reset();
		this->rootParent = thisObj;
	}
	if(nextParentObj != nullptr)
	{
		this->parent = nextParentObj;
		this->rootParent = nextParentObj->rootParent;
		this->ActiveUpdateChain(nextParentObj->_active_total);
		if (nextParentObj->IsDestroy())
			this->Destroy();
		nextParentObj->AddChild(thisObj);
	}
}
