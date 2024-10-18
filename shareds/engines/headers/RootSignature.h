#pragma once
#include <stdafx.h>


class RootSignature : public std::enable_shared_from_this<RootSignature>
{
public:
    RootSignature();
    virtual ~RootSignature();

    virtual void Init();

    ComPtr<ID3D12RootSignature> _rootSignature;

};

