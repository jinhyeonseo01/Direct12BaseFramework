#pragma once

#include <stdafx.h>

namespace dxe
{
    class Model : public std::enable_shared_from_this<Model>
    {
    public:
        std::vector<std::shared_ptr<Mesh>> _meshList;

        Model();
        virtual ~Model();
    };
}

