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
#include "D3D12DescriptorHeapManager.h"
#include "D3D12CommandBuffer.h"
#include "AbstractLayer.h"

class D3D12Texture
{
public:
	//D3D12Texture(const std::wstring FilePath, std::string InName);
	D3D12Texture(std::string InName, void* InData, size_t InSize, TextureDesc& InDesc);

	virtual ~D3D12Texture();

	void ApplyTexture(std::uint32_t InSlot, ID3D12GraphicsCommandList* CommandList = nullptr);
	void UpdateTexture(const void* pSrcData, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY);
	void CreateShaderResourceView(ID3D12Device* device, ID3D12Resource* tex, D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptor, bool isCubeMap);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const { return SRV.GetGPU(); }

private:
	TextureDesc Desc;
	WindowsD3D12Device* Owner;
	ComPtr<ID3D12Resource> Texture;
	D3D12DescriptorHandle SRV;
};
