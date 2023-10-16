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
	/* Horizontal box widget automatically arranges child widgets horizontally. */
	class cbHorizontalBox : public cbSlottedBox
	{
		cbClassBody(cbClassConstructor, cbHorizontalBox, cbSlottedBox)
	protected:
		/* Default Horizontal box slot. */
		class cbHorizontalBoxSlot : public cbSlot
		{
			cbClassBody(cbClassConstructor, cbHorizontalBoxSlot, cbSlot)
			friend cbHorizontalBox;
		private:
			bool bIsInserted;
			eSlotAlignment Alignment;
			std::shared_ptr<cbWidget> Content;

			cbDimension Dimension;
			cbVector Location;
			float SlotWeight;

		private:
			void UpdateHorizontalDimension()
			{
				if (Alignment == eSlotAlignment::BoundToContent)
					Dimension.Width = Content ?  Content->GetNonAlignedWidth() + Content->GetPadding().GetWidth() : 0.0f;
				else if (Alignment == eSlotAlignment::BoundToSlot)
					Dimension.Width = GetOwner<cbHorizontalBox>()->ComputeEquallyDividedSlotWidth() * SlotWeight;
			}
			void UpdateVerticalDimension()
			{
				Dimension.Height = GetOwner<cbHorizontalBox>()->GetHeight();
			}
			void UpdateHorizontalLocation()
			{
				Location.X = (GetOwner<cbHorizontalBox>()->GetNextSlotLocation(eHorizontalAlignment::Align_Left, this) + GetWidth() / 2.0f);
			}
			void UpdateVerticalLocation()
			{
				Location.Y = (GetOwner<cbHorizontalBox>()->GetLocation().Y);
			}

			void Inserted() { bIsInserted = true; OnInserted(); }
			virtual void OnInserted() { }

		public:
			cbFORCEINLINE cbHorizontalBoxSlot(cbHorizontalBox* pOwner, const cbWidget::SharedPtr& pContent, const eSlotAlignment CustomAlignment = eSlotAlignment::BoundToContent)
				: Super(pOwner)
				, bIsInserted(false)
				, Alignment(CustomAlignment)
				, Content(pContent)
				, Location(cbVector::Zero())
				, Dimension(cbDimension())
				, SlotWeight(1.0f)
			{
				if (Content)
				{
					UpdateVerticalDimension();
					UpdateHorizontalDimension();
					UpdateVerticalLocation();
					UpdateHorizontalLocation();

					Content->AttachToSlot(this);
				}
			}

			cbFORCEINLINE cbHorizontalBoxSlot(const cbHorizontalBoxSlot& Widget, cbSlottedBox* NewOwner)
				: Super(Widget, NewOwner)
				, bIsInserted(false)
				, Alignment(Widget.Alignment)
				, Content(Widget.Content->CloneWidget())
				, Location(Widget.Location)
				, Dimension(Widget.Dimension)
				, SlotWeight(Widget.SlotWeight)
			{
				if (Content)
				{
					UpdateVerticalDimension();
					UpdateHorizontalDimension();
					UpdateVerticalLocation();
					UpdateHorizontalLocation();

					Content->AttachToSlot(this);
				}
			}

			virtual ~cbHorizontalBoxSlot()
			{
				Content = nullptr;
			}

			virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) override
			{
				return cbHorizontalBoxSlot::Create(*this, NewOwner);
			}

			virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }

			virtual void ReplaceContent(const cbWidget::SharedPtr& pContent) override final;

			virtual bool HasContent() const override final { return Content != nullptr; }
			virtual cbWidget::SharedPtr GetSharedContent() const override final { return Content; }
			virtual cbWidget* GetContent() const override final { return Content.get(); }

			virtual bool HasAnyChildren() const override final { return Content != nullptr; }
			virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>{ Content.get() }; }

			virtual cbBounds GetBounds() const override final { return cbBounds(GetDimension(), GetLocation()); }
			virtual cbVector GetLocation() const override { return Location; }
			virtual cbDimension GetDimension() const override { return Dimension; }
			virtual float GetWidth() const override { return Dimension.Width; }
			virtual float GetHeight() const override { return Dimension.Height; }

			/* It determines how much space it will occupy in an evenly divided Slot Width. */
			inline void SetSlotWeight(const float Weight)
			{
				SlotWeight = Weight;
				GetOwner<cbHorizontalBox>()->SlotAttributeUpdated();
			}
			float GetSlotWeight() const { return SlotWeight; }

			/* Determines whether the width is calculated based on the content, or evenly divides the Width of the slottedbox. */
			inline void SetSlotAlignment(const eSlotAlignment& CustomAlignment)
			{
				Alignment = CustomAlignment;
				GetOwner<cbHorizontalBox>()->SlotAttributeUpdated();
			}
			eSlotAlignment GetSlotAlignment() const { return Alignment; }

			virtual void UpdateVerticalAlignment() override
			{
				UpdateVerticalDimension();
				UpdateVerticalLocation();

				if (Content)
					Content->UpdateVerticalAlignment();
			}

			virtual void UpdateHorizontalAlignment() override
			{
				UpdateHorizontalDimension();
				UpdateHorizontalLocation();

				if (Content) 
					Content->UpdateHorizontalAlignment();
			}
		};

	private:
		cbTransform Transform;
		std::size_t slotsize;
		std::vector<cbHorizontalBoxSlot::SharedPtr> mSlots;

	public:
		cbHorizontalBox();
		cbHorizontalBox(const cbHorizontalBox& Other, cbSlot* NewOwner = nullptr);

		virtual ~cbHorizontalBox()
		{
			for (auto& Slot : mSlots)
				Slot = nullptr;
			mSlots.clear();
			slotsize = 0;
		}

		virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override;

	public:
		virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
		virtual float GetRotation() const override final;
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
		/* Creates a slot and inserts content. */
		cbSlot* Insert(const cbWidget::SharedPtr& Content, const eSlotAlignment& Alignment, const std::optional<std::size_t> Index = std::nullopt);
	protected:
		/* Inserts HorizontalBox slot. */
		cbSlot* Insert(const cbHorizontalBoxSlot::SharedPtr& pSlot, const std::size_t Index);
	public:
		/* Creates a slot and inserts content. */
		inline cbHorizontalBox* operator[](const cbWidget::SharedPtr& Widget)
		{
			Insert(Widget);
			return this;
		}

		/* It determines how much space it will occupy in an evenly divided Slot Width. */
		void SetSlotWeight(const float SlotWeight, const std::uint32_t& Index)
		{
			if (IsIndexExist(Index))
			{
				GetSlot<cbHorizontalBoxSlot>(Index)->SetSlotWeight(SlotWeight);
			}
		}

		/* Determines whether the slot width is calculated based on the content, or evenly divides the Width of the slottedbox. */
		void SetSlotAlignment(const eSlotAlignment& InCustomAlignment, const std::uint32_t& Index)
		{
			if (IsIndexExist(Index))
			{
				GetSlot<cbHorizontalBoxSlot>(Index)->SetSlotAlignment(InCustomAlignment);
			}
		}

		/* Returns slot that intersect the Bounds. */
		cbSlot* GetOverlappingSlot(const cbBounds& Bounds) const;

	private:
		virtual std::size_t GetSlotIndex(const cbSlot* Slot) const override final;

	protected:
		/* Computes equally divided slot width. */
		inline float ComputeEquallyDividedSlotWidth() const
		{
			float Width = 0.0f;
			float SlotWeight = 0;

			const std::size_t Size = GetSlotSize();
			if (Size == 0)
				return IsItWrapped() ? 1.0f : GetWidth();

			for (std::size_t i = 0; i < Size; i++)
			{
				const auto& Slot = GetSlot<cbHorizontalBox::cbHorizontalBoxSlot>(i);

				if (Slot->IsHidden())
					continue;

				if (Slot->GetSlotAlignment() == eSlotAlignment::BoundToContent)
				{
					const auto Content = Slot->GetContent();
					Width = (Width + (Content->GetNonAlignedWidth() + Content->GetPadding().GetWidth()));
				}
				else if (Slot->GetSlotAlignment() == eSlotAlignment::BoundToSlot)
				{
					SlotWeight += Slot->GetSlotWeight();
				}
			}

			const float SlotWidth = ((GetWidth() - Width) / (SlotWeight == 0.0f ? 1.0f : SlotWeight));

			return SlotWidth < 1.0f ? 1.0f : SlotWidth;
		}

		std::size_t GetNextSlotIndex() const;
		cbSlot* GetNextSlot(const cbHorizontalBoxSlot* Slot, const bool ExcludeHidden = true) const;
		cbSlot* GetPreviousSlot(const cbHorizontalBoxSlot* Slot, const bool ExcludeHidden = true) const;

		/* Determines the position of the slot relative to the alignment and index. */
		inline constexpr float GetNextSlotLocation(const eHorizontalAlignment InHAlignment, const cbHorizontalBoxSlot* pSlot) const
		{
			switch (InHAlignment)
			{
				case eHorizontalAlignment::Align_Left:
				{
					const cbSlot* Slot = GetPreviousSlot(pSlot);
					if (!Slot)
						return GetBounds().GetLeft();
					return Slot->GetBounds().GetRight();
				}
				case eHorizontalAlignment::Align_Right:
				{
					const cbSlot* Slot = GetPreviousSlot(pSlot);
					if (!Slot)
						return GetBounds().GetRight();
					return Slot->GetBounds().GetLeft();
				}
			}

			return 0.0f;
		}

	public:
		/* If the Indexes are valid, it will swap the slots. */
		bool SwapSlots(const std::size_t sourceIndex, const std::size_t destIndex);
		bool SwapSlotWithFront(const std::size_t index);
		bool SwapSlotWithBack(const std::size_t index);

		/* If the index is valid, it will replace the contents of the slot. */
		bool ReplaceSlotContent(std::size_t Index, const cbWidget::SharedPtr& New);

		void RemoveSlots();
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
		void SlotAttributeUpdated();

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
