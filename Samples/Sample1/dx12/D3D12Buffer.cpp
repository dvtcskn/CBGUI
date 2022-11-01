/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Coþkun.
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* ---------------------------------------------------------------------------------------
*/

#include "pch.h"
#include "D3D12Buffer.h"
#include "D3D12CommandBuffer.h"
#include "D3D12DeviceLocator.h"
#include "D3D12Viewport.h"

D3D12ConstantBuffer::D3D12ConstantBuffer(std::string Name, sBufferDesc& InDesc)
    : BufferDesc(InDesc)
    , Owner(D3D12DeviceLocator::Get().GetInterface())
    , Ptr(nullptr)
    , bIsMapped(false)
    , ViewHeap(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
{
    ZeroMemory(&Ptr, sizeof(Ptr));

    ID3D12Device* m_device = Owner->GetDevice();

    Owner->GetDescriptorHeapManager()->AllocateDescriptor(&ViewHeap);

    const UINT constantBufferSize = (BufferDesc.Size + 255) & ~255;    // CB size is required to be 256-byte aligned.

    auto HeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto Desc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

    ThrowIfFailed(m_device->CreateCommittedResource(
        &HeapDesc,
        D3D12_HEAP_FLAG_NONE,
        &Desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_spUploadBuffer)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_spUploadBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = constantBufferSize;
    m_device->CreateConstantBufferView(&cbvDesc, ViewHeap.GetCPU());

    m_spUploadBuffer->SetName(L"cb");
}

D3D12ConstantBuffer::~D3D12ConstantBuffer()
{
    Unmap();
    Owner = nullptr;
    m_spUploadBuffer = nullptr;
    Ptr = nullptr;
}

std::size_t D3D12ConstantBuffer::GetSize() const
{
    return BufferDesc.Size;
}

void D3D12ConstantBuffer::ApplyBuffer(UINT RootParameterIndex, ID3D12GraphicsCommandList* InCMDBuffer)
{
    InCMDBuffer->SetGraphicsRootDescriptorTable(RootParameterIndex, ViewHeap.GetGPU());
}

void D3D12ConstantBuffer::Map(const void* Data)
{
    if (!bIsMapped)
    {
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_spUploadBuffer->Map(0, &readRange, &Ptr));
        memcpy(Ptr, Data, BufferDesc.Size);
        bIsMapped = true;
    }
    else
    {
        memcpy(Ptr, Data, BufferDesc.Size);
    }
}

void D3D12ConstantBuffer::Unmap()
{
    if (bIsMapped)
        m_spUploadBuffer->Unmap(0, nullptr);
}

D3D12VertexBuffer::SharedPtr D3D12VertexBuffer::CreateD3D12VertexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource)
{
    return std::make_shared<D3D12VertexBuffer>(InDesc, InSubresource);
}

D3D12VertexBuffer::D3D12VertexBuffer(sBufferDesc& InDesc, sBufferSubresource* Subresource)
    : BufferDesc(InDesc)
    , Owner(D3D12DeviceLocator::Get().GetInterface())
    , pData(nullptr)
{
    ID3D12Device* m_device = Owner->GetDevice();
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(BufferDesc.Size);
    HRESULT hr = m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        nullptr,
        IID_PPV_ARGS(&m_pResource));

    auto HeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto UploadDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferDesc.Size);

    hr = m_device->CreateCommittedResource(
        &HeapDesc,
        D3D12_HEAP_FLAG_NONE,
        &UploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&m_spUploadBuffer));

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_pResource->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = (UINT)BufferDesc.Stride;
    m_vertexBufferView.SizeInBytes = (UINT)BufferDesc.Size;

    m_spUploadBuffer->Map(0, nullptr, &pData);

    Owner->GetIMCommandList()->WaitForGPU();
    Owner->GetIMCommandList()->BeginRecordCommandList();
    UpdateSubresource(Owner->GetIMCommandList()->Get(), Subresource);
    Owner->GetIMCommandList()->FinishRecordCommandList();
    Owner->GetIMCommandList()->ExecuteCommandList();
    Owner->GetIMCommandList()->WaitForGPU();
}

std::size_t D3D12VertexBuffer::GetSize() const
{
    return (std::size_t)BufferDesc.Size;
}

D3D12VertexBuffer::~D3D12VertexBuffer()
{
    m_spUploadBuffer->Unmap(0, nullptr);
    m_pResource = nullptr;
    m_spUploadBuffer = nullptr;
    Owner = nullptr;
    pData = nullptr;
}

void D3D12VertexBuffer::ApplyBuffer(ID3D12GraphicsCommandList* InCMDBuffer)
{
    InCMDBuffer->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}

void D3D12VertexBuffer::ResizeBuffer(std::size_t Size, void* InSubresource)
{
}

void D3D12VertexBuffer::UpdateSubresource(ID3D12GraphicsCommandList* pCmdList, sBufferSubresource* Subresource)
{
    if (!Subresource)
        return;

    memcpy((BYTE*)pData + Subresource->Location, Subresource->pSysMem, Subresource->Size);

    D3D12_RESOURCE_BARRIER preCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
    pCmdList->ResourceBarrier(1, &preCopyBarriers);

    pCmdList->CopyBufferRegion(m_pResource.Get(), (UINT64)Subresource->Location, m_spUploadBuffer.Get(), (UINT64)Subresource->Location, (UINT64)Subresource->Size);

    D3D12_RESOURCE_BARRIER postCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    pCmdList->ResourceBarrier(1, &postCopyBarriers);
}

void D3D12VertexBuffer::UpdateSubresource(sBufferSubresource* Subresource)
{
    if (!Subresource)
        return;

    Owner->GetIMCommandList()->BeginRecordCommandList();
    UpdateSubresource(Owner->GetIMCommandList()->Get(), Subresource);
    Owner->GetIMCommandList()->FinishRecordCommandList();
    Owner->GetIMCommandList()->ExecuteCommandList();
}

D3D12IndexBuffer::SharedPtr D3D12IndexBuffer::CreateD3D12IndexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource)
{
    return std::make_shared<D3D12IndexBuffer>(InDesc, InSubresource);
}

D3D12IndexBuffer::D3D12IndexBuffer(sBufferDesc& InDesc, sBufferSubresource* Subresource)
    : BufferDesc(InDesc)
    , Owner(D3D12DeviceLocator::Get().GetInterface())
    , pData(nullptr)
{
    ID3D12Device* m_device = Owner->GetDevice();
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(BufferDesc.Size);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_INDEX_BUFFER,
        nullptr,
        IID_PPV_ARGS(&m_pResource)));

    auto HeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto UploadDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferDesc.Size);

    HRESULT hr = m_device->CreateCommittedResource(
        &HeapDesc,
        D3D12_HEAP_FLAG_NONE,
        &UploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&m_spUploadBuffer));

    // Initialize the vertex buffer view.
    IBView.BufferLocation = m_pResource->GetGPUVirtualAddress();
    IBView.Format = DXGI_FORMAT_R32_UINT;
    IBView.SizeInBytes = (UINT)BufferDesc.Size;

    m_spUploadBuffer->Map(0, nullptr, &pData);

    Owner->GetIMCommandList()->WaitForGPU();
    Owner->GetIMCommandList()->BeginRecordCommandList();
    UpdateSubresource(Owner->GetIMCommandList()->Get(), Subresource);
    Owner->GetIMCommandList()->FinishRecordCommandList();
    Owner->GetIMCommandList()->ExecuteCommandList();
    Owner->GetIMCommandList()->WaitForGPU();
}

std::size_t D3D12IndexBuffer::GetSize() const
{
    return (std::size_t)BufferDesc.Size;
}

void D3D12IndexBuffer::ApplyBuffer(ID3D12GraphicsCommandList* InCMDBuffer)
{
    InCMDBuffer->IASetIndexBuffer(&IBView);
}

D3D12IndexBuffer::~D3D12IndexBuffer()
{
    m_spUploadBuffer->Unmap(0, nullptr);
    m_pResource = nullptr;
    m_spUploadBuffer = nullptr;
    Owner = nullptr;
    pData = nullptr;
}

void D3D12IndexBuffer::ResizeBuffer(std::size_t Size, sBufferSubresource* InSubresource)
{

}

void D3D12IndexBuffer::UpdateSubresource(ID3D12GraphicsCommandList* pCmdList, sBufferSubresource* Subresource)
{
    if (!Subresource)
        return;

    memcpy((BYTE*)pData + Subresource->Location, Subresource->pSysMem, Subresource->Size);

    D3D12_RESOURCE_BARRIER preCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource.Get(), D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
    pCmdList->ResourceBarrier(1, &preCopyBarriers);

    pCmdList->CopyBufferRegion(m_pResource.Get(), (UINT64)Subresource->Location, m_spUploadBuffer.Get(), (UINT64)Subresource->Location, (UINT64)Subresource->Size);

    D3D12_RESOURCE_BARRIER postCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    pCmdList->ResourceBarrier(1, &postCopyBarriers);
}

void D3D12IndexBuffer::UpdateSubresource(sBufferSubresource* Subresource)
{
    if (!Subresource)
        return;

    Owner->GetIMCommandList()->BeginRecordCommandList();
    UpdateSubresource(Owner->GetIMCommandList()->Get(), Subresource);
    Owner->GetIMCommandList()->FinishRecordCommandList();
    Owner->GetIMCommandList()->ExecuteCommandList();
}
