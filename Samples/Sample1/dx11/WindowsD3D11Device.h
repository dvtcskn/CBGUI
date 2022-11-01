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
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <string>
#include "AbstractLayer.h"

using namespace Microsoft::WRL;

class D3D11Viewport;

class WindowsD3D11Device final : public AbstractGI, public std::enable_shared_from_this<WindowsD3D11Device>
{
public:
	WindowsD3D11Device();
	virtual ~WindowsD3D11Device();
	virtual void InitWindow(std::uint32_t InWidth, std::uint32_t InHeight, bool bFullscreen, HWND InHWND) override final;
	virtual void Present() override final;
	bool GetDeviceIdentification(std::wstring& InVendorID, std::wstring& InDeviceID);
	IDXGIAdapter1* GetAdapter();

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) override final;
	virtual void FullScreen(const bool value) override final;
	virtual void Vsync(const bool value) override final;
	virtual void VsyncInterval(const UINT value) override final;

	virtual bool IsFullScreen() const override final;
	virtual bool IsVsyncEnabled() const override final;
	virtual UINT GetVsyncInterval() const override final;

	virtual std::vector<sDisplayMode> GetAllSupportedResolutions() const override final;

private:
	ComPtr<ID3D11Device1> Direct3DDevice;
	ComPtr<ID3D11DeviceContext1> Direct3DDeviceIMContext;
	ComPtr<IDXGIFactory4> DXGIFactory;
	std::unique_ptr<D3D11Viewport> Viewport;
	std::uint32_t VendorId;

public:
	FORCEINLINE ID3D11Device1* GetDevice() const
	{
		return Direct3DDevice.Get();
	}
	FORCEINLINE ID3D11DeviceContext1* GetDeviceIMContext() const
	{
		return Direct3DDeviceIMContext.Get();
	}

	FORCEINLINE bool IsNvDeviceID() const
	{
		return VendorId == 0x10DE;
	}

	FORCEINLINE bool IsAMDDeviceID() const
	{
		return VendorId == 0x1002;
	}

	FORCEINLINE bool IsIntelDeviceID() const
	{
		return VendorId == 0x8086;
	}

	FORCEINLINE bool IsSoftwareDevice() const
	{
		return VendorId == 0x1414;
	}

	FORCEINLINE ComPtr<IDXGIFactory4> GetFactory() const
	{
		return DXGIFactory;
	}

	D3D11Viewport* GetViewportContext() const;

private:
	void InitializeFactory();

	void Share();
};
