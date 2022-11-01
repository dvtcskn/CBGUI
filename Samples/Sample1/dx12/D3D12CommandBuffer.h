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

#include <d3d12.h>
#include <wrl\client.h>
using namespace Microsoft::WRL;

#include "WindowsD3D12Device.h"
#include "D3D12FrameBuffer.h"
#include "AbstractLayer.h"

class D3D12CommandBuffer final
{
public:
	D3D12CommandBuffer();
	virtual ~D3D12CommandBuffer();

	FORCEINLINE ID3D12GraphicsCommandList* Get() const { return CommandList.Get(); }
	FORCEINLINE ID3D12Device* GetDevice() const { return Owner->GetDevice(); }

	void BeginRecordCommandList();
	void FinishRecordCommandList();
	void ExecuteCommandList();

	void ResourceBarrier(UINT NumBarriers, const D3D12_RESOURCE_BARRIER* pBarriers);

	void SetHeaps();
	void WaitForGPU();

	void SetScissorRect(std::uint32_t X, std::uint32_t Y, std::uint32_t Z, std::uint32_t W);
	void SetStencilRef(std::uint32_t Ref) { CommandList->OMSetStencilRef(Ref); }
	void SetPipelineState(ID3D12PipelineState* pPipelineState);
	void SetGraphicsRootSignature(ID3D12RootSignature* pRootSignature);
	void SetDescriptorHeaps(std::uint32_t Size, ID3D12DescriptorHeap** Heaps);
	void RSSetViewports(std::uint32_t Size, const D3D12_VIEWPORT* pViewports);
	void IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);

	void Draw(std::uint32_t VertexCount, std::uint32_t VertexStartOffset = 0);
	void DrawIndexedInstanced(std::uint32_t IndexCountPerInstance, std::uint32_t InstanceCount, std::uint32_t StartIndexLocation, INT BaseVertexLocation, std::uint32_t StartInstanceLocation);
	void DrawIndexedInstanced(const ObjectDrawParameters& Params);

	void SetVertexBuffer(AVertexBuffer* VBuffer);
	void SetIndexBuffer(AIndexBuffer* IBuffer);
	void UpdateBufferSubresource(AVertexBuffer* Buffer, sBufferSubresource* Subresource);
	void UpdateBufferSubresource(AVertexBuffer* Buffer, std::size_t Location, std::size_t Size, void* pSrcData);
	void UpdateBufferSubresource(AIndexBuffer* Buffer, sBufferSubresource* Subresource);
	void UpdateBufferSubresource(AIndexBuffer* Buffer, std::size_t Location, std::size_t Size, void* pSrcData);

	void ClearDepthFBO(D3D12FrameBuffer* FBO);
	void ClearBackBuffer();
	void SetBackBufferAsRenderTarget(D3D12FrameBuffer* Stencil = nullptr);
	void ApplyBackBufferAsResource(std::uint32_t StartSlot = 0);

private:
	WindowsD3D12Device* Owner;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
};
