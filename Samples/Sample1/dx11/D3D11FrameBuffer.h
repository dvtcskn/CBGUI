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

#include "WindowsD3D11Device.h"
#include "D3D11CommandBuffer.h"
#include <memory>
#include <vector>
#include "AbstractLayer.h"

class D3D11FrameBuffer final
{
public:
	using SharedPtr = std::shared_ptr<D3D11FrameBuffer>;
	static D3D11FrameBuffer::SharedPtr CreateD3D11FrameBuffer(std::string InName, std::vector<FrameBufferAttachmentInfo> InAttachments, FBODesc InDesc = FBODesc());
	static D3D11FrameBuffer::SharedPtr CreateD3D11FrameBuffer(std::string InName, ComPtr<ID3D11Texture2D> InBackBuffer, std::vector<ComPtr<ID3D11RenderTargetView>> InBackBufferRTs,
																				  ComPtr<ID3D11ShaderResourceView> InBackBufferSRV);

	D3D11FrameBuffer(std::string InName, std::vector<FrameBufferAttachmentInfo> InAttachments, FBODesc InDesc = FBODesc());
	D3D11FrameBuffer(std::string InName, ComPtr<ID3D11Texture2D> InBackBuffer, std::vector<ComPtr<ID3D11RenderTargetView>> InBackBufferRTs, ComPtr<ID3D11ShaderResourceView> InBackBufferSRV);

	virtual ~D3D11FrameBuffer()
	{
		FrameBuffer.clear();
		Owner = nullptr;
	}

	FBODesc GetDesc() const
	{
		return FDesc;
	}
	std::string GetName() const
	{
		return Name;
	}
	std::uint32_t GetFrameBufferCount() const
	{
		return FDesc.FrameBufferCount;
	}
	std::uint32_t GetAttachmentCount() const
	{
		return static_cast<std::uint32_t>(FrameBuffer.size());
	}
	std::uint32_t GetRenderTargetAttachmentCount() const
	{
		return ColorAttachmentCount;
	}

	void ApplyFrameBuffer(D3D11CommandBuffer* InCMDBuffer = nullptr, D3D11FrameBuffer* Stencil = nullptr);
	void ApplyFrameBufferAsResource(std::uint32_t StartSlot, D3D11CommandBuffer* InCMDBuffer = nullptr);

	FORCEINLINE ComPtr<ID3D11DepthStencilView> GetDepthStencil(std::uint32_t InSlot = 0) { return FrameBuffer[DepthIndex].DepthStencilElements[InSlot].Get(); }
	FORCEINLINE ComPtr<ID3D11RenderTargetView> GetRenderTarget(std::uint32_t AttachmentIndex = 0, std::uint32_t InSlot = 0) { return FrameBuffer[AttachmentIndex].RenderTargetElements[InSlot].Get(); }

	void ClearRTs(D3D11CommandBuffer* InCMDBuffer = nullptr);

protected:
	void CreateFrameBuffer(std::vector<FrameBufferAttachmentInfo> InAttachments)
	{
		FrameBuffer.resize(InAttachments.size());

		for (int i = 0; i < InAttachments.size(); i++)
		{
			auto& Attachment = InAttachments[i];
			auto& mFBO = FrameBuffer[i];

			bool InStencil = Attachment.Usage == eImageUsage::Depth;
			ComPtr<ID3D11Texture2D> Texture;
			D3D11_TEXTURE2D_DESC TextureDesc;
			ZeroMemory(&TextureDesc, sizeof(TextureDesc));
			TextureDesc.Width = FDesc.Dimensions.X;
			TextureDesc.Height = FDesc.Dimensions.Y;
			TextureDesc.MipLevels = 1;
			TextureDesc.ArraySize = FDesc.Array.Size;
			TextureDesc.Format = InStencil ? DXGI_FORMAT_R32G8X24_TYPELESS : (Attachment.Usage == eImageUsage::Depth) ? DXGI_FORMAT_R32_TYPELESS : (Attachment.Format);
			TextureDesc.SampleDesc.Count = FDesc.MSLevel.Count;
			TextureDesc.SampleDesc.Quality = FDesc.MSLevel.Quality;
			TextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

			if (Attachment.Usage == eImageUsage::Color)
				TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			else if (Attachment.Usage == eImageUsage::Depth)
				TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			TextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
			TextureDesc.CPUAccessFlags = NULL;
			TextureDesc.MiscFlags = NULL;

			Owner->GetDevice()->CreateTexture2D(&TextureDesc, /*InData ? InData : */NULL, Texture.GetAddressOf());

			mFBO.Texture = Texture;

			{
				{
					Texture->GetDesc(&TextureDesc);
					CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
						!FDesc.Array.bIsArray ? D3D_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
						InStencil && (Attachment.Usage == eImageUsage::Depth) ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : (Attachment.Usage == eImageUsage::Depth) ? DXGI_FORMAT_R32_FLOAT : TextureDesc.Format,
						0, (Attachment.Usage == eImageUsage::Depth) ? 1 : TextureDesc.MipLevels,  // Mips
						0, TextureDesc.ArraySize   // Array
					);

					ComPtr<ID3D11ShaderResourceView> ShaderResource;
					Owner->GetDevice()->CreateShaderResourceView(Texture.Get(), &srvDesc, ShaderResource.GetAddressOf());
					mFBO.ShaderResource = ShaderResource;
				}
				/*for (std::uint32_t i = 0; i < FDesc.Array.Size; ++i) {
					CD3D11_SHADER_RESOURCE_VIEW_DESC srvElementDesc(
						!FDesc.Array.bIsArray ? D3D_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
						InStencil && (Attachment.Usage == EImageUsage::Depth) ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : (Attachment.Usage == EImageUsage::Depth) ? DXGI_FORMAT_R32_FLOAT : TextureDesc.Format,
						0, 1,  // Mips
						i, 1   // Array
					);

					ComPtr<ID3D11ShaderResourceView> ShaderResource;
					Owner->GetDevice()->CreateShaderResourceView(Texture.Get(), &srvElementDesc, ShaderResource.GetAddressOf());
					mFBO.ShaderResourceElements.push_back(ShaderResource);
				}*/
			}
			
			if (Attachment.Usage == eImageUsage::Color)
			{
				for (std::uint32_t i = 0; i < FDesc.Array.Size; ++i) {
					CD3D11_RENDER_TARGET_VIEW_DESC rtvElementDesc(
						!FDesc.Array.bIsArray ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
						TextureDesc.Format,
						0,          // Mips
						i, 1        // Array
					);

					ComPtr<ID3D11RenderTargetView> renderTargetView;
					Owner->GetDevice()->CreateRenderTargetView(Texture.Get(), &rtvElementDesc, renderTargetView.GetAddressOf());
					mFBO.RenderTargetElements.push_back(renderTargetView);
				}
			} 
			else if (Attachment.Usage == eImageUsage::Depth)
			{
				{
					Texture->GetDesc(&TextureDesc);
					CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc(
						!FDesc.Array.bIsArray ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DARRAY,
						InStencil ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT,
						0,          // Mips
						1, 1        // Array
					);

					ComPtr<ID3D11DepthStencilView> depthStencilView;
					Owner->GetDevice()->CreateDepthStencilView(Texture.Get(), &depthStencilDesc, depthStencilView.GetAddressOf());
					mFBO.DepthResource = depthStencilView;
				}
				for (std::uint32_t i = 0; i < FDesc.Array.Size; ++i) {
					CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc(
						!FDesc.Array.bIsArray ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DARRAY,
						InStencil ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT,
						0,          // Mips
						i, 1        // Array
					);

					ComPtr<ID3D11DepthStencilView> depthStencilView;
					Owner->GetDevice()->CreateDepthStencilView(Texture.Get(), &depthStencilDesc, depthStencilView.GetAddressOf());
					mFBO.DepthStencilElements.push_back(depthStencilView);
				}
			}

			if (Attachment.Usage == eImageUsage::Color)
			{
				ColorAttachmentCount++;
				mFBO.InUsage = eImageUsage::Color;
			}
			else if (Attachment.Usage == eImageUsage::Depth)
			{
				mFBO.InUsage = eImageUsage::Depth;
				DepthIndex = ColorAttachmentCount;
			}
		}
	}

private:
	WindowsD3D11Device* Owner;

	FBODesc FDesc;
	std::string Name;

	struct FBO
	{
		eImageUsage InUsage;
		ComPtr<ID3D11Texture2D> Texture;
		ComPtr<ID3D11ShaderResourceView> ShaderResource;
		std::vector<ComPtr<ID3D11ShaderResourceView>> ShaderResourceElements;
		std::vector<ComPtr<ID3D11RenderTargetView>> RenderTargetElements;
		ComPtr<ID3D11DepthStencilView> DepthResource;
		std::vector<ComPtr<ID3D11DepthStencilView>> DepthStencilElements;

		FBO() 
			: InUsage(eImageUsage::UNKNOWN)
			, Texture(nullptr)
			, ShaderResource(nullptr)
			, DepthResource(nullptr)
		{}

		~FBO()
		{
			Texture = nullptr;
			ShaderResource = nullptr;

			for (auto& Res : ShaderResourceElements)
			{
				Res = nullptr;
			}
			ShaderResourceElements.clear();

			for (auto& Res : RenderTargetElements)
			{
				Res = nullptr;
			}
			RenderTargetElements.clear();

			for (auto& Res : DepthStencilElements)
			{
				Res = nullptr;
			}
			DepthStencilElements.clear();
		}
	};

	std::vector<FBO> FrameBuffer;
	std::uint32_t ColorAttachmentCount;

	int DepthIndex;
};
