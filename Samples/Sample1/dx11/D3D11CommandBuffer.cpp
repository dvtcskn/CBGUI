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
#include "D3D11CommandBuffer.h"
#include "D3D11Buffer.h"
#include "D3D11Shader.h"
#include "D3D11Viewport.h"
#include "D3D11Factory.h"

D3D11CommandBuffer::D3D11CommandBuffer()
	: CommandList(nullptr)
	, DeferredCTX(nullptr)
	, Owner(D3D11Factory::Get().GetOwner())
	, StencilRef(0)
	, bIsSingleThreaded(true)
{
	//bIsSingleThreaded = !Owner->IsNvDeviceID();

	if (!bIsSingleThreaded)
	{
		ComPtr<ID3D11DeviceContext> lDirect3DDeviceIMContext;
		Owner->GetDevice()->CreateDeferredContext(0, lDirect3DDeviceIMContext.GetAddressOf());
		lDirect3DDeviceIMContext->QueryInterface(__uuidof (ID3D11DeviceContext1), (void **)&DeferredCTX);
		lDirect3DDeviceIMContext = nullptr;
	} 
	else
	{
		DeferredCTX = Owner->GetDeviceIMContext();
	}
}

D3D11CommandBuffer::~D3D11CommandBuffer()
{
	ClearCMDStates();
	DeferredCTX->ClearState();
	DeferredCTX->Flush();
	DeferredCTX = nullptr;

	CommandList = nullptr;
	Owner = nullptr;
}

void D3D11CommandBuffer::BeginRecordCommandList()
{
	ClearCMDStates();
	ClearState(); 
}

void D3D11CommandBuffer::FinishRecordCommandList()
{
	if (!bIsSingleThreaded)
	{
		DeferredCTX->FinishCommandList(
#if _DEBUG
			false,
#else
			true,
#endif
		CommandList.GetAddressOf());
	}
}

void D3D11CommandBuffer::ExecuteCommandList()
{
	if (!bIsSingleThreaded)
	{
		if (!CommandList)
			return;
		Owner->GetDeviceIMContext()->ExecuteCommandList(CommandList.Get(), 
#if _DEBUG
			false
#else
			true
#endif
		);
		CommandList = nullptr;
	}
}

void D3D11CommandBuffer::ClearState()
{
	DeferredCTX->ClearState();
	StencilRef = 0;
}

void D3D11CommandBuffer::Draw(std::uint32_t VertexCount, std::uint32_t VertexStartOffset)
{
	DeferredCTX->Draw(VertexCount, VertexStartOffset);
}

void D3D11CommandBuffer::DrawIndexedInstanced(std::uint32_t IndexCountPerInstance, std::uint32_t InstanceCount, std::uint32_t StartIndexLocation, INT BaseVertexLocation, std::uint32_t StartInstanceLocation)
{
	DeferredCTX->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);

	ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
	DeferredCTX->PSSetShaderResources(0, (std::uint32_t)std::size(pSRVs), pSRVs);

	ID3D11Buffer* pCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { 0 };
	DeferredCTX->PSSetConstantBuffers(0, (std::uint32_t)std::size(pCBs), pCBs);
}

void D3D11CommandBuffer::DrawIndexedInstanced(const ObjectDrawParameters& DrawParameters)
{
	DrawIndexedInstanced(
		DrawParameters.IndexCountPerInstance,
		DrawParameters.InstanceCount,
		DrawParameters.StartIndexLocation,
		DrawParameters.BaseVertexLocation,
		DrawParameters.StartInstanceLocation);
}

void D3D11CommandBuffer::SetScissorRect(std::uint32_t X, std::uint32_t Y, std::uint32_t Z, std::uint32_t W)
{
	D3D11_RECT Rect;
	Rect.top = X;
	Rect.left = Y;
	Rect.bottom = Z;
	Rect.right = W;
	DeferredCTX->RSSetScissorRects(1, &Rect);
}

void D3D11CommandBuffer::SetRenderTarget(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
	DeferredCTX->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}

void D3D11CommandBuffer::SetVertexBuffer(AVertexBuffer* VBuffer)
{
	if (!VBuffer)
		return;

	static_cast<D3D11VertexBuffer*>(VBuffer->GetInterface())->ApplyBuffer(this);
}

void D3D11CommandBuffer::SetIndexBuffer(AIndexBuffer* IBuffer)
{
	if (!IBuffer)
		return;

	static_cast<D3D11IndexBuffer*>(IBuffer->GetInterface())->ApplyBuffer(this);
}

void D3D11CommandBuffer::UpdateBufferSubresource(AVertexBuffer* Buffer, sBufferSubresource* Subresource)
{
	if (!Subresource || !Buffer)
		return;

	UpdateBufferSubresource(Buffer, Subresource->Location, Subresource->Size, Subresource->pSysMem);
}

void D3D11CommandBuffer::UpdateBufferSubresource(AVertexBuffer* Buffer, std::size_t Location, std::size_t Size, const void* pSrcData)
{
	if (!Buffer)
		return;

	static_cast<D3D11VertexBuffer*>(Buffer->GetInterface())->UpdateSubresource(this, Location, Size, pSrcData);
}

void D3D11CommandBuffer::UpdateBufferSubresource(AIndexBuffer* Buffer, sBufferSubresource* Subresource)
{
	if (!Subresource || !Buffer)
		return;

	UpdateBufferSubresource(Buffer, Subresource->Location, Subresource->Size, Subresource->pSysMem);
}

void D3D11CommandBuffer::UpdateBufferSubresource(AIndexBuffer* Buffer, std::size_t Location, std::size_t Size, const void* pSrcData)
{
	if (!Buffer)
		return;

	static_cast<D3D11IndexBuffer*>(Buffer->GetInterface())->UpdateSubresource(this, Location, Size, pSrcData);
}

void D3D11CommandBuffer::ClearBackBuffer()
{
	const auto Viewport = Owner->GetViewportContext();
	const auto BackBuffer = Viewport->GetBackBuffer();
	BackBuffer->ClearRTs(this);
}

void D3D11CommandBuffer::SetBackBufferAsRenderTarget(D3D11FrameBuffer* Stencil)
{
	const auto Viewport = Owner->GetViewportContext();
	const auto BackBuffer = Viewport->GetBackBuffer();
	BackBuffer->ApplyFrameBuffer(this, Stencil);
}

void D3D11CommandBuffer::ApplyBackBufferAsResource(std::uint32_t StartSlot)
{
	const auto Viewport = Owner->GetViewportContext();
	const auto BackBuffer = Viewport->GetBackBuffer();
	BackBuffer->ApplyFrameBufferAsResource(StartSlot, this);
}

void D3D11CommandBuffer::ClearCMDStates()
{
	// DeferredCTX->ClearState();
	//
	// Unbind IB and VB
	//

	ID3D11Buffer* pVBs[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
	std::uint32_t countsAndOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
	DeferredCTX->IASetInputLayout(NULL);
	DeferredCTX->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DeferredCTX->IASetVertexBuffers(0, (std::uint32_t)std::size(pVBs), pVBs, countsAndOffsets, countsAndOffsets);

	//
	// Unbind shaders
	//
	DeferredCTX->VSSetShader(NULL, NULL, 0);
	DeferredCTX->GSSetShader(NULL, NULL, 0);
	DeferredCTX->PSSetShader(NULL, NULL, 0);
	DeferredCTX->CSSetShader(NULL, NULL, 0);

	//
	// Unbind resources
	//
	ID3D11RenderTargetView *pRTVs[8] = { nullptr };
	DeferredCTX->OMSetRenderTargets((std::uint32_t)std::size(pRTVs), pRTVs, NULL);

	ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
	DeferredCTX->VSSetShaderResources(0, (std::uint32_t)std::size(pSRVs), pSRVs);
	DeferredCTX->GSSetShaderResources(0, (std::uint32_t)(std::uint32_t)std::size(pSRVs), pSRVs);
	DeferredCTX->PSSetShaderResources(0, (std::uint32_t)std::size(pSRVs), pSRVs);
	DeferredCTX->CSSetShaderResources(0, (std::uint32_t)std::size(pSRVs), pSRVs);

	ID3D11UnorderedAccessView* pUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT] = { 0 };
	std::uint32_t pUAVInitialCounts[D3D11_PS_CS_UAV_REGISTER_COUNT] = { 0 };
	DeferredCTX->CSSetUnorderedAccessViews(0, (std::uint32_t)std::size(pUAVs), pUAVs, pUAVInitialCounts);

	ID3D11Buffer* pCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { 0 };
	DeferredCTX->VSSetConstantBuffers(0, (std::uint32_t)std::size(pCBs), pCBs);
	DeferredCTX->GSSetConstantBuffers(0, (std::uint32_t)std::size(pCBs), pCBs);
	DeferredCTX->PSSetConstantBuffers(0, (std::uint32_t)std::size(pCBs), pCBs);
	DeferredCTX->CSSetConstantBuffers(0, (std::uint32_t)std::size(pCBs), pCBs);

	ID3D11SamplerState* pSs[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { 0 };
	DeferredCTX->VSSetSamplers(NULL, (std::uint32_t)std::size(pSs), pSs);
	DeferredCTX->PSSetSamplers(NULL, (std::uint32_t)std::size(pSs), pSs);
	DeferredCTX->DSSetSamplers(NULL, (std::uint32_t)std::size(pSs), pSs);
	DeferredCTX->HSSetSamplers(NULL, (std::uint32_t)std::size(pSs), pSs);
	DeferredCTX->CSSetSamplers(NULL, (std::uint32_t)std::size(pSs), pSs);

	DeferredCTX->RSSetState(NULL);
	DeferredCTX->OMSetDepthStencilState(nullptr, 0);
	DeferredCTX->OMSetBlendState(nullptr, 0, 0xFFFFFFFF);

	StencilRef = 0;
}
