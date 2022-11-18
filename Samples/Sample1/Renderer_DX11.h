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

#include <cstdint>
#include "ICanvas.h"
#include "cbWidget.h"
#include "AbstractLayer.h"
#include "dx11/D3D11Material.h"
#include "UIMaterialStyle.h"
#include "dx11/D3D11Buffer.h"
#include "dx11/D3D11Pipeline.h"
#include "dx11/D3D11CommandBuffer.h"
#include "dx11/D3D11FrameBuffer.h"

class WindowsPlatform;

class Renderer_DX11 final : public IRenderer
{
public:
	Renderer_DX11(WindowsPlatform* pOwner);
	virtual ~Renderer_DX11();

	virtual void BeginPlay() override final;
	virtual void Tick(const float DeltaTime) override final;

	virtual void Render() override final;
	virtual void Draw(ICanvas::WidgetHierarchy* Node, const cbgui::cbIntBounds& ScissorsRect, AVertexBuffer* VertexBuffer, AIndexBuffer* IndexBuffer);
	/*
	* Stencil Clipping
	*/
	bool DepthPass(cbgui::cbWidgetObj* Widget);

	IUIMaterialStyle* GetMaterialStyle(std::string Name) const;

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) override final;

	virtual void InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam) override final;

private:
	WindowsPlatform* Owner;
	std::unique_ptr<D3D11CommandBuffer> CMD;
	std::unique_ptr<D3D11Pipeline> DepthMainPipeline;
	std::unique_ptr<D3D11Pipeline> DepthPipeline;
	std::unique_ptr<D3D11VertexBuffer> DepthVertexBuffer;
	std::unique_ptr<D3D11FrameBuffer> DepthFBO;
	std::unique_ptr<D3D11ConstantBuffer> WidgetConstantBuffer;
	
	D3D11Material* LastMaterial;
	D3D11Material::SharedPtr DefaultUIFontMaterial;
	D3D11Material::SharedPtr DefaultUILineMaterial;
	D3D11Material::SharedPtr DefaultUIFlatColorMaterial;
	D3D11Material::SharedPtr DefaultStartScreenMaterial;

	UIFontMaterialStyle::SharedPtr FontStyle;
	UIMaterialStyle::SharedPtr DefaultFlatColorMatStyle;
	UIMaterialStyle::SharedPtr DefaultLineColorMatStyle;
	UIMaterialStyle::SharedPtr StartScreenWave;

	std::unique_ptr<D3D11ConstantBuffer> GradientConstantBuffer;
	unsigned int GradientIndex;

	bool bEnableStencilClipping;
	bool bShowLines;
};
