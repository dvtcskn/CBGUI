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

#include <vector>
#include "cbClassBody.h"
#include "cbMath.h"

namespace cbgui
{
	class cbWidgetObj;
	class cbWidget;
	class cbSlottedBox;
	class cbSlot;

	/* Canvas is a layout widget that lets you arrange child widgets and position them in a relative coordinate space. */
	class cbICanvas
	{
		cbBaseClassBody(cbClassDefaultProtectedConstructor, cbICanvas)
	public:
		virtual std::string GetName() const = 0;

		virtual cbBounds GetScreenBounds() const = 0;
		virtual cbDimension GetScreenDimension() const = 0;
		virtual cbVector GetCenter() const = 0;
		virtual float GetScreenRotation() const = 0;

		virtual void Add(cbWidget* Widget) = 0;
		virtual void Add(const std::shared_ptr<cbWidget>& Widget) = 0;
		virtual bool IsWidgetExist(cbWidget* Widget) const = 0;
		virtual void RemoveFromCanvas(cbWidget* Widget) = 0;

		/* Called when any widget transform is updated or the Vertex Color Style is updated. */
		virtual void WidgetUpdated(cbWidgetObj* Widget) = 0;
		/*
		* Called when the vertex size gets bigger or smaller. 
		* e.g; When a letter is added to the text.
		*/
		virtual void VerticesSizeChanged(cbWidgetObj* Widget, const std::size_t NewSize) = 0;

		virtual void NewSlotAdded(cbSlottedBox* Parent, cbSlot* Slot) = 0;
		virtual void SlotRemoved(cbSlottedBox* Parent, cbSlot* Slot) = 0;
		virtual void SlotContentReplaced(cbSlot* Parent, cbWidget* Old, cbWidget* New) = 0;

		virtual void VisibilityChanged(cbWidgetObj* Widget) = 0;

		virtual void ZOrderModeUpdated(cbWidgetObj* Widget) = 0;
		virtual void ZOrderChanged(cbWidgetObj* Widget, const std::int32_t ZOrder) = 0;

		/*
		* Returns all cbWidgets that intersect the Bounds.
		* Useful for drag and drop operation.
		*/
		virtual std::vector<cbWidget*> GetOverlappingWidgets(const cbBounds& Bounds) const= 0;
	};
}
