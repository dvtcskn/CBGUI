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
#include "D3D12FrameBuffer.h"

void D3D12FrameBuffer::CreateSRV(FBO& pFBO, CD3DX12_RESOURCE_DESC& Desc, INT mipLevel, INT arraySize, INT firstArraySlice)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
	{
		srvDesc.Format = Desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = (UINT)Desc.Width;
		srvDesc.Buffer.StructureByteStride = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	}
	else
	{
		switch (Desc.Format)
		{
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_TYPELESS: srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case DXGI_FORMAT_R16_TYPELESS: srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
			break;
		case DXGI_FORMAT_D16_UNORM: srvDesc.Format = DXGI_FORMAT_R16_UNORM;
			break;
		case DXGI_FORMAT_R32G8X24_TYPELESS: srvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		default:
			srvDesc.Format = pFBO.Texture->GetDesc().Format;
			break;
		}

		if (Desc.SampleDesc.Count == 1)
		{
			if (Desc.DepthOrArraySize == 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

				srvDesc.Texture2D.MostDetailedMip = (mipLevel == -1) ? 0 : mipLevel;
				srvDesc.Texture2D.MipLevels = 1;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

				srvDesc.Texture2DArray.MostDetailedMip = (mipLevel == -1) ? 0 : mipLevel;
				srvDesc.Texture2DArray.MipLevels = 1;

				srvDesc.Texture2DArray.FirstArraySlice = (firstArraySlice == -1) ? 0 : firstArraySlice;
				srvDesc.Texture2DArray.ArraySize = (arraySize == -1) ? Desc.DepthOrArraySize : arraySize;
			}
		}
		else
		{
			if (Desc.DepthOrArraySize == 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
				srvDesc.Texture2DMSArray.FirstArraySlice = (firstArraySlice == -1) ? 0 : firstArraySlice;
				srvDesc.Texture2DMSArray.ArraySize = (arraySize == -1) ? Desc.DepthOrArraySize : arraySize;
			}
		}
	}

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	ID3D12Device* pDevice = Owner->GetDevice();
	Owner->GetDescriptorHeapManager()->AllocateDescriptor(&pFBO.SRV);
	UINT index = 0;
	pDevice->CreateShaderResourceView(pFBO.Texture.Get(), &srvDesc, pFBO.SRV.GetCPU(index));
}
