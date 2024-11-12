#pragma once
#include "stdafx.h"
#include <memory>

#include "DescriptorTable.h"
#include "EObject.h"
#include "Shader.h"
#include "Texture.h"

class Material : public EObject
{
public:
    std::weak_ptr<Shader> shader;

    std::unordered_map<std::string, int> _propertyInts;
    std::unordered_map<std::string, float> _propertyFloats;
    std::unordered_map<std::string, Vector4> _propertyVectors;
    std::unordered_map<std::string, Matrix> _propertyMatrixs;
    std::unordered_map<std::string, std::weak_ptr<Texture>> _propertyTextures;

    void GetTextureDatas(std::shared_ptr<DescriptorTable> table, std::shared_ptr<Shader> shader);
    int GetData(std::string name, int& field);
    float GetData(std::string name, float& field);
    Vector4 GetData(std::string name, Vector4& field);
    Vector3 GetData(std::string name, Vector3& field);
    Vector2 GetData(std::string name, Vector2& field);
    Matrix GetData(std::string name, Matrix& field);

    std::shared_ptr<Texture> SetData(std::string name, const std::shared_ptr<Texture>& field);
    int SetData(std::string name, const int& field);
    float SetData(std::string name, const float& field);
    Vector4 SetData(std::string name, const Vector4& field);
    Vector3 SetData(std::string name, const  Vector3& field);
    Vector2 SetData(std::string name, const Vector2& field);
    Matrix SetData(std::string name, const Matrix& field);
};

