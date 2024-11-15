#pragma once
#include <stdafx.h>


class RootSignature : public std::enable_shared_from_this<RootSignature>
{
public:
    RootSignature();
    virtual ~RootSignature();

    ComPtr<ID3D12RootSignature> GetRootSignature();
    ComPtr<ID3D12RootSignature> GetComputeRootSignature();

    virtual void Init();

private:
    virtual void InitCompute();

public:
    ComPtr<ID3D12RootSignature> _rootSignature;
    ComPtr<ID3D12RootSignature> _computeRootSignature;

    std::vector<D3D12_DESCRIPTOR_RANGE> _ranges;
};
