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
#include "UIMaterialStyle.h"
#include "AbstractLayer.h"
#include "dx12/D3D12Material.h"
#include "dx12/D3D12Buffer.h"
#include "dx12/D3D12Pipeline.h"
#include "dx12/D3D12CommandBuffer.h"
#include "dx12/D3D12FrameBuffer.h"

class WindowsPlatform;

class Renderer_DX12 final : public IRenderer
{
public:
	Renderer_DX12(WindowsPlatform* pOwner);
	virtual ~Renderer_DX12();

	virtual void BeginPlay() override final;
	virtual void Tick(const float DeltaTime) override final;

	virtual void Render() override final;
	virtual void Draw(ICanvas::WidgetHierarchy* Node, const cbgui::cbIntBounds& ScissorsRect, AVertexBuffer* VertexBuffer, AIndexBuffer* IndexBuffer);
	bool DepthPass(cbgui::cbWidgetObj* Widget);

	IUIMaterialStyle* GetMaterialStyle(std::string Name) const;

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) override final;

	virtual void InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam) override final;

private:
	WindowsPlatform* Owner;
	std::unique_ptr<D3D12CommandBuffer> CMD;
	std::unique_ptr<D3D12RootSignature> RootSignature;
	std::unique_ptr<D3D12Pipeline> DepthMainPipeline;
	std::unique_ptr<D3D12Pipeline> DepthPipeline;
	std::unique_ptr<D3D12VertexBuffer> DepthVertexBuffer;
	std::shared_ptr<D3D12VertexBuffer> DepthFullScreenVertexBuffer;
	std::unique_ptr<D3D12FrameBuffer> DepthFBO;
	std::unique_ptr<D3D12ConstantBuffer> WidgetConstantBuffer;

	D3D12Material* LastMaterial;
	D3D12Material::SharedPtr DefaultUIFontMaterial;
	D3D12Material::SharedPtr DefaultUILineMaterial;
	D3D12Material::SharedPtr DefaultUIFlatColorMaterial;
	D3D12Material::SharedPtr DefaultStartScreenMaterial;

	UIFontMaterialStyle::SharedPtr FontStyle;
	UIMaterialStyle::SharedPtr DefaultFlatColorMatStyle;
	UIMaterialStyle::SharedPtr DefaultLineColorMatStyle;
	UIMaterialStyle::SharedPtr StartScreenWave;

	bool bEnableStencilClipping;
	bool bShowLines;

	std::unique_ptr<D3D12ConstantBuffer> GradientConstantBuffer;
	unsigned int GradientIndex;
};
