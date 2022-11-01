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

#include <string>
#include <vector>
#include <d3d11.h>
#include <d3d12.h>
#include <assert.h>
#include <wrl/client.h>
#include "AbstractLayer.h"
#include "D3D11Shader.h"
#include "D3D11CommandBuffer.h"

using namespace Microsoft::WRL;

class D3D11Buffer
{
protected:
	enum class ResourceTypeFlags : std::uint8_t
	{
		eDEFAULT,
		eVERTEX_BUFFER,
		eINDEX_BUFFER,
		eCONSTANT_BUFFER,
		eSTRUCTURED_BUFFER,
	};

private:
	WindowsD3D11Device* Owner;
	std::string Name;

protected:
	ComPtr<ID3D11Buffer> Buffer;
	sBufferDesc BufferDesc;

protected:
	D3D11Buffer(std::string InName, sBufferDesc InDesc, D3D11_SUBRESOURCE_DATA* InSubresource = NULL, ResourceTypeFlags InType = ResourceTypeFlags::eDEFAULT);
	D3D11Buffer(sBufferDesc InDesc, D3D11_SUBRESOURCE_DATA* InSubresource = NULL, ResourceTypeFlags InType = ResourceTypeFlags::eDEFAULT, bool InDynamic = false);

	void CreateBuffer(sBufferDesc& InDesc, D3D11_SUBRESOURCE_DATA* InSubresource = NULL, ResourceTypeFlags InType = ResourceTypeFlags::eDEFAULT, bool InDynamic = false);

	WindowsD3D11Device* GetOwner() const { return Owner; }

public:
	using SharedPtr = std::shared_ptr<D3D11Buffer>;
	
	virtual ~D3D11Buffer() {
		Release();
	}

	void Release()
	{
		Buffer = nullptr;
		Owner = nullptr;
	}

	bool IsMapable() const;

	FORCEINLINE ComPtr<ID3D11Buffer> GetBuffer() { return Buffer; }
	FORCEINLINE sBufferDesc GetBufferDesc() { return BufferDesc; }

	void Map(const void* Ptr, D3D11_MAP Map = D3D11_MAP::D3D11_MAP_WRITE_DISCARD, ID3D11DeviceContext1* InRHICommandList = nullptr);
};

class D3D11ConstantBuffer final : public D3D11Buffer
{
public:
	using SharedPtr = std::shared_ptr<D3D11ConstantBuffer>;
private:
	std::uint32_t Slot;
	std::vector<eShaderType> ShaderVisibility;

public:
	D3D11ConstantBuffer(std::string InName, sBufferDesc& InDesc, std::uint32_t InSlot, std::vector<eShaderType> Visibility = std::vector<eShaderType>())
		: D3D11Buffer(InName, InDesc, NULL, ResourceTypeFlags::eCONSTANT_BUFFER)
		, Slot(InSlot)
		, ShaderVisibility(Visibility)
	{}

	virtual ~D3D11ConstantBuffer() = default;

	void SetSlot(std::uint32_t inSlot) { Slot = inSlot; }
	void ApplyConstantBuffer(D3D11CommandBuffer* InCMDBuffer = nullptr);
};

class D3D11IndexBuffer final : public D3D11Buffer, public IIndexBuffer
{
public:
	using SharedPtr = std::shared_ptr<D3D11IndexBuffer>;
	static D3D11IndexBuffer::SharedPtr CreateD3D11IndexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource = nullptr);

public:
	D3D11IndexBuffer(sBufferDesc& InDesc, D3D11_SUBRESOURCE_DATA* InSubresource = nullptr)
		: D3D11Buffer(InDesc, InSubresource, ResourceTypeFlags::eINDEX_BUFFER)
	{}

	virtual ~D3D11IndexBuffer() = default;

	void ResizeBuffer(std::size_t Size, D3D11_SUBRESOURCE_DATA* Subresource = NULL);

	void ApplyBuffer(ID3D11DeviceContext1* InRHICommandList = nullptr);

	void ApplyBuffer(D3D11CommandBuffer* InCMDBuffer = nullptr)
	{
		if (InCMDBuffer)
			ApplyBuffer(static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX());
		else
			ApplyBuffer(GetOwner()->GetDeviceIMContext());
	}

	virtual void UpdateSubresource(sBufferSubresource* Subresource) override final;
	void UpdateSubresource(std::size_t Location, std::size_t Size, const void* pSrcData);
	void UpdateSubresource(D3D11CommandBuffer* InCMDBuffer, std::size_t Location, std::size_t Size, const void* pSrcData);
};

class D3D11VertexBuffer final : public D3D11Buffer, public IVertexBuffer
{
public:
	using SharedPtr = std::shared_ptr<D3D11VertexBuffer>;
	static D3D11VertexBuffer::SharedPtr CreateD3D11VertexBuffer(sBufferDesc& InDesc, sBufferSubresource* InSubresource = nullptr);

public:
	D3D11VertexBuffer(sBufferDesc& InDesc, D3D11_SUBRESOURCE_DATA* InSubresource = nullptr)
		: D3D11Buffer(InDesc, InSubresource, ResourceTypeFlags::eVERTEX_BUFFER)
	{}

	virtual ~D3D11VertexBuffer() = default;

	void ResizeBuffer(std::size_t Size, D3D11_SUBRESOURCE_DATA* Subresource = NULL);

	void ApplyBuffer(ID3D11DeviceContext1* InRHICommandList = nullptr);

	void ApplyBuffer(D3D11CommandBuffer* InCMDBuffer = nullptr)
	{
		if (InCMDBuffer)
			ApplyBuffer(static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX());
		else
			ApplyBuffer(GetOwner()->GetDeviceIMContext());
	}

	virtual void UpdateSubresource(sBufferSubresource* Subresource) override final;
	void UpdateSubresource(std::size_t Location, std::size_t Size, const void* pSrcData);
	void UpdateSubresource(D3D11CommandBuffer* InCMDBuffer, std::size_t Location, std::size_t Size, const void* pSrcData);
};
