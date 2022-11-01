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
#include "D3D12Pipeline.h"
#include "D3D12Viewport.h"
#include "D3D12DeviceLocator.h"

D3D12Pipeline::D3D12Pipeline(const D3D12PipelineDesc& Desc)
    : Owner(D3D12DeviceLocator::Get().GetInterface())
{
    ZeroMemory(&PSODesc, sizeof(PSODesc));
    PSODesc.NodeMask = 1;
    PSODesc.SampleMask = 0xFFFFFFFFu;
    PSODesc.SampleDesc.Count = 1;
    PSODesc.InputLayout.NumElements = 0;

    CreatePipelineState(Desc);
}

D3D12Pipeline::~D3D12Pipeline()
{
    ClearPipelineState();
}

void D3D12Pipeline::ClearPipelineState()
{
    PSO = nullptr;

    VertexAttribute = nullptr;
    RootSignature = nullptr;

    Owner = nullptr;
    ShaderAttachments.clear();
}

void D3D12Pipeline::ApplyPipeline(D3D12CommandBuffer* InCMDBuffer) const
{
    InCMDBuffer->SetGraphicsRootSignature(RootSignature->Get());
    InCMDBuffer->SetPipelineState(PSO.Get());
}

void D3D12Pipeline::Recompile()
{
    std::vector<ComPtr<ID3DBlob>> Blobs;

    for (auto& Attachment : ShaderAttachments)
    {
        D3D12Shader(Attachment.ByteCode, Attachment.Size, Attachment.FunctionName, Attachment.Type, Attachment.ShaderDefines);
        D3D12Shader Shader = Attachment.ByteCode ? D3D12Shader(Attachment.ByteCode, Attachment.Size, Attachment.FunctionName, Attachment.Type, Attachment.ShaderDefines)
                                                 : D3D12Shader(Attachment.Location, Attachment.FunctionName, Attachment.Type, Attachment.ShaderDefines);
        Blobs.push_back(Shader.GetBlob());
        switch (Attachment.Type)
        {
        case eShaderType::Vertex:
        {
            PSODesc.VS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.VS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Pixel:
        {
            PSODesc.PS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.PS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Geometry:
        {
            PSODesc.GS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.GS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::HULL:
        {
            PSODesc.HS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.HS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Domain:
        {
            PSODesc.DS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.DS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Compute:
        {
            break;
        }
        case eShaderType::Amplification:
        {
            break;
        }
        case eShaderType::Mesh:
        {
            break;
        }
        }
    }

    PSODesc.pRootSignature = RootSignature->Get();

    auto InputLayout = VertexAttribute->Get();
    PSODesc.InputLayout.NumElements = (UINT)VertexAttribute->GetSize();
    PSODesc.InputLayout.pInputElementDescs = InputLayout.data();

    auto Device = Owner->GetDevice();
    HRESULT HR = Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&PSO));

    if (!SUCCEEDED(HR))
    {
        // Error
    }

    for (auto& Blob : Blobs)
        Blob = nullptr;
    Blobs.clear();
}

void D3D12Pipeline::CreatePipelineState(const D3D12PipelineDesc& InDesc)
{
    ShaderAttachments = InDesc.ShaderAttachments;

    std::vector<ComPtr<ID3DBlob>> Blobs;

    for (auto& Attachment : ShaderAttachments)
    {
        D3D12Shader Shader = Attachment.ByteCode ? D3D12Shader(Attachment.ByteCode, Attachment.Size, Attachment.FunctionName, Attachment.Type, Attachment.ShaderDefines)
                                                 : D3D12Shader(Attachment.Location, Attachment.FunctionName, Attachment.Type, Attachment.ShaderDefines);

        Blobs.push_back(Shader.GetBlob());
        switch (Attachment.Type)
        {
        case eShaderType::Vertex:
        {
            PSODesc.VS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.VS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Pixel:
        {
            PSODesc.PS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.PS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Geometry:
        {
            PSODesc.GS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.GS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::HULL:
        {
            PSODesc.HS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.HS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Domain:
        {
            PSODesc.DS.pShaderBytecode = Shader.GetByteCode();
            PSODesc.DS.BytecodeLength = Shader.GetByteCodeSize();
            break;
        }
        case eShaderType::Compute:
        {
            break;
        }
        case eShaderType::Amplification:
        {
            break;
        }
        case eShaderType::Mesh:
        {
            break;
        }
        }
    }

    RootSignature = std::make_shared<D3D12RootSignature>(Owner->GetDevice(), InDesc.DescriptorSetLayout);
    PSODesc.pRootSignature = RootSignature->Get();

    VertexAttribute = std::make_shared<D3D12VertexAttribute>(InDesc.VertexLayout);

    auto InputLayout = VertexAttribute->Get();
    PSODesc.InputLayout.NumElements = (UINT)VertexAttribute->GetSize();
    PSODesc.InputLayout.pInputElementDescs = InputLayout.data();

    PSODesc.NodeMask = 0;
    PSODesc.BlendState = (InDesc.BlendAttribute);
    PSODesc.RasterizerState = (InDesc.RasterizerAttribute);
    PSODesc.SampleMask = 0xFFFFFFFFu;
    PSODesc.SampleDesc.Count = 1;
    PSODesc.SampleDesc.Quality = 0;
    PSODesc.DepthStencilState = (InDesc.DepthStencilAttribute);
    PSODesc.PrimitiveTopologyType = (InDesc.PrimitiveTopologyType);

    //PSODesc.StreamOutput;
    //PSODesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE::D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    //PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    PSODesc.DSVFormat = (InDesc.DSVFormat);

    for (UINT i = 0; i < 8; i++)
        PSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
    for (UINT i = 0; i < InDesc.NumRenderTargets; i++)
        PSODesc.RTVFormats[i] = (InDesc.RTVFormats[i]);

    PSODesc.NumRenderTargets = InDesc.NumRenderTargets;

    auto Device = Owner->GetDevice();
    HRESULT HR = Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&PSO));

    if (!SUCCEEDED(HR))
    {
        // Error
    }

    for (auto& Blob : Blobs)
        Blob = nullptr;
    Blobs.clear();

    PSO->SetName(L"");
}
