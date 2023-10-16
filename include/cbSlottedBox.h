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

#include "cbWidget.h"

namespace cbgui
{
	class cbSlot;

	/* Abstract base class for Slotted Widgets. */
	class cbSlottedBox : public cbWidget
	{
		cbClassBody(cbClassNoDefaults, cbSlottedBox, cbWidget);
		friend cbSlot;
	protected:
		cbSlottedBox()
			: cbWidget()
			, bIsItWrapped(false)
		{}

		cbSlottedBox(const cbSlottedBox& Widget)
			: cbWidget(Widget)
			, bIsItWrapped(Widget.bIsItWrapped)
		{}

	private:
		bool bIsItWrapped;

	public:
		virtual ~cbSlottedBox() = default;

		virtual void BeginPlay() override final;
		virtual void Tick(float DeltaTime) override final;
	private:
		virtual void OnBeginPlay() {}
		virtual void OnTick(float DeltaTime) {}

	public:
		virtual std::vector<cbSlot*> GetSlots() const = 0;
		virtual cbSlot* GetSlot(const std::size_t Index) const = 0;
		virtual std::size_t GetSlotSize(const bool ExcludeHidden = false) const = 0;
	private:
		virtual std::size_t GetSlotIndex(const cbSlot* Slot) const = 0;

	public:
		virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const override;
		virtual bool OnKeyUp(std::uint64_t KeyCode) override;
		virtual bool OnKeyDown(std::uint64_t KeyCode) override;
		virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) override;
		virtual bool OnMouseMove(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) override;

		/* Resets the focus and input state. */
		virtual void ResetInput() override final;

	public:
		bool IsItWrapped() const;
		/* Compresses the Slotted Widget to the unaligned maximum dimensions of all the slot content in it. */
		bool Wrap();
		/* Decompresses/UnWrap the box. */
		bool UnWrap();

	private:
		/* Compresses/Wrap Vertical. */
		virtual bool WrapVertical() = 0;
		/* Compresses/Wrap Horizontal. */
		virtual bool WrapHorizontal() = 0;
		/* Decompresses/UnWrap Vertical. */
		virtual bool UnWrapVertical() = 0;
		/* Decompresses/UnWrap Horizontal. */
		virtual bool UnWrapHorizontal() = 0;

	public:
		void UpdateSlotAlignments() { UpdateSlotVerticalAlignment(); UpdateSlotHorizontalAlignment(); }
	private:
		virtual void UpdateSlotVerticalAlignment() = 0;
		virtual void UpdateSlotHorizontalAlignment() = 0;

	private:
		/*
		* Necessary functions for slotand Slotted Widget communication.
		*/

		/* Called when the Slot or Slot content visibility changes. */
		void Notify_VisibilityChanged(cbSlot* Sender);
		virtual void OnSlotVisibilityChanged(cbSlot* Slot) = 0;
		/* Called when the Slot content Dimension changes. */
		void Notify_DimensionChanged(cbSlot* Sender);
		virtual void OnSlotDimensionUpdated(cbSlot* Slot) = 0;
		/* Called when the Slot or Slot content requested Remove From Parent. */
		void Notify_RemoveFromParent(cbSlot* Sender);
		virtual bool OnRemoveSlot(cbSlot* Slot) = 0;

		/* Called when the Slot or Slot content Replaced. */
		void SlotContentInsertedOrReplaced(cbSlot* Slot);
	};

	/*
	* Abstract base class for Slot Widgets.
	*/
	class cbSlot : public cbWidgetObj
	{
		cbClassBody(cbClassNoDefaults, cbSlot, cbWidgetObj)
		friend class cbWidget;
		friend cbSlottedBox;
	private:
		cbSlottedBox* Owner;
		bool bIsEnabled;
		/*
		* To Do :
		* Cache CulledBounds
		*/
		//cbBounds CulledBounds;
	protected:
		cbSlot(cbSlottedBox* pOwner)
			: Super()
			, Owner(pOwner)
			, bIsEnabled(true)
		{}

		cbSlot(const cbSlot& Widget, cbSlottedBox* NewOwner)
			: Super()
			, Owner(NewOwner ? NewOwner : Widget.Owner)
			, bIsEnabled(Widget.bIsEnabled)
		{}

	public:
		virtual ~cbSlot()
		{
			Owner = nullptr;
		}

	public:
		void BeginPlay() { if (HasContent()) GetContent()->BeginPlay(); OnBeginPlay(); }
		void Tick(float DeltaTime) { if (HasContent()) GetContent()->Tick(DeltaTime); OnTick(DeltaTime); }
	private:
		virtual void OnBeginPlay() { }
		virtual void OnTick(float DeltaTime) { }

	public:
		template<typename T>
		std::shared_ptr<T> Clone(cbSlottedBox* NewOwner) { return std::static_pointer_cast<T>(CloneSlot(NewOwner)); }
		virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) = 0;

	public:
		virtual std::string GetName() const override;

		virtual bool IsInserted() const = 0;

		/* Returns slot index. */
		std::size_t GetIndex() const;

		virtual bool HasGeometry() const override { return false; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override;

		/* Returns true if Content exist. */
		virtual bool HasAnyChildren() const override { return false; }
		/* Returns raw Content. */
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override { return std::vector<cbWidgetObj*>(); }

		virtual bool IsInside(const cbVector& Location) const
		{
			if (GetRotation() != 0.0f)
				return cbgui::IsInside(GetBounds(), Location, GetRotation(), GetRotatorOrigin());
			return cbgui::IsInside(GetBounds(), Location);
		}
		virtual bool Intersect(const cbBounds& Other) const
		{
			if (GetRotation() != 0.0f)
				return cbgui::Intersect(GetBounds(), Other, GetRotation(), GetRotatorOrigin());
			return cbgui::Intersect(GetBounds(), Other);
		}

		template<class T>
		constexpr T* GetOwner() const
		{
			return cbgui::cbCast<T>(Owner);
		}
		virtual bool HasOwner() const override final { return Owner != nullptr; }
		virtual cbWidgetObj* GetOwner() const override final { return Owner; }

		bool HasCanvas() const { return IsInserted() ? Owner->HasCanvas() : false; }
		cbICanvas* GetCanvas() const { return IsInserted() ? Owner->GetCanvas() : nullptr; }

		/* Returns true if Content exist. */
		virtual bool HasContent() const = 0;
		template<class T>
		std::shared_ptr<T> GetSharedContent(const bool Dynamic = false) const
		{
			if (Dynamic)
				return std::dynamic_pointer_cast<T>(GetSharedContent());
			return std::static_pointer_cast<T>(GetSharedContent());
		}
		/* Returns Shared Content. */
		virtual cbWidget::SharedPtr GetSharedContent() const = 0;
		template<class T>
		T* GetContent() const
		{
			return cbgui::cbCast<T>(GetContent());
		}
		/* Returns Raw Content. */
		virtual cbWidget* GetContent() const = 0;
		virtual void ReplaceContent(const cbWidget::SharedPtr& Content) = 0;

		void RemoveFromParent();

		virtual void SetVertexColorAlpha(std::optional<float> Alpha, bool PropagateToChildren = true) override final;
		virtual std::optional<float> GetVertexColorAlpha() const override final;

		/* Returns owner origin. */
		virtual cbVector GetOrigin() const override final;
		virtual cbVector GetRotatorOrigin() const override final;
		/* Returns owner rotation. */
		virtual bool IsRotated() const override final;
		virtual float GetRotation() const override final;

		virtual bool IsEnabled() const override final;
		/* Returns content state. */
		bool IsContentEnabled() const;
		void EnableContent();
		void DisableContent();

		void UpdateAlignments() { UpdateVerticalAlignment(); UpdateHorizontalAlignment(); }
		virtual void UpdateVerticalAlignment() = 0;
		virtual void UpdateHorizontalAlignment() = 0;
		virtual void UpdateRotation() { if (HasContent()) GetContent()->UpdateRotation(); }
		void UpdateStatus();

		/*
		* To Do :
		* Cache CulledBounds
		*/
		//void UpdateCulledBounds();
		//void UpdateVerticalCulledBounds();
		//void UpdateHorizontalCulledBounds();
		virtual cbBounds GetCulledBounds() const override;
		/* Checks if it appears on the screen. */
		virtual bool IsItCulled() const override;

		/* Returns ZOrder Mode. Default : eZOrderMode::InOrder */
		virtual eZOrderMode GetZOrderMode() const override;
		/* Returns owner ZOrder. */
		virtual std::int32_t GetZOrder() const override;

		/* Indicates whether content is an active widget. */
		virtual bool IsFocused() const;
		/* Indicates whether content will be an active widget. */
		virtual bool IsFocusable() const;
		/* Returns content Focus Mode. */
		virtual eFocusMode GetFocusMode() const;

		/* 
		* Hide/Collapse or make the slot content visible.
		* To Do: Create a Visibility State in the slot.
		*/
		virtual void Hidden(bool value);

		virtual bool IsVisible() const override { return HasContent() ? GetContent()->IsVisible() : false; }
		virtual bool IsInvisible() const override { return HasContent() ? GetContent()->IsInvisible() : true; }
		/* Returns whether the content is hidden/collapsed. */
		virtual bool IsHidden() const override { return HasContent() ? GetContent()->IsHidden() : true; }

		virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const;
		virtual bool OnKeyUp(std::uint64_t KeyCode);
		virtual bool OnKeyDown(std::uint64_t KeyCode);
		virtual bool IsInteractableWithMouse() const;
		virtual bool OnMouseEnter(const cbMouseInput& Mouse);
		virtual bool OnMouseLeave(const cbMouseInput& Mouse);
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse);
		virtual bool OnMouseMove(const cbMouseInput& Mouse);
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse);
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse);
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse);
		virtual void ResetInput();

	private:
		void Notify_DimensionChanged();
		void Notify_RemoveFromParent();
		void Notify_VisibilityChanged();

	protected:
		/* Called when Content Replaced.*/
		void OnContentInsertedOrReplaced();
	};
}
