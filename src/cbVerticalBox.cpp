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

#include "pch.h"
#include "cbVerticalBox.h"
#include "cbComponent.h"
#include "cbCanvas.h"

namespace cbgui
{
	void cbVerticalBox::cbVerticalBoxSlot::ReplaceContent(const cbWidget::SharedPtr& pContent)
	{
		if (!pContent)
			return;

		if (Content)
			Content = nullptr;
		Content = pContent;
		Content->AttachToSlot(this);
		OnContentInsertedOrReplaced();
	}

	cbVerticalBox::cbVerticalBox()
		: Super()
		, Transform(cbTransform())
		, slotsize(0)
	{}

	void cbVerticalBox::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
	{
		if (X.has_value() && Y.has_value())
		{
			if ((HasOwner() || IsAlignedToCanvas()))
			{
				if (Force)
				{
					if (GetVerticalAlignment() != eVerticalAlignment::Align_NONE && GetHorizontalAlignment() != eHorizontalAlignment::Align_NONE)
						return;
				}
				else
				{
					return;
				}
			}
			if (Transform.SetLocation(cbVector(*X, *Y)))
			{
				UpdateSlotAlignments();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else if (X.has_value())
		{
			if ((HasOwner() || IsAlignedToCanvas()))
			{
				if (Force)
				{
					if (GetHorizontalAlignment() != eHorizontalAlignment::Align_NONE)
						return;
				}
				else
				{
					return;
				}
			}
			if (Transform.SetPositionX(*X))
			{
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else if (Y.has_value())
		{
			if ((HasOwner() || IsAlignedToCanvas()))
			{
				if (Force)
				{
					if (GetVerticalAlignment() != eVerticalAlignment::Align_NONE)
						return;
				}
				else
				{
					return;
				}
			}
			if (Transform.SetPositionY(*Y))
			{
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbVerticalBox::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
	{
		if (Width.has_value() && Height.has_value())
		{
			if (Transform.SetDimension(cbDimension(*Width, *Height)))
			{
				if (HasOwner())
					DimensionUpdated();
				else if (IsAlignedToCanvas())
					AlignToCanvas();
				else
				{
					UpdateSlotAlignments();
					NotifyCanvas_WidgetUpdated();
				}
			}
		}
		else if (Width.has_value())
		{
			if (Transform.SetWidth(*Width))
			{
				if (HasOwner())
					DimensionUpdated();
				else if (IsAlignedToCanvas())
					AlignToCanvas();
				else
				{
					UpdateSlotHorizontalAlignment();
					NotifyCanvas_WidgetUpdated();
				}
			}
		}
		else if (Height.has_value())
		{
			if (Transform.SetHeight(*Height))
			{
				if (HasOwner())
					DimensionUpdated();
				else if (IsAlignedToCanvas())
					AlignToCanvas();
				else
				{
					UpdateSlotVerticalAlignment();
					NotifyCanvas_WidgetUpdated();
				}
			}
		}
	}

	void cbVerticalBox::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			NotifyCanvas_WidgetUpdated();

			for (const auto& Slot : mSlots)
				if (Slot)
					Slot->UpdateRotation();
		}
	}

	void cbVerticalBox::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbVerticalBox::UpdateRotation()
	{
		if (HasOwner())
			Transform.SetRollOffset(GetOwner()->GetRotation());
		else if (IsAlignedToCanvas())
			Transform.SetRollOffset(GetCanvas()->GetScreenRotation());
		else
			Transform.SetRollOffset(0.0f);
		NotifyCanvas_WidgetUpdated();

		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateRotation();
	}

	cbSlot* cbgui::cbVerticalBox::Insert(const cbWidget::SharedPtr& Content, const std::optional<std::size_t> Index)
	{
		return Insert(Content, eSlotAlignment::BoundToContent, Index);
	}

	cbSlot* cbVerticalBox::Insert(const cbWidget::SharedPtr& Content, const eSlotAlignment& Alignment, const std::optional<std::size_t> SlotIndex)
	{
		if (!Content)
			return nullptr;

		return Insert(cbVerticalBoxSlot::Create(this, Content, Alignment), SlotIndex.has_value() ? SlotIndex.value() : GetNextSlotIndex());
	}

	cbSlot* cbgui::cbVerticalBox::Insert(const cbVerticalBoxSlot::SharedPtr& Slot, const std::size_t Index)
	{
		if (!Slot)
			return nullptr;

		if (IsIndexExist(Index))
		{
			slotsize++;
			mSlots.insert(mSlots.begin() + Index, Slot);
		}
		else
		{
			slotsize++;
			mSlots.push_back(Slot);
		}

		Slot->Inserted();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->NewSlotAdded(this, Slot.get());

		ResetInput();

		UpdateSlotAlignments();
		if (IsItWrapped())
			Wrap();

		Slot->UpdateRotation();
		Slot->UpdateStatus();

		return Slot.get();
	}

	void cbVerticalBox::SlotAttributeUpdated()
	{
		ResetInput();

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotVerticalAlignment();
	}

	void cbVerticalBox::UpdateVerticalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerHeight = GetOwner()->GetHeight();
			const std::optional<const eVerticalAlignment> CustomAlignment = GetNonAlignedHeight() > OwnerHeight ? std::optional<eVerticalAlignment>(eVerticalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsHeightAligned())
				Transform.ResetHeightAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetVerticalAlignment(), GetOwner()->GetBounds(), GetVerticalAnchor()) || ForceAlign)
			{
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else if (IsAlignedToCanvas())
		{
			const auto Canvas = GetCanvas();
			const float OwnerHeight = Canvas->GetScreenDimension().GetHeight();
			const std::optional<const eVerticalAlignment> CustomAlignment = GetNonAlignedHeight() > OwnerHeight ? std::optional<eVerticalAlignment>(eVerticalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsHeightAligned())
				Transform.ResetHeightAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetVerticalAlignment(), Canvas->GetScreenBounds(), GetVerticalAnchor()) || ForceAlign)
			{
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}			
			else if (GetVerticalAlignment() == eVerticalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetY(CanvasOffset.Y);
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsHeightAligned())
			{
				Transform.ResetHeightAlignment();
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else
			{
				UpdateSlotVerticalAlignment();
			}
		}
	}

	void cbVerticalBox::UpdateHorizontalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerWidth = GetOwner()->GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetHorizontalAlignment(), GetOwner()->GetBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else if (IsAlignedToCanvas())
		{
			const auto Canvas = GetCanvas();
			const float OwnerWidth = Canvas->GetScreenDimension().GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetHorizontalAlignment(), Canvas->GetScreenBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetX(CanvasOffset.X);
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsWidthAligned())
			{
				Transform.ResetWidthAlignment();
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else
			{
				UpdateSlotHorizontalAlignment();
			}
		}
	}

	bool cbVerticalBox::WrapVertical()
	{
		if (!IsItWrapped())
			return false;

		if (GetSlotSize() == 0)
		{
			if (Transform.CompressHeight(1.0f))
			{
				return true;
			}
			return false;
		}

		float Height = 0.0;
		const std::size_t Size = GetSlotSize();
		for (std::size_t i = 0; i < Size; i++)
		{
			const auto& Slot = GetSlot(i);
			if (Slot->IsHidden())
				continue;
			const auto& Content = Slot->GetContent();
			const float SlotHeight = Content->GetNonAlignedHeight();
			const cbMargin& Padding = Content->GetPadding();
			Height += SlotHeight + (Padding.Top + Padding.Bottom);
		}

		return Transform.CompressHeight(Height);
	}

	bool cbVerticalBox::WrapHorizontal()
	{
		if (!IsItWrapped())
			return false;

		if (GetSlotSize() == 0)
		{
			return Transform.CompressWidth(1.0f);
		}

		float Width = 0.0f;
		const std::size_t Size = GetSlotSize();
		for (std::size_t i = 0; i < Size; i++)
		{
			const auto& Slot = GetSlot(i);
			if (Slot->IsHidden())
				continue;
			const auto& Content = Slot->GetContent();
			const float SlotWidth = Content->GetNonAlignedWidth();
			const cbMargin& Padding = Content->GetPadding();
			if ((SlotWidth + (Padding.Left + Padding.Right)) > Width)
				Width = SlotWidth + (Padding.Left + Padding.Right);
		}

		return Transform.CompressWidth(Width);
	}

	bool cbVerticalBox::UnWrapVertical()
	{
		if (Transform.IsHeightCompressed())
		{
			Transform.ResetHeightCompressed();
			return true;
		}
		return false;
	}

	bool cbVerticalBox::UnWrapHorizontal()
	{
		if (Transform.IsWidthCompressed())
		{
			Transform.ResetWidthCompressed();
			return true;
		}
		return false;
	}

	void cbVerticalBox::OnSlotVisibilityChanged(cbSlot* Slot)
	{
	}

	void cbVerticalBox::OnSlotDimensionUpdated(cbSlot* Slot)
	{
	}

	void cbVerticalBox::OnAttach()
	{
		WrapVertical();
		WrapHorizontal();
		NotifyCanvas_WidgetUpdated();
		UpdateSlotAlignments();
	}

	void cbVerticalBox::OnRemoveFromParent()
	{
		if (Transform.IsHeightAligned() || Transform.IsWidthAligned())
		{
			Transform.ResetHeightAlignment(); 
			Transform.ResetWidthAlignment();
			NotifyCanvas_WidgetUpdated();
		}

		UnWrap();
	}

	void cbVerticalBox::UpdateSlotVerticalAlignment()
	{
		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateVerticalAlignment();
	}

	void cbVerticalBox::UpdateSlotHorizontalAlignment()
	{
		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateHorizontalAlignment();
	}

	void cbVerticalBox::UpdateStatus()
	{
		SetFocus(false);
		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateStatus();
	}

	cbSlot* cbVerticalBox::GetOverlappingSlot(const cbBounds& Bounds) const
	{
		cbSlot::SharedPtr pSlot = nullptr;
		for (const auto& Slot : mSlots)
		{
			if (Slot->IsHidden())
				continue;

			if (Slot->Intersect(Bounds))
			{
				pSlot = Slot;
				break;
			}
		}

		return pSlot ? pSlot.get() : nullptr;
	}

	bool cbVerticalBox::ReplaceSlotContent(std::size_t Index, const cbWidget::SharedPtr& New)
	{
		if (!New)
			return false;

		if (cbSlot* Slot = GetSlot(Index))
		{
			cbWidget::SharedPtr Old = Slot->GetSharedContent();
			Slot->ReplaceContent(New);
			if (cbICanvas* Canvas = GetCanvas())
				Canvas->SlotContentReplaced(Slot, Old.get(), New.get());
			Old = nullptr;

			return true;
		}
		return false;
	}

	bool cbVerticalBox::OnRemoveSlot(cbSlot* Slot)
	{
		if (!Slot || slotsize == 0)
		{
			return false;
		}

		cbVerticalBoxSlot::SharedPtr pSlot = nullptr;
		std::vector<cbVerticalBoxSlot::SharedPtr>::iterator it = mSlots.begin();
		while (it != mSlots.end())
		{
			if ((*it).get() == Slot)
			{
				pSlot = (*it);
				it = mSlots.erase(it);
				slotsize--;
				break;
			}
			else {
				it++;
			}
		}

		if (!pSlot)
			return false;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();

		ResetInput();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->SlotRemoved(this, Slot);

		pSlot = nullptr;

		return true;
	}

	bool cbVerticalBox::RemoveSlot(const std::size_t SlotIndex)
	{
		if (SlotIndex < 0 || SlotIndex >= mSlots.size() || slotsize == 0)
			return false;

		cbSlot::SharedPtr Slot = mSlots[SlotIndex];
		auto Content = Slot->GetContent();
		Content->RemoveFromParent();

		ResetInput();

		return true;
	}

	std::size_t cbVerticalBox::GetSlotIndex(const cbSlot* Slot) const
	{
		return std::distance(mSlots.begin(), std::find_if(mSlots.begin(), mSlots.end(), [&](const cbSlot::SharedPtr& Ptr)
			{
				return Ptr.get() == Slot;
			}));
	}

	bool cbVerticalBox::SwapSlots(const std::size_t sourceIndex, const std::size_t destIndex)
	{
		if (sourceIndex >= slotsize || destIndex >= slotsize)
			return false;

		std::swap(mSlots[sourceIndex], mSlots[destIndex]);

		UpdateSlotAlignments();
		ResetInput();

		return true;
	}

	bool cbVerticalBox::SwapSlotWithFront(const std::size_t index)
	{
		if ((index + 1) >= slotsize)
			return false;

		std::swap(mSlots[index], mSlots[index + 1]);

		UpdateSlotAlignments();
		ResetInput();

		return true;
	}

	bool cbVerticalBox::SwapSlotWithBack(const std::size_t index)
	{
		if (index >= slotsize || index == 0)
			return false;

		std::swap(mSlots[index], mSlots[index - 1]);

		UpdateSlotAlignments();
		ResetInput();

		return true;
	}

	std::size_t cbVerticalBox::GetNextSlotIndex() const
	{
		for (std::size_t i = 0; i < slotsize; i++)
		{
			if (!GetSlot(i))
			{
				return i;
			}
		}

		return slotsize;
	}

	cbSlot* cbVerticalBox::GetNextSlot(const cbVerticalBoxSlot* pSlot, const bool ExcludeHidden) const
	{
		cbSlot* Slot = nullptr;
		bool found = false;
		for (const auto& pVB_Slot : mSlots)
		{
			if (pVB_Slot.get() == pSlot)
			{
				found = true;
				continue;
			}
			else if (!found)
			{
				continue;
			}
			if (ExcludeHidden)
			{
				if (!pVB_Slot->IsHidden())
				{
					Slot = pVB_Slot.get();
					break;
				}
			}
			else
			{
				Slot = pVB_Slot.get();
				break;
			}
		}

		return Slot;
	}

	cbSlot* cbVerticalBox::GetPreviousSlot(const cbVerticalBoxSlot* pSlot, const bool ExcludeHidden) const
	{
		cbSlot* Slot = nullptr;
		for (const auto& pVB_Slot : mSlots)
		{
			if (pVB_Slot.get() == pSlot)
			{
				break;
			}
			if (ExcludeHidden)
			{
				if (!pVB_Slot->IsHidden())
					Slot = pVB_Slot.get();
			}
			else
			{
				Slot = pVB_Slot.get();
			}
		}

		return Slot;
	}

	std::size_t cbVerticalBox::GetSlotSize(const bool ExcludeHidden) const
	{
		if (ExcludeHidden)
		{
			std::size_t Size = 0;
			for (const auto& Slot : mSlots)
			{
				if (Slot->IsHidden())
					continue;
				Size++;
			}
			return Size;
		}
		return slotsize;
	}

	std::vector<cbSlot*> cbVerticalBox::GetSlots() const
	{
		std::vector<cbSlot*> Result;
		Result.reserve(mSlots.size());
		std::transform(mSlots.cbegin(), mSlots.cend(), std::back_inserter(Result), [](auto& ptr) { return ptr.get(); });
		return Result;
	}

	bool cbVerticalBox::HasAnyChildren() const
	{
		return mSlots.size() > 0 || HasAnyComponents();
	}

	std::vector<cbWidgetObj*> cbVerticalBox::GetAllChildren() const
	{
		std::vector<cbWidgetObj*> Children;

		const auto& Components = GetAllComponents();
		Children.reserve(mSlots.size() + Components.size());
		std::transform(Components.cbegin(), Components.cend(), std::back_inserter(Children), [](auto& ptr) { return ptr; });
		std::transform(mSlots.cbegin(), mSlots.cend(), std::back_inserter(Children), [](auto& ptr) { return ptr.get(); });

		return Children;
	}

	std::vector<cbGeometryVertexData> cbVerticalBox::GetVertexData(const bool LineGeometry) const 
	{
		if (LineGeometry)
		{
			cbBounds Bounds(GetDimension());
			std::vector<cbVector4> Data;
			Data.push_back(cbVector4(Bounds.GetCorner(0), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(1), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(2), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(3), 0.0f, 1.0f));

			std::vector<cbVector> TC;
			cbBounds Rect(cbDimension(1.0f, 1.0f), cbVector(0.5f, 0.5f));
			const std::array<unsigned int, 4> Edges = { 0,1,2,3 };
			for (std::size_t i = 0; i < 4; i++)
				TC.push_back(Rect.GetCorner(Edges[i]));

			return cbGeometryFactory::GetAlignedVertexData(Data, TC,
				cbColor::White(),
				GetLocation(), GetRotation(), IsRotated() ? GetOrigin() : cbVector::Zero());
		}
		return std::vector<cbGeometryVertexData>(); 
	};

	std::vector<std::uint32_t> cbVerticalBox::GetIndexData(const bool LineGeometry) const 
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return std::vector<std::uint32_t>();
	};

	cbGeometryDrawData cbVerticalBox::GetGeometryDrawData(const bool LineGeometry) const 
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("NONE", 0, 0, 0, 0);
	};

	bool cbVerticalBox::HideSlot(const std::size_t Index, const bool value)
	{
		if (cbSlot* Slot = GetSlot(Index))
		{
			Slot->Hidden(value);
			ResetInput();
			return true;
		}
		return false;
	}
}
