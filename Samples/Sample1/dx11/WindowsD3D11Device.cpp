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
#include <iostream>
#include <DXGIDebug.h>
#include "WindowsD3D11Device.h"
#include "D3D11Factory.h"
#include "D3D11Buffer.h"
#include "D3D11Viewport.h"
#include "D3D11Pipeline.h"
#include "D3D11FrameBuffer.h"
#include "D3D11Texture.h"
#include "D3D11ShaderStates.h"
#include "D3D11CommandBuffer.h"
#include "AbstractLayer.h"

#define DEBUG_D3DDEVICE 0

WindowsD3D11Device::WindowsD3D11Device()
	: VendorId(0)
{
	auto WideStringToString = [](const std::wstring& utf16) -> std::string
	{
		const int utf16Length = (int)utf16.length() + 1;
		const int len = WideCharToMultiByte(0, 0, utf16.data(), utf16Length, 0, 0, 0, 0);
		std::string STR(len, '\0');
		WideCharToMultiByte(0, 0, utf16.data(), utf16Length, STR.data(), len, 0, 0);
		return STR;
	};

	std::uint32_t m_dxgiFactoryFlags = 0;
#if _DEBUG && DEBUG_D3DDEVICE
	{
		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			m_dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

			DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			{
				80,
			};
			DXGI_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}
	}
#endif

	CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(&DXGIFactory));

	std::uint32_t createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if _DEBUG && DEBUG_D3DDEVICE
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIAdapter1* pAdapter = GetAdapter();

	D3D_FEATURE_LEVEL featureLevel[] =
	{
		//D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_1_0_CORE,
	};

	UINT numLevelsRequested = 3;
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;

	ComPtr<ID3D11Device> lDirect3DDevice;
	ComPtr<ID3D11DeviceContext> lDirect3DDeviceIMContext;

	SIZE_T MaxVideoMemory = 0;

	UINT FeatureIndex = 0;
	D3D_FEATURE_LEVEL ReqFeatureLevel = featureLevel[FeatureIndex];

	while (FeatureIndex < 10)
	{
		if (SUCCEEDED(D3D11CreateDevice(
			pAdapter ? pAdapter : NULL,											// pAdapter
			(pAdapter) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,	// DriverType
			NULL,										// Software
			createFlags,								// Flags
			&ReqFeatureLevel,							// pFeatureLevels
			1,											// FeatureLevels
			D3D11_SDK_VERSION,							// SDKVersion
			&lDirect3DDevice,							// ppDevice
			&m_d3dFeatureLevel,							// pFeatureLevel
			&lDirect3DDeviceIMContext					// ppImContentteContext
		)))
		{
			DXGI_ADAPTER_DESC1 desc;
			pAdapter->GetDesc1(&desc);
			VendorId = desc.VendorId;
			pAdapter->GetDesc1(&desc);
			MaxVideoMemory = desc.DedicatedVideoMemory;
			break;
		}
		else
		{
			FeatureIndex++;
			ReqFeatureLevel = featureLevel[FeatureIndex];
		}
	}

	lDirect3DDevice->QueryInterface(__uuidof (ID3D11Device1), (void**)& Direct3DDevice);
	lDirect3DDeviceIMContext->QueryInterface(__uuidof (ID3D11DeviceContext1), (void**)& Direct3DDeviceIMContext);

	{
		std::string FeatureLevel = m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_2 ? "D3D_FEATURE_LEVEL_12_2" : m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 ? "D3D_FEATURE_LEVEL_12_1"
			: m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0 ? "D3D_FEATURE_LEVEL_12_0" : m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1 ? "D3D_FEATURE_LEVEL_11_1"
			: m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 ? "D3D_FEATURE_LEVEL_11_0" : m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1 ? "D3D_FEATURE_LEVEL_10_1"
			: m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0 ? "D3D_FEATURE_LEVEL_10_0" : m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_3 ? "D3D_FEATURE_LEVEL_9_3"
			: m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_2 ? "D3D_FEATURE_LEVEL_9_2" : m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_1 ? "D3D_FEATURE_LEVEL_9_1"
			: m_d3dFeatureLevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE ? "D3D_FEATURE_LEVEL_1_0_CORE" : "D3D_FEATURE_LEVEL_Unknown";
		std::cout << FeatureLevel << std::endl;
	};

#ifdef _DEBUG
	{
		std::string Name = "Direct3DDevice";
		Direct3DDevice->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(Name) - 1, Name.c_str());
	}
	{
		std::string Name = "Direct3DDeviceIMContext";
		Direct3DDeviceIMContext->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(Name) - 1, Name.c_str());
	}
#endif

	lDirect3DDevice = nullptr;
	lDirect3DDeviceIMContext = nullptr;

	if (pAdapter)
		pAdapter->Release();

	InitializeFactory();
	Share();
}

void WindowsD3D11Device::InitializeFactory()
{
	AbstractLayer::Get().RegisterVertexBufferObject(&D3D11VertexBuffer::CreateD3D11VertexBuffer);
	AbstractLayer::Get().RegisterIndexBufferObject(&D3D11IndexBuffer::CreateD3D11IndexBuffer);
}

void WindowsD3D11Device::Share()
{
	D3D11Factory::Get().SetFactoryOwner(this);
}

WindowsD3D11Device::~WindowsD3D11Device()
{
#if _DEBUG && DEBUG_D3DDEVICE
	Direct3DDeviceIMContext->ClearState();
	Direct3DDeviceIMContext->Flush();

	ID3D11Debug* D3D11Debug = nullptr;
	Direct3DDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)(&D3D11Debug));
#endif

	Direct3DDevice = nullptr;
	Direct3DDeviceIMContext = nullptr;

	Viewport = nullptr;
	DXGIFactory = nullptr;

#if _DEBUG && DEBUG_D3DDEVICE
	if (D3D11Debug)
		D3D11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	IDXGIDebug* debugDev = nullptr;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugDev));
	if (debugDev)
		debugDev->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
}

D3D11Viewport* WindowsD3D11Device::GetViewportContext() const
{
	return Viewport.get();
}

void WindowsD3D11Device::InitWindow(std::uint32_t InWidth, std::uint32_t InHeight, bool bFullscreen, HWND InHWND)
{
	if (!Viewport)
		Viewport = std::make_unique<D3D11Viewport>(this, DXGIFactory, InWidth, InHeight, bFullscreen, InHWND);
}

void WindowsD3D11Device::Present()
{
	Viewport->Present();

	Direct3DDeviceIMContext->DiscardView(Viewport->GetCurrentBackBufferRT().Get());
	Direct3DDeviceIMContext->ClearState();
}

bool WindowsD3D11Device::IsFullScreen() const
{
	return Viewport->IsFullScreen();
}

bool WindowsD3D11Device::IsVsyncEnabled() const
{
	return Viewport->IsVsyncEnabled();
}

UINT WindowsD3D11Device::GetVsyncInterval() const
{
	return Viewport->GetVsyncInterval();
}

void WindowsD3D11Device::ResizeWindow(std::size_t Width, std::size_t Height)
{
	Viewport->ResizeSwapChain(Width, Height);
}

void WindowsD3D11Device::FullScreen(const bool value)
{
	Viewport->FullScreen(value);
}

void WindowsD3D11Device::Vsync(const bool value)
{
	Viewport->Vsync(value);
}

void WindowsD3D11Device::VsyncInterval(const UINT value)
{
	Viewport->VsyncInterval(value);
}

std::vector<sDisplayMode> WindowsD3D11Device::GetAllSupportedResolutions() const
{
	return Viewport->GetAllSupportedResolutions();
}

IDXGIAdapter1* WindowsD3D11Device::GetAdapter()
{
	IDXGIAdapter1* pAdapter;
	std::vector<IDXGIAdapter1*> vAdapters;

	for (std::uint32_t i = 0; DXGIFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
	}

	SIZE_T videoMemory = 0;
	for (auto& Adapter : vAdapters)
	{
		DXGI_ADAPTER_DESC1 desc;
		Adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (desc.DedicatedVideoMemory > videoMemory)
		{
			videoMemory = desc.DedicatedVideoMemory;

			if (SUCCEEDED(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				pAdapter = Adapter;
			}
		}
	}

	// WARP
	//if (!pAdapter)
	//{
	//	if (FAILED(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter))))
	//	{
	//		std::cout << "Direct3D Adapter - WARP" << std::endl;
	//	}
	//}

	return pAdapter;
}

bool WindowsD3D11Device::GetDeviceIdentification(std::wstring& InVendorID, std::wstring& InDeviceID)
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);
	int i = 0;
	std::wstring id;
	// locate primary display device
	while (EnumDisplayDevices(NULL, i, &dd, 0))
	{
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			id = dd.DeviceID;
			break;
		}
		i++;
	}
	//if (id == "") return false;
	// get vendor ID
	InVendorID = id.substr(8, 4);
	// get device ID
	InDeviceID = id.substr(17, 4);
	return true;
}
