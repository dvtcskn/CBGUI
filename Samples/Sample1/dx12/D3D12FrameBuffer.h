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
#include "D3D12DeviceLocator.h"
#include "D3D12DescriptorHeapManager.h"
#include "AbstractLayer.h"

class D3D12FrameBuffer
{
	struct FBO
	{
		eImageUsage InUsage;
		ComPtr<ID3D12Resource> Texture;
		D3D12DescriptorHandle RTs;
		D3D12DescriptorHandle DSV;
		D3D12DescriptorHandle SRV;

		FBO()
			: InUsage(eImageUsage::UNKNOWN)
			, RTs(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
			, DSV(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV))
			, SRV(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
		{}

		~FBO()
		{
			Texture = nullptr;
		}
	};

private:
	WindowsD3D12Device* Owner;
	FBODesc Desc;
	std::string Name;
	std::uint32_t ColorAttachmentCount;
	std::uint32_t DepthAttachmentCount;
	std::vector<FBO> FrameBuffer;

public:
	D3D12FrameBuffer(std::string InName, std::vector<FrameBufferAttachmentInfo> InAttachments, FBODesc InDesc = FBODesc())
		: Owner(D3D12DeviceLocator::Get().GetInterface())
		, Desc(InDesc)
		, Name(InName)
		, ColorAttachmentCount(NULL)
		, DepthAttachmentCount(0)
	{
		CreateFrameBuffer(InAttachments);
	}

	D3D12FrameBuffer(std::string InName, const std::vector<ComPtr<ID3D12Resource>>& InBuffer, const std::vector<D3D12DescriptorHandle>& InBufferRTs, 
					 const std::vector<D3D12DescriptorHandle>& InBufferSRV)
		: Owner(D3D12DeviceLocator::Get().GetInterface())
		, Desc(FBODesc())
		, Name(InName)
		, ColorAttachmentCount(NULL)
		, DepthAttachmentCount(0)
	{
		for (std::size_t i = 0; i < InBuffer.size(); i++)
		{
			FBO mFBO;
			mFBO.InUsage = eImageUsage::Color;
			mFBO.Texture = InBuffer[i];
			mFBO.RTs = InBufferRTs[i];
			mFBO.SRV = InBufferSRV[i];
			FrameBuffer.push_back(mFBO);
			Desc.FrameBufferCount++;
		}
	}

	~D3D12FrameBuffer()
	{
		FrameBuffer.clear();
		Owner = nullptr;
	}

	ID3D12Resource* GetTexture() const
	{
		return FrameBuffer.at(0).Texture.Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTCPUDescHandle() const
	{
		return FrameBuffer.at(0).RTs.GetCPU();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescHandle() const
	{
		return FrameBuffer.at(0).DSV.GetCPU();
	}

private:
	void CreateFrameBuffer(std::vector<FrameBufferAttachmentInfo> InAttachments)
	{
		auto Manager = Owner->GetDescriptorHeapManager();

		FrameBuffer.resize(InAttachments.size());

		for (int i = 0; i < InAttachments.size(); i++)
		{
			auto& Attachment = InAttachments[i];
			auto& mFBO = FrameBuffer[i];
			ComPtr<ID3D12Resource> m_pResource = nullptr;

			std::uint32_t mipLevel = -1;
			std::uint32_t arraySize = -1;
			std::uint32_t firstArraySlice = -1;

			CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(Attachment.Format, Desc.Dimensions.X, Desc.Dimensions.Y, 1, 1, 1, 0, 
				Attachment.Usage == eImageUsage::Depth? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

			CreateTexture(mFBO, resourceDesc, Attachment.Usage);

			if (Attachment.Usage == eImageUsage::Color)
				CreateRTV(mFBO, resourceDesc, mipLevel, arraySize, firstArraySlice);
			else
				CreateDSV(mFBO, resourceDesc, arraySize, firstArraySlice);

			CreateSRV(mFBO, resourceDesc, mipLevel, arraySize, firstArraySlice);

			if (Attachment.Usage == eImageUsage::Color)
			{
				mFBO.InUsage = eImageUsage::Color;
				ColorAttachmentCount++;
			}
			else if (Attachment.Usage == eImageUsage::Depth)
			{
				mFBO.InUsage = eImageUsage::Depth;
				DepthAttachmentCount++;
			}
		}
	}

	void CreateTexture(FBO& pFBO, CD3DX12_RESOURCE_DESC& Desc, eImageUsage Usage)
	{
		D3D12_CLEAR_VALUE clearValue = {};
		D3D12_RESOURCE_STATES states = {};

		if (Usage == eImageUsage::Color)
		{
			clearValue.Format = Desc.Format;
			clearValue.Color[0] = 0.0f;
			clearValue.Color[1] = 0.0f;
			clearValue.Color[2] = 0.0f;
			clearValue.Color[3] = 0.0f;

			const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			if (clearColor)
			{
				clearValue.Color[0] = clearColor[0];
				clearValue.Color[1] = clearColor[1];
				clearValue.Color[2] = clearColor[2];
				clearValue.Color[3] = clearColor[3];
			}

			states = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		else if (Usage == eImageUsage::Depth)
		{
			clearValue.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;// (Desc.Format == (DXGI_FORMAT_R32_TYPELESS || DXGI_FORMAT_R32G8X24_TYPELESS)) ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : Desc.Format;
			clearValue.DepthStencil.Depth = 0.0f;
			clearValue.DepthStencil.Stencil = 0;

			states = D3D12_RESOURCE_STATE_COMMON;
			if (Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE)
			    states |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			states |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}

		auto HeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = Owner->GetDevice()->CreateCommittedResource(
			&HeapDesc,
			D3D12_HEAP_FLAG_NONE,
			&Desc,
			states,
			&clearValue,
			IID_PPV_ARGS(&pFBO.Texture));

		assert(hr == S_OK);

#if _DEBUG
		pFBO.Texture->SetName(L"FrameBuffer");
#endif
	}

	void CreateRTV(FBO& pFBO, CD3DX12_RESOURCE_DESC& Desc, INT mipLevel = -1, INT arraySize = -1, INT firstArraySlice = -1)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

		rtvDesc.Format = Desc.Format;

		if (Desc.DepthOrArraySize == 1)
		{
			if (Desc.SampleDesc.Count == 1)
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = (mipLevel == -1) ? 0 : mipLevel;
			}
			else
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			}
		}
		else
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.ArraySize = arraySize;
			rtvDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
			rtvDesc.Texture2DArray.MipSlice = (mipLevel == -1) ? 0 : mipLevel;
		}

		{
			ID3D12Device* pDevice = Owner->GetDevice();
			Owner->GetDescriptorHeapManager()->AllocateDescriptor(&pFBO.RTs);
			UINT index = 0;
			pDevice->CreateRenderTargetView(pFBO.Texture.Get(), &rtvDesc, pFBO.RTs.GetCPU(index));
		}
	}

	void CreateSRV(FBO& pFBO, CD3DX12_RESOURCE_DESC& Desc, INT mipLevel = -1, INT arraySize = -1, INT firstArraySlice = -1);

	void CreateDSV(FBO& pFBO, CD3DX12_RESOURCE_DESC& Desc, std::int32_t arraySlice = -1, std::int32_t arraySize = -1)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC DSViewDesc = {};
		DSViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		if (Desc.SampleDesc.Count == 1)
		{
			if (Desc.DepthOrArraySize == 1)
			{
				DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				DSViewDesc.Texture2D.MipSlice = 0;
			}
			else
			{
				DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				DSViewDesc.Texture2DArray.MipSlice = 0;
				DSViewDesc.Texture2DArray.FirstArraySlice = arraySlice;
				DSViewDesc.Texture2DArray.ArraySize = arraySize;
			}
		}
		else
		{
			DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		}

		ID3D12Device* pDevice = Owner->GetDevice();
		Owner->GetDescriptorHeapManager()->AllocateDescriptor(&pFBO.DSV);
		UINT index = 0;
		pDevice->CreateDepthStencilView(pFBO.Texture.Get(), &DSViewDesc, pFBO.DSV.GetCPU(index));
	}
};
