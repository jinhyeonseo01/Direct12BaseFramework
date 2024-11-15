#include "stdafx.h"
#include "RenderTargetGroup.h"

#include "GraphicManager.h"
#include "graphic_config.h"

RenderTargetGroup::RenderTargetGroup()
{
}

RenderTargetGroup::~RenderTargetGroup()
{
}

void RenderTargetGroup::Create(std::vector<std::shared_ptr<RenderTexture>>& renderTargetList,
                               std::shared_ptr<RenderTexture> depthStencil)
{
    _renderTargetTextureList.clear();
    _renderTargetTextureList = renderTargetList;
    _depthStencilTexture = depthStencil;

    D3D12_DESCRIPTOR_HEAP_DESC _renderTaretGroupHeapDesc = {};
    _renderTaretGroupHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    int totalNumDesc = 0;
    for (auto& texture : _renderTargetTextureList)
        totalNumDesc += texture->GetRTV()->GetDesc().NumDescriptors;
    _renderTaretGroupHeapDesc.NumDescriptors = totalNumDesc;
    _renderTaretGroupHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    _renderTaretGroupHeapDesc.NodeMask = 0;

    auto device = GraphicManager::main->_device;

    device->CreateDescriptorHeap(&_renderTaretGroupHeapDesc, ComPtrIDAddr(_renderTargetHeap));
    auto RTOffsetPtr = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
    auto size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (int i = 0; i < _renderTargetTextureList.size(); i++)
    {
        int currentCount = _renderTargetTextureList[i]->GetRTV()->GetDesc().NumDescriptors;
        for (int j = 0; j < currentCount; j++)
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE currentHandle(RTOffsetPtr, i + j, size);
            unsigned int tempDstSize = 1;
            unsigned int tempSrcSize = 1;
            device->CreateRenderTargetView(_renderTargetTextureList[i]->GetResource().Get(),
                                           &_renderTargetTextureList[i]->_RTV_ViewDesc, currentHandle);
            auto currentSrcHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _renderTargetTextureList[i]->GetRTV()->GetCPUDescriptorHandleForHeapStart(), j, size);
            device->CopyDescriptors(
                1, &currentHandle, &tempDstSize,
                1, &currentSrcHandle
                , &tempSrcSize,
                D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


            _renderTargetHandleToTextureIndexList.push_back(i);
            _renderTargetHandleList.push_back(currentHandle);
        }
    }
    _depthStencilHandle = _depthStencilTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();
}

void RenderTargetGroup::ResourceBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    for (int i = 0; i < _renderTargetTextureList.size(); i++)
        GraphicManager::main->ResourceBarrier(_renderTargetTextureList[i]->GetResource().Get(), before, after);
}

void RenderTargetGroup::OMSetRenderTargets(uint32_t count, uint32_t offset)
{
    auto commandList = GraphicManager::main->GetCurrentCommandList();
    if (_renderTargetHandleList.size() != 0)
    {
        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_rect);

        commandList->OMSetRenderTargets(count, &_renderTargetHandleList[offset],
                                        _renderTargetHandleList.size() <= 1 ? FALSE : TRUE/*다중*/, &_depthStencilHandle);
    }
}

void RenderTargetGroup::OMSetRenderTargets()
{
    auto commandList = GraphicManager::main->GetCurrentCommandList();
    if (_renderTargetHandleList.size() != 0)
    {
        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_rect);

        commandList->OMSetRenderTargets(_renderTargetHandleList.size(), &_renderTargetHandleList[0],
                                        _renderTargetHandleList.size() != 0/*다중*/, &_depthStencilHandle);
    }
}

void RenderTargetGroup::ClearRenderTargetView(uint32_t index)
{
    auto commandList = GraphicManager::main->GetCurrentCommandList();
    commandList->ClearRenderTargetView(_renderTargetHandleList[index], _renderTargetTextureList[
                                           _renderTargetHandleToTextureIndexList[index]]->GetClearColor(), 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetViews()
{
    auto commandList = GraphicManager::main->GetCurrentCommandList();
    for (int i = 0; i < _renderTargetHandleList.size(); i++)
    {
        commandList->ClearRenderTargetView(_renderTargetHandleList[i], _renderTargetTextureList[
                                               _renderTargetHandleToTextureIndexList[i]]->GetClearColor(), 0, nullptr);
    }
}

void RenderTargetGroup::ClearDepthStencilView()
{
    auto commandList = GraphicManager::main->GetCurrentCommandList();
    commandList->ClearDepthStencilView(_depthStencilHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::SetViewport(int width, int height)
{
    _viewport = D3D12_VIEWPORT{0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0, 1.0f};
    _rect = D3D12_RECT{0, 0, static_cast<int>(width), static_cast<int>(height)};
}
