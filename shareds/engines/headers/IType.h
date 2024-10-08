#pragma once
#include <string>
#include <typeinfo>
#include <cctype>
#include <functional>
#include <unordered_map>

#include "debug_console.h"

namespace dxe
{
    class IType
    {
    private:
        std::string _className;
    public:
        IType();
        virtual ~IType();
        virtual std::string& GetTypeName();
        virtual std::string GetTypeFullName();

    private:
        static std::unordered_map<std::string, std::function<void*()>> typeTable;
        static bool _first;
    protected:
        template<class T, class = std::enable_if<std::is_base_of<IType, T>::value, T*>::type>
        //�̿ϼ� ���
        static T* TypeRegister(T* obj)
        {
            if (_first) {
                typeTable.reserve(2048);
                _first = false;
            }
            auto& key = obj->_className;
            if(!typeTable.contains(key))
	            typeTable.emplace(std::make_pair(key, []() { return dynamic_cast<IType*>(new T()); }));
            return obj;
        }
    public:
        template<class T, class = std::enable_if<std::is_base_of<IType, T>::value, T*>::type>
        //�̿ϼ� ���
        static T* CreateObject(const std::string& typeName) {
            auto it = typeTable.find(typeName);
            if (it != typeTable.end()) {
                return dynamic_cast<T*>(static_cast<IType*>(it->second()));  // ��ϵ� ������ ȣ�� �� ��ü ��ȯ
            }
            return nullptr;  // �ش� key�� ���� �� nullptr ��ȯ
        }
    };
}

