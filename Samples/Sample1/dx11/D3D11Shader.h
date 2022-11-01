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

#include <d3d11_1.h>
#include <d3d11shader.h>
#include "WindowsD3D11Device.h"
#include <vector>
#include "AbstractLayer.h"

#include <wrl\client.h>
using namespace Microsoft::WRL;

#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib")

class D3D11Shader final
{
private:
	ComPtr<ID3D11DeviceChild> Shader;
	eShaderType ShaderType;
	ComPtr<ID3DBlob> ByteCode;
	WindowsD3D11Device* Owner;
	std::string FunctionName;

public:
	using SharedPtr = std::shared_ptr<D3D11Shader>;

	D3D11Shader(std::wstring InSrcFile, std::string InFunctionName, eShaderType InProfile, std::vector<ShaderDefines> InDefines = std::vector<ShaderDefines>());
	D3D11Shader(const void* InCode, std::size_t Size, std::string InFunctionName, eShaderType InProfile, std::vector<ShaderDefines> InDefines = std::vector<ShaderDefines>());

	virtual ~D3D11Shader() 
	{
		if (ByteCode)
			ByteCode = nullptr;
		Owner  = nullptr;
		Shader = nullptr;
	}

	void CompileShaderFromFile(std::wstring InSrcFile, std::string InFunctionName, eShaderType InProfile, std::vector<ShaderDefines> InDefines = std::vector<ShaderDefines>());
	void CompileShader(const void* InCode, std::size_t Size, std::string InFunctionName, eShaderType InProfile, std::vector<ShaderDefines> InDefines = std::vector<ShaderDefines>());

	void CreateShader();
	void CreatePixelShader();
	void CreateVertexShader();
	void CreateHullShader();
	void CreateDomainShader();
	void CreateGeometryShader();
	void CreateComputeShader();

	FORCEINLINE ComPtr<ID3D11DeviceChild> GetShaderResources() { return Shader; }
	FORCEINLINE ComPtr<ID3DBlob> GetByteCode() { return ByteCode; }
};
