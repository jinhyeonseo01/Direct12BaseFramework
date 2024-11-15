#include "stdafx.h"
#include "Material.h"

void Material::GetTextureDatas(std::shared_ptr<DescriptorTable> table, std::shared_ptr<Shader> shader)
{
    for (auto& texture : _propertyTextures)
    {
        std::string name = texture.first;
        if (_propertyTextures[name].lock() != nullptr)
            table->SetCurrentGroupHandle(shader, name, texture.second.lock()->GetSRVHandle());
    }
}


int Material::GetData(std::string name, int& field)
{
    if (_propertyInts.contains(name))
    {
        return field = _propertyInts[name];
    }
    return field = 0;
}

float Material::GetData(std::string name, float& field)
{
    if (_propertyFloats.contains(name))
    {
        return field = _propertyFloats[name];
    }
    return field = 0.5f;
}

Vector4 Material::GetData(std::string name, Vector4& field)
{
    if (_propertyVectors.contains(name))
    {
        return field = _propertyVectors[name];
    }
    return field = Vector4(1, 1, 1, 1);
}

Vector3 Material::GetData(std::string name, Vector3& field)
{
    if (_propertyVectors.contains(name))
    {
        return field = Vector3(_propertyVectors[name]);
    }
    return field = Vector3::Zero;
}

Vector2 Material::GetData(std::string name, Vector2& field)
{
    if (_propertyVectors.contains(name))
    {
        return field = Vector2(_propertyVectors[name]);
    }
    return field = Vector2::Zero;
}

Matrix Material::GetData(std::string name, Matrix& field)
{
    if (_propertyMatrixs.contains(name))
    {
        return field = _propertyMatrixs[name];
    }
    return field = Matrix::Identity;
}

std::shared_ptr<Texture> Material::SetData(std::string name, const std::shared_ptr<Texture>& field)
{
    if (field != nullptr)
        _propertyTextures[name] = field;
    return field;
}

int Material::SetData(std::string name, const int& field)
{
    return _propertyInts[name] = field;
}

float Material::SetData(std::string name, const float& field)
{
    return _propertyFloats[name] = field;
}

Vector4 Material::SetData(std::string name, const Vector4& field)
{
    return _propertyVectors[name] = field;
}

Vector3 Material::SetData(std::string name, const Vector3& field)
{
    _propertyVectors[name] = Vector4(field.x, field.y, field.z, 1.0f);
    return field;
}

Vector2 Material::SetData(std::string name, const Vector2& field)
{
    _propertyVectors[name] = Vector4(field.x, field.y, 0, 1.0f);
    return field;
}

Matrix Material::SetData(std::string name, const Matrix& field)
{
    return _propertyMatrixs[name] = field;
}
