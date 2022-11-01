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

#include <array>
#include <vector>
#include <map>
#include "WindowsD3D12Device.h"
#include "D3D12Shader.h"
#include "D3D12ShaderStates.h"
#include "D3D12CommandBuffer.h"
#include "AbstractLayer.h"

struct D3D12PipelineDesc
{
public:
    D3D12PipelineDesc() = default;
    ~D3D12PipelineDesc()
    {
        DescriptorSetLayout.clear();
        ShaderAttachments.clear();
        VertexLayout.clear();
    }
    CD3DX12_RASTERIZER_DESC RasterizerAttribute = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    CD3DX12_DEPTH_STENCIL_DESC1 DepthStencilAttribute = CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEFAULT);
    CD3DX12_BLEND_DESC BlendAttribute = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;

    std::vector<DescriptorSetLayoutBinding> DescriptorSetLayout;
    std::vector<ShaderAttachment> ShaderAttachments;
    std::vector<VertexAttributeDesc> VertexLayout;

    UINT NumRenderTargets = 1;
    std::array<DXGI_FORMAT, 8> RTVFormats;
    DXGI_FORMAT DSVFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
};

class D3D12Pipeline
{
public:
	D3D12Pipeline(const D3D12PipelineDesc& Desc);
	virtual ~D3D12Pipeline();

    void ApplyPipeline(D3D12CommandBuffer* InCMDBuffer = nullptr) const;
    void ClearPipelineState();

    std::vector<ShaderAttachment> GetShaderAttachments() const { return ShaderAttachments; }
    std::vector<VertexAttributeDesc> GetVertexAttribute() const { return VertexAttribute->GetVertexAttributeDesc(); }

    void Recompile();

private:
    void CreatePipelineState(const D3D12PipelineDesc& InDesc);

private:
    WindowsD3D12Device* Owner;
    std::vector<ShaderAttachment> ShaderAttachments;
    std::shared_ptr<D3D12VertexAttribute> VertexAttribute;
    std::shared_ptr<D3D12RootSignature> RootSignature;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
    ComPtr<ID3D12PipelineState> PSO;
};
