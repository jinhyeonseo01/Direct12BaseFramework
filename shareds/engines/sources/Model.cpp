#include "stdafx.h"
#include "Model.h"

Model::Model()
{
    _meshList.reserve(64);
}

Model::~Model()
{
    _meshList.clear();
}
