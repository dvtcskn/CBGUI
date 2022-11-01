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

#include <memory>
#include "cbSlottedBox.h"
#include "cbComponent.h"
#include "cbGeometry.h"

namespace cbgui
{
	/* ScrollBox can navigate through numerous widgets. It can be horizontal or vertical. */
	class cbScrollBox : public cbSlottedBox
	{
		cbClassBody(cbClassConstructor, cbScrollBox, cbSlottedBox);
	protected:
		/* ScrollBox default ScrollBar Component. */
		class cbScrollBarComponent : public cbComponent
		{
			cbClassBody(cbClassConstructor, cbScrollBarComponent, cbComponent)
				friend cbScrollBox;
		protected:
			/* ScrollBar default Handle Component. */
			class cbScrollBarHandleComponent : public cbComponent
			{
				cbClassBody(cbClassConstructor, cbScrollBarHandleComponent, cbComponent)
			private:
				cbButtonVertexColorStyle VertexColorStyle;
				eButtonState ButtonState;
				std::optional<cbVector> mouseOffset;
				float Percent;
				float Length;
				float Thickness;
				bool bIsScrollable;
				float LocationOffset;
				bool bIsItFocused;

			public:
				cbScrollBarHandleComponent(cbScrollBarComponent* pOwner);
				virtual ~cbScrollBarHandleComponent();

				virtual cbBounds GetBounds() const override final { return cbBounds(GetDimension(), GetLocation()); }
				virtual cbVector GetLocation() const override final;
				virtual cbDimension GetDimension() const override final;
				virtual float GetWidth() const override final;
				virtual float GetHeight() const override final;

				/* Aligns and checks Length */
				virtual void UpdateVerticalAlignment() override final;
				/* Aligns and checks Length */
				virtual void UpdateHorizontalAlignment() override final;

				virtual void UpdateRotation() override final;
				virtual void UpdateStatus() override final;

				/* Scroll by percentage. */
				bool Slide(const float Percent);
				/* Scroll by position. */
				bool Slide(const cbVector& Location);
				/* Return Scroll percent */
				inline const float GetPercent() const { return Percent; }
				inline const float GetLength() const { return Length; }

				inline bool IsScrollable() const { return bIsScrollable; }

				inline float GetThickness() const { return Thickness; }
				void SetThickness(const float Thickness);

				inline std::optional<cbVector> GetMouseOffset() const { return mouseOffset; }
				void SetMouseOffset(cbVector Location);
				inline void ClearMouseOffset() { mouseOffset = std::nullopt; }

				void Enable();
				void Disable();

				inline bool IsPressed() const { return ButtonState == eButtonState::Pressed; }
				inline bool IsHovered() const { return ButtonState == eButtonState::Hovered; }

				bool OnMouseEnter(const cbMouseInput& Mouse);
				bool OnMouseLeave(const cbMouseInput& Mouse);
				bool OnMouseMove(const cbMouseInput& Mouse);
				bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse);
				bool OnMouseButtonUp(const cbMouseInput& Mouse);
				bool OnMouseButtonDown(const cbMouseInput& Mouse);

				/* Resets the focus and input state. */
				virtual void ResetInput() override final;

				eButtonState GetState() const { return ButtonState; }

				/* Determines whether it is an active handle. */
				inline void SetFocus(const bool value) { bIsItFocused = value; }
				/* Indicates whether there is an active handle. */
				bool IsFocused() const { if (!GetOwner<cbScrollBox>()->IsFocusable()) return false; return bIsItFocused; }

			public:
				virtual bool HasGeometry() const override final { return true; }
				virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
				virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
				virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;
				cbButtonVertexColorStyle GetVertexColorStyle() const { return VertexColorStyle; }
				void SetVertexColorStyle(const cbButtonVertexColorStyle& style);

			private:
				void OnPressed();
				void OnReleased();
				void OnHovered();
				void OnUnhovered();

				void SetButtonStatus(const eButtonState ButtonStatus);
				/* Sets handle length. */
				bool SetLength(float lenght);
				/* Sets handle position offset. */
				bool SetLocationOffset(const float value);
			};

		private:
			cbVertexColorStyle VertexColorStyle;
			float Thickness;
			float BarThickness;
			// Offset
			float ScrollAmount;
			cbMargin Padding;
			float ScrollStepPercent;
			bool bIsHidden;
			bool bShowScrollBarIfScrollable;
			cbScrollBarHandleComponent::SharedPtr Handle;

		public:
			cbScrollBarComponent(cbScrollBox* pOwner)
				: Super(pOwner)
				, Thickness(10.0f)
				, BarThickness(5.0f)
				, ScrollAmount(0.0f)
				, Padding(cbMargin(0))
				, ScrollStepPercent(0.1f)
				, bIsHidden(false)
				, bShowScrollBarIfScrollable(false)
				, Handle(cbScrollBarHandleComponent::Create(this))
			{}

			virtual ~cbScrollBarComponent()
			{
				Handle = nullptr;
			}

		public:
			cbScrollBarHandleComponent* GetHandle() const { return Handle.get(); }
			/* Resets the focus and input state. */
			virtual void ResetInput() override final;

			inline bool IsHandlePressed() const { return Handle->IsPressed(); }

			virtual bool HasAnyComponents() const override final { return Handle != nullptr; }
			virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>{ Handle.get()}; }

			virtual bool HasAnyChildren() const override final { return Handle != nullptr; }
			virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>{ Handle.get()}; }

			virtual bool HasGeometry() const override final { return true; }
			virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
			virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
			virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

			void SetHandleVertexColorStyle(const cbButtonVertexColorStyle& style) { Handle->SetVertexColorStyle(style); }
			void SetBarVertexColorStyle(const cbVertexColorStyle& style);

			cbVertexColorStyle GetVertexColorStyle() const { return VertexColorStyle; }
			cbButtonVertexColorStyle GetHandleVertexColorStyle() const { return Handle->GetVertexColorStyle(); }

			void SetHandleName(const std::string Name) const { Handle->SetName(Name); }

		private:
			/* Called when the scroll state changes. */ 
			void OnScrollable();

			void OnScroll();
			/* Sets the scroll offset. */
			bool SetScrollAmount(float value);

		public:
			virtual cbVector GetLocation() const;
			virtual cbBounds GetBounds() const override final { return cbBounds(GetDimension(), GetLocation()); }

			virtual cbDimension GetDimension() const override final { return cbDimension(GetWidth(), GetHeight()); }
			virtual float GetWidth() const override final;
			virtual float GetHeight() const override final;

			/* Returns the scroll offset. */
			float GetScrollAmount() const;

			inline cbMargin GetPadding() const { return Padding; }
			void SetPadding(cbMargin InPadding);

			/* Sets cbScrollBarComponent Thickness */
			void SetThickness(const float Thickness);
			/* Returns cbScrollBarComponent Thickness */
			float GetThickness() const;

			/* Sets Bar Thickness */
			void SetBarThickness(const float value);
			/* Sets Bar Thickness */
			inline float GetBarThickness() const { return BarThickness; }
			/* Sets Handle Thickness */
			void SetHandleThickness(const float value);

			/* Returns the Scrollbox Length with padding. */
			float GetLength() const;
			inline float GetHandleLength() const { return Handle->GetLength(); }

			virtual void UpdateVerticalAlignment() override final;
			virtual void UpdateHorizontalAlignment() override final;
			virtual void UpdateRotation() override final;
			virtual void UpdateStatus() override final;

			/* Scroll by position. */
			void Scroll(const cbVector& InMouseLocation, std::optional<cbVector> MouseOffset = std::nullopt);
			/* Scroll by WheelDelta. */
			void ScrollWheel(float WheelDelta);
			/* Scroll by percentage. */
			void Scroll(const float Percent);
			/* Resets the percentage to 0.0. */
			void ResetScrollBar();

			inline const float GetCurrentPercent() const { return Handle->GetPercent(); }
			inline bool IsScrollable() const { return Handle->IsScrollable(); }
			inline eOrientation GetOrientation() const { return GetOwner<cbScrollBox>()->GetOrientation(); }

			void Hidden(bool Hide);
			/* If it can scroll it is visible, if not it is hidden.. */
			void ShowScrollBarIfScrollable(bool value);
			bool IsVisible() const;
			virtual bool IsHidden() const override final { return GetOwner()->IsHidden() ? true : !IsVisible(); }

			/* Determines whether it is an active component. */
			bool IsFocused() const { return GetOwner<cbScrollBox>()->IsFocused(); }
			/* Indicates whether there is an active component. */
			bool IsFocusable() const { return GetOwner<cbScrollBox>()->IsFocusable(); }
		};

	protected:
		class cbScrollBoxSlot : public cbSlot
		{
			cbClassBody(cbClassConstructor, cbScrollBoxSlot, cbSlot)
			friend cbScrollBox;
		private:
			bool bIsInserted;
			std::shared_ptr<cbWidget> Content;
			cbDimension Dimension;
			cbVector Location;

		private:
			void UpdateHorizontalDimension();
			void UpdateVerticalDimension();
			void UpdateHorizontalLocation();
			void UpdateVerticalLocation();

			void Inserted() { bIsInserted = true; OnInserted(); }
			virtual void OnInserted() { }

		public:
			cbScrollBoxSlot(cbScrollBox* InOwner, const cbWidget::SharedPtr& pContent);
			virtual ~cbScrollBoxSlot()
			{
				Content = nullptr;
			}

			virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }

			virtual void ReplaceContent(const cbWidget::SharedPtr& pContent) override final;

			virtual bool HasContent() const override final { return Content != nullptr; }
			virtual cbWidget::SharedPtr GetSharedContent() const override final { return Content; }
			virtual cbWidget* GetContent() const override final { return Content.get(); }

			virtual bool HasAnyChildren() const final { return Content != nullptr; }
			virtual std::vector<cbWidgetObj*> GetAllChildren() const final { return std::vector<cbWidgetObj*>{ Content.get() }; }

			virtual cbBounds GetBounds() const override final { return cbBounds(GetDimension(), GetLocation()); }
			virtual cbVector GetLocation() const override { return Location; }
			virtual cbDimension GetDimension() const override;
			virtual float GetWidth() const override;
			virtual float GetHeight() const override;

			virtual void UpdateVerticalAlignment() override;
			virtual void UpdateHorizontalAlignment() override;
		};

	public:
		cbScrollBox(const eOrientation orientation = eOrientation::Vertical);

	public:
		virtual ~cbScrollBox();

	public:
		virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
		virtual float GetRotation() const override final { return Transform.GetRotation(); }
		virtual cbBounds GetBounds() const override final { return Transform.GetBounds(); }
		virtual cbMargin GetPadding() const override final { return Transform.GetPadding(); }
		virtual cbDimension GetDimension() const override final { return Transform.GetDimension(); }
		virtual float GetWidth() const override final { return Transform.GetWidth(); }
		virtual float GetHeight() const override final { return Transform.GetHeight(); }

		/* Return NonAligned/Wrapped width of the Widget */
		virtual float GetNonAlignedWidth() const override final { return Transform.GetNonAlignedWidth(); }
		/* Return NonAligned/Wrapped height of the Widget */
		virtual float GetNonAlignedHeight() const override final { return Transform.GetNonAlignedHeight(); }

		/* Set the Screen Position of the Widget.*/
		virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false) override final;
		/* Set NonAligned/NonWrapped Dimension of the Widget */
		virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height) override final;
		/* Padding is used to align with offset. The padding only works if attached. */
		virtual void SetPadding(const cbMargin& Padding) override final;
		/* 2D Rotation. Requires Stencil Clipping during rendering, */
		virtual void SetRotation(const float Rotation) override final;

		void SetScrollBarPadding(const cbMargin& Padding);

		/* Vertically aligns the widget and slots. */
		virtual void UpdateVerticalAlignment(const bool ForceAlign = false) override final;
		/* Horizontally aligns the widget and slots. */
		virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) override final;
		/* Updates the Status of widget and slots. */
		virtual void UpdateStatus() override final;
		/* Updates the Rotation of widget and slots. */
		virtual void UpdateRotation() override final;

	protected:
		virtual void UpdateSlotVerticalAlignment() override final;
		virtual void UpdateSlotHorizontalAlignment() override final;

	private:
		virtual bool HasAnyChildren() const override final;
		/* Returns slots and components. */
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final;

		virtual bool HasAnyComponents() const override final { return ScrollBar != nullptr; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>{ ScrollBar.get() }; }

		virtual bool HasGeometry() const override final { return false; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

	public:
		/* Creates a slot and inserts content. */
		cbSlot* Insert(const cbWidget::SharedPtr& Content, const std::optional<std::size_t> Index = std::nullopt);
	protected:
		/* Inserts ScrollBox slot. */
		cbSlot* Insert(const cbScrollBoxSlot::SharedPtr& pSlot, const std::size_t Index);
	public:
		/* Creates a slot and inserts content. */
		inline cbScrollBox* operator[](const cbWidget::SharedPtr& Widget)
		{
			Insert(Widget);
		}

		inline eOrientation GetOrientation() const { return Orientation; }
		/* It changes its orientation vertically or horizontally. */
		bool SetOrientation(const eOrientation orientation);

		float GetScrollBarThickness() const;
		/* Set ScrollBarComponent Thickness. */
		void SetScrollBarThickness(const float value);
		void SetScrollBar_BarThickness(const float value);
		void SetScrollBar_HandleThickness(const float value);

		/* Hide/Collapse or make the ScrollBar visible. */
		void HideScrollBar(const bool value) { ScrollBar->Hidden(value); }
		/* Hide/Collapse ScrollBar if not scrollable. */
		void ShowScrollBarIfScrollable(bool value) { ScrollBar->ShowScrollBarIfScrollable(value); }

		/* Returns current percentage. */
		inline float GetPercent() const { return ScrollBar->GetCurrentPercent(); }
		/* Returns step percentage. e.g. Mouse Wheel Step */
		inline float GetStepPercent() const { return ScrollBar->ScrollStepPercent; }
		/* Sets step percentage. e.g. Mouse Wheel Step */
		inline void SetStepPercent(const float Value) { ScrollBar->ScrollStepPercent = Value; }

		/* Scroll by percentage. */
		void Scroll(const float Percent);
		/* Scroll to the widget. */
		void ScrollSlotIntoView(const std::size_t Index);
		/* Resets the percentage to 0.0. */
		void ResetScrollBar();

	protected:
		/* Returns the size of the region outside the ScrollBox size, based on alignment. */
		std::optional<float> GetOverflowingExtent(const bool ExcludeHidden = false) const;
		/* Returns the sum of the ScrollBox Width and the Width of the region outside it. */
		float GetTotalAreaWidth(const bool ExcludeHidden = false) const;
		/* Returns the sum of the ScrollBox Height and the Height of the region outside it. */
		float GetTotalAreaHeight(const bool ExcludeHidden = false) const;

		bool CanScrollUp();
		bool CanScrollDown();
		bool CanScrollLeft();
		bool CanScrollRight();

		/* Determines the position of the slot relative to the alignment. */
		float GetNextSlotLocation(const eVerticalAlignment InVAlignment, const cbScrollBoxSlot* Slot, const float ScrollWeight = 1.0f) const;
		/* Determines the position of the slot relative to the alignment. */
		float GetNextSlotLocation(const eHorizontalAlignment InHAlignment, const cbScrollBoxSlot* Slot, const float ScrollWeight = 1.0f) const;

		std::size_t GetNextSlotIndex() const;
		cbSlot* GetNextSlot(const cbScrollBoxSlot* Slot, const bool ExcludeHidden = true) const;
		cbSlot* GetPreviousSlot(const cbScrollBoxSlot* Slot, const bool ExcludeHidden = true) const;

		virtual std::size_t GetSlotIndex(const cbSlot* Slot) const override final;

	public:
		/* Returns slot that intersect the Bounds. */
		cbSlot* GetOverlappingSlot(const cbBounds& Bounds) const;

		/* If the Indexes are valid, it will swap the slots. */
		bool SwapSlots(const std::size_t sourceIndex, const std::size_t destIndex);
		bool SwapSlotWithFront(const std::size_t index);
		bool SwapSlotWithBack(const std::size_t index);

		/* If the index is valid, it will replace the contents of the slot. */
		bool ReplaceSlotContent(std::size_t Index, const cbWidget::SharedPtr& New);

		bool RemoveSlot(const std::size_t SlotIndex);
	private:
		/* Called when the Slot or Slot content requested Remove From Parent. */
		virtual bool OnRemoveSlot(cbSlot* Slot) override final;

	public:
		/* Hide/Collapse slot. */
		bool HideSlot(const std::size_t Index, const bool value);
		virtual std::size_t GetSlotSize(const bool ExcludeHidden = false) const override final;

		virtual std::vector<cbSlot*> GetSlots() const override final;
		template<class T>
		T* GetSlot(const std::size_t Index, const bool Dynamic = false) const
		{
			if (Dynamic)
				return dynamic_cast<T*>(GetSlot(Index));
			return static_cast<T*>(GetSlot(Index));
		}
		virtual cbSlot* GetSlot(const std::size_t Index) const override final { return Index < slotsize ? mSlots[Index].get() : nullptr; }
		inline bool IsIndexExist(const std::size_t Index) const { return GetSlot(Index) != nullptr ? true : false; }

	protected:
		cbScrollBarComponent* GetScrollBar() const { return ScrollBar.get(); }

	public:
		void SetScrollBarName(const std::string Name) const { ScrollBar->SetName(Name); }
		void SetScrollBarHandleName(const std::string Name) const { ScrollBar->SetHandleName(Name); }
		inline void SetScrollBarBackgroundVertexColorStyle(const cbVertexColorStyle& style) { ScrollBar->SetBarVertexColorStyle(style); }
		inline void SetScrollBarHandleVertexColorStyle(const cbButtonVertexColorStyle& style) { ScrollBar->SetHandleVertexColorStyle(style); }
		cbVertexColorStyle GetScrollBarBackgroundVertexColorStyle() const { return ScrollBar->GetVertexColorStyle(); }
		cbButtonVertexColorStyle GetScrollBarHandleVertexColorStyle() const { return ScrollBar->GetHandleVertexColorStyle(); }

	public:
		inline bool IsHandlePressed() const { return ScrollBar->Handle->IsPressed(); }

	public:
		virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) override;
		virtual bool OnMouseMove(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) override;

	private:
		virtual void OnAttach() override final;
		virtual void OnRemoveFromParent() override final;

	private:
		virtual void OnSlotDimensionUpdated(cbSlot* Slot) override final;
		virtual void OnSlotVisibilityChanged(cbSlot* Slot) override final;

		virtual bool WrapVertical() override final;
		virtual bool WrapHorizontal() override final;
		virtual bool UnWrapVertical() override final;
		virtual bool UnWrapHorizontal() override final;

		void OnScrollBoxSlotContentInsertedOrReplaced(cbSlot* Slot);

	private:
		cbTransform Transform;
		eOrientation Orientation;
		std::size_t slotsize;
		std::vector<cbScrollBoxSlot::SharedPtr> mSlots;
		cbScrollBarComponent::UniquePtr ScrollBar;
	};
}
