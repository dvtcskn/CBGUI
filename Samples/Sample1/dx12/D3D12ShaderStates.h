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

#include "WindowsD3D12Device.h"
#include <mutex>
#include <vector>
#include "D3D12Shader.h"
#include "AbstractLayer.h"

class D3D12VertexAttribute
{
public:
    D3D12VertexAttribute(std::vector<VertexAttributeDesc> InDesc)
        : VertexAttributeDescData(InDesc)
    {
        InputLayout.resize(VertexAttributeDescData.size());
        for (std::uint32_t i = 0; i < VertexAttributeDescData.size(); i++)
        {
            InputLayout[i].SemanticName = VertexAttributeDescData[i].name.c_str();
            InputLayout[i].SemanticIndex = 0;
            InputLayout[i].Format = VertexAttributeDescData[i].format;
            InputLayout[i].InputSlot = 0;// VertexAttributeDescData[i].bufferIndex;
            InputLayout[i].AlignedByteOffset = VertexAttributeDescData[i].offset;
            InputLayout[i].InputSlotClass = VertexAttributeDescData[i].isInstanced ?
                D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            InputLayout[i].InstanceDataStepRate = VertexAttributeDescData[i].isInstanced ? 1 : 0;
        }
    }

    virtual ~D3D12VertexAttribute()
    {
        VertexAttributeDescData.clear();
        InputLayout.clear();
    }

    FORCEINLINE std::vector<D3D12_INPUT_ELEMENT_DESC> Get() const { return InputLayout; }
    FORCEINLINE std::size_t GetSize() const { return InputLayout.size(); }
    std::vector<VertexAttributeDesc> GetVertexAttributeDesc() const { return VertexAttributeDescData; }

private:
    std::vector<VertexAttributeDesc> VertexAttributeDescData;
    std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
};

enum class EDescriptorType : std::uint8_t
{
    eUniformBuffer,
    eSampler_ANISOTROPIC_16,
    eSampler_Point,
    eTexture,
    eUAV,
};

struct DescriptorSetLayoutBinding
{
    std::uint32_t Location;
    EDescriptorType DescriptorType;
    eShaderType ShaderType;
    UINT RegisterSpace;
    DescriptorSetLayoutBinding() = default;
    DescriptorSetLayoutBinding(EDescriptorType InType, eShaderType InShaderType, std::uint32_t InLocation, UINT InRegisterSpace = 0)
        : Location(InLocation)
        , DescriptorType(InType)
        , ShaderType(InShaderType)
        , RegisterSpace(InRegisterSpace)
    {}
};

class D3D12RootSignature
{
public:
    D3D12RootSignature(ID3D12Device* Device, std::vector<DescriptorSetLayoutBinding> Bindings)
    {
        auto GetVisibility = [](const eShaderType ShaderType) -> D3D12_SHADER_VISIBILITY
        {
            if (ShaderType == eShaderType::Vertex)
                return D3D12_SHADER_VISIBILITY_VERTEX;
            else if (ShaderType == eShaderType::Pixel)
                return D3D12_SHADER_VISIBILITY_PIXEL;
            else if (ShaderType == eShaderType::Geometry)
                return D3D12_SHADER_VISIBILITY_GEOMETRY;
            else if (ShaderType == eShaderType::HULL)
                return D3D12_SHADER_VISIBILITY_HULL;
            else if (ShaderType == eShaderType::Domain)
                return D3D12_SHADER_VISIBILITY_DOMAIN;
            else if (ShaderType == eShaderType::Mesh)
                return D3D12_SHADER_VISIBILITY_MESH;
            else if (ShaderType == eShaderType::Amplification)
                return D3D12_SHADER_VISIBILITY_AMPLIFICATION;

            return D3D12_SHADER_VISIBILITY_ALL;
        };

        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        auto BindingsSize = Bindings.size();
        for (const auto& Binding : Bindings)
            if (Binding.DescriptorType == EDescriptorType::eSampler_Point || Binding.DescriptorType == EDescriptorType::eSampler_ANISOTROPIC_16)
                if (BindingsSize > 0)
                    BindingsSize--;

        std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges;
        ranges.resize(BindingsSize);
        std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
        rootParameters.resize(BindingsSize);

        for (int i = 0; i < Bindings.size(); i++)
        {
            auto& Binding = Bindings[i];

            D3D12_DESCRIPTOR_RANGE_TYPE rangeType;
            if (Binding.DescriptorType == EDescriptorType::eUniformBuffer)
                rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            else if (Binding.DescriptorType == EDescriptorType::eSampler_Point || Binding.DescriptorType == EDescriptorType::eSampler_ANISOTROPIC_16)
            {
                continue;
                rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            }
            else if (Binding.DescriptorType == EDescriptorType::eTexture)
            {
                rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            }
            else if (Binding.DescriptorType == EDescriptorType::eUAV)
                rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

            ranges[i].Init(rangeType, 1, Binding.Location, Binding.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

            rootParameters[i].InitAsDescriptorTable(1, &ranges[i], GetVisibility(Binding.ShaderType));
        }

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS/* |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS*/;

        std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers;

        for (std::size_t i = 0; i < Bindings.size(); i++)
        {
            auto& Binding = Bindings[i];

            if (Binding.DescriptorType == EDescriptorType::eSampler_Point)
            {
                D3D12_STATIC_SAMPLER_DESC sampler = {};
                sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
                sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                sampler.MipLODBias = 0;
                sampler.MaxAnisotropy = 0;
                sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
                sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                sampler.MinLOD = 0.0f;
                sampler.MaxLOD = D3D12_FLOAT32_MAX;
                sampler.ShaderRegister = Binding.Location;
                sampler.RegisterSpace = Binding.RegisterSpace;
                sampler.ShaderVisibility = GetVisibility(Binding.ShaderType);

                StaticSamplers.push_back(sampler);
            }
            else  if (Binding.DescriptorType == EDescriptorType::eSampler_ANISOTROPIC_16)
            {
                D3D12_STATIC_SAMPLER_DESC sampler = {};
                sampler.Filter = D3D12_FILTER_ANISOTROPIC;
                sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                sampler.MipLODBias = 0;
                sampler.MaxAnisotropy = D3D12_DEFAULT_MAX_ANISOTROPY;
                sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
                sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
                sampler.MinLOD = 0.0f;
                sampler.MaxLOD = D3D12_FLOAT32_MAX;
                sampler.ShaderRegister = Binding.Location;
                sampler.RegisterSpace = Binding.RegisterSpace;
                sampler.ShaderVisibility = GetVisibility(Binding.ShaderType);

                StaticSamplers.push_back(sampler);
            }
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        if (StaticSamplers.size() > 0)
            rootSignatureDesc.Init_1_1((UINT)rootParameters.size(), &rootParameters[0], (UINT)StaticSamplers.size(), &StaticSamplers[0], rootSignatureFlags);
        else
            rootSignatureDesc.Init_1_1((UINT)rootParameters.size(), &rootParameters[0], 0, nullptr, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        std::string Error;
        HRESULT Serialize_HR = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
        if (error)
            Error = (static_cast<char*>(error->GetBufferPointer()));
        HRESULT RootSignature_HR = Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
        ThrowIfFailed(RootSignature_HR);
    }

    virtual ~D3D12RootSignature()
    {
        RootSignature = nullptr;
    }

    ID3D12RootSignature* Get() const { return RootSignature.Get(); }

private:
    ComPtr<ID3D12RootSignature> RootSignature;
};
