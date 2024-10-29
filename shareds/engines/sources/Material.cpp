#include "Material.h"

void Material::SetTextureDatas(std::shared_ptr<DescriptorTable> table, std::shared_ptr<Shader> shader)
{
    for(auto& texture : _propertyTextures)
    {
        std::string name = texture.first;
        table->SetCurrentGroupHandle(shader, name, texture.second.lock()->GetSRVHandle());
    }
}
