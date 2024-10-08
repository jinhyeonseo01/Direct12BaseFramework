#pragma once

#include <stdafx.h>

namespace dxe
{
    class GraphicManager
    {
    public:
        std::weak_ptr<Engine> _engine;
        GraphicManager();
        virtual ~GraphicManager();
    };

}

