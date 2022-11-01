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
#include "D3D11Texture.h"
//#include "WICTextureLoader_Internal.h"
//#include "DDSTextureLoader_Internal.h"
#include "D3D11Factory.h"
#include "wincodec.h"

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

//D3D11Texture::D3D11Texture(const std::wstring FilePath, std::string InName)
//	: Owner(D3D11Factory::Get().GetOwner())
//	, Name(InName)
//{
//	CreateTextureFromFile(FilePath);
//}

D3D11Texture::D3D11Texture(std::string InName, void* InBuffer, size_t InSize, TextureDesc& InDesc)
	: Owner(D3D11Factory::Get().GetOwner())
	, Desc(InDesc)
	, Name(InName)
{
	CreateTexture2DFromMemory(InBuffer, InSize, InDesc);
}

//void D3D11Texture::CreateTextureFromFile(const std::wstring FilePath)
//{
//	auto WideStringToString = [](const std::wstring& utf16) -> std::string
//	{
//		const int utf16Length = (int)utf16.length() + 1;
//		const int len = WideCharToMultiByte(0, 0, utf16.data(), utf16Length, 0, 0, 0, 0);
//		std::string STR(len, '\0');
//		WideCharToMultiByte(0, 0, utf16.data(), utf16Length, STR.data(), len, 0, 0);
//		return STR;
//	};
//
//	std::wstring ws(FilePath);
//	std::string str = WideStringToString(ws);
//	//std::size_t found = str.find_last_of("/\\");
//	//Name = std::string(str.begin() + found, str.end());
//	std::string Ext = std::string(str.end() - 4, str.end());
//
//	{
//		ComPtr<ID3D11Resource> tempRes;
//		ComPtr<ID3D11ShaderResourceView> tempSRV;
//		std::uint32_t BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
//		std::uint32_t MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
//		size_t MS = 0;
//		HRESULT HR;
//
//        bool DDS = false;
//        if (Ext.find("DDS") != std::string::npos || Ext.find("dds") != std::string::npos)
//            DDS = true;
//
//        auto CTX = Owner->GetDeviceIMContext();
//        if (!DDS) 
//        {
//			std::uint32_t LoadFlags = DirectX::WIC_LOADER_FLAGS_Internal::WIC_LOADER_DEFAULT;
//			HR = DirectX::CreateWICTextureFromFileEx_Internal(Owner->GetDevice(), CTX, FilePath.c_str(), MS,
//				D3D11_USAGE_DEFAULT, BindFlags, 0, MiscFlags, LoadFlags, tempRes.GetAddressOf(), tempSRV.GetAddressOf());
//		}
//		else {
//			HR = DirectX::CreateDDSTextureFromFileEx_Internal(Owner->GetDevice(), CTX, FilePath.c_str(), MS,
//				D3D11_USAGE_DEFAULT, BindFlags, 0, MiscFlags, false, tempRes.GetAddressOf(), tempSRV.GetAddressOf());
//		}
//
//		if (HR == S_OK)
//		{
//			tempRes->QueryInterface(__uuidof (ID3D11Texture2D), (void**)& pTexture);
//			tempSRV->QueryInterface(__uuidof (ID3D11ShaderResourceView), (void**)& pTextureSRV);
//
//			tempRes = nullptr;
//			tempSRV = nullptr;
//
//			D3D11_TEXTURE2D_DESC pDesc;
//			pTexture->GetDesc(&pDesc);
//			Desc.Dimensions.X = pDesc.Width;
//			Desc.Dimensions.Y = pDesc.Height;
//			Desc.MipLevels = pDesc.MipLevels;
//		}
//	}
//}

void D3D11Texture::CreateTexture2DFromMemory(void* InBuffer, size_t InSize, TextureDesc& InDesc)
{
	Desc = InDesc;
	{
		D3D11_TEXTURE2D_DESC TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(TextureDesc));
		TextureDesc.Width = Desc.Dimensions.X;
		TextureDesc.Height = Desc.Dimensions.Y;
		TextureDesc.MipLevels = Desc.MipLevels;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = Desc.Format;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA Data;
		Data.pSysMem = InBuffer;
		Data.SysMemPitch = TextureDesc.Width * BytesPerPixel(Desc.Format);
		Data.SysMemSlicePitch = Data.SysMemPitch * TextureDesc.Height;

		HRESULT HR = Owner->GetDevice()->CreateTexture2D(&TextureDesc, &Data, pTexture.GetAddressOf());
		if (HR != S_OK)
		{
			DebugBreak();
		}

		{
			pTexture->GetDesc(&TextureDesc);
			CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
				D3D_SRV_DIMENSION_TEXTURE2D,
				TextureDesc.Format,
				0, TextureDesc.MipLevels,  // Mips
				0, TextureDesc.ArraySize   // Array
			);

			Owner->GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, pTextureSRV.GetAddressOf());
		}
	}
}

void D3D11Texture::UpdateTexture(const void* pSrcData, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY)
{
	D3D11_BOX box{};
	box.left = (std::uint32_t)MinX;
	box.right = (std::uint32_t)MaxX;
	box.top = (std::uint32_t)MinY;
	box.bottom = (std::uint32_t)MaxY;
	box.front = 0;
	box.back = 1;
	
	Owner->GetDeviceIMContext()->UpdateSubresource(pTexture.Get(), 0, &box, pSrcData, (std::uint32_t)RowPitch * BytesPerPixel(Desc.Format), 0);
}

void D3D11Texture::ApplyTexture(std::uint32_t InSlot, D3D11CommandBuffer* InCMDBuffer)
{
	ID3D11DeviceContext1* CMD = InCMDBuffer ? static_cast<D3D11CommandBuffer*>(InCMDBuffer)->GetDeferredCTX() : Owner->GetDeviceIMContext();
	CMD->PSSetShaderResources(InSlot, 1, pTextureSRV.GetAddressOf());

	CMD = nullptr;
}
