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

#include <string>
#include <cbCanvas.h>
#include <AbstractLayer.h>
#include "UIMaterialStyle.h"

class WindowsPlatform;

struct cbKeyboardChar
{
	bool bIsPressed = false;
	std::uint64_t KeyCode = 0;
};

class ICanvas : public cbgui::cbICanvas
{
	cbBaseClassBody(cbClassNoDefaults, ICanvas)
public:
	struct GeometryDrawParams
	{
		std::size_t VertexOffset;
		std::size_t IndexOffset;
		std::size_t VertexSize;
		std::size_t IndexSize;

		GeometryDrawParams()
			: VertexOffset(0)
			, IndexOffset(0)
			, VertexSize(0)
			, IndexSize(0)
		{}

		~GeometryDrawParams() = default;
	};

	struct WidgetHierarchy
	{
		std::vector<WidgetHierarchy*> Nodes;

		cbgui::cbWidgetObj* Widget;
		bool bVertexDirty = true;
		bool bIndexDirty = true;
		std::optional<std::string> MaterialName;
		GeometryDrawParams DrawParams;

		WidgetHierarchy()
			: Widget(nullptr)
			, MaterialName(std::nullopt)
			, bVertexDirty(true)
			, bIndexDirty(true)
		{}

		~WidgetHierarchy()
		{
			Widget = nullptr;
			for (auto& Node : Nodes)
			{
				delete Node;
				Node = nullptr;
			}
			Nodes.clear();
		}
	};

protected:
	WindowsPlatform* PlatformOwner;

protected:
	ICanvas(WindowsPlatform* Owner)
		: PlatformOwner(Owner)
	{}
public:
	virtual ~ICanvas()
	{
		PlatformOwner = nullptr;
	}

	virtual void BeginPlay() = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam) = 0;

	virtual AVertexBuffer* GetVertexBuffer() const = 0;
	virtual AIndexBuffer* GetIndexBuffer() const = 0;

	virtual std::vector<WidgetHierarchy*> GetWidgetHierarchy() const = 0;

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) = 0;

	WindowsPlatform* GetPlatformOwner() const { return PlatformOwner; };
};
