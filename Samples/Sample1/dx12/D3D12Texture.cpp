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
#include "D3D12Texture.h"
//#include "WICTextureLoader12.h"
//#include "DDSTextureLoader12.h"
#include "D3D12DeviceLocator.h"

static auto DXGI_BitsPerPixel = [](_In_ DXGI_FORMAT fmt) -> size_t
{
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_YUY2:
        return 32;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        return 24;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return 16;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_NV11:
        return 12;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
};

static std::uint32_t BytesPerPixel(DXGI_FORMAT Format)
{
    return (std::uint32_t)DXGI_BitsPerPixel(Format) / 8;
}

//D3D12Texture::D3D12Texture(const std::wstring FilePath, std::string InName)
//    : Owner(D3D12DeviceLocator::Get().GetInterface())
//    , SRV(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
//{
//    auto WideStringToString = [](const std::wstring & s) -> std::string
//    {
//        int len;
//        int slength = (int)s.length() + 1;
//        len = WideCharToMultiByte(0, 0, s.c_str(), slength, 0, 0, 0, 0);
//        std::string r(len, '\0');
//        WideCharToMultiByte(0, 0, s.c_str(), slength, &r[0], len, 0, 0);
//        return r;
//    };
//
//    std::unique_ptr<uint8_t[]> decodedData;
//
//    auto Device = Owner->GetDevice();
//    auto IMCommandList = Owner->GetIMCommandList();
//    auto DescriptorHeapManager = Owner->GetDescriptorHeapManager();
//
//    std::string str = WideStringToString(FilePath);
//    std::string Ext = std::string(str.end() - 4, str.end());
//
//    bool DDS = false;
//    if (Ext.find("DDS") != std::string::npos || Ext.find("dds") != std::string::npos)
//        DDS = true;
//
//    if (DDS)
//    {
//        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
//        ThrowIfFailed(
//            DirectX::LoadDDSTextureFromFile(Device, FilePath.c_str(), Texture.ReleaseAndGetAddressOf(),
//                decodedData, subresources));
//
//        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(Texture.Get(), 0,
//            static_cast<UINT>(subresources.size()));
//
//        // Create the GPU upload buffer.
//        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//
//        auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
//
//        ComPtr<ID3D12Resource> uploadRes;
//        ThrowIfFailed(
//            Device->CreateCommittedResource(
//                &heapProps,
//                D3D12_HEAP_FLAG_NONE,
//                &desc,
//                D3D12_RESOURCE_STATE_GENERIC_READ,
//                nullptr,
//                IID_PPV_ARGS(uploadRes.GetAddressOf())));
//
//        IMCommandList->WaitForGPU();
//        IMCommandList->BeginRecordCommandList();
//
//        UpdateSubresources(IMCommandList->Get(), Texture.Get(), uploadRes.Get(),
//            0, 0, static_cast<UINT>(subresources.size()), subresources.data());
//
//        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(Texture.Get(),
//            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
//        IMCommandList->ResourceBarrier(1, &barrier);
//
//        IMCommandList->FinishRecordCommandList();
//        IMCommandList->ExecuteCommandList();
//        IMCommandList->WaitForGPU();
//
//        uploadRes = nullptr;
//    }
//    else
//    {
//        D3D12_SUBRESOURCE_DATA subresource;
//        ThrowIfFailed(
//            DirectX::LoadWICTextureFromFile(Device, FilePath.c_str(), Texture.ReleaseAndGetAddressOf(),
//                decodedData, subresource));
//
//        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(Texture.Get(), 0, 1);
//
//        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//
//        auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
//
//        ComPtr<ID3D12Resource> uploadRes;
//        // Create the GPU upload buffer.
//        ThrowIfFailed(
//            Device->CreateCommittedResource(
//                &heapProps,
//                D3D12_HEAP_FLAG_NONE,
//                &desc,
//                D3D12_RESOURCE_STATE_GENERIC_READ,
//                nullptr,
//                IID_PPV_ARGS(uploadRes.GetAddressOf())));
//
//        IMCommandList->WaitForGPU();
//        IMCommandList->BeginRecordCommandList();
//
//        UpdateSubresources(IMCommandList->Get(), Texture.Get(), uploadRes.Get(),
//            0, 0, 1, &subresource);
//
//        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(Texture.Get(),
//            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
//        IMCommandList->ResourceBarrier(1, &barrier);
//
//        IMCommandList->FinishRecordCommandList();
//        IMCommandList->ExecuteCommandList();
//        IMCommandList->WaitForGPU();
//
//        uploadRes = nullptr;
//    }
//
//    DescriptorHeapManager->AllocateDescriptor(&SRV);
//    CreateShaderResourceView(Device, Texture.Get(), SRV.GetCPU(), false);
//}

D3D12Texture::D3D12Texture(std::string InName, void* InData, size_t InSize, TextureDesc& InDesc)
    : Owner(D3D12DeviceLocator::Get().GetInterface())
    , Desc(InDesc)
    , SRV(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
{
    auto Device = Owner->GetDevice();
    auto IMCommandList = Owner->GetIMCommandList();
    auto DescriptorHeapManager = Owner->GetDescriptorHeapManager();

    D3D12_SUBRESOURCE_DATA subresource;
    subresource.pData = InData;
    subresource.RowPitch = InDesc.Dimensions.X * BytesPerPixel(InDesc.Format);
    subresource.SlicePitch = subresource.RowPitch * InDesc.Dimensions.Y;

    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = InDesc.MipLevels;
    textureDesc.Format = InDesc.Format;
    textureDesc.Width = InDesc.Dimensions.X;
    textureDesc.Height = InDesc.Dimensions.Y;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    auto TextureHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_CUSTOM);
    TextureHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
    TextureHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

    ThrowIfFailed(Device->CreateCommittedResource(
        &TextureHeap,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&Texture)));

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(Texture.Get(), 0, 1);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    ComPtr<ID3D12Resource> uploadRes;
    // Create the GPU upload buffer.
    ThrowIfFailed(
        Device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadRes.GetAddressOf())));

    IMCommandList->WaitForGPU();
    IMCommandList->BeginRecordCommandList();

    UpdateSubresources(IMCommandList->Get(), Texture.Get(), uploadRes.Get(),
        0, 0, 1, &subresource);

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(Texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    IMCommandList->ResourceBarrier(1, &barrier);

    IMCommandList->FinishRecordCommandList();
    IMCommandList->ExecuteCommandList();
    IMCommandList->WaitForGPU();

    uploadRes = nullptr;

    DescriptorHeapManager->AllocateDescriptor(&SRV);
    CreateShaderResourceView(Device, Texture.Get(), SRV.GetCPU(), false);
}

D3D12Texture::~D3D12Texture()
{
    Owner = nullptr;
    Texture = nullptr;
}

void D3D12Texture::ApplyTexture(std::uint32_t InSlot, ID3D12GraphicsCommandList* CommandList)
{
    if (Texture)
        CommandList->SetGraphicsRootDescriptorTable(1, SRV.GetGPU());
}

void D3D12Texture::UpdateTexture(const void* pSrcData, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY)
{
    auto IMCommandList = Owner->GetIMCommandList()->Get();

    Owner->GetIMCommandList()->WaitForGPU();
    Owner->GetIMCommandList()->BeginRecordCommandList();

    D3D12_BOX box{};
    box.left = (std::uint32_t)MinX;
    box.right = (std::uint32_t)MaxX;
    box.top = (std::uint32_t)MinY;
    box.bottom = (std::uint32_t)MaxY;
    box.front = 0;
    box.back = 1;

    D3D12_RESOURCE_BARRIER preCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(Texture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
    IMCommandList->ResourceBarrier(1, &preCopyBarriers);

    CD3DX12_RANGE readRange(0, 0);
    Texture->Map(0, &readRange, nullptr);
    Texture->WriteToSubresource(0, &box, pSrcData, (std::uint32_t)RowPitch * BytesPerPixel(Desc.Format), 0);
    Texture->Unmap(0, nullptr);

    D3D12_RESOURCE_BARRIER postCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(Texture.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    IMCommandList->ResourceBarrier(1, &postCopyBarriers);

    Owner->GetIMCommandList()->FinishRecordCommandList();
    Owner->GetIMCommandList()->ExecuteCommandList();
    Owner->GetIMCommandList()->WaitForGPU();
}

void D3D12Texture::CreateShaderResourceView(ID3D12Device* device, ID3D12Resource* tex, D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptor, bool isCubeMap)
{
    const auto desc = tex->GetDesc();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    switch (desc.Dimension)
    {
    case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
        if (desc.DepthOrArraySize > 1)
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            srvDesc.Texture1DArray.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
            srvDesc.Texture1DArray.ArraySize = static_cast<UINT>(desc.DepthOrArraySize);
        }
        else
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
        }
        break;

    case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
        if (isCubeMap)
        {
            if (desc.DepthOrArraySize > 6)
            {
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                srvDesc.TextureCubeArray.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
                srvDesc.TextureCubeArray.NumCubes = static_cast<UINT>(desc.DepthOrArraySize / 6);
            }
            else
            {
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                srvDesc.TextureCube.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
            }
        }
        else if (desc.DepthOrArraySize > 1)
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
            srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(desc.DepthOrArraySize);
        }
        else
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
        }
        break;

    case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        srvDesc.Texture3D.MipLevels = (!desc.MipLevels) ? UINT(-1) : desc.MipLevels;
        break;

    case D3D12_RESOURCE_DIMENSION_BUFFER:
        //("ERROR: CreateShaderResourceView cannot be used with DIMENSION_BUFFER.\n");
        throw std::invalid_argument("buffer resources not supported");

    case D3D12_RESOURCE_DIMENSION_UNKNOWN:
    default:
        //("ERROR: CreateShaderResourceView cannot be used with DIMENSION_UNKNOWN (%d).\n", desc.Dimension);
        throw std::invalid_argument("unknown resource dimension");
    }

    device->CreateShaderResourceView(tex, &srvDesc, srvDescriptor);
}
