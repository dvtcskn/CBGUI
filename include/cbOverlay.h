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

#include "cbSlottedBox.h"

namespace cbgui
{
	/* Overlay widgets allow widgets to be placed on top of each other. */
	class cbOverlay : public cbSlottedBox
	{
		cbClassBody(cbClassConstructor, cbOverlay, cbSlottedBox);
	public:
		/* Default Overlay box slot. */
		class cbOverlaySlot : public cbSlot
		{
			cbClassBody(cbClassConstructor, cbOverlaySlot, cbSlot)
			friend cbOverlay;
		private:
			bool bIsInserted;
			std::shared_ptr<cbWidget> Content;

			void Inserted() { bIsInserted = true; OnInserted(); }
			virtual void OnInserted() { }

		public:
			cbFORCEINLINE cbOverlaySlot(cbOverlay* pOwner, const cbWidget::SharedPtr& pContent)
				: Super(pOwner)
				, bIsInserted(false)
				, Content(pContent)
			{
				if (Content)
					Content->AttachToSlot(this);
			}

			virtual ~cbOverlaySlot()
			{
				Content = nullptr;
			}

			virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }

			virtual void ReplaceContent(const cbWidget::SharedPtr& pContent) override final;

			virtual bool HasContent() const  override final { return Content != nullptr; }
			virtual cbWidget::SharedPtr GetSharedContent() const override final { return Content; }
			virtual cbWidget* GetContent() const override final { return Content.get(); }

			virtual bool HasAnyChildren() const override final { return Content != nullptr; }
			virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>{ Content.get() }; }

			virtual bool IsInside(const cbVector& Location) const override final { return GetContent()->IsInside(Location); }
			virtual cbBounds GetBounds() const override final { return cbBounds(GetDimension(), GetLocation()); }
			virtual cbVector GetLocation() const override { return GetOwner<cbOverlay>()->GetLocation(); }
			virtual cbDimension GetDimension() const override { return GetOwner()->GetDimension(); }
			virtual float GetWidth() const override { return GetOwner()->GetWidth(); }
			virtual float GetHeight() const override { return GetOwner()->GetHeight(); }

			virtual void UpdateVerticalAlignment() override { if (HasContent()) GetContent()->UpdateVerticalAlignment(); }
			virtual void UpdateHorizontalAlignment() override { if (HasContent()) GetContent()->UpdateHorizontalAlignment(); }
		};

	private:
		cbTransform Transform;
		std::size_t slotsize;
		std::vector<cbOverlaySlot::SharedPtr> mSlots;

	public:
		cbOverlay();
	public:
		virtual ~cbOverlay() 
		{
			for (auto& Slot : mSlots)
				Slot = nullptr;
			mSlots.clear();
			slotsize = 0;
		}

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

	public:
		virtual bool HasAnyChildren() const override final;
		/* Returns slots and components. */
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final;

		virtual bool HasAnyComponents() const override final { return false; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>(); }

		virtual bool HasGeometry() const override final { return false; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

	public:
		/* Creates a slot and inserts content. */
		cbSlot* Insert(const cbWidget::SharedPtr& Content, const std::optional<std::size_t> Index = std::nullopt);
	protected:
		/* Inserts Overlay slot. */
		cbSlot* Insert(const cbOverlaySlot::SharedPtr& pSlot, const std::size_t Index);
	public:
		/* Creates a slot and inserts content. */
		inline cbOverlay* operator[](const cbWidget::SharedPtr& Widget)
		{
			Insert(Widget);
			return this;
		}

	public:
		/* Returns slot that intersect the Bounds. */
		cbSlot* GetOverlappingSlot(const cbBounds& Bounds) const;

		virtual bool OnMouseMove(const cbMouseInput& Mouse) override;

	protected:
		std::size_t GetNextSlotIndex() const;
		cbSlot* GetNextSlot(const cbOverlaySlot* Slot, const bool ExcludeHidden = true) const;
		cbSlot* GetPreviousSlot(const cbOverlaySlot* Slot, const bool ExcludeHidden = true) const;

	private:
		virtual std::size_t GetSlotIndex(const cbSlot* Slot) const override final;

	public:
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
		T* GetSlot(const std::size_t Index) const
		{
			return cbgui::cbCast<T>(GetSlot(Index));
		}
		virtual cbSlot* GetSlot(const std::size_t Index) const override final { return Index < slotsize ? mSlots[Index].get() : nullptr; }
		inline bool IsIndexExist(const std::size_t Index) const { return GetSlot(Index) != nullptr ? true : false; }

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
	};
}
