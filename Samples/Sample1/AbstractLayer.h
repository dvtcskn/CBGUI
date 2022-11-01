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

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <dxgi1_6.h>
#include <stdexcept>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

using Microsoft::WRL::ComPtr;

inline void ThrowIfFailed(const HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::runtime_error("HRESULT : " + std::to_string(hr));
	}
}

struct sDisplayMode
{
	struct sRefreshRate
	{
		UINT Numerator = 0;
		UINT Denominator = 0;
	};

	UINT Width = 0;
	UINT Height = 0;
	sRefreshRate RefreshRate;
};

class AbstractGI
{
protected:
	AbstractGI() = default;
public:
	virtual ~AbstractGI() = default;
	virtual void InitWindow(std::uint32_t InWidth, std::uint32_t InHeight, bool bFullscreen, HWND InHWND) = 0;

	virtual void Present() = 0;

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) = 0;
	virtual void FullScreen(const bool value) = 0;
	virtual bool IsFullScreen() const = 0;
	virtual void Vsync(const bool value) = 0;
	virtual bool IsVsyncEnabled() const = 0;
	virtual void VsyncInterval(const UINT value) = 0;
	virtual UINT GetVsyncInterval() const = 0;

	virtual std::vector<sDisplayMode> GetAllSupportedResolutions() const = 0;
};

class IRenderer
{
protected:
	IRenderer() = default;
public:
	virtual ~IRenderer() = default;
public:
	virtual void BeginPlay() = 0;
	virtual void Tick(const float DeltaTime) = 0;

	virtual void Render() = 0;
	virtual void ResizeWindow(std::size_t Width, std::size_t Height) = 0;
	virtual void InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

enum class EMaterialBlendMode
{
	Opaque,
	Masked,
};

struct VertexAttributeDesc
{
	std::string name;
	DXGI_FORMAT format;
	uint32_t bufferIndex;
	uint32_t offset;
	bool isInstanced;
};

struct ObjectDrawParameters
{
	std::uint32_t IndexCountPerInstance;
	std::uint32_t InstanceCount;
	std::uint32_t StartIndexLocation;
	std::int32_t BaseVertexLocation;
	std::uint32_t StartInstanceLocation;

	explicit ObjectDrawParameters()
		: IndexCountPerInstance(NULL)
		, InstanceCount(1)
		, StartIndexLocation(NULL)
		, BaseVertexLocation(NULL)
		, StartInstanceLocation(NULL)
	{}
};

struct TextureDesc
{
	struct Size
	{
		std::uint32_t X;
		std::uint32_t Y;
		Size()
			: X(NULL)
			, Y(NULL)
		{}
	};

	Size Dimensions;
	uint32_t MipLevels;
	uint32_t LayerCount;
	DXGI_FORMAT Format;

	TextureDesc()
		: MipLevels(NULL)
		, LayerCount(NULL)
		, Format(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN)
	{}
};

struct ShaderDefines
{
	std::string Name;
	std::string Definition;
};

enum class eShaderType : std::uint8_t
{
	Vertex,
	Pixel,
	Geometry,
	Compute,
	HULL,
	Domain,
	Mesh,
	Amplification,
};

struct ShaderAttachment
{
	std::wstring Location;
	std::string FunctionName;
	eShaderType Type;
	std::vector<ShaderDefines> ShaderDefines;
	void* ByteCode;
	std::size_t Size;

	ShaderAttachment() = default;
	ShaderAttachment(std::wstring InLocation, std::string inEntryName, eShaderType InType)
		: Location(InLocation)
		, FunctionName(inEntryName)
		, Type(InType)
		, ByteCode(nullptr)
		, Size(0)
	{}
	ShaderAttachment(void* pByteCode, std::size_t inSize, std::string inEntryName, eShaderType InType)
		: Location(L"")
		, FunctionName(inEntryName)
		, Type(InType)
		, ByteCode(pByteCode)
		, Size(inSize)
	{}

	~ShaderAttachment()
	{
		ShaderDefines.clear();
		ByteCode = nullptr;
	}
};

enum class eImageUsage
{
	UNKNOWN,
	Color,
	Depth,
};

struct FBODesc
{
	struct Size
	{
		std::uint32_t X;
		std::uint32_t Y;
		Size()
			: X(NULL)
			, Y(NULL)
		{}
	};
	struct MSLevel
	{
		std::uint32_t Count;
		std::uint32_t Quality;
		MSLevel()
			: Count(1)
			, Quality(NULL)
		{}
	};
	struct ArraySize
	{
		std::uint32_t Size;
		bool bIsArray;
		ArraySize()
			: Size(1)
			, bIsArray(false)
		{}
	};

	Size Dimensions;
	std::uint32_t FrameBufferCount;
	ArraySize Array;
	MSLevel MSLevel;

	FBODesc()
		: FrameBufferCount(1)
	{}
};

struct FrameBufferAttachmentInfo
{
	DXGI_FORMAT Format;
	eImageUsage Usage;

	FrameBufferAttachmentInfo()
		: Format(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN)
		, Usage(eImageUsage::Color)
	{}
};

struct sBufferDesc
{
	std::uint64_t Size;
	std::uint64_t Stride;

	explicit sBufferDesc()
		: Size(NULL)
		, Stride(NULL)
	{}

	sBufferDesc(std::uint64_t InBufferSize, std::uint64_t InStride = NULL)
		: Size(InBufferSize)
		, Stride(InStride)
	{}

	~sBufferDesc()
	{
		Size = NULL;
		Stride = NULL;
	}
};

struct sBufferSubresource
{
	void* pSysMem;
	std::size_t Size;
	std::size_t Location;

	explicit sBufferSubresource()
		: pSysMem(NULL)
		, Size(NULL)
		, Location(NULL)
	{}
};

class IVertexBuffer 
{
protected:
	IVertexBuffer() = default;
public:
	using SharedPtr = std::shared_ptr<IVertexBuffer>;
	using WeakPtr = std::weak_ptr<IVertexBuffer>;
	using UniquePtr = std::unique_ptr<IVertexBuffer>;

	virtual ~IVertexBuffer() = default;

	virtual void UpdateSubresource(sBufferSubresource* Subresource) = 0;
};

class IIndexBuffer
{
protected:
	IIndexBuffer() = default;
public:
	using SharedPtr = std::shared_ptr<IIndexBuffer>;
	using WeakPtr = std::weak_ptr<IIndexBuffer>;
	using UniquePtr = std::unique_ptr<IIndexBuffer>;

	virtual ~IIndexBuffer() = default;

	virtual void UpdateSubresource(sBufferSubresource* Subresource) = 0;
};

class AbstractLayer
{
private:
	AbstractLayer() = default;
	AbstractLayer(const AbstractLayer& Other) = delete;
	AbstractLayer& operator=(const AbstractLayer&) = delete;

public:
	std::function<IVertexBuffer::SharedPtr(sBufferDesc&, sBufferSubresource*)> fCreateVertexBufferObject;
	std::function<IIndexBuffer::SharedPtr(sBufferDesc&, sBufferSubresource*)> fCreateIndexBufferObject;

public:
	~AbstractLayer()
	{
		fCreateVertexBufferObject = nullptr;
		fCreateIndexBufferObject = nullptr;
	}

	static AbstractLayer& Get()
	{
		static AbstractLayer instance;
		return instance;
	}

	void RegisterVertexBufferObject(std::function<IVertexBuffer::SharedPtr(sBufferDesc&, sBufferSubresource*)> pfnCreate)
	{
		fCreateVertexBufferObject = pfnCreate;
	}
	IVertexBuffer::SharedPtr CreateVertexBufferObject(sBufferDesc& InData, sBufferSubresource* Subresource)
	{
		return fCreateVertexBufferObject(InData, Subresource);
	}

	void RegisterIndexBufferObject(std::function<IIndexBuffer::SharedPtr(sBufferDesc&, sBufferSubresource*)> pfnCreate)
	{
		fCreateIndexBufferObject = pfnCreate;
	}
	IIndexBuffer::SharedPtr CreateIndexBufferObject(sBufferDesc& InData, sBufferSubresource* Subresource)
	{
		return fCreateIndexBufferObject(InData, Subresource);
	}
};

class AVertexBuffer
{
public:
	using SharedPtr = std::shared_ptr<AVertexBuffer>;
	using WeakPtr = std::weak_ptr<AVertexBuffer>;
	using UniquePtr = std::unique_ptr<AVertexBuffer>;

public:
	AVertexBuffer(sBufferDesc InDesc, sBufferSubresource* Subresource)
	{
		Interface = AbstractLayer::Get().CreateVertexBufferObject(InDesc, Subresource);
	}

	virtual ~AVertexBuffer()
	{
		Interface = nullptr;
	}

	void UpdateSubresource(std::size_t Location, std::size_t Size, void* pSrcData)
	{
		sBufferSubresource Subresource;
		Subresource.Location = Location;
		Subresource.pSysMem = pSrcData;
		Subresource.Size = Size;
		Interface->UpdateSubresource(&Subresource);
	}
	void UpdateSubresource(sBufferSubresource* Subresource)
	{
		Interface->UpdateSubresource(Subresource);
	}

	FORCEINLINE IVertexBuffer* GetInterface() const { return Interface.get(); }

private:
	std::shared_ptr<IVertexBuffer> Interface;
};

class AIndexBuffer
{
public:
	using SharedPtr = std::shared_ptr<AIndexBuffer>;
	using WeakPtr = std::weak_ptr<AIndexBuffer>;
	using UniquePtr = std::unique_ptr<AIndexBuffer>;

public:
	AIndexBuffer(sBufferDesc InDesc, sBufferSubresource* Subresource)
	{
		Interface = AbstractLayer::Get().CreateIndexBufferObject(InDesc, Subresource);
	}

	virtual ~AIndexBuffer()
	{
		Interface = nullptr;
	}

	void UpdateSubresource(std::size_t Location, std::size_t Size, void* pSrcData)
	{
		sBufferSubresource Subresource;
		Subresource.Location = Location;
		Subresource.pSysMem = pSrcData;
		Subresource.Size = Size;
		Interface->UpdateSubresource(&Subresource);
	}
	void UpdateSubresource(sBufferSubresource* Subresource)
	{
		Interface->UpdateSubresource(Subresource);
	}

	FORCEINLINE IIndexBuffer* GetInterface() const { return Interface.get(); }

private:
	std::shared_ptr<IIndexBuffer> Interface;
};
