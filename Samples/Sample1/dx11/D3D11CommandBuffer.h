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

#include <d3d11_1.h>
#include <wrl\client.h>
using namespace Microsoft::WRL;

#include "WindowsD3D11Device.h"
#include "AbstractLayer.h"

class D3D11FrameBuffer;

class D3D11CommandBuffer final
{
public:
	D3D11CommandBuffer();
	virtual ~D3D11CommandBuffer();

	FORCEINLINE ID3D11DeviceContext1* GetDeferredCTX() const { return DeferredCTX.Get(); }

	void BeginRecordCommandList();
	void FinishRecordCommandList();
	void ExecuteCommandList();
	void ClearState();

	void SetScissorRect(std::uint32_t X, std::uint32_t Y, std::uint32_t Z, std::uint32_t W);
	void SetStencilRef(std::uint32_t Ref) { StencilRef = Ref; }
	std::uint32_t GetStencilRef() const { return StencilRef; }

	void Draw(std::uint32_t VertexCount, std::uint32_t VertexStartOffset = 0);
	void DrawIndexedInstanced(std::uint32_t IndexCountPerInstance, std::uint32_t InstanceCount, std::uint32_t StartIndexLocation, INT BaseVertexLocation, std::uint32_t StartInstanceLocation);
	void DrawIndexedInstanced(const ObjectDrawParameters& Params);

	void ClearBackBuffer();
	void SetBackBufferAsRenderTarget(D3D11FrameBuffer* Stencil = nullptr);
	void ApplyBackBufferAsResource(std::uint32_t StartSlot = 0);

	void SetVertexBuffer(AVertexBuffer* VBuffer);
	void SetIndexBuffer(AIndexBuffer* IBuffer);
	void UpdateBufferSubresource(AVertexBuffer* Buffer, sBufferSubresource* Subresource);
	void UpdateBufferSubresource(AVertexBuffer* Buffer, std::size_t Location, std::size_t Size, const void* pSrcData);
	void UpdateBufferSubresource(AIndexBuffer* Buffer, sBufferSubresource* Subresource);
	void UpdateBufferSubresource(AIndexBuffer* Buffer, std::size_t Location, std::size_t Size, const void* pSrcData);

	void ClearCMDStates();
	
	void SetRenderTarget(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);

private:
	WindowsD3D11Device* Owner;
	ComPtr<ID3D11CommandList> CommandList;
	ComPtr<ID3D11DeviceContext1> DeferredCTX;

	bool bIsSingleThreaded;
	std::uint32_t StencilRef;
};
