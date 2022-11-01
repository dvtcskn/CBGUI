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
#include "D3D12Shader.h"
#include "D3D12Viewport.h"
#include "D3D12DeviceLocator.h"

D3D12CommandBuffer::D3D12CommandBuffer()
	: Owner(D3D12DeviceLocator::Get().GetInterface())
{
	auto Device = Owner->GetDevice();
	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)));
	CommandList->Close();
}

D3D12CommandBuffer::~D3D12CommandBuffer()
{
	CommandList = nullptr;
	CommandAllocator = nullptr;
	Owner = nullptr;
}

void D3D12CommandBuffer::BeginRecordCommandList()
{
	//Owner->GetViewportContext()->WaitForGpu();

	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(CommandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
}

void D3D12CommandBuffer::FinishRecordCommandList()
{
	CommandList->Close();
}

void D3D12CommandBuffer::ExecuteCommandList()
{
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { CommandList.Get() };
	Owner->GetGraphicsQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	//Owner->GetViewportContext()->WaitForGpu();
}

void D3D12CommandBuffer::ResourceBarrier(UINT NumBarriers, const D3D12_RESOURCE_BARRIER* pBarriers)
{
	CommandList->ResourceBarrier(NumBarriers, pBarriers);
}

void D3D12CommandBuffer::SetPipelineState(ID3D12PipelineState* pPipelineState)
{
	CommandList->SetPipelineState(pPipelineState);
}

void D3D12CommandBuffer::SetGraphicsRootSignature(ID3D12RootSignature* pRootSignature)
{
	CommandList->SetGraphicsRootSignature(pRootSignature);
}

void D3D12CommandBuffer::SetDescriptorHeaps(std::uint32_t Size, ID3D12DescriptorHeap** Heaps)
{
	CommandList->SetDescriptorHeaps(Size, Heaps);
}

void D3D12CommandBuffer::RSSetViewports(std::uint32_t Size, const D3D12_VIEWPORT* pViewports)
{
	CommandList->RSSetViewports(Size, pViewports);
}

void D3D12CommandBuffer::IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
{
	CommandList->IASetPrimitiveTopology(PrimitiveTopology);
}

void D3D12CommandBuffer::SetHeaps() 
{
	Owner->GetDescriptorHeapManager()->SetHeaps(CommandList.Get());
}

void D3D12CommandBuffer::WaitForGPU()
{
	const auto Viewport = Owner->GetViewportContext();
	Viewport->WaitForGpu();
}

void D3D12CommandBuffer::Draw(std::uint32_t VertexCount, std::uint32_t VertexStartOffset)
{
	CommandList->DrawInstanced(VertexCount, 1, VertexStartOffset, 0);
}

void D3D12CommandBuffer::DrawIndexedInstanced(std::uint32_t IndexCountPerInstance, std::uint32_t InstanceCount, std::uint32_t StartIndexLocation, INT BaseVertexLocation, std::uint32_t StartInstanceLocation)
{
	CommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void D3D12CommandBuffer::DrawIndexedInstanced(const ObjectDrawParameters& DrawParameters)
{
	DrawIndexedInstanced(
		DrawParameters.IndexCountPerInstance,
		DrawParameters.InstanceCount,
		DrawParameters.StartIndexLocation,
		DrawParameters.BaseVertexLocation,
		DrawParameters.StartInstanceLocation);
}

void D3D12CommandBuffer::SetVertexBuffer(AVertexBuffer* VBuffer)
{
	if (!VBuffer)
		return;

	static_cast<D3D12VertexBuffer*>(VBuffer->GetInterface())->ApplyBuffer(CommandList.Get());
}

void D3D12CommandBuffer::SetIndexBuffer(AIndexBuffer* IBuffer)
{
	if (!IBuffer)
		return;

	static_cast<D3D12IndexBuffer*>(IBuffer->GetInterface())->ApplyBuffer(CommandList.Get());
}

void D3D12CommandBuffer::UpdateBufferSubresource(AVertexBuffer* Buffer, sBufferSubresource* Subresource)
{
	if (!Subresource || !Buffer)
		return;

	static_cast<D3D12VertexBuffer*>(Buffer->GetInterface())->UpdateSubresource(CommandList.Get(), Subresource);
}

void D3D12CommandBuffer::UpdateBufferSubresource(AVertexBuffer* Buffer, std::size_t Location, std::size_t Size, void* pSrcData)
{
	if (!Buffer || !pSrcData)
		return;

	sBufferSubresource Subresource;
	Subresource.Location = Location;
	Subresource.pSysMem = pSrcData;
	Subresource.Size = Size;

	UpdateBufferSubresource(Buffer, &Subresource);
}

void D3D12CommandBuffer::UpdateBufferSubresource(AIndexBuffer* Buffer, sBufferSubresource* Subresource)
{
	if (!Subresource || !Buffer)
		return;

	static_cast<D3D12IndexBuffer*>(Buffer->GetInterface())->UpdateSubresource(CommandList.Get(), Subresource);
}

void D3D12CommandBuffer::UpdateBufferSubresource(AIndexBuffer* Buffer, std::size_t Location, std::size_t Size, void* pSrcData)
{
	if (!Buffer || !pSrcData)
		return;

	sBufferSubresource Subresource;
	Subresource.Location = Location;
	Subresource.pSysMem = pSrcData;
	Subresource.Size = Size;

	UpdateBufferSubresource(Buffer, &Subresource);
}

void D3D12CommandBuffer::ClearDepthFBO(D3D12FrameBuffer* FBO)
{
	CommandList->ClearDepthStencilView(FBO->GetDSVCPUDescHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 0.0f, 0, 0, nullptr);
}

void D3D12CommandBuffer::ClearBackBuffer()
{
	const auto Viewport = Owner->GetViewportContext();
	auto FrameBuffer = Viewport->GetFrameBuffer();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(FrameBuffer->GetRTCPUDescHandle(), 0, Owner->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	// Record commands.
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void D3D12CommandBuffer::SetBackBufferAsRenderTarget(D3D12FrameBuffer* Stencil)
{
	const auto Viewport = Owner->GetViewportContext();
	auto FrameBuffer = Viewport->GetFrameBuffer();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(FrameBuffer->GetRTCPUDescHandle(), 0, Owner->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	if (Stencil)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(Stencil->GetDSVCPUDescHandle());
		CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	}
	else
	{
		CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	}
}

void D3D12CommandBuffer::ApplyBackBufferAsResource(std::uint32_t StartSlot)
{
	const auto Viewport = Owner->GetViewportContext();
	auto FrameBuffer = Viewport->GetFrameBuffer();

	//BackBuffer->ApplyFrameBufferAsResource(StartSlot, this);
}

void D3D12CommandBuffer::SetScissorRect(std::uint32_t X, std::uint32_t Y, std::uint32_t Z, std::uint32_t W)
{
	D3D12_RECT Rect;
	Rect.top = X;
	Rect.left = Y;
	Rect.bottom = Z;
	Rect.right = W;
	CommandList->RSSetScissorRects(1, &Rect);
}
