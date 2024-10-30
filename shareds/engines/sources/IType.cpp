#include "stdafx.h"
#include "IType.h"

#include "debug_console.h"

dxe::IType::IType()
{

}

dxe::IType::~IType()
{

}

std::string& dxe::IType::GetTypeName()
{
    std::string fullName = GetTypeFullName();

    // "struct " �Ǵ� "class " ���� ���λ� ����
    size_t pos = fullName.find_last_of("::");
    if (pos != std::string::npos) {
        _className = fullName.substr(pos + 1);
    }

    // ������ �κ��� �ܼ�ȭ (��: "1Derived" -> "Derived")
    // ���ڰ� ���Ե� ��� ����
    for (size_t i = 0; i < _className.length(); ++i) {
        if (isdigit(_className[i]))
            continue;
        _className = _className.substr(i);
        break;
    }

    //TypeRegister(this);
    return _className;
}

std::string dxe::IType::GetTypeFullName()
{
    return std::string{typeid(*this).name()};
}

bool dxe::IType::_first = true;
std::unordered_map<std::string, std::function<void* ()>> dxe::IType::typeTable;