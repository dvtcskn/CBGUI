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
#include <string>
#include "D3D11CommandBuffer.h"
#include "AbstractLayer.h"

class D3D11Texture final
{
public:
	using SharedPtr = std::shared_ptr<D3D11Texture>;
	using WeakPtr = std::weak_ptr<D3D11Texture>;
	using UniquePtr = std::unique_ptr<D3D11Texture>;

	//D3D11Texture(const std::wstring FilePath, std::string InName = std::string());
	D3D11Texture(std::string InName, void* InBuffer, size_t InSize, TextureDesc& InDesc);

	virtual ~D3D11Texture()
	{
		Release();
	}

	void Release()
	{
		Owner = nullptr;
		pTexture = nullptr;
		pTextureSRV = nullptr;
	}

	//void CreateTextureFromFile(const std::wstring FilePath);

	std::string GetName() const { return Name; }

	void UpdateTexture(const void* pSrcData, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY);
	void CreateTexture2DFromMemory(void* InBuffer, size_t InSize, TextureDesc& InDesc);
	void ApplyTexture(std::uint32_t InSlot, D3D11CommandBuffer* InCMDBuffer = nullptr);

protected:
	WindowsD3D11Device* Owner;
	ComPtr<ID3D11Texture2D> pTexture;
	ComPtr<ID3D11ShaderResourceView> pTextureSRV;
	TextureDesc Desc;

	std::string Name;
};
