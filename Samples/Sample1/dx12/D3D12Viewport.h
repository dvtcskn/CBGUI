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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <DXGIDebug.h>

#include <wrl\client.h>
using namespace Microsoft::WRL;

#include "D3D12DescriptorHeapManager.h"
#include "D3D12FrameBuffer.h"

class WindowsD3D12Device;

class D3D12Viewport final
{
public:
	using SharedPtr = std::shared_ptr<D3D12Viewport>;
	static SharedPtr Create(WindowsD3D12Device* InOwner, ComPtr<IDXGIFactory4> InFactory, std::uint32_t InSizeX, std::uint32_t InSizeY, bool IsFullscreen, HWND InHandle);

	D3D12Viewport(class WindowsD3D12Device* InOwner, ComPtr<IDXGIFactory4> InFactory, std::uint32_t InSizeX, std::uint32_t InSizeY, bool IsFullscreen, HWND InHandle);
	~D3D12Viewport();

	void Present();
	void MoveToNextFrame();
	void WaitForGpu();

	void ResizeSwapChain(std::size_t Width, std::size_t Height);
	void FullScreen(const bool value);
	void Vsync(const bool value);
	void VsyncInterval(const UINT value);

	bool IsFullScreen() const { return bIsFullScreen; }
	bool IsVsyncEnabled() const { return bIsVSYNCEnabled; }
	UINT GetVsyncInterval() const { return SyncInterval; }

	std::vector<sDisplayMode> GetAllSupportedResolutions() const;

	FORCEINLINE ComPtr<IDXGISwapChain3> GetSwapChain() const { return SwapChain; }
	//FORCEINLINE ComPtr<IDXGISurface2> GetDXGIBackBuffer() const { return DXGIBackBuffer; }
	FORCEINLINE HWND GetHandle() const { return WindowHandle; }
	FORCEINLINE D3D12_VIEWPORT GetViewport() { return viewport; }
	FORCEINLINE std::uint32_t GetViewportWidth() const { return SizeX; }
	FORCEINLINE std::uint32_t GetViewportHeight() const { return SizeY; }
	FORCEINLINE UINT GetFrameIndex() const { return FrameIndex; }

	void CreateRenderTargets();
	void CopyToBackBuffer();

	D3D12FrameBuffer* GetFrameBuffer() const { return FrameBuffer.get(); }

private:
	WindowsD3D12Device* Owner;
	HWND WindowHandle;

	D3D12_VIEWPORT viewport;
	std::uint32_t SizeX;
	std::uint32_t SizeY;

	ComPtr<IDXGISwapChain3> SwapChain;

	UINT m_rtvDescriptorSize;
	ComPtr<ID3D12Resource> m_renderTargets[2];
	D3D12DescriptorHandle BackBuffer;
	std::unique_ptr<D3D12FrameBuffer> FrameBuffer;

	UINT SwapChainBufferCount;
	std::uint32_t CurrentBackBuffer;
	UINT FrameIndex;

	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValues[2];

	ComPtr<ID3D12CommandAllocator> m_commandAllocators[2];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	DXGI_PRESENT_PARAMETERS PresentParams;

	UINT SyncInterval;
	bool bIsFullScreen;
	bool bIsVSYNCEnabled;
};
