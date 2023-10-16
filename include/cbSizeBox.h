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
	/*
	* A widget that allows you to specify the minimum and maximum size. 
	* There is a content slot. 
	* Even if it is wrapped, the content always stays to the desired size,
	* as the minimum and maximum size will not change.
	*/
	class cbSizeBox : public cbSlottedBox
	{
		cbClassBody(cbClassConstructor, cbSizeBox, cbSlottedBox);
	protected:
		/* Default SizeBox slot. */
		class cbSizeBoxSlot : public cbSlot
		{
			cbClassBody(cbClassConstructor, cbSizeBoxSlot, cbSlot);
			friend cbSizeBox;
		private:
			bool bIsInserted;
			std::shared_ptr<cbWidget> Content;

			void Inserted() { bIsInserted = true; OnInserted(); }
			virtual void OnInserted() { }

		public:
			cbFORCEINLINE cbSizeBoxSlot(cbSizeBox* pOwner, const cbWidget::SharedPtr& pContent = nullptr)
				: Super(pOwner)
				, bIsInserted(false)
				, Content(pContent)
			{
				if (Content)
					Content->AttachToSlot(this);
			}

			cbFORCEINLINE cbSizeBoxSlot(const cbSizeBoxSlot& Widget, cbSlottedBox* NewOwner)
				: Super(Widget, NewOwner)
				, bIsInserted(false)
				, Content(Widget.Content->CloneWidget())
			{
				if (Content)
					Content->AttachToSlot(this);
			}

			virtual ~cbSizeBoxSlot()
			{
				Content = nullptr;
			}

		public:
			virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }

			virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) override
			{
				return cbSizeBoxSlot::Create(*this, NewOwner);
			}

			virtual void ReplaceContent(const cbWidget::SharedPtr& Content) override final;

			virtual bool HasContent() const override final { return Content != nullptr; }
			virtual cbWidget::SharedPtr GetSharedContent() const override final { return Content; }
			virtual cbWidget* GetContent() const override final { return Content.get(); }

			virtual bool HasAnyChildren() const override final { return Content != nullptr; }
			virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>{ Content.get() }; }

			virtual cbBounds GetBounds() const override final { return GetOwner()->GetBounds(); }
			virtual cbVector GetLocation() const override final { return GetOwner<cbSizeBox>()->GetLocation(); }
			virtual cbDimension GetDimension() const override final { return GetOwner()->GetDimension(); }
			virtual float GetWidth() const override final { return GetDimension().GetWidth(); }
			virtual float GetHeight() const override final { return GetDimension().GetHeight(); }

			virtual void UpdateVerticalAlignment() override final { if (HasContent()) Content->UpdateVerticalAlignment(); }
			virtual void UpdateHorizontalAlignment() override final { if (HasContent()) Content->UpdateHorizontalAlignment(); }
			virtual void UpdateRotation() override final { if (HasContent()) Content->UpdateRotation(); }
		};

	public:
		cbSizeBox();
		cbSizeBox(const cbSizeBox& Other, cbSlot* NewOwner = nullptr);
		virtual ~cbSizeBox();

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
		virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false);
		/* Set NonAligned/NonWrapped Dimension of the Widget */
		virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height);
		/* Padding is used to align with offset. The padding only works if attached into slot. */
		virtual void SetPadding(const cbMargin& Padding) override final;
		/* 2D Rotation. Requires Stencil Clipping during rendering, */
		virtual void SetRotation(const float Rotation) override final;

		/* Updates the Rotation of widgets and slots. */
		virtual void UpdateRotation() override final;
		/* Updates the status of widgets and slots.*/
		virtual void UpdateStatus() override final;
		/* Vertically aligns the widget and slots. */
		virtual void UpdateVerticalAlignment(const bool ForceAlign = false) override final;
		/* Horizontally aligns the widget and slots. */
		virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) override final;
	private:
		virtual void UpdateSlotVerticalAlignment() override final;
		virtual void UpdateSlotHorizontalAlignment() override final;

		virtual std::size_t GetSlotIndex(const cbSlot* Slot) const override final { return 0; };

	public:
		/* Overrides the minimum and maximum width. */
		void SetMinMaxWidth(std::optional<float> inWidth);
		/* Overrides the minimum and maximum Height. */
		void SetMinMaxHeight(std::optional<float> inHeight);
		/* Overrides the minimum width. */
		void SetMinimumWidth(std::optional<float> inWidth);
		/* Overrides the minimum Height. */
		void SetMinimumHeight(std::optional<float> inHeight);
		/* Overrides the maximum width. */
		void SetMaximumWidth(std::optional<float> inWidth);
		/* Overrides the maximum Height. */
		void SetMaximumHeight(std::optional<float> inHeight);

	public:
		/* If not created, it creates a slot and inserts content. */
		void SetContent(const cbWidget::SharedPtr& Content);
	protected:
		void SetSlot(const cbSizeBoxSlot::SharedPtr& Slot);
	public:
		/* Returns Slot content. */
		cbWidget* GetContent() const { return Slot ? Slot->GetContent() : nullptr; }

		virtual std::vector<cbSlot*> GetSlots() const override final { return std::vector<cbSlot*>{ Slot.get() }; }
		virtual cbSlot* GetSlot(const std::size_t Index = 0) const override final { return Index == 0 ? Slot.get() : nullptr; }
		virtual std::size_t GetSlotSize(const bool ExcludeHidden = false) const override final;

		/* Hide/Collapse or make the slot visible. */
		void HideContent(bool value);
		void RemoveContent();

		/* Returns slots. */
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final;
		/* Checks whether a slot has been created. */
		virtual bool HasAnyChildren() const override final { return Slot != nullptr; }

		virtual bool HasAnyComponents() const override final { return false; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>(); }

		virtual bool HasGeometry() const override final { return false; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

	private:
		virtual void OnAttach() override final;
		virtual void OnRemoveFromParent() override final;

		virtual void OnSlotVisibilityChanged(cbSlot* Slot) override final;
		virtual void OnSlotDimensionUpdated(cbSlot* Slot) override final;

		virtual bool OnRemoveSlot(cbSlot* Slot) override final;

		virtual bool WrapVertical() override final;
		virtual bool WrapHorizontal() override final;
		virtual bool UnWrapVertical() override final;
		virtual bool UnWrapHorizontal() override final;

	private:
		cbTransform Transform;

		std::optional<float> MinimumWidth;
		std::optional<float> MinimumHeight;
		std::optional<float> MaximumWidth;
		std::optional<float> MaximumHeight;

		cbSizeBoxSlot::SharedPtr Slot;
	};
}
