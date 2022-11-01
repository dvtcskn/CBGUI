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

#include <assert.h>
#include "d3dx12.h"

class WindowsD3D12Device;

struct D3D12DescriptorHandle
{
public:
    D3D12DescriptorHandle(/*D3D12DescriptorHeap* Owner,*/ D3D12_DESCRIPTOR_HEAP_TYPE Type, const uint32_t inDescriptorSize = 1);
    virtual ~D3D12DescriptorHandle() = default;

    inline void Reset(D3D12_CPU_DESCRIPTOR_HANDLE inCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE inGPUDescriptor)
    {
        CPUDescriptor = inCPUDescriptor;
        GPUDescriptor = inGPUDescriptor;
    }

    inline uint32_t GetSize() const { return DescriptorSize; }
    inline D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const { return Type; }

    inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPU(const uint32_t i = 0) const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(CPUDescriptor, i * IncrementSize);
    }

    inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(const uint32_t i = 0) const
    {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(GPUDescriptor, i * IncrementSize);
    }

private:
    D3D12_DESCRIPTOR_HEAP_TYPE Type;
    uint32_t DescriptorSize;
    uint32_t IncrementSize;

    CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptor;
    CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptor;

    //D3D12DescriptorHeap* Owner;
};

class D3D12DescriptorHeapManager
{
private:
    struct D3D12DescriptorHeap
    {
    public:
        D3D12DescriptorHeap(D3D12DescriptorHeapManager* Owner, ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_TYPE Type, const uint32_t Count);
        ~D3D12DescriptorHeap();

        inline bool AllocateDescriptor(D3D12DescriptorHandle* DescriptorHandle)
        {
            if ((DescriptorCount + DescriptorHandle->GetSize()) > TotalDescriptorSize)
                return false;

            DescriptorHandle->Reset(CD3DX12_CPU_DESCRIPTOR_HANDLE(Heap->GetCPUDescriptorHandleForHeapStart(), DescriptorCount * IncrementSize), 
                                    Heap->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ?
                                    CD3DX12_GPU_DESCRIPTOR_HANDLE(Heap->GetGPUDescriptorHandleForHeapStart(), DescriptorCount * IncrementSize) 
                                    : D3D12_GPU_DESCRIPTOR_HANDLE());

            DescriptorCount += DescriptorHandle->GetSize();

            return true;
        }

        void Free(D3D12DescriptorHandle* Handle)
        {
            //TotalFreedDescriptorSize += Handle->GetSize();

            //if (TotalFreedDescriptorSize >= TotalDescriptorSize)
            //{
            //    Owner->Free(this);
            //}
        }

        inline ID3D12DescriptorHeap* GetHeap() const { return Heap; }

    private:
        uint32_t IncrementSize;
        uint32_t DescriptorCount;
        uint32_t TotalDescriptorSize;
        //uint32_t TotalFreedDescriptorSize;

        ID3D12DescriptorHeap* Heap;
        D3D12DescriptorHeapManager* Owner;
    };

public:
    D3D12DescriptorHeapManager();
    ~D3D12DescriptorHeapManager();

    inline void AllocateDescriptor(D3D12DescriptorHandle* DescriptorHandle)
    {
        /*
        * TO DO
        * Check if there is an available Heap or create a new one.
        * Return Handle ?
        */
        switch (DescriptorHandle->GetHeapType())
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV: RTV_Heap->AllocateDescriptor(DescriptorHandle); break;
            case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV: DSV_Heap->AllocateDescriptor(DescriptorHandle); break;
            case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: CBV_SRV_UAV_Heap->AllocateDescriptor(DescriptorHandle); break;
            case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: Sampler_Heap->AllocateDescriptor(DescriptorHandle); break;
        }
    }

    void SetHeaps(ID3D12GraphicsCommandList* cmd);

protected:
    void Free(D3D12DescriptorHeap* Heap)
    {
        // TO DO
        // Destroy existing one and create new D3D12DescriptorHeap
    }

private:
    WindowsD3D12Device* Owner;
    // Make vectors
    std::unique_ptr<D3D12DescriptorHeap> RTV_Heap;
    std::unique_ptr<D3D12DescriptorHeap> DSV_Heap;
    std::unique_ptr<D3D12DescriptorHeap> CBV_SRV_UAV_Heap;
    std::unique_ptr<D3D12DescriptorHeap> Sampler_Heap;
};
