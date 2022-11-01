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
#include "D3D11Buffer.h"
#include "D3D11Factory.h"

D3D11Buffer::D3D11Buffer(std::string InName, sBufferDesc InDesc, D3D11_SUBRESOURCE_DATA* InSubresource, ResourceTypeFlags InType)
	: Owner(D3D11Factory::Get().GetOwner())
	, Buffer(nullptr)
	, Name(InName)
{
	CreateBuffer(InDesc, InSubresource, InType);
}

D3D11Buffer::D3D11Buffer(sBufferDesc InDesc, D3D11_SUBRESOURCE_DATA* InSubresource, ResourceTypeFlags InType, bool InDynamic)
	: Owner(D3D11Factory::Get().GetOwner())
	, Buffer(nullptr)
	, Name("")
{
	CreateBuffer(InDesc, InSubresource, InType, InDynamic);
}

void D3D11Buffer::CreateBuffer(sBufferDesc& InDesc, D3D11_SUBRESOURCE_DATA * InSubresource, ResourceTypeFlags InType, bool InDynamic)
{
	auto BindFlag = [&](ResourceTypeFlags InAType) -> std::uint32_t
	{
		switch (InAType)
		{
		case D3D11Buffer::ResourceTypeFlags::eDEFAULT:
			return NULL;
			break;
		case D3D11Buffer::ResourceTypeFlags::eVERTEX_BUFFER:
			return D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			break;
		case D3D11Buffer::ResourceTypeFlags::eINDEX_BUFFER:
			return D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			break;
		case D3D11Buffer::ResourceTypeFlags::eCONSTANT_BUFFER:
			return D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
			break;
		case D3D11Buffer::ResourceTypeFlags::eSTRUCTURED_BUFFER:
			return /*D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS |*/ D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			break;
		}
		return NULL;
	};

	BufferDesc = InDesc;

	D3D11_SUBRESOURCE_DATA SUBRESOURCE = { 0 };
	if (InSubresource)
	{
		SUBRESOURCE.pSysMem = InSubresource->pSysMem;
		SUBRESOURCE.SysMemPitch = InSubresource->SysMemPitch;
		SUBRESOURCE.SysMemSlicePitch = InSubresource->SysMemSlicePitch;
	};

	D3D11_BUFFER_DESC DESC = { 0 };
	DESC.ByteWidth = static_cast<std::uint32_t>(BufferDesc.Size);
	DESC.StructureByteStride = static_cast<std::uint32_t>(BufferDesc.Stride);
	DESC.BindFlags = BindFlag(InType);
	DESC.Usage = InDynamic ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT;
	DESC.CPUAccessFlags = InDynamic ? D3D11_CPU_ACCESS_WRITE : NULL;

	if (InType == ResourceTypeFlags::eCONSTANT_BUFFER || InType == ResourceTypeFlags::eSTRUCTURED_BUFFER)
	{
		DESC.Usage = D3D11_USAGE_DYNAMIC;
		DESC.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (InType == ResourceTypeFlags::eSTRUCTURED_BUFFER) 
		{
			DESC.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		}
	}

	ID3D11Device1* d3dDevice = Owner->GetDevice();
	d3dDevice->CreateBuffer(&DESC, InSubresource ? &SUBRESOURCE : NULL, &Buffer);
}

void D3D11Buffer::Map(const void* Ptr, D3D11_MAP Map, ID3D11DeviceContext1* InRHICommandList)
{
	ID3D11DeviceContext1* CTX = InRHICommandList ? InRHICommandList : Owner->GetDeviceIMContext();

	HRESULT HR;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HR = CTX->Map(Buffer.Get(), 0, Map, 0, &mappedResource);
	memcpy(mappedResource.pData, Ptr, BufferDesc.Size);
	CTX->Unmap(Buffer.Get(), 0);
}

bool D3D11Buffer::IsMapable() const
{
	D3D11_BUFFER_DESC DESC = { 0 };
	Buffer->GetDesc(&DESC);
	return DESC.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC && DESC.CPUAccessFlags == D3D11_CPU_ACCESS_WRITE;
}

void D3D11ConstantBuffer::ApplyConstantBuffer(D3D11CommandBuffer* InCMDBuffer)
{
	ID3D11DeviceContext1* CMD = InCMDBuffer ? static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX() : GetOwner()->GetDeviceIMContext();

	if (ShaderVisibility.size() > 0)
	{
		for (const auto& Visibility : ShaderVisibility)
		{
			switch (Visibility)
			{
			case eShaderType::Vertex:
				CMD->VSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
				break;
			case eShaderType::Pixel:
				CMD->PSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
				break;
			case eShaderType::Geometry:
				CMD->GSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
				break;
			case eShaderType::Compute:
				CMD->CSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
				break;
			case eShaderType::HULL:
				CMD->HSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
				break;
			case eShaderType::Domain:
				CMD->DSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
				break;
			}
		}
	}
	else
	{
		CMD->VSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
		CMD->PSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
		CMD->GSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
		CMD->CSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
		CMD->HSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
		CMD->DSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
	}
}

D3D11IndexBuffer::SharedPtr D3D11IndexBuffer::CreateD3D11IndexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource)
{
	D3D11_SUBRESOURCE_DATA SUBRESOURCE_DATA;
	SUBRESOURCE_DATA.SysMemPitch = NULL;
	SUBRESOURCE_DATA.SysMemSlicePitch = NULL;

	if (InSubresource)
		SUBRESOURCE_DATA.pSysMem = InSubresource->pSysMem;
	else
		SUBRESOURCE_DATA.pSysMem = nullptr;

	return std::make_unique<D3D11IndexBuffer>(InDesc, InSubresource ? &SUBRESOURCE_DATA : nullptr);
}

void D3D11IndexBuffer::ResizeBuffer(std::size_t Size, D3D11_SUBRESOURCE_DATA* Subresource)
{
	Buffer = nullptr;
	BufferDesc.Size = Size;
	CreateBuffer(BufferDesc, Subresource, ResourceTypeFlags::eINDEX_BUFFER);
}

void D3D11IndexBuffer::ApplyBuffer(ID3D11DeviceContext1* InRHICommandList)
{
	if (InRHICommandList)
	{
		std::uint32_t offset = NULL;
		InRHICommandList->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, offset);
	}
	else
	{
		std::uint32_t offset = NULL;
		GetOwner()->GetDeviceIMContext()->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, offset);
	}
}

void D3D11IndexBuffer::UpdateSubresource(std::size_t Location, std::size_t Size, const void* pSrcData)
{
	D3D11_BOX box{};
	box.left = (std::uint32_t)Location;
	box.right = BufferDesc.Size < Size ? (std::uint32_t)(Location + BufferDesc.Size) : (std::uint32_t)(Location + Size);
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	if (box.left >= box.right)
		return;

	GetOwner()->GetDeviceIMContext()->UpdateSubresource1(Buffer.Get(), 0, &box, pSrcData, 0, 0, 0);
}

void D3D11IndexBuffer::UpdateSubresource(sBufferSubresource* Subresource)
{
	if (!Subresource)
		return;

	D3D11_BOX box{};
	box.left = (std::uint32_t)Subresource->Location;
	box.right = BufferDesc.Size < Subresource->Size ? (std::uint32_t)(Subresource->Location + BufferDesc.Size) : (std::uint32_t)(Subresource->Location + Subresource->Size);
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	if (box.left >= box.right)
		return;

	GetOwner()->GetDeviceIMContext()->UpdateSubresource1(Buffer.Get(), 0, &box, Subresource->pSysMem, 0, 0, 0);
}

void D3D11IndexBuffer::UpdateSubresource(D3D11CommandBuffer* InCMDBuffer, std::size_t Location, std::size_t Size, const void* pSrcData)
{
	D3D11_BOX box{};
	box.left = (std::uint32_t)Location;
	box.right = BufferDesc.Size < Size ? (std::uint32_t)(Location + BufferDesc.Size) : (std::uint32_t)(Location + Size);
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	if (box.left >= box.right)
		return;

	if (InCMDBuffer)
	{
		const auto& CTX = static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX();

		/*HRESULT hr = S_OK;
		bool needWorkaround = false;
		D3D11_DEVICE_CONTEXT_TYPE contextType = CTX->GetType();

		if (D3D11_DEVICE_CONTEXT_DEFERRED == contextType)
		{
			D3D11_FEATURE_DATA_THREADING threadingCaps = { FALSE, FALSE };

			hr = Owner->GetDevice()->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingCaps, sizeof(threadingCaps));
			if (SUCCEEDED(hr))
			{
				if (!threadingCaps.DriverCommandLists)
				{
					needWorkaround = true;
				}
			}
		}

		const void* pAdjustedSrcData = pSrcData;

		const std::uint32_t srcRowPitch = 0;
		const std::uint32_t srcDepthPitch = 0;
		const std::uint32_t dstSubresource = 0;

		if (needWorkaround)
		{
			D3D11_BOX alignedBox = box;

			const bool m_bBC = true;
			// convert from pixels to blocks
			if (m_bBC)
			{
				alignedBox.left /= 4;
				alignedBox.right /= 4;
				alignedBox.top /= 4;
				alignedBox.bottom /= 4;
			}

			const std::uint32_t srcBytesPerElement = 0;
			pAdjustedSrcData = ((const BYTE*)pSrcData) - (alignedBox.front * srcDepthPitch) - (alignedBox.top * srcRowPitch) - (alignedBox.left * srcBytesPerElement);
		}

		CTX->UpdateSubresource1(Buffer.Get(), dstSubresource, &box, pAdjustedSrcData, srcRowPitch, srcDepthPitch, 0);*/

		CTX->UpdateSubresource1(Buffer.Get(), 0, &box, pSrcData, 0, 0, 0);
	}
	else
	{
		GetOwner()->GetDeviceIMContext()->UpdateSubresource1(Buffer.Get(), 0, &box, pSrcData, 0, 0, 0);
	}
}

D3D11VertexBuffer::SharedPtr D3D11VertexBuffer::CreateD3D11VertexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource)
{
	D3D11_SUBRESOURCE_DATA SUBRESOURCE_DATA;
	SUBRESOURCE_DATA.SysMemPitch = NULL;
	SUBRESOURCE_DATA.SysMemSlicePitch = NULL;

	if (InSubresource)
		SUBRESOURCE_DATA.pSysMem = InSubresource->pSysMem;
	else
		SUBRESOURCE_DATA.pSysMem = nullptr;

	return std::make_shared<D3D11VertexBuffer>(InDesc, InSubresource ? &SUBRESOURCE_DATA : nullptr);
}

void D3D11VertexBuffer::ResizeBuffer(std::size_t Size, D3D11_SUBRESOURCE_DATA* Subresource)
{
	Buffer = nullptr;
	BufferDesc.Size = Size;
	CreateBuffer(BufferDesc, Subresource, ResourceTypeFlags::eVERTEX_BUFFER);
}

void D3D11VertexBuffer::ApplyBuffer(ID3D11DeviceContext1* InRHICommandList)
{
	if (InRHICommandList)
	{
		std::uint32_t offset = NULL;
		std::uint32_t Stride = static_cast<std::uint32_t>(BufferDesc.Stride);
		InRHICommandList->IASetVertexBuffers(0, 1, Buffer.GetAddressOf(), &Stride, &offset);
	}
	else
	{
		std::uint32_t offset = NULL;
		std::uint32_t Stride = static_cast<std::uint32_t>(BufferDesc.Stride);
		GetOwner()->GetDeviceIMContext()->IASetVertexBuffers(0, 1, Buffer.GetAddressOf(), &Stride, &offset);
	}
}

void D3D11VertexBuffer::UpdateSubresource(std::size_t Location, std::size_t Size, const void* pSrcData)
{
	D3D11_BOX box{};
	box.left = (std::uint32_t)Location;
	box.right = BufferDesc.Size < Size ? (std::uint32_t)(Location + BufferDesc.Size) : (std::uint32_t)(Location + Size);
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	if (box.left >= box.right)
		return;

	GetOwner()->GetDeviceIMContext()->UpdateSubresource1(Buffer.Get(), 0, &box, pSrcData, 0, 0, 0);
}

void D3D11VertexBuffer::UpdateSubresource(sBufferSubresource* Subresource)
{
	if (!Subresource)
		return;

	D3D11_BOX box{};
	box.left = (std::uint32_t)Subresource->Location;
	box.right = BufferDesc.Size < Subresource->Size ? (std::uint32_t)(Subresource->Location + BufferDesc.Size) : (std::uint32_t)(Subresource->Location + Subresource->Size);
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	if (box.left >= box.right)
		return;

	GetOwner()->GetDeviceIMContext()->UpdateSubresource1(Buffer.Get(), 0, &box, Subresource->pSysMem, 0, 0, 0);
}

void D3D11VertexBuffer::UpdateSubresource(D3D11CommandBuffer* InCMDBuffer, std::size_t Location, std::size_t Size, const void* pSrcData)
{
	D3D11_BOX box{};
	box.left = (std::uint32_t)Location;
	box.right = BufferDesc.Size < Size ? (std::uint32_t)(Location + BufferDesc.Size) : (std::uint32_t)(Location + Size);
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	if (box.left >= box.right)
		return;

	if (InCMDBuffer)
	{
		const auto& CTX = static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX();

		/*HRESULT hr = S_OK;
		bool needWorkaround = false;
		D3D11_DEVICE_CONTEXT_TYPE contextType = CTX->GetType();

		if (D3D11_DEVICE_CONTEXT_DEFERRED == contextType)
		{
			D3D11_FEATURE_DATA_THREADING threadingCaps = { FALSE, FALSE };

			hr = Owner->GetDevice()->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingCaps, sizeof(threadingCaps));
			if (SUCCEEDED(hr))
			{
				if (!threadingCaps.DriverCommandLists)
				{
					needWorkaround = true;
				}
			}
		}

		const void* pAdjustedSrcData = pSrcData;

		const std::uint32_t srcRowPitch = 0;
		const std::uint32_t srcDepthPitch = 0;
		const std::uint32_t dstSubresource = 0;

		if (needWorkaround)
		{
			D3D11_BOX alignedBox = box;

			const bool m_bBC = true;
			// convert from pixels to blocks
			if (m_bBC)
			{
				alignedBox.left /= 4;
				alignedBox.right /= 4;
				alignedBox.top /= 4;
				alignedBox.bottom /= 4;
			}

			const std::uint32_t srcBytesPerElement = 0;
			pAdjustedSrcData = ((const BYTE*)pSrcData) - (alignedBox.front * srcDepthPitch) - (alignedBox.top * srcRowPitch) - (alignedBox.left * srcBytesPerElement);
		}

		CTX->UpdateSubresource1(Buffer.Get(), dstSubresource, &box, pAdjustedSrcData, srcRowPitch, srcDepthPitch, 0);*/

		CTX->UpdateSubresource1(Buffer.Get(), 0, &box, pSrcData, 0, 0, 0);
	}
	else
	{
		GetOwner()->GetDeviceIMContext()->UpdateSubresource1(Buffer.Get(), 0, &box, pSrcData, 0, 0, 0);
	}
}
