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
#include <map>
#include "D3D11Shader.h"
#include "D3D11ShaderStates.h"
#include "D3D11CommandBuffer.h"
#include "AbstractLayer.h"

struct PipelineDesc
{
public:
	PipelineDesc() = default;
	~PipelineDesc()
	{
		ShaderAttachments.clear();
		VertexLayout.clear();
	}
	CD3D11_SAMPLER_DESC SamplerAttribute = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	CD3D11_RASTERIZER_DESC1 RasterizerAttribute = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
	CD3D11_DEPTH_STENCIL_DESC DepthStencilAttribute = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
	CD3D11_BLEND_DESC1 BlendAttribute = CD3D11_BLEND_DESC1(D3D11_DEFAULT);
	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;

	std::vector<ShaderAttachment> ShaderAttachments;
	std::vector<VertexAttributeDesc> VertexLayout;
};

class D3D11Pipeline
{
public:
	D3D11Pipeline(std::string InName, PipelineDesc InDesc);

	virtual ~D3D11Pipeline()
	{
		Owner = nullptr;
		ClearPipelineState();
	}

	void ApplyPipeline(D3D11CommandBuffer* InCMDBuffer = nullptr) const;
	void ClearPipelineState();
	PipelineDesc GetPipelineDesc() const { return Desc; }

	std::vector<ShaderAttachment> GetShaderAttachments() const { return ShaderAttachments; }
	std::vector<VertexAttributeDesc> GetVertexAttribute() const { return VertexAttribute->GetVertexAttributeDesc(); }

	void Recompile();

protected:
	void CreatePipelineState(PipelineDesc InDesc);

	WindowsD3D11Device* Owner;
	PipelineDesc Desc;

	ComPtr<ID3D11BlendState1> BlendState;
	ComPtr<ID3D11DepthStencilState> DepthStencilState;
	ComPtr<ID3D11RasterizerState1> RasterizerState;
	ComPtr<ID3D11SamplerState> SamplerState;

	std::vector<ShaderAttachment> ShaderAttachments;
	std::map<eShaderType, ComPtr<ID3D11DeviceChild>> Shaders;
	ComPtr<ID3D11InputLayout> InputLayout;
	std::unique_ptr<D3D11VertexAttribute> VertexAttribute;

	D3D_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
	std::string Name;
};
