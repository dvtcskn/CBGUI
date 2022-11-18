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

#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <string>

#include "StepTimer.h"
#include "framework.h"
#include "AbstractLayer.h"
#include "ICanvas.h"

class WindowsPlatform final
{
public:
	WindowsPlatform();
	virtual ~WindowsPlatform();

	LRESULT MsgProc(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam);
	void BeginPlay();
	void Render();
	void Tick(double DeltaTime);
	void MessageLoop();

	HWND GetHWND() { return m_hWnd; }

	void RequestTermination();

	const std::vector<ICanvas*>& GetCanvases() const { return Canvases; }
	const std::size_t GetCanvasSize() const { return Canvases.size(); }
	const ICanvas* GetCanvas(const std::size_t index) const { return Canvases.at(index); }

	void WindowMode(const int value);
	void ResizeWindow(std::size_t Width, std::size_t Height);
	void Vsync(const bool value);
	void VsyncInterval(const UINT value);

	std::uint32_t GetWindowWidth() const { return WindowWidth; }
	std::uint32_t GetWindowHeight() const { return WindowHeight; }
	bool IsWindowFullscreen() const { return WindowFullscreen; }

	HRESULT GetDisplayResolution(int& width, int& height);

private:
	void InitGI();
	bool CreateWindows(std::wstring title, RECT rect, bool bfullscreen);

	void ConstructWidgets();

private:
	std::unique_ptr<AbstractGI> Device;
	std::unique_ptr<IRenderer> Renderer;

	std::vector<ICanvas*> Canvases;

private:
	StepTimer mStepTimer;
	HWND m_hWnd;

	int WindowsMode;
	std::uint32_t WindowWidth;
	std::uint32_t WindowHeight;
	bool WindowFullscreen;
};
