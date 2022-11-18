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
#include <algorithm>
#include "D3D12Viewport.h"
#include "WindowsD3D12Device.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#if _DEBUG
	#pragma comment(lib, "dxguid.lib")
#endif

D3D12Viewport::SharedPtr D3D12Viewport::Create(WindowsD3D12Device* InOwner, ComPtr<IDXGIFactory4> InFactory, std::uint32_t InSizeX, std::uint32_t InSizeY, bool bInIsFullscreen, HWND InHandle)
{
	return std::make_shared<D3D12Viewport>(InOwner, InFactory, InSizeX, InSizeY, bInIsFullscreen, InHandle);
}

D3D12Viewport::D3D12Viewport(WindowsD3D12Device* InOwner, ComPtr<IDXGIFactory4> InFactory, std::uint32_t InSizeX, std::uint32_t InSizeY, bool IsFullscreen, HWND InHandle)
	: Owner(InOwner)
	, SizeX(InSizeX)
	, SizeY(InSizeY)
	, WindowHandle(InHandle)
	, SwapChainBufferCount(2)
	, CurrentBackBuffer(0)
	, FrameIndex(0)
	, m_rtvDescriptorSize(0)
	, SyncInterval(1)
	, bIsFullScreen(IsFullscreen)
	, bIsVSYNCEnabled(false)
	, BackBuffer(D3D12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2))
{
	PresentParams.DirtyRectsCount = 0;
	PresentParams.pDirtyRects = 0;
	PresentParams.pScrollRect = 0;
	PresentParams.pScrollOffset = 0;

	DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc;
	SecureZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
	m_SwapChainDesc.Width = InSizeX;
	m_SwapChainDesc.Height = InSizeY;
	m_SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_SwapChainDesc.BufferCount = SwapChainBufferCount;
	m_SwapChainDesc.SampleDesc.Count = 1;
	m_SwapChainDesc.SampleDesc.Quality = 0;
	m_SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	m_SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc;
	fsSwapChainDesc = {};
	fsSwapChainDesc.Windowed = !bIsFullScreen;

	ComPtr<IDXGISwapChain1> swapChain1;
	// Create a SwapChain from a Win32 window.
	HRESULT HR = InFactory->CreateSwapChainForHwnd(
		Owner->GetGraphicsQueue(),
		InHandle,
		&m_SwapChainDesc,
		&fsSwapChainDesc, nullptr, swapChain1.ReleaseAndGetAddressOf()
	);

	swapChain1.As(&SwapChain);
	//SwapChain->GetBuffer(0, IID_PPV_ARGS(&DXGIBackBuffer));
	//DXGIBackBuffer = nullptr;

	InFactory->MakeWindowAssociation(InHandle, DXGI_MWA_VALID);

	FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	viewport = { 0.0f, 0.0f, (float)InSizeX,  (float)InSizeY, 0.0f, 1.0f };

	//RECT Rect;
	//GetClientRect(InHandle, &Rect);

	auto Manager = Owner->GetDescriptorHeapManager();
	Manager->AllocateDescriptor(&BackBuffer);

	m_rtvDescriptorSize = Owner->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CreateRenderTargets();

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ThrowIfFailed(Owner->GetDevice()->CreateFence(m_fenceValues[FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValues[FrameIndex]++;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
	{
		ThrowIfFailed(Owner->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[0])));
		ThrowIfFailed(Owner->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[1])));

		ThrowIfFailed(Owner->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
		m_commandList->Close();
	}

	WaitForGpu();
}

D3D12Viewport::~D3D12Viewport()
{
	{
		m_commandAllocators[FrameIndex]->Reset();
		m_commandList->Reset(m_commandAllocators[FrameIndex].Get(), nullptr);

		m_commandList->Close();
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
		Owner->GetGraphicsQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		SwapChain->SetFullscreenState(false, nullptr);

		WaitForGpu();
	}

	m_renderTargets[0] = nullptr;
	m_renderTargets[1] = nullptr;

	FrameBuffer = nullptr;

	m_commandAllocators[0] = nullptr;
	m_commandAllocators[1] = nullptr;
	m_commandList = nullptr;

	m_fence = nullptr;
	CloseHandle(m_fenceEvent);

	SwapChain = nullptr;

	Owner = nullptr;
	WindowHandle = nullptr;
}

void D3D12Viewport::CreateRenderTargets()
{
	for (std::size_t i = 0; i < SwapChainBufferCount; i++)
		m_renderTargets[i] = nullptr;

	{
		FrameBuffer = nullptr;

		std::string Name = "Deferred_Backbuffer";
		std::vector<FrameBufferAttachmentInfo> Infos;
		{
			FrameBufferAttachmentInfo Info; // Albedo
			Info.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			Info.Usage = eImageUsage::Color;
			Infos.push_back(Info);
		}

		FBODesc Desc;
		Desc.Dimensions.X = SizeX;
		Desc.Dimensions.Y = SizeY;
		FrameBuffer = std::make_unique<D3D12FrameBuffer>(Name, Infos, Desc);
	}

	const auto Device = Owner->GetDevice();

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(BackBuffer.GetCPU());

		// Create a RTV for each frame.
		for (UINT n = 0; n < SwapChainBufferCount; n++)
		{
			SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			Device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}
}

void D3D12Viewport::Present()
{
	WaitForGpu();

	CopyToBackBuffer();

	{
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
		Owner->GetGraphicsQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	SwapChain->Present(!bIsVSYNCEnabled ? 0 : SyncInterval, !bIsVSYNCEnabled && !bIsFullScreen ? DXGI_PRESENT_ALLOW_TEARING : 0);
	MoveToNextFrame();
}

// Wait for pending GPU work to complete.
void D3D12Viewport::WaitForGpu()
{
	auto m_commandQueue = Owner->GetGraphicsQueue();
	// Schedule a Signal command in the queue.
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[FrameIndex]));

	// Wait until the fence has been processed.
	ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[FrameIndex], m_fenceEvent));
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	m_fenceValues[FrameIndex]++;
}

// Prepare to render the next frame.
void D3D12Viewport::MoveToNextFrame()
{
	auto m_commandQueue = Owner->GetGraphicsQueue();
	// Schedule a Signal command in the queue.
	const UINT64 currentFenceValue = m_fenceValues[FrameIndex];
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

	// Update the frame index.
	FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (m_fence->GetCompletedValue() < m_fenceValues[FrameIndex])
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[FrameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	m_fenceValues[FrameIndex] = currentFenceValue + 1;
}

void D3D12Viewport::CopyToBackBuffer()
{
	ID3D12Resource* FrameBufferTexture = FrameBuffer->GetTexture();

	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(m_commandAllocators[FrameIndex]->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(m_commandList->Reset(m_commandAllocators[FrameIndex].Get(), nullptr));

	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(FrameBufferTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
	m_commandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

	m_commandList->CopyResource(m_renderTargets[FrameIndex].Get(), FrameBufferTexture);

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(FrameBufferTexture, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
#if 1
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
#else
	// Overlays requires...
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
#endif

	m_commandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);

	ThrowIfFailed(m_commandList->Close());
}

void D3D12Viewport::ResizeSwapChain(std::size_t Width, std::size_t Height)
{
	SizeX = (uint32_t)Width;
	SizeY = (uint32_t)Height;
	viewport = { 0.0f, 0.0f, (float)SizeX,  (float)SizeY, 0.0f, 1.0f };

	FrameBuffer = nullptr;

	for (std::size_t i = 0; i < SwapChainBufferCount; i++)
		m_renderTargets[i] = nullptr;

	// Wait for SwapChain Resources to be destroyed.
	WaitForGpu();

	m_commandAllocators[FrameIndex]->Reset();
	m_commandList->Reset(m_commandAllocators[FrameIndex].Get(), nullptr);

	m_commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	Owner->GetGraphicsQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	SwapChain->ResizeBuffers(SwapChainBufferCount, SizeX, SizeY, DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargets();

	if (bIsFullScreen)
	{
		SwapChain->SetFullscreenState(false, nullptr);
		SwapChain->SetFullscreenState(true, nullptr);
	}
	WaitForGpu();
}

void D3D12Viewport::FullScreen(const bool value)
{
	if (bIsFullScreen == value)
		return;

	bIsFullScreen = value;

	SwapChain->SetFullscreenState(value, nullptr);

	ResizeSwapChain(SizeX, SizeY);
}

void D3D12Viewport::Vsync(const bool value)
{
	bIsVSYNCEnabled = value;
}

void D3D12Viewport::VsyncInterval(const UINT value)
{
	SyncInterval = value;
}

std::vector<sDisplayMode> D3D12Viewport::GetAllSupportedResolutions() const
{
	std::vector<sDisplayMode> Result;
	{
		HRESULT HResult = S_OK;
		IDXGIAdapter1* Adapter = Owner->GetAdapter();

		//Owner->GetAdapter(Adapter);

		// get the description of the adapter
		DXGI_ADAPTER_DESC AdapterDesc;
		Adapter->GetDesc(&AdapterDesc);

		IDXGIOutput* Output;
		HResult = Adapter->EnumOutputs(0, &Output);
		if (DXGI_ERROR_NOT_FOUND == HResult)
		{
			return std::vector<sDisplayMode>();
		}
		if (FAILED(HResult))
		{
			return std::vector<sDisplayMode>();
		}

		// TODO: GetDisplayModeList is a terribly SLOW call.  It can take up to a second per invocation.
		//  We might want to work around some DXGI badness here.
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		std::uint32_t NumModes = 0;
		HResult = Output->GetDisplayModeList(Format, 0, &NumModes, NULL);
		if (HResult == DXGI_ERROR_NOT_FOUND)
		{
			return  std::vector<sDisplayMode>();
		}
		else if (HResult == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			return  std::vector<sDisplayMode>();
		}

		DXGI_MODE_DESC* ModeList = new DXGI_MODE_DESC[NumModes];
		Output->GetDisplayModeList(Format, 0, &NumModes, ModeList);

		for (std::uint32_t m = 0; m < NumModes; m++)
		{
			sDisplayMode Mode;
			Mode.Width = ModeList[m].Width;
			Mode.Height = ModeList[m].Height;
			Mode.RefreshRate.Denominator = ModeList[m].RefreshRate.Denominator;
			Mode.RefreshRate.Numerator = ModeList[m].RefreshRate.Numerator;
			Result.push_back(Mode);
		}

		delete[] ModeList;
	}
	return Result;
};
