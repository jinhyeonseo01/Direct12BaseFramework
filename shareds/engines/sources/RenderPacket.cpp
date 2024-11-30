#include "stdafx.h"
#include "RenderPacket.h"


RenderPacket::RenderPacket(std::shared_ptr<Mesh> mesh, std::weak_ptr<Material> material,
    std::function<void(const RenderPacket& renderPack)> renderFunction, float zDepth)
{
    this->mesh = mesh;
    this->material = material;
    this->renderFunction = renderFunction;
    this->zDepth = zDepth;
}

RenderPacket::RenderPacket()
{
}

RenderPacket::~RenderPacket()
{
}

int RenderPacket::Order()
{
    return 0;
}

void RenderPacket::SetLifeExtension(std::shared_ptr<Material> material)
{
    materialLC = material;
}

bool RenderPacket::operator==(const RenderPacket& rpOther) const
{
    if (material.lock() && material.lock()->shader.lock())
    {
        auto mat = material.lock();
        auto shader = mat->shader.lock();
        auto mesh = this->mesh.lock();

        auto mat2 = rpOther.material.lock();
        auto shader2 = rpOther.material.lock()->shader.lock();
        auto mesh2 = rpOther.mesh.lock();
        if (shader->_info._renderQueueType == shader2->_info._renderQueueType)
            if (shader->_info._renderQueue == shader2->_info._renderQueue)
                if (shader == shader2)
                    return true;
    }
    return false;
}

bool RenderPacket::operator<(const RenderPacket& rpOther) const
{
    if (material.lock() && material.lock()->shader.lock())
    {
        auto mat = material.lock();
        auto shader = mat->shader.lock();
        auto mesh = this->mesh.lock();

        auto mat2 = rpOther.material.lock();
        auto shader2 = rpOther.material.lock()->shader.lock();
        auto mesh2 = rpOther.mesh.lock();

        if (shader->_info._renderQueueType == shader2->_info._renderQueueType)
        {
            if (shader->_info._renderQueueType == RenderQueueType::Transparent)
                return zDepth > rpOther.zDepth;
            if (shader->_info._renderQueue == shader2->_info._renderQueue)
            {
                if (shader == shader2) {
                    return mesh < mesh2;
                }
                else
                    return shader < shader2;
            }
            else
                return shader->_info._renderQueue < shader2->_info._renderQueue;
        }
        else
            return shader->_info._renderQueueType < shader2->_info._renderQueueType;
    }
    return true;
}
