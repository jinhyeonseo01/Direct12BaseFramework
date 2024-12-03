#include "stdafx.h"
#include "RenderPacket.h"


RenderPacket::RenderPacket(std::shared_ptr<Mesh> mesh, std::weak_ptr<Material> material,
    std::function<void(const RenderPacket& renderPack)> renderFunction, float zDepth)
{
    this->mesh = mesh;
    this->material = material;
    this->renderFunction = renderFunction;
    this->sortingOrder = zDepth;
}

RenderPacket::RenderPacket()
{
    this->mesh = std::weak_ptr<Mesh>();
}

RenderPacket::~RenderPacket()
{
    materialLC = nullptr;
}

RenderPacket::RenderPacket(const RenderPacket& renderPacket)
{
    this->mesh = renderPacket.mesh;
    this->material = renderPacket.material;
    this->renderFunction = renderPacket.renderFunction;
    this->sortingOrder = renderPacket.sortingOrder;
    this->materialLC = renderPacket.materialLC;
    this->meshLC = renderPacket.meshLC;
}

RenderPacket::RenderPacket(RenderPacket&& renderPacket) noexcept
{
    this->mesh = renderPacket.mesh;
    this->material = renderPacket.material;
    this->renderFunction = renderPacket.renderFunction;
    this->sortingOrder = renderPacket.sortingOrder;
    this->materialLC = renderPacket.materialLC;
    this->meshLC = renderPacket.meshLC;
}

RenderPacket& RenderPacket::operator=(const RenderPacket& renderPacket)
{
    if (this == &renderPacket)
        return *this;

    this->mesh = renderPacket.mesh;
    this->material = renderPacket.material;
    this->renderFunction = renderPacket.renderFunction;
    this->sortingOrder = renderPacket.sortingOrder;
    this->materialLC = renderPacket.materialLC;
    this->meshLC = renderPacket.meshLC;

    return *this;
}

RenderPacket& RenderPacket::operator=(RenderPacket&& renderPacket) noexcept
{
    if (this == &renderPacket)
        return *this;

    this->mesh = renderPacket.mesh;
    this->material = renderPacket.material;
    this->renderFunction = renderPacket.renderFunction;
    this->sortingOrder = renderPacket.sortingOrder;
    this->materialLC = renderPacket.materialLC;
    this->meshLC = renderPacket.meshLC;

    return *this;
}

std::shared_ptr<RenderPacket> RenderPacket::Init(std::shared_ptr<Mesh> mesh, std::weak_ptr<Material> material,
    std::function<void(const RenderPacket& renderPack)> renderFunction, float zDepth)
{
    this->mesh = mesh;
    this->material = material;
    this->renderFunction = renderFunction;
    this->sortingOrder = zDepth;
}

int RenderPacket::Order()
{
    return sortingOrder;
}

void RenderPacket::SetLifeExtension(std::shared_ptr<Material> material)
{
    materialLC = material;
}
void RenderPacket::SetLifeExtension(std::shared_ptr<Mesh> mesh)
{
    meshLC = mesh;
}

bool RenderPacket::operator==(const RenderPacket& rpOther) const
{
    if (material.lock() && material.lock()->shader.lock())
    {
        auto mat = material.lock();
        auto shader = mat->shader.lock();

        auto mat2 = rpOther.material.lock();
        auto shader2 = rpOther.material.lock()->shader.lock();

        if (shader->_info._renderQueueType == shader2->_info._renderQueueType)
            if (shader->_info._renderQueue == shader2->_info._renderQueue)
                if (shader == shader2)
                    return true;
    }
    return false;
}

bool RenderPacket::operator<(const RenderPacket& rpOther) const
{
    auto materialPtr = material.lock();
    if (materialPtr != nullptr && materialPtr->shader.lock() != nullptr)
    {
        auto shader = materialPtr->shader.lock();
        auto mesh = this->mesh.lock();

        auto mat2 = rpOther.material.lock();
        auto shader2 = rpOther.material.lock()->shader.lock();
        auto mesh2 = rpOther.mesh.lock();

        if (shader->_info._renderQueueType == shader2->_info._renderQueueType)
        {
            if (shader->_info._renderQueueType == RenderQueueType::Transparent)
                return sortingOrder > rpOther.sortingOrder;
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
