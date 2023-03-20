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
#include "D3D11Pipeline.h"
#include "D3D11Factory.h"
#include "D3D11Buffer.h"
#include "D3D11Viewport.h"
#include "D3D11CommandBuffer.h"
#include <stdexcept>

D3D11Pipeline::D3D11Pipeline(std::string InName, PipelineDesc InDesc)
	: Owner(D3D11Factory::Get().GetOwner())
	, Name(InName)
	, Desc(InDesc)
	, PrimitiveTopologyType(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	CreatePipelineState(InDesc);
}

void D3D11Pipeline::CreatePipelineState(PipelineDesc InDesc)
{
	ShaderAttachments = InDesc.ShaderAttachments;
	for (auto& Attachment : ShaderAttachments)
	{
		D3D11Shader Shader = Attachment.ByteCode ? D3D11Shader(Attachment.ByteCode, Attachment.Size, Attachment.FunctionName, Attachment.Type)
												 : D3D11Shader(Attachment.Location, Attachment.FunctionName, Attachment.Type);

		if (InDesc.VertexLayout.size() > 0 && Attachment.Type == eShaderType::Vertex && !VertexAttribute)
		{
			VertexAttribute = std::make_unique<D3D11VertexAttribute>(InDesc.VertexLayout, Shader.GetByteCode().Get());
			InputLayout = VertexAttribute->Get();
		}

		Shader.CreateShader();
		Shaders.insert({ Attachment.Type, Shader.GetShaderResources() });
	}

	PrimitiveTopologyType = InDesc.PrimitiveTopologyType;

	Owner->GetDevice()->CreateSamplerState(&InDesc.SamplerAttribute, SamplerState.GetAddressOf());
	InDesc.RasterizerAttribute.ScissorEnable = TRUE;
	Owner->GetDevice()->CreateRasterizerState1(&InDesc.RasterizerAttribute, RasterizerState.GetAddressOf());
	Owner->GetDevice()->CreateDepthStencilState(&InDesc.DepthStencilAttribute, DepthStencilState.GetAddressOf());
	Owner->GetDevice()->CreateBlendState1(&InDesc.BlendAttribute, BlendState.GetAddressOf());
}

void D3D11Pipeline::ApplyPipeline(ID3D11DeviceContext1* Context, std::uint32_t StencilRef) const
{
	ID3D11DeviceContext1* CMD = Context;

	{
		ID3D11Buffer* pCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { 0 };

		if (Shaders.find(eShaderType::Vertex) != Shaders.end())
		{
			CMD->VSSetShader(static_cast<ID3D11VertexShader*>(Shaders.at(eShaderType::Vertex).Get()), NULL, NULL);
		}
		else
		{
			CMD->VSSetShader(NULL, NULL, 0);
		}

		if (Shaders.find(eShaderType::HULL) != Shaders.end())
		{
			CMD->HSSetShader(static_cast<ID3D11HullShader*>(Shaders.at(eShaderType::HULL).Get()), NULL, NULL);
		}
		else
		{
			CMD->HSSetShader(NULL, NULL, 0);
		}

		if (Shaders.find(eShaderType::Domain) != Shaders.end())
		{
			CMD->DSSetShader(static_cast<ID3D11DomainShader*>(Shaders.at(eShaderType::Domain).Get()), NULL, NULL);
		}
		else
		{
			CMD->DSSetShader(NULL, NULL, 0);
		}

		if (Shaders.find(eShaderType::Geometry) != Shaders.end())
		{
			CMD->GSSetShader(static_cast<ID3D11GeometryShader*>(Shaders.at(eShaderType::Geometry).Get()), NULL, NULL);
		}
		else
		{
			CMD->GSSetShader(NULL, NULL, 0);
		}

		if (Shaders.find(eShaderType::Pixel) != Shaders.end())
		{
			CMD->PSSetShader(static_cast<ID3D11PixelShader*>(Shaders.at(eShaderType::Pixel).Get()), NULL, NULL);
		}
		else
		{
			CMD->PSSetShader(NULL, NULL, 0);
		}

		if (Shaders.find(eShaderType::Compute) != Shaders.end())
		{
			CMD->CSSetShader(static_cast<ID3D11ComputeShader*>(Shaders.at(eShaderType::Compute).Get()), NULL, NULL);
		}
		else
		{
			CMD->CSSetShader(NULL, NULL, 0);
		}
	}

	if (InputLayout)
	{
		CMD->IASetInputLayout(InputLayout.Get());
	}
	else
	{
		ID3D11Buffer* pVBs[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
		std::uint32_t countsAndOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
		CMD->IASetInputLayout(nullptr);
		CMD->IASetVertexBuffers(0, (std::uint32_t)std::size(pVBs), pVBs, countsAndOffsets, countsAndOffsets);
		CMD->IASetIndexBuffer(nullptr, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	}

	if (RasterizerState)
		CMD->RSSetState(RasterizerState.Get());
	else
		CMD->RSSetState(nullptr);

	if (DepthStencilState)
		CMD->OMSetDepthStencilState(DepthStencilState.Get(), StencilRef);
	else
		CMD->OMSetDepthStencilState(nullptr, 0);

	D3D11_VIEWPORT pViewport(Owner->GetViewportContext()->GetViewport());
	CMD->RSSetViewports(1, &pViewport);
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	CMD->OMSetBlendState(BlendState.Get(), 0, 0xFFFFFFFF);
	CMD->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
	CMD->IASetPrimitiveTopology(PrimitiveTopologyType);

	CMD = nullptr;
}

void D3D11Pipeline::ClearPipelineState()
{
	BlendState = nullptr;
	DepthStencilState = nullptr;
	RasterizerState = nullptr;
	SamplerState = nullptr;

	for (auto& Shader : Shaders)
	{
		Shader.second = nullptr;
	}
	Shaders.clear();
	VertexAttribute = nullptr;
	InputLayout = nullptr;

	ShaderAttachments.clear();
}

void D3D11Pipeline::SetStencilRef(ID3D11DeviceContext1* Context, std::uint32_t Ref)
{
	if (DepthStencilState)
		Context->OMSetDepthStencilState(DepthStencilState.Get(), Ref);
	else
		Context->OMSetDepthStencilState(nullptr, 0);
}

void D3D11Pipeline::Recompile()
{
	for (auto& Shader : Shaders)
	{
		Shader.second = nullptr;
	}
	Shaders.clear();

	for (auto& Attachment : ShaderAttachments)
	{
		D3D11Shader Shader = Attachment.ByteCode ? D3D11Shader(Attachment.ByteCode, Attachment.Size, Attachment.FunctionName, Attachment.Type)
												 : D3D11Shader(Attachment.Location, Attachment.FunctionName, Attachment.Type);

		/*if (InVertexLayout.size() > 0 && Attachment.Type == EShaderType::Vertex && !VertexAttribute)
		{
			VertexAttribute = D3D11VertexAttribute::Create(InVertexLayout, Shader->GetByteCode().Get());
			InputLayout = VertexAttribute->Get();
		}*/

		Shader.CreateShader();
		Shaders.insert({ Attachment.Type, Shader.GetShaderResources() });
	}
}
