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
#pragma once

#include "WindowsD3D12Device.h"
#include "D3D12DescriptorHeapManager.h"
#include "AbstractLayer.h"

class D3D12ConstantBuffer
{
    D3D12DescriptorHandle ViewHeap;
    ComPtr<ID3D12Resource> m_spUploadBuffer;
    sBufferDesc BufferDesc;
    WindowsD3D12Device* Owner;
    void* Ptr;
    bool bIsMapped;
public:
    D3D12ConstantBuffer(std::string Name, sBufferDesc& InDesc);
    virtual ~D3D12ConstantBuffer();
    std::size_t GetSize() const;
    void ApplyBuffer(UINT RootParameterIndex, ID3D12GraphicsCommandList* InCMDBuffer);

    void Map(const void* Ptr);
    void Unmap();

    D3D12DescriptorHandle GetHeapHandle() const { return ViewHeap; }
};

class D3D12VertexBuffer : public IVertexBuffer
{
    ComPtr<ID3D12Resource> m_pResource;
    ComPtr<ID3D12Resource> m_spUploadBuffer;

    sBufferDesc BufferDesc;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    WindowsD3D12Device* Owner;
    void* pData;
public:
    using SharedPtr = std::shared_ptr<D3D12VertexBuffer>;
    static D3D12VertexBuffer::SharedPtr CreateD3D12VertexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource = nullptr);

public:
    D3D12VertexBuffer(sBufferDesc& InDesc, sBufferSubresource* Subresource);
    virtual ~D3D12VertexBuffer();
    std::size_t GetSize() const;
    void ApplyBuffer(ID3D12GraphicsCommandList* InCMDBuffer);
    void ResizeBuffer(std::size_t Size, void* InSubresource = nullptr);
    void UpdateSubresource(ID3D12GraphicsCommandList* InCMDBuffer, sBufferSubresource* Subresource = nullptr);
    virtual void UpdateSubresource(sBufferSubresource* Subresource) override;

    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return m_vertexBufferView; }
};

class D3D12IndexBuffer : public IIndexBuffer
{
    ComPtr<ID3D12Resource> m_pResource;
    ComPtr<ID3D12Resource> m_spUploadBuffer;

    sBufferDesc BufferDesc;
    D3D12_INDEX_BUFFER_VIEW IBView;
    WindowsD3D12Device* Owner;
    void* pData;
public:
    using SharedPtr = std::shared_ptr<D3D12IndexBuffer>;
    static D3D12IndexBuffer::SharedPtr CreateD3D12IndexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource = nullptr);

public:
    D3D12IndexBuffer(sBufferDesc& InDesc, sBufferSubresource* Subresource);
    virtual ~D3D12IndexBuffer();
    std::size_t GetSize() const;
    void ApplyBuffer(ID3D12GraphicsCommandList* InCMDBuffer);
    void ResizeBuffer(std::size_t Size, sBufferSubresource* Subresource = nullptr);
    void UpdateSubresource(ID3D12GraphicsCommandList* InCMDBuffer, sBufferSubresource* Subresource = nullptr);
    virtual void UpdateSubresource(sBufferSubresource* Subresource) override;
};
