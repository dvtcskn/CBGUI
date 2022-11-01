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
#include <map>
#include <vector>
#include "AbstractLayer.h"

#include "dx12/D3D12Texture.h"
#include "dx12/D3D12CommandBuffer.h"
#include "dx12/D3D12Pipeline.h"

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

class D3D12Material : public std::enable_shared_from_this<D3D12Material>
{
public:
	using SharedPtr = std::shared_ptr<D3D12Material>;
	using WeakPtr = std::weak_ptr<D3D12Material>;
	using UniquePtr = std::unique_ptr<D3D12Material>;

public:
	class D3D12MaterialInstance : public std::enable_shared_from_this<D3D12MaterialInstance>
	{
	protected:
		friend D3D12Material;

		D3D12Material* Parent;
		std::map<std::uint32_t, std::shared_ptr<D3D12Texture>> Textures;
		std::string Name;
		std::string Path;
		std::string ParentName;

	public:
		using SharedPtr = std::shared_ptr<D3D12MaterialInstance>;
		using WeakPtr = std::weak_ptr<D3D12MaterialInstance>;
		using UniquePtr = std::unique_ptr<D3D12MaterialInstance>;

	public:
		D3D12MaterialInstance(D3D12Material* InParent, std::string& InName)
			: Parent(InParent)
			, Name(InName)
			, ParentName(InParent->GetName())
			, Path(InName)
		{}

		virtual ~D3D12MaterialInstance()
		{
			for (auto& Texture : Textures)
			{
				Texture.second = nullptr;
			}
			Textures.clear();

			Parent = nullptr;
		}

		FORCEINLINE std::string GetPath() const { return Path; }
		FORCEINLINE void SetPath(std::string InPath) { Path = InPath; }

		FORCEINLINE D3D12Material* GetParent() const
		{
			return Parent;
		}

		FORCEINLINE std::string GetName() const { return Name; }

		FORCEINLINE D3D12Texture* GetTexture(std::uint32_t index) const { return Textures.at(index).get(); }

		void AddTexture(std::string InName, std::uint32_t InOrder, void* InData, size_t InSize, TextureDesc& InDesc);
		//void AddTexture(std::wstring InPath, std::uint32_t InOrder, std::string InName);
		//void AddTexture(std::wstring InPath, std::uint32_t InOrder);

		void ApplyMaterialInstance(D3D12CommandBuffer* InCMDBuffer = nullptr) const;

		void UpdateTexture(std::size_t TextureIndex, const void* pSrcData, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY);
	};

public:
	D3D12Material(std::string InName, EMaterialBlendMode InBlendMode, D3D12PipelineDesc InPipelineDesc);

	virtual ~D3D12Material();
	D3D12MaterialInstance* CreateInstance(std::string InName);

	void RecompileProgram();

	FORCEINLINE std::string GetName() const { return Name; }
	FORCEINLINE std::string GetPath() const { return Path; }
	FORCEINLINE void SetPath(std::string InPath) { Path = InPath; }
	FORCEINLINE D3D12Pipeline* GetPipeline() const { return Pipeline.get(); }

	FORCEINLINE D3D12MaterialInstance* GetInstance(std::string InName) const
	{
		for (auto& Instance : Instances)
		{
			if (Instance->GetName() == InName)
			{
				return Instance.get();
			}
		}
		return nullptr;
	}

	void ApplyMaterial(D3D12CommandBuffer* InCMDBuffer = nullptr) const;

	EMaterialBlendMode BlendMode;

private:
	friend D3D12MaterialInstance;

	std::string Name;
	std::string Path;
	std::unique_ptr<D3D12Pipeline> Pipeline;

	std::vector<std::shared_ptr<D3D12Material::D3D12MaterialInstance>> Instances;
};
