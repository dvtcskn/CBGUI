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

#include <cbgui.h>
#include <unordered_map>
#include "WindowsPlatform.h"
#include "ICanvas.h"
#include "Animation.h"

class DemoCanvasBase : public ICanvas
{
	cbClassBody(cbClassConstructor, DemoCanvasBase, ICanvas)
public:
	DemoCanvasBase(WindowsPlatform* WPlatformOwner);
	DemoCanvasBase(WindowsPlatform* WPlatformOwner, const cbDimension& pScreenDimension);
public:
	virtual ~DemoCanvasBase();

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

	virtual void ResizeWindow(std::size_t Width, std::size_t Height) override;

	virtual void InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	virtual std::string GetName() const override { return Name; }
	void SetName(const std::string& name) { Name = name; }

	virtual cbVector GetCenter() const override { return Transform.GetCenter(); }
	virtual cbBounds GetScreenBounds() const override { return Transform.GetBounds(); }
	virtual float GetScreenRotation() const override { return Transform.GetRotation(); }
	virtual cbDimension GetScreenDimension() const override { return Transform.GetDimension(); }
	void SetScreenDimension(const cbDimension& pScreenDimension);

	virtual void ZOrderModeUpdated(cbWidgetObj* Object) override {}
	virtual void ZOrderChanged(cbWidgetObj* Object, const std::int32_t ZOrder);

	virtual void Add(cbWidget* Object) override;
	virtual void Add(const std::shared_ptr<cbWidget>& Object) override;
	virtual bool IsWidgetExist(cbWidget* Widget) const override;
	virtual void RemoveFromCanvas(cbWidget* Object) override;

	virtual void WidgetUpdated(cbWidgetObj* Object) override;

	virtual void NewSlotAdded(cbSlottedBox* Parent, cbSlot* Slot) override;
	virtual void SlotRemoved(cbSlottedBox* Parent, cbSlot* Slot) override;
	virtual void SlotContentReplaced(cbSlot* Parent, cbWidget* Old, cbWidget* New) override;

	virtual void VisibilityChanged(cbWidgetObj* Object) override {}

	virtual void VerticesSizeChanged(cbWidgetObj* Object, const std::size_t NewSize) override;

	virtual void NewSlotContentAdded(cbSlot* Parent, cbWidget* Content);

	virtual std::vector<cbWidget*> GetOverlappingWidgets(const cbBounds& Bounds) const override;

	std::vector<cbWidget::SharedPtr> GetWidgets() const { return Widgets; }
	virtual std::vector<WidgetHierarchy*> GetWidgetHierarchy() const override { return Hierarchy; }

	virtual AVertexBuffer* GetVertexBuffer() const override { return VertexBuffer.get(); }
	virtual AIndexBuffer* GetIndexBuffer() const override { return IndexBuffer.get(); }

	cbAnimation* GetAnimation() const { return Animation.get(); }

	/*
	* Set material by class ID, geometry component name, or geometry type.
	*/
	virtual void SetMaterial(WidgetHierarchy* pWP);

private:
	void SortWidgetsByZOrder();
	void ReorderWidgets();
	void ReuploadGeometry();

	void SetVertexOffset(WidgetHierarchy* pWP);
	void SetIndexOffset(WidgetHierarchy* pWP);

private:
	cbgui::cbTransform Transform;

	cbWidget* FocusedObj;

	std::string Name;

	std::shared_ptr<cbAnimation> Animation;

	std::vector<cbWidget::SharedPtr> Widgets;
	std::vector<WidgetHierarchy*> Hierarchy;

	std::map<cbWidgetObj*, WidgetHierarchy*> WidgetsList;

	std::size_t PrevVertexOffset;
	std::size_t PrevIndexOffset;

	std::unique_ptr<AVertexBuffer> VertexBuffer;
	std::unique_ptr<AIndexBuffer> IndexBuffer;

	cbKeyboardChar KeyboardBTNEvent;
};
