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
#include "D3D11FrameBuffer.h"
#include "D3D11Factory.h"
#include "D3D11CommandBuffer.h"

D3D11FrameBuffer::SharedPtr D3D11FrameBuffer::CreateD3D11FrameBuffer(std::string InName, std::vector<FrameBufferAttachmentInfo> InAttachments, FBODesc InDesc)
{
	return std::make_shared<D3D11FrameBuffer>(InName, InAttachments, InDesc);
}

D3D11FrameBuffer::SharedPtr D3D11FrameBuffer::CreateD3D11FrameBuffer(std::string InName, ComPtr<ID3D11Texture2D> InBackBuffer, std::vector<ComPtr<ID3D11RenderTargetView>> InBackBufferRTs,
																						 ComPtr<ID3D11ShaderResourceView> InBackBufferSRV)
{
	return std::make_shared<D3D11FrameBuffer>(InName, InBackBuffer, InBackBufferRTs, InBackBufferSRV);
}

D3D11FrameBuffer::D3D11FrameBuffer(std::string InName, std::vector<FrameBufferAttachmentInfo> InAttachments, FBODesc InDesc)
	: Owner(D3D11Factory::Get().GetOwner())
	, FDesc(InDesc)
	, Name(InName)
	, ColorAttachmentCount(NULL)
	, DepthIndex(-1)
{
	CreateFrameBuffer(InAttachments);
}

D3D11FrameBuffer::D3D11FrameBuffer(std::string InName, ComPtr<ID3D11Texture2D> InBackBuffer, std::vector<ComPtr<ID3D11RenderTargetView>> InBackBufferRTs, ComPtr<ID3D11ShaderResourceView> InBackBufferSRV)
	: Owner(D3D11Factory::Get().GetOwner())
	, FDesc(FBODesc())
	, Name(InName)
	, ColorAttachmentCount(NULL)
	, DepthIndex(-1)
{
	FBO mFBO;
	mFBO.InUsage = eImageUsage::Color;
	mFBO.Texture = InBackBuffer;
	mFBO.RenderTargetElements = InBackBufferRTs;
	mFBO.ShaderResource = InBackBufferSRV;
	FrameBuffer.push_back(mFBO);
	FDesc.FrameBufferCount = 1;
}

void D3D11FrameBuffer::ApplyFrameBuffer(D3D11CommandBuffer* InCMDBuffer, D3D11FrameBuffer* Stencil)
{
	ID3D11DeviceContext1* CMD = InCMDBuffer ? static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX() : Owner->GetDeviceIMContext();
	
	std::vector<ComPtr<ID3D11RenderTargetView>> RenderTargetElements;
	//std::vector<ComPtr<ID3D11DepthStencilView>> DepthStencilElements;
	for (auto& FB : FrameBuffer)
	{
		if (FB.RenderTargetElements.size() > 0)
		{
			for (auto& RenderTargetElement : FB.RenderTargetElements)
			{
				RenderTargetElements.push_back(RenderTargetElement);
			}
		}
		/*if (FB.DepthStencilElements.size() > 0)
		{
			for (auto& DepthStencilElement : FB.DepthStencilElements)
			{
				DepthStencilElements.push_back(DepthStencilElement);
			}
		}*/
	}

	if (Stencil)
	{
		D3D11FrameBuffer* FBO = static_cast<D3D11FrameBuffer*>(Stencil);
		auto Depth = FBO->GetDepthStencil();
		//CMD->OMSetRenderTargets(RenderTargetElements.size(), RenderTargetElements.front().GetAddressOf(), DepthStencilElements.size() > 0 ? DepthStencilElements[0].Get() : NULL);
		CMD->OMSetRenderTargets(static_cast<std::uint32_t>(RenderTargetElements.size()), RenderTargetElements.data()->GetAddressOf(), Depth.Get());
	}
	else
	{
		//CMD->OMSetRenderTargets(RenderTargetElements.size(), RenderTargetElements.front().GetAddressOf(), DepthStencilElements.size() > 0 ? DepthStencilElements[0].Get() : NULL);
		CMD->OMSetRenderTargets(static_cast<std::uint32_t>(RenderTargetElements.size()), RenderTargetElements.data()->GetAddressOf(), FrameBuffer.size() >= DepthIndex ? FrameBuffer[DepthIndex].DepthResource ? FrameBuffer[DepthIndex].DepthResource.Get() : NULL : NULL);
	}

	CMD = nullptr;
}

void D3D11FrameBuffer::ApplyFrameBufferAsResource(std::uint32_t StartSlot, D3D11CommandBuffer* InCMDBuffer)
{
	ID3D11DeviceContext1* CMD = InCMDBuffer ? static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX() : Owner->GetDeviceIMContext();
	int i = StartSlot;
	for (auto& FB : FrameBuffer)
	{
		if (i == DepthIndex)
			break;
		if(FB.ShaderResource)
			CMD->PSSetShaderResources(i, 1, FB.ShaderResource.GetAddressOf());
		i++;
	}

	CMD = nullptr;
}

void D3D11FrameBuffer::ClearRTs(D3D11CommandBuffer* InCMDBuffer)
{
	ID3D11DeviceContext1* CMD = InCMDBuffer ? static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX() : Owner->GetDeviceIMContext();
	for (auto& FB : FrameBuffer)
	{
		if (FB.RenderTargetElements.size() > 0)
		{
			for (auto& RenderTargetElement : FB.RenderTargetElements)
			{
				float zeros[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				CMD->ClearRenderTargetView(RenderTargetElement.Get(), zeros);
			}
		}
		if (FB.DepthStencilElements.size() > 0)
		{
			std::uint32_t ClearFlags = 0;
			ClearFlags |= D3D11_CLEAR_DEPTH;
			ClearFlags |= D3D11_CLEAR_STENCIL;
			CMD->ClearDepthStencilView(FB.DepthResource.Get(), ClearFlags, 0.0f, (UINT8)0);

			for (auto& DepthStencilElement : FB.DepthStencilElements)
			{
				std::uint32_t ClearFlags = 0;
				ClearFlags |= D3D11_CLEAR_DEPTH;
				ClearFlags |= D3D11_CLEAR_STENCIL;
				CMD->ClearDepthStencilView(DepthStencilElement.Get(), ClearFlags, 0.0f, (UINT8)0);
			}
		}
	}

	CMD = nullptr;
}
