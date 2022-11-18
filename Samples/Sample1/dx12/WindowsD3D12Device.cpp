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
#include "WindowsD3D12Device.h"
#include <algorithm>
#include <assert.h>
#include "AbstractLayer.h"
#include "D3D12Buffer.h"
#include "D3D12DeviceLocator.h"
#include "dxgidebug.h"
#include "D3D12Viewport.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12CommandBuffer.h"

#define DEBUG_D3DDEVICE 0

WindowsD3D12Device::WindowsD3D12Device()
	: bTypedUAVLoadSupport_R11G11B10_FLOAT(false)
	, bTypedUAVLoadSupport_R16G16B16A16_FLOAT(false)
	, useGPUBasedValidation(false)
{
	HRESULT HR = E_FAIL;
	bool useDebugLayers = false;
	DWORD dxgiFactoryFlags = 0;

#if _DEBUG && DEBUG_D3DDEVICE
	useDebugLayers = true;

	if (useDebugLayers)
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
		{
			debugInterface->EnableDebugLayer();

			if (useGPUBasedValidation)
			{
				Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface1;
				if (SUCCEEDED((debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)))))
				{
					debugInterface1->SetEnableGPUBasedValidation(true);
				}
			}
		}
		else
		{
			std::cout << "Unable to enable D3D12 debug validation layer." std::endl;
		}
		debugInterface = nullptr;
	}
#endif

	Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(DxgiFactory.GetAddressOf()));

	Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

	//D3D12EnableExperimentalFeatures();

	ComPtr<IDXGIAdapter1> adapter = GetAdapter();

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

	UINT FeatureIndex = 0;
	D3D_FEATURE_LEVEL ReqFeatureLevel = featureLevel[FeatureIndex];

	HRESULT hr;

	while (FeatureIndex < 5)
	{
		hr = D3D12CreateDevice(
			adapter.Get(),
			ReqFeatureLevel,
			IID_PPV_ARGS(Direct3DDevice.ReleaseAndGetAddressOf())
		);
		if (hr == S_OK)
		{
			break;
		}
		else
		{
			FeatureIndex++;
			ReqFeatureLevel = featureLevel[FeatureIndex];
		}
	}
	ThrowIfFailed(hr);

	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.NodeMask = 0;
		ThrowIfFailed(Direct3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&GraphicsQueue)));
#if _DEBUG
		GraphicsQueue->SetName(L"GraphicsQueue");
#endif
	}
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		queueDesc.NodeMask = 0;
		ThrowIfFailed(Direct3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&ComputeQueue)));
#if _DEBUG
		ComputeQueue->SetName(L"ComputeQueue");
#endif
	}

	HR = Direct3DDevice->GetDeviceRemovedReason();
	assert(HR != E_FAIL);

	D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
	if (SUCCEEDED(Direct3DDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
	{
		if (FeatureData.TypedUAVLoadAdditionalFormats)
		{
			D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
			{
				DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
			};

			if (SUCCEEDED(Direct3DDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
				(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
			{
				bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
			}

			Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			if (SUCCEEDED(Direct3DDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
				(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
			{
				bTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
			}
		}
	}

#if _DEBUG && DEBUG_D3DDEVICE
	ComPtr<ID3D12InfoQueue> d3dInfoQueue;
	if (SUCCEEDED(Direct3DDevice.As(&d3dInfoQueue)))
	{
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		D3D12_MESSAGE_ID hide[] =
		{
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,

			D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif

	PostInit();
}

WindowsD3D12Device::~WindowsD3D12Device()
{
	GPUFlush();

#if _DEBUG && DEBUG_D3DDEVICE
	ID3D12DebugDevice* DebugDevice = nullptr;
	Direct3DDevice->QueryInterface(__uuidof(ID3D12DebugDevice), (void**)(&DebugDevice));
#endif

	Viewport = nullptr;

	DescriptorHeapManager = nullptr;
	DxgiFactory = nullptr;
	IMCommandList = nullptr;
	GraphicsQueue = nullptr;
	ComputeQueue = nullptr;
	Direct3DDevice = nullptr;

#if _DEBUG && DEBUG_D3DDEVICE
	if (DebugDevice)
		DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	IDXGIDebug* debugDev = nullptr;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugDev));
	if (debugDev)
		debugDev->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
}

void WindowsD3D12Device::PostInit()
{
	Share();
	InitializeFactory();

	IMCommandList = std::make_unique<D3D12CommandBuffer>();

	DescriptorHeapManager = std::make_unique<D3D12DescriptorHeapManager>();
}

void WindowsD3D12Device::InitializeFactory()
{
	AbstractLayer::Get().RegisterVertexBufferObject(&D3D12VertexBuffer::CreateD3D12VertexBuffer);
	AbstractLayer::Get().RegisterIndexBufferObject(&D3D12IndexBuffer::CreateD3D12IndexBuffer);
}

void WindowsD3D12Device::Share()
{
	D3D12DeviceLocator::Get().SetInterface(this);
}

D3D12Viewport* WindowsD3D12Device::GetViewportContext() const
{
	return Viewport.get();
}

void WindowsD3D12Device::InitWindow(std::uint32_t InWidth, std::uint32_t InHeight, bool bFullscreen, HWND InHWND)
{
	if (!Viewport)
		Viewport = std::make_unique<D3D12Viewport>(this, DxgiFactory, InWidth, InHeight, bFullscreen, InHWND);
}

void WindowsD3D12Device::Present()
{
	Viewport->Present();

	// If we are doing GPU Validation, flush every frame
	if (useGPUBasedValidation)
		GPUFlush();
}

void WindowsD3D12Device::ResizeWindow(std::size_t Width, std::size_t Height)
{
	GPUFlush();
	Viewport->ResizeSwapChain(Width, Height);
}

void WindowsD3D12Device::FullScreen(const bool value)
{
	GPUFlush();
	Viewport->FullScreen(value);
}

void WindowsD3D12Device::Vsync(const bool value)
{
	Viewport->Vsync(value);
}

void WindowsD3D12Device::VsyncInterval(const UINT value)
{
	Viewport->VsyncInterval(value);
}

bool WindowsD3D12Device::IsFullScreen() const
{
	return Viewport->IsFullScreen();
}

bool WindowsD3D12Device::IsVsyncEnabled() const
{
	return Viewport->IsVsyncEnabled();
}

UINT WindowsD3D12Device::GetVsyncInterval() const
{
	return Viewport->GetVsyncInterval();
}

std::vector<sDisplayMode> WindowsD3D12Device::GetAllSupportedResolutions() const
{
	return Viewport->GetAllSupportedResolutions();
}

void WindowsD3D12Device::GPUFlush(D3D12_COMMAND_LIST_TYPE queueType)
{
	ID3D12Fence* pFence;
	ThrowIfFailed(Direct3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)));

	ID3D12CommandQueue* queue = (queueType == D3D12_COMMAND_LIST_TYPE_COMPUTE) ? GetComputeQueue() : GetGraphicsQueue();

	ThrowIfFailed(queue->Signal(pFence, 1));

	HANDLE mHandleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	pFence->SetEventOnCompletion(1, mHandleFenceEvent);
	WaitForSingleObject(mHandleFenceEvent, INFINITE);
	CloseHandle(mHandleFenceEvent);

	pFence->Release();
}

void WindowsD3D12Device::GPUFlush()
{
	GPUFlush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	GPUFlush(D3D12_COMMAND_LIST_TYPE_DIRECT);
}

IDXGIAdapter* WindowsD3D12Device::FindAdapter(const WCHAR* InTargetName)
{
	IDXGIAdapter* targetAdapter = NULL;
	IDXGIFactory* IDXGIFactory_0001 = NULL;
	HRESULT hres = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&IDXGIFactory_0001);
	if (hres != S_OK)
	{
		return targetAdapter;
	}

	unsigned int adapterNo = 0;
	while (SUCCEEDED(hres))
	{
		IDXGIAdapter* pAdapter = NULL;
		hres = IDXGIFactory_0001->EnumAdapters(adapterNo, (IDXGIAdapter**)&pAdapter);

		if (SUCCEEDED(hres))
		{
			DXGI_ADAPTER_DESC aDesc;
			pAdapter->GetDesc(&aDesc);

			// If no name is specified, return the first adapater.  This is the same behaviour as the 
			// default specified for D3D11CreateDevice when no adapter is specified.
			if (wcslen(InTargetName) == 0)
			{
				targetAdapter = pAdapter;
				break;
			}

			std::wstring aName = aDesc.Description;
			if (aName.find(InTargetName) != std::string::npos)
			{
				targetAdapter = pAdapter;
			}
			else
			{
				pAdapter->Release();
			}
		}

		adapterNo++;
	}

	if (IDXGIFactory_0001)
		IDXGIFactory_0001->Release();

	return targetAdapter;
}

IDXGIAdapter1* WindowsD3D12Device::GetAdapter()
{
	IDXGIAdapter1* pAdapter;
	std::vector<IDXGIAdapter1*> vAdapters;

	for (std::uint32_t i = 0; DxgiFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
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

bool WindowsD3D12Device::GetDeviceIdentification(std::wstring& InVendorID, std::wstring& InDeviceID)
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
