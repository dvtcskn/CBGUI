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

#include <memory>
#include <map>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <string>
#include <d3d12.h>
#include "D3DX12.h"
#include "D3D12DescriptorHeapManager.h"
#include "AbstractLayer.h"

using namespace Microsoft::WRL;

class D3D12Viewport;
class D3D12CommandBuffer;

class WindowsD3D12Device final : public AbstractGI, public std::enable_shared_from_this<WindowsD3D12Device>
{
public:
	WindowsD3D12Device();
	virtual ~WindowsD3D12Device();
	virtual void InitWindow(std::uint32_t InWidth, std::uint32_t InHeight, bool bFullscreen, HWND InHWND) override final;
	virtual void Present() override final;
	bool GetDeviceIdentification(std::wstring& InVendorID, std::wstring& InDeviceID);
	IDXGIAdapter* FindAdapter(const WCHAR* InTargetName);
	IDXGIAdapter1* GetAdapter();

	void GPUFlush(D3D12_COMMAND_LIST_TYPE queueType);
	void GPUFlush();

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) override final;
	virtual void FullScreen(const bool value) override final;
	virtual void Vsync(const bool value) override final;
	virtual void VsyncInterval(const UINT value) override final;

	virtual bool IsFullScreen() const override final;
	virtual bool IsVsyncEnabled() const override final;
	virtual UINT GetVsyncInterval() const override final;

	virtual std::vector<sDisplayMode> GetAllSupportedResolutions() const override final;

private:
	void PostInit();

private:
	ComPtr<ID3D12Device> Direct3DDevice;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<ID3D12CommandQueue> GraphicsQueue;
	ComPtr<ID3D12CommandQueue> ComputeQueue;

	std::unique_ptr<D3D12Viewport> Viewport;

	std::unique_ptr<D3D12DescriptorHeapManager> DescriptorHeapManager;

	std::unique_ptr<D3D12CommandBuffer> IMCommandList;

	bool useGPUBasedValidation;

	bool bTypedUAVLoadSupport_R11G11B10_FLOAT;
	bool bTypedUAVLoadSupport_R16G16B16A16_FLOAT;

public:
	FORCEINLINE ID3D12Device* GetDevice() const
	{
		return Direct3DDevice.Get();
	}

	FORCEINLINE ID3D12CommandQueue* GetGraphicsQueue()
	{
		return GraphicsQueue.Get();
	}

	FORCEINLINE ID3D12CommandQueue* GetComputeQueue()
	{
		return ComputeQueue.Get();
	}

	FORCEINLINE D3D12CommandBuffer* GetIMCommandList() const
	{
		return IMCommandList.get();
	}

	FORCEINLINE D3D12DescriptorHeapManager* GetDescriptorHeapManager() const
	{
		return DescriptorHeapManager.get();
	}

	FORCEINLINE bool Is_DXGI_FORMAT_R11G11B10_FLOAT_Supported()
	{
		return bTypedUAVLoadSupport_R11G11B10_FLOAT;
	}
	FORCEINLINE bool Is_DXGI_FORMAT_R16G16B16A16_FLOAT_Supported()
	{
		return bTypedUAVLoadSupport_R16G16B16A16_FLOAT;
	}

	FORCEINLINE ComPtr<IDXGIFactory4> GetFactory() const
	{
		return DxgiFactory;
	}

	D3D12Viewport* GetViewportContext() const;

private:
	void InitializeFactory();

	void Share();
};
