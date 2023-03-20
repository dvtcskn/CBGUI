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
#include "D3D11Material.h"

//void D3D11Material::MaterialInstance::AddTexture(std::wstring InPath, std::uint32_t InOrder, std::string InName)
//{
//	Textures.insert({ InOrder, D3D11Texture::CreateD3D11Texture(InPath, InName)});
//}
//
void D3D11Material::MaterialInstance::AddTexture(std::string InName, std::uint32_t InOrder, void* InData, size_t InSize, TextureDesc& InDesc)
{
	Textures.insert({ InOrder, std::make_shared<D3D11Texture>(Name + "_" + std::to_string(Textures.size()), InData, InSize, InDesc) });
}
//
//void D3D11Material::MaterialInstance::AddTexture(std::wstring InPath, std::uint32_t InOrder)
//{
//	Textures.insert({ InOrder, D3D11Texture::CreateD3D11Texture(InPath, Name + "_" + std::to_string(Textures.size())) });
//}

void D3D11Material::MaterialInstance::UpdateTexture(std::size_t TextureIndex, const void* pSrcData, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY)
{
	Textures[(uint32_t)TextureIndex]->UpdateTexture(pSrcData, RowPitch, MinX, MinY, MaxX, MaxY);
}

void D3D11Material::MaterialInstance::ApplyMaterialInstance(D3D11CommandBuffer* InCMDBuffer) const
{
	for (auto& Texture : Textures)
	{
		Texture.second->ApplyTexture(Texture.first, InCMDBuffer);
	}
}

D3D11Material::D3D11Material(std::string InName, EMaterialBlendMode InBlendMode, PipelineDesc InPipelineDesc)
	: Name(InName)
	, BlendMode(InBlendMode)
	, Path(InName)
{
	Pipeline = std::make_unique<D3D11Pipeline>(InName, InPipelineDesc);
}

D3D11Material::~D3D11Material()
{
	for (auto& Instance : Instances)
	{
		Instance = nullptr;
	}
	Instances.clear();
	Pipeline = nullptr;
}

D3D11Material::MaterialInstance* D3D11Material::CreateInstance(std::string InName)
{
	auto Mat = GetInstance(InName);
	if (Mat)
	{
		return Mat;
	}

	auto Instance = std::make_shared<D3D11Material::MaterialInstance>(this, InName);
	Instances.push_back(Instance);
	return Instance.get();
}

void D3D11Material::RecompileProgram()
{
	Pipeline->Recompile();
}

void D3D11Material::ApplyMaterial(D3D11CommandBuffer* InCMDBuffer) const
{
	InCMDBuffer->SetPipeline(Pipeline.get());
}
