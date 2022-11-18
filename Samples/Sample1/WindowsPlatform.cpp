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
#include "WindowsPlatform.h"
#include <algorithm>

#include "dx11/WindowsD3D11Device.h"
#include "dx12/WindowsD3D12Device.h"

#include "DemoCanvas.h"

#include "Renderer_DX11.h"
#include "Renderer_DX12.h"

#define WINDOW_STYLE_NORMAL					(WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CAPTION)
#define WINDOW_STYLE_BORDERLESS				(WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP)

#define APP_NAME L"Sample1"

namespace EngineConsole {
	static void AttachConsoleToEngine()
	{
//#ifdef _DEBUG
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w+", stdout);
		SetConsoleTitle(TEXT("Sample1_Console"));
//#endif
	}
}

enum class WindowState
{
	Normal,
	Minimized,
	Maximized,
};

WindowState GetWindowState(HWND WindowHandle)
{
	if (IsZoomed(WindowHandle))
		return WindowState::Maximized;

	if (IsIconic(WindowHandle))
		return WindowState::Minimized;

	return WindowState::Normal;
}

RECT ArrangeWindow(const RECT& rect)
{
	RECT result = rect;
	{
		HMONITOR hMonitor;
		MONITORINFO mi;
		hMonitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		RECT desktop = mi.rcMonitor;
		const int centreX = (int)desktop.left + (int)(desktop.right - desktop.left) / 2;
		const int centreY = (int)desktop.top + (int)(desktop.bottom - desktop.top) / 2;
		const int winW = rect.right - rect.left;
		const int winH = rect.bottom - rect.top;
		int left = centreX - winW / 2;
		int right = left + winW;
		int top = centreY - winH / 2;
		int bottom = top + winH;
		result.left = std::max(left, (int)desktop.left);
		result.right = std::min(right, (int)desktop.right);
		result.bottom = std::min(bottom, (int)desktop.bottom);
		result.top = std::max(top, (int)desktop.top);

	}
	return result;
}

LRESULT CALLBACK WindowProc(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowsPlatform* pWP = (WindowsPlatform*)GetWindowLongPtr(hWnd, 0);
	if (pWP)
		return pWP->MsgProc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WindowsPlatform::WindowsPlatform()
	: m_hWnd(NULL)
	, WindowsMode(1)
	, WindowWidth(1366)
	, WindowHeight(768)
	, WindowFullscreen(false)
{
	InitGI();
	ConstructWidgets();
}

WindowsPlatform::~WindowsPlatform()
{
	for (auto& Canvas : Canvases)
	{
		delete Canvas;
		Canvas = nullptr;
	}
	Canvases.clear();

	Renderer = nullptr;
	Device = nullptr;

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

void WindowsPlatform::InitGI()
{
	RECT rect = ArrangeWindow(RECT({ 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) }));
	CreateWindows(L"Sample1", rect, WindowFullscreen);
	
	bool bD3D12 = false;
	//bD3D12 = true;

	if (bD3D12)
		Device = std::make_unique<WindowsD3D12Device>();
	else
		Device = std::make_unique<WindowsD3D11Device>();

	Device->InitWindow(WindowWidth, WindowHeight, WindowFullscreen, GetHWND());

	if (bD3D12)
		Renderer = std::make_unique<Renderer_DX12>(this);
	else
		Renderer = std::make_unique<Renderer_DX11>(this);

	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(hCursor);
}

void WindowsPlatform::ConstructWidgets()
{
	auto DC = new DemoCanvas(this);
	Canvases.push_back(DC);
}

bool WindowsPlatform::CreateWindows(std::wstring title, RECT rect, bool bfullscreen)
{
	EngineConsole::AttachConsoleToEngine();
	HINSTANCE hInstance = GetModuleHandle(NULL);
	//WNDCLASSEX windowClass = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc,
	//	0L, sizeof(void*), hInstance, NULL, NULL, NULL, NULL, APP_NAME, NULL };

	WNDCLASSEX windowClass;
	memset(&windowClass, 0, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0L;
	windowClass.cbWndExtra = sizeof(void*);
	windowClass.hInstance = hInstance;
	windowClass.hIcon = NULL;// LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;// (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;// APP_NAME;
	windowClass.lpszClassName = APP_NAME;
	windowClass.hIconSm = NULL;// LoadIcon(NULL, IDI_WINLOGO);

	RegisterClassEx(&windowClass);

	std::uint32_t windowStyle = WINDOW_STYLE_BORDERLESS;

	AdjustWindowRect(&rect, windowStyle, FALSE);

	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		APP_NAME,
		title.c_str(),
		windowStyle,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		GetDesktopWindow(),
		NULL,
		hInstance,
		NULL
	);

	if (!m_hWnd)
	{
#ifdef _DEBUG
		DWORD errorCode = GetLastError();
		DebugBreak();
#endif
		MessageBox(NULL, L"Cannot create window", 0, MB_OK | MB_ICONERROR);
		//return E_FAIL;
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	SetWindowLongPtr(m_hWnd, 0, (LONG_PTR)this);
	UpdateWindow(m_hWnd);
	return true;
}

void WindowsPlatform::MessageLoop()
{
	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) // PM_NOYIELD
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		{
			if (GetWindowState(m_hWnd) != WindowState::Minimized)
			{
				mStepTimer.Tick([&]()
					{
						Tick(float(mStepTimer.GetElapsedSeconds()));
						Render();
						Device->Present();
					});
			}
		}
	}
}

LRESULT WindowsPlatform::MsgProc(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		int iKeyPressed = static_cast<int>(wParam);

		switch (iKeyPressed)
		{
		case VK_ESCAPE:
			RequestTermination();
			return true;
			break;

		case VK_TAB:
			return 0;
			break;

		case VK_SPACE:
			break;

		}

	}

	switch (uMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SYSKEYDOWN:
		if (wParam == VK_F4)
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

	case WM_ENTERSIZEMOVE:
		break;

	case WM_EXITSIZEMOVE:
		break;

	case WM_KEYDOWN:
		break;

	case WM_SIZE:
		break;
	}

	if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ||
		uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
	{
		for (auto& Canvas : Canvases)
		{
			Canvas->InputProcess(hWnd, uMsg, wParam, lParam);
		}
		Renderer->InputProcess(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WindowsPlatform::RequestTermination()
{
	// This triggers the termination of the application
	PostQuitMessage(0);
}

void WindowsPlatform::WindowMode(const int value)
{
	if (WindowsMode == value)
		return;

	switch (value)
	{
	case 0:		// Fullscreen
	{
		SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_BORDERLESS);

		RECT rect = ArrangeWindow(RECT({ 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) }));
		MoveWindow(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);

		Device->FullScreen(true);

		if (!Device->IsFullScreen())
		{
			SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_NORMAL);
			return;
		}

		UpdateWindow(m_hWnd);

		WindowsMode = value;
	}
		break;
	case 1:		// Borderless Windowed
	{
		if (Device->IsFullScreen())
		{
			Device->FullScreen(false);
		}

		SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_BORDERLESS);

		RECT rect = ArrangeWindow(RECT({ 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) }));
		AdjustWindowRect(&rect, WINDOW_STYLE_BORDERLESS, FALSE);
		MoveWindow(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
		UpdateWindow(m_hWnd);

		WindowsMode = value;
	}
		break;
	case 2:		// Windowed
	{
		if (Device->IsFullScreen())
		{
			Device->FullScreen(false);
		}

		SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_NORMAL);

		RECT rect = ArrangeWindow(RECT({ 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) }));
		AdjustWindowRect(&rect, WINDOW_STYLE_NORMAL, FALSE);
		MoveWindow(m_hWnd, rect.left/2, rect.top/2, rect.right - rect.left, rect.bottom - rect.top, true);
		UpdateWindow(m_hWnd);

		WindowsMode = value;
	}
		break;
	default:
		break;
	}

	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(hCursor);
}

void WindowsPlatform::ResizeWindow(std::size_t Width, std::size_t Height)
{
	WindowWidth = (uint32_t)Width;
	WindowHeight = (uint32_t)Height;

	RECT rect = ArrangeWindow(RECT({ 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) }));

	if (WindowsMode == 0) // Fullscreen
	{
		AdjustWindowRect(&rect, WINDOW_STYLE_BORDERLESS, FALSE);
	}
	else if (WindowsMode == 1) // Borderless Windowed
	{
		AdjustWindowRect(&rect, WINDOW_STYLE_BORDERLESS, FALSE);
	}
	else if (WindowsMode == 2) // Windowed
	{
		AdjustWindowRect(&rect, WINDOW_STYLE_NORMAL, FALSE);
	}

	MoveWindow(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);

	Device->ResizeWindow(WindowWidth, WindowHeight);
	Renderer->ResizeWindow(WindowWidth, WindowHeight);

	ShowWindow(m_hWnd, SW_RESTORE);
	UpdateWindow(m_hWnd);

	for (const auto& Canvas : Canvases)
		Canvas->ResizeWindow(WindowWidth, WindowHeight);

	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(hCursor);
}

void WindowsPlatform::Vsync(const bool value)
{
	Device->Vsync(value);
}

void WindowsPlatform::VsyncInterval(const UINT value)
{
	Device->VsyncInterval(value);
}

HRESULT WindowsPlatform::GetDisplayResolution(int& width, int& height)
{
	if (m_hWnd != INVALID_HANDLE_VALUE)
	{
		HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);

		if (GetMonitorInfo(monitor, &info))
		{
			width = info.rcMonitor.right - info.rcMonitor.left;
			height = info.rcMonitor.bottom - info.rcMonitor.top;
			return S_OK;
		}
	}

	return E_FAIL;
}

void WindowsPlatform::BeginPlay()
{
	for (auto& Canvas : Canvases)
	{
		Canvas->BeginPlay();
	}
	Renderer->BeginPlay();
}

void WindowsPlatform::Tick(double fElapsedTimeSeconds)
{
	for (auto& Canvas : Canvases)
	{
		Canvas->Tick((float)fElapsedTimeSeconds);
	}
	Renderer->Tick((float)fElapsedTimeSeconds);
}

void WindowsPlatform::Render()
{
	Renderer->Render();
}
