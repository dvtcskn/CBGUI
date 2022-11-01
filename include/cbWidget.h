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
#include <optional>
#include <memory>
#include <vector>
#include <functional>

#include "cbClassBody.h"
#include "cbStates.h"
#include "cbInput.h"
#include "cbTransform.h"
#include "cbGeometry.h"

#include "cbCanvas.h"

namespace cbgui
{
	class cbSlot;
	struct cbComponent;

	/* Interface class for all Widget Objects. */
	class cbWidgetObj
	{
		cbBaseClassBody(cbClassDefaultProtectedConstructor, cbWidgetObj)
	public:
		virtual std::string GetName() const = 0;

		/*
		* If it has no owner and no canvas, returns the center position.
		* If owner exists, returns the origin of the owner.
		* If canvas exists, returns the center position of the canvas.
		*/
		virtual cbVector GetOrigin() const = 0;

		virtual cbBounds GetBounds() const = 0;
		/* Return Location of the Widget, in world space */
		virtual cbVector GetLocation() const = 0;
		/* 2D Rotation. Requires Stencil Clipping during rendering, */
		virtual float GetRotation() const = 0;
		/* Returns Aligned/Wrapped dimension of the Widget */
		virtual cbDimension GetDimension() const = 0;
		/* Return Aligned/Wrapped width of the Widget */
		virtual float GetWidth() const = 0;
		/* Return Aligned/Wrapped height of the Widget */
		virtual float GetHeight() const = 0;

		/* Checks if it appears on the screen. */
		virtual bool IsItCulled() const = 0;
		/*
		* Scissor Rects
		* If rotated, it requires Stencil Clipping during rendering, 
		* otherwise it will give bad results.
		*/
		virtual cbBounds GetCulledBounds() const = 0;

		virtual eZOrderMode GetZOrderMode() const = 0;
		virtual std::int32_t GetZOrder() const = 0;

		virtual bool IsEnabled() const = 0;
		virtual bool IsVisible() const = 0;
		/* 
		* If the widget is invisible, it can still be focused. 
		* If there is geometry, it will not be drawn.
		*/
		virtual bool IsInvisible() const = 0;
		/* If the widget is Hidden/Collapsed, it cannot be focused. */
		virtual bool IsHidden() const = 0;

		virtual bool HasOwner() const = 0;
		virtual cbWidgetObj* GetOwner() const = 0;

		/* Returns owner by index. */
		template<class T>
		T* GetRootOwner(std::optional<std::size_t> OwnerIndex = std::nullopt, const bool SkipSlot = true, const bool Dynamic = false) const
		{
			if (Dynamic)
				return dynamic_cast<T*>(GetRootOwner(OwnerIndex, SkipSlot));
			return static_cast<T*>(GetRootOwner(OwnerIndex, SkipSlot));
		}
		/* Returns owner by index. */
		cbWidgetObj* GetRootOwner(std::optional<std::size_t> OwnerIndex = std::nullopt, const bool SkipSlot = true) const;

		virtual bool HasGeometry() const = 0;
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const = 0;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const = 0;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const = 0;

		virtual bool HasAnyChildren() const = 0;
		virtual std::vector<cbWidgetObj*> GetAllChildren() const = 0;
	};

	/* Abstract base class for Widget. */
	class cbWidget : public cbWidgetObj, public std::enable_shared_from_this<cbWidget>
	{
		cbClassBody(cbClassNoDefaults, cbWidget, cbWidgetObj)
		friend cbSlot;
	protected:
		cbWidget();

	public:
		virtual ~cbWidget();

		virtual void BeginPlay() = 0;
		virtual void Tick(float DeltaTime) = 0;

		inline void SetName(const std::string& inName) { Name = inName; }
		virtual std::string GetName() const override final { return Name.has_value() ? Name.value() : GetClassID(); }

		bool IsItSharedObject() const { return !weak_from_this().expired(); }
		std::optional<long> GetSharedObjectUseCount() const { if (!IsItSharedObject()) return std::nullopt; return weak_from_this().use_count(); }

		/*
		* If it has no owner and no canvas, returns the center position.
		* If owner exists, returns the origin of the owner.
		* If canvas exists, returns the center position of the canvas.
		*/
		virtual cbVector GetOrigin() const override;

		/* Return NonAligned/Wrapped width of the Widget */
		virtual float GetNonAlignedWidth() const = 0;
		/* Return NonAligned/Wrapped height of the Widget */
		virtual float GetNonAlignedHeight() const = 0;

		/* Set the Screen Position of the Widget.*/
		virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false) = 0;
		void SetLocation(const cbVector& Location, bool Force = false) { SetXY(Location.X, Location.Y, Force); }
		void SetX(const float X, bool Force = false) { SetXY(X, std::nullopt, Force); }
		void SetY(const float Y, bool Force = false) { SetXY(std::nullopt, Y, Force); }

		/* Set NonAligned/NonWrapped Dimension of the Widget */
		virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height) = 0;
		void SetDimension(const cbDimension& Dimension) { SetWidthHeight(Dimension.Width, Dimension.Height); }
		void SetWidth(const float Width) { SetWidthHeight(Width, std::nullopt); }
		void SetHeight(const float Height) { SetWidthHeight(std::nullopt, Height); }

		bool IsRotated() const { return GetRotation() != 0.0f; }
		/* 2D Rotation. Requires Stencil Clipping during rendering, */
		virtual void SetRotation(const float Rotation) = 0;

		virtual cbMargin GetPadding() const = 0;
		/* Padding is used to align with offset. The padding only works if attached into slot. */
		virtual void SetPadding(const cbMargin& Padding) = 0;

		virtual bool IsInside(const cbVector& Location) const;
		virtual bool Intersect(const cbBounds& Other) const;

		/* Checks if it appears on the screen. */
		virtual bool IsItCulled() const override;
		/*
		* Scissor Rects
		* If rotated, it requires Stencil Clipping during rendering,
		* otherwise it will give bad results.
		*/
		virtual cbBounds GetCulledBounds() const override;

		void SetZOrderMode(const eZOrderMode Mode);
		virtual eZOrderMode GetZOrderMode() const override final { return ZOrderMode; }
		void SetZOrder(const std::int32_t& Order);
		virtual std::int32_t GetZOrder() const override final;

		inline eAnchors GetVerticalAnchor() const { return VerticalAnchor; }
		inline eAnchors GetHorizontalAnchor() const { return HorizontalAnchor; }
		inline eVerticalAlignment GetVerticalAlignment() const { return mVAlignment; }
		inline eHorizontalAlignment GetHorizontalAlignment() const { return mHAlignment; }

		void SetCanvasAnchor(const eCanvasAnchor CanvasAnchor);
		void SetVerticalAnchor(const eAnchors& Anchor);
		void SetHorizontalAnchor(const eAnchors& Anchor);
		void SetVerticalAlignment(const eVerticalAlignment& CustomAlignment);
		void SetHorizontalAlignment(const eHorizontalAlignment& CustomAlignment);

		void UpdateAlignments(const bool ForceAlign = false);
		virtual void UpdateVerticalAlignment(const bool ForceAlign = false) = 0;
		virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) = 0;
		virtual void UpdateRotation() = 0;
		/* Updates the Enable/Disable status. Propagates to children. */
		virtual void UpdateStatus() = 0;

		inline eCanvasAnchor GetCanvasAnchor() const { return CanvasAnchor; }
		/* Adds a widget to the screen. */
		void AddToCanvas(cbICanvas* Canvas);
		/* Removes a widget from the screen. */
		void RemoveFromCanvas();
	private:
		virtual void OnAttachToCanvas() {}
		virtual void OnRemoveFromCanvas() {}

	public:
		inline bool IsAlignedToCanvas() const { return bIsAlignedToCanvas && Canvas && !Owner; }
		void SetAlignToCanvas(bool Value);
		void AlignToCanvas(const bool ForceAlign = false);

		/* 
		* Removes widget from the parent widget.
		* AddToCanvas() is called if KeepItOnTheCanvas is true.
		*/
		void RemoveFromParent(const bool KeepItOnTheCanvas = false);
		/* Attaches and aligns the widget to the parent widget(Slot). */
		void AttachToSlot(cbSlot* Parent);
	private:
		virtual void OnAttach() = 0;
		virtual void OnRemoveFromParent() = 0;

	protected:
		/* Notify owner, dimension updated. The owner realigns the widget. */
		bool DimensionUpdated();
		/* Notify canvas, transform updated. */
		bool NotifyCanvas_WidgetUpdated();

	public:
		template<class T>
		constexpr T* GetOwner(const bool Dynamic = false) const
		{
			if (Dynamic)
				return dynamic_cast<T*>(Owner);
			return static_cast<T*>(Owner);
		}
		virtual bool HasOwner() const override final;
		virtual cbWidgetObj* GetOwner() const override final;

		template<class T>
		T* GetCanvas(const bool Dynamic = false) const
		{
			if (Dynamic)
				return dynamic_cast<T*>(GetCanvas());
			return static_cast<T*>(GetCanvas());
		}
		/* If the canvas does not exist, it checks if the owner has a canvas. */
		virtual bool HasCanvas() const;
		/* If the canvas does not exist, it returns the owner canvas. */
		virtual cbICanvas* GetCanvas() const;

		virtual bool HasAnyComponents() const = 0;
		virtual std::vector<cbComponent*> GetAllComponents() const = 0;

	private:
		virtual void OnFocus() {}
		virtual void OnLostFocus() {}
		virtual void OnVisibilityChanged() {}

	public:
		/* Indicates whether there is an active widget. */
		inline bool IsFocused() const { return bFocused; }
		/* Indicates whether it will be an active widget. */
		inline bool IsFocusable() const { return /*IsHidden() ? false :*/ bFocusable; }
		inline eFocusMode GetFocusMode() const { return FocusMode; }
		/* Determines whether it can be an active widget. */
		inline void SetFocusable(bool value) { SetFocus(false); bFocusable = value; }
		inline void SetFocusMode(const eFocusMode Mode) { SetFocus(false); FocusMode = Mode; }
		/* Determines whether it is an active widget. */
		void SetFocus(const bool value);

		virtual bool IsVisible() const override final { return GetVisibilityState() == eVisibility::Visible; }
		/*
		* If the widget is invisible, it can still be focused.
		* If there is geometry, it will not be drawn.
		*/
		virtual bool IsInvisible() const override final { return GetVisibilityState() == eVisibility::Invisible; }
		/* If the widget is Hidden/Collapsed, it cannot be focused. */
		virtual bool IsHidden() const override final { return GetVisibilityState() == eVisibility::Hidden; }
		eVisibility GetVisibilityState() const { return Visibility; }
		/* 
		* Sets its visibility.
		* If collapsed/hidden, it cannot be visible and cannot be focused.
		*/
		void SetVisibilityState(const eVisibility visibility);

		virtual bool IsEnabled() const override final;
		bool Enable();
		bool Disable();
	private:
		virtual void OnDisabled() {}
		virtual void OnEnabled() {}

	public:
		virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const { return IsFocusable() ? true : false; }
		virtual bool OnKeyUp(std::uint64_t KeyCode) { return false; }
		virtual bool OnKeyDown(std::uint64_t KeyCode) { return false; }
		virtual bool IsInteractableWithMouse() const { return true; }
		virtual bool OnMouseEnter(const cbMouseInput& Mouse) { return false; }
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) { return false; }
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) { return false; }
		virtual bool OnMouseMove(const cbMouseInput& Mouse) { return false; }
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) { return false; }
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) { return false; }
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) { return false; }

		/* Resets the focus and input state. */
		virtual void ResetInput() = 0;

	public:
		bool bShouldNotifyCanvas;

	private:
		std::optional<std::string> Name;

		bool bIsEnabled;
		eVisibility Visibility;
		bool bFocusable;
		bool bFocused;
		eFocusMode FocusMode;

		bool bIsAlignedToCanvas;
		eVerticalAlignment mVAlignment;
		eHorizontalAlignment mHAlignment;
		eAnchors VerticalAnchor;
		eAnchors HorizontalAnchor;
		eCanvasAnchor CanvasAnchor;

		eZOrderMode ZOrderMode;
		std::int32_t ZOrder;

		cbSlot* Owner;
		cbICanvas* Canvas;
	};
}
