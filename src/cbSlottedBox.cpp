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
#include "cbSlottedBox.h"
#include "cbCanvas.h"
#include "cbComponent.h"

#include "cbScrollBox.h"

namespace cbgui
{
	std::string cbSlot::GetName() const
	{
		if (IsInserted())
			return Owner->GetName() + "::" + GetClassID();
		return GetClassID();
	}

	std::size_t cbSlot::GetIndex() const
	{
		if (IsInserted())
			return Owner->GetSlotIndex(this);
		return 0;
	};

	std::vector<cbGeometryVertexData> cbSlot::GetVertexData(const bool LineGeometry) const
	{
		/*if (LineGeometry)
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
				GetLocation(), GetRotation(), GetRotation() != 0.0f ? GetOrigin() : cbVector::Zero());
		}*/
		return std::vector<cbGeometryVertexData>();
	};

	std::vector<std::uint32_t> cbSlot::GetIndexData(const bool LineGeometry) const
	{
		//if (LineGeometry)
		//	return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return std::vector<std::uint32_t>();
	};

	cbGeometryDrawData cbSlot::GetGeometryDrawData(const bool LineGeometry) const
	{
		//if (LineGeometry)
		//	return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("NONE", 0, 0, 0, 0);
	};

	cbVector cbSlot::GetOrigin() const
	{
		if (IsInserted())
			return Owner->GetOrigin();
		return cbVector();
	}

	cbVector cbSlot::GetRotatorOrigin() const
	{
		if (IsInserted())
			return Owner->GetRotatorOrigin();
		return cbVector();
	}

	bool cbSlot::IsRotated() const
	{
		if (IsInserted())
			return Owner->IsRotated();
		return false;
	}

	float cbSlot::GetRotation() const
	{
		if (IsInserted())
			return Owner->GetRotation();
		return 0.0f;
	}

	eZOrderMode cbSlot::GetZOrderMode() const
	{
		return eZOrderMode::InOrder;
	}

	std::int32_t cbSlot::GetZOrder() const
	{
		return IsInserted() ? Owner->GetZOrder() : 0;
	}

	bool cbSlot::IsEnabled() const
	{
		return bIsEnabled;
	}

	bool cbgui::cbSlot::IsContentEnabled() const
	{
		return HasContent() ? GetContent()->IsEnabled() : false;
	}

	void cbSlot::EnableContent()
	{
		if (HasContent())
			GetContent()->Enable();
	}

	void cbSlot::DisableContent()
	{
		if (HasContent())
			GetContent()->Disable();
	}

	void cbSlot::UpdateStatus()
	{
		if (!IsInserted())
			return;

		const auto NewValue = Owner->IsEnabled();

		if (NewValue == bIsEnabled)
			return;

		bIsEnabled = NewValue;

		if (HasContent())
			GetContent()->UpdateStatus();
	}

	bool cbSlot::IsFocused() const
	{
		return HasContent() ? GetContent()->IsFocused() : false;
	}
	bool cbSlot::IsFocusable() const
	{
		return HasContent() ? GetContent()->IsFocusable() : false;
	}
	eFocusMode cbSlot::GetFocusMode() const
	{
		return HasContent() ? GetContent()->GetFocusMode() : eFocusMode::ZOrder;
	}

	void cbSlot::Hidden(bool value) 
	{
		if (HasContent()) 
			GetContent()->SetVisibilityState(value ? eVisibility::Hidden : eVisibility::Visible); 
	}

	bool cbSlot::IsInteractableWithKey(std::optional<std::uint64_t> KeyCode) const
	{
		//if (!IsFocusable())
		//	return false;
		if (HasContent())
			return GetContent()->IsInteractableWithKey(KeyCode);
		return false;
	}
	bool cbSlot::OnKeyUp(std::uint64_t KeyCode) 
	{
		if (HasContent())
			return GetContent()->OnKeyUp(KeyCode);
		return false;
	}
	bool cbSlot::OnKeyDown(std::uint64_t KeyCode) 
	{
		if (HasContent())
			return GetContent()->OnKeyDown(KeyCode);
		return false;
	}
	bool cbSlot::IsInteractableWithMouse() const
	{
		if (HasContent())
			return GetContent()->IsInteractableWithMouse();
		return false;
	}
	bool cbSlot::OnMouseEnter(const cbMouseInput& Mouse) 
	{
		if (HasContent())
			return GetContent()->OnMouseEnter(Mouse);
		return false;
	}
	bool cbSlot::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (HasContent())
			return GetContent()->OnMouseLeave(Mouse);
		return false;
	}
	bool cbSlot::OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse)
	{
		if (HasContent())
			return GetContent()->OnMouseWheel(WheelDelta, Mouse);
		return false;
	}
	bool cbSlot::OnMouseMove(const cbMouseInput& Mouse) 
	{
		if (HasContent())
			return GetContent()->OnMouseMove(Mouse);
		return false;
	}
	bool cbSlot::OnMouseButtonDoubleClick(const cbMouseInput& Mouse) 
	{
		if (HasContent())
			return GetContent()->OnMouseButtonDoubleClick(Mouse);
		return false;
	}
	bool cbSlot::OnMouseButtonUp(const cbMouseInput& Mouse) 
	{
		if (HasContent())
			return GetContent()->OnMouseButtonUp(Mouse);
		return false;
	}
	bool cbSlot::OnMouseButtonDown(const cbMouseInput& Mouse) 
	{
		if (HasContent())
			return GetContent()->OnMouseButtonDown(Mouse);
		return false;
	}

	void cbSlot::ResetInput()
	{
		if (HasContent())
			GetContent()->ResetInput();
	}

	//void cbSlot::UpdateCulledBounds()
	//{
	//	//UpdateVerticalCulledBounds();
	//	//UpdateHorizontalCulledBounds();
	//	CulledBounds = GetRotation() != 0.0f ? cbgui::RecalculateBounds(GetDimension(), GetLocation(), GetRotation(), GetOrigin()).Crop(Owner->GetCulledBounds()) : GetBounds().Crop(Owner->GetCulledBounds());
	//}

	/*void cbSlot::UpdateVerticalCulledBounds()
	{
	}

	void cbSlot::UpdateHorizontalCulledBounds()
	{
	}*/

	cbBounds cbSlot::GetCulledBounds() const
	{
		if (!IsInserted())
			return GetBounds();

		if (GetRotation() != 0.0f)
			return cbgui::RecalculateBounds(GetDimension(), GetLocation(), GetRotation(), GetRotatorOrigin()).Crop(Owner->GetCulledBounds());
		else
			return cbBounds(GetBounds()).Crop(Owner->GetCulledBounds());

		//return CulledBounds;
	}

	bool cbSlot::IsItCulled() const
	{
		if (!IsInserted())
			return true;

		if (Owner->IsItCulled())
			return true;

		const cbBounds& OwnerRect = Owner->GetBounds();
		const cbBounds& Rect = GetBounds();
		return OwnerRect.Min.X >= Rect.Max.X || OwnerRect.Min.Y >= Rect.Max.Y;
	}

	void cbSlot::RemoveFromParent()
	{
		auto pOwner = Owner;
		Owner = nullptr;
		if (pOwner)
			pOwner->OnRemoveSlot(this);
	}

	void cbSlot::SetVertexColorAlpha(std::optional<float> Alpha, bool PropagateToChildren)
	{
		if (HasContent())
			GetContent()->SetVertexColorAlpha(Alpha, PropagateToChildren);
	}

	std::optional<float> cbSlot::GetVertexColorAlpha() const
	{
		if (IsInserted())
			return Owner->GetVertexColorAlpha();
		return std::optional<float>();
	}

	void cbSlot::Notify_DimensionChanged()
	{
		if (Owner && IsInserted())
			Owner->Notify_DimensionChanged(this);
	}

	void cbSlot::Notify_RemoveFromParent()
	{
		if (Owner && IsInserted())
			Owner->Notify_RemoveFromParent(this);
	}

	void cbSlot::Notify_VisibilityChanged()
	{
		if (Owner && IsInserted())
			Owner->Notify_VisibilityChanged(this);
	}

	void cbSlot::OnContentInsertedOrReplaced()
	{
		if (Owner && IsInserted())
			Owner->SlotContentInsertedOrReplaced(this);
	}

	void cbSlottedBox::BeginPlay()
	{
		const auto& Components = GetAllComponents();
		for (const auto& Component : Components)
			if (Component)
				Component->BeginPlay();

		const std::size_t SlotSize = GetSlotSize();
		for (std::size_t i = 0; i < SlotSize; i++)
		{
			const auto Slot = GetSlot(i);
			if (Slot)
				Slot->BeginPlay();
		}

		OnBeginPlay();
	}

	void cbSlottedBox::Tick(float DeltaTime)
	{
		if (!IsEnabled())
			return;

		const auto& Components = GetAllComponents();
		for (const auto& Component : Components)
			if (Component)
				Component->Tick(DeltaTime);

		const std::size_t SlotSize = GetSlotSize();
		for (std::size_t i = 0; i < SlotSize; i++)
		{
			const auto Slot = GetSlot(i);
			if (Slot)
				Slot->Tick(DeltaTime);
		}

		OnTick(DeltaTime);
	}

	bool cbSlottedBox::IsInteractableWithKey(std::optional<std::uint64_t> KeyCode) const
	{
		if (!IsFocusable())
			return false;

		auto mSlots = GetSlots();
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsFocusable() && Slot->IsInteractableWithKey(KeyCode))
				return true;
		}
		return false;
	}

	bool cbSlottedBox::OnKeyUp(std::uint64_t KeyCode)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		auto mSlots = GetSlots();
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsFocused() && Slot->IsInteractableWithKey(KeyCode))
				Slot->OnKeyUp(KeyCode);
		}

		return true;
	}

	bool cbSlottedBox::OnKeyDown(std::uint64_t KeyCode)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		auto mSlots = GetSlots();
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsFocused() && Slot->IsInteractableWithKey(KeyCode))
				Slot->OnKeyDown(KeyCode);
		}

		return true;
	}

	bool cbSlottedBox::OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		auto mSlots = GetSlots();
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsFocused() && !Slot->IsHidden())
				Slot->OnMouseWheel(WheelDelta, Mouse);
		}

		return true;
	}

	bool cbSlottedBox::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		auto mSlots = GetSlots();

		std::vector<cbSlot*> ImmediateFocuses;

		cbSlot* Focus = nullptr;
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsHidden())
				continue;

			if (Slot->IsFocusable())
			{
				if (Slot->IsFocused())
				{
					if (!Slot->IsInside(Mouse.MouseLocation))
					{
						if (!Slot->OnMouseLeave(Mouse))
						{
							Focus = Slot;
							break;
						}
					}
					else
					{
						Focus = Slot;
						break;
					}
				}
				else if (Slot->IsInside(Mouse.MouseLocation))
				{
					if (Slot->GetFocusMode() == eFocusMode::ZOrder)
					{
						Focus = Slot;
					}
				}
			}
		}

		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsHidden())
				continue;

			if (Slot->IsFocusable() && Slot->GetFocusMode() == eFocusMode::Immediate)
			{
				if (Slot->IsInside(Mouse.MouseLocation))
				{
					ImmediateFocuses.push_back(Slot);
				}
				else if (Slot->IsFocused())
				{
					if (!Slot->OnMouseLeave(Mouse))
					{
						ImmediateFocuses.push_back(Slot);
						break;
					}
				}
			}
		}

		if (Focus)
		{
			if (!Focus->IsFocused())
				Focus->OnMouseEnter(Mouse);
			else
				Focus->OnMouseMove(Mouse);
		}

		for (const auto& Slot : ImmediateFocuses)
		{
			if (!Slot->IsFocused())
				Slot->OnMouseEnter(Mouse);
			else
				Slot->OnMouseMove(Mouse);
		}

		return true;
	}

	bool cbSlottedBox::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		auto mSlots = GetSlots();
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsHidden())
				continue;

			if (Slot->IsFocused())
			{
				Slot->OnMouseButtonDoubleClick(Mouse);
			}
		}

		return true;
	}

	bool cbSlottedBox::OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		auto mSlots = GetSlots();
		auto slotsize = GetSlotSize();
		if (slotsize == 0)
			slotsize = GetSlotSize();

		for (std::size_t i = 0; i < slotsize; i++)
		{
			const auto& Slot = mSlots[i];
			if (!Slot)
				continue;

			if (Slot->IsHidden())
				continue;

			if (Slot->IsFocused())
			{
				Slot->OnMouseButtonUp(Mouse);
			}
		}

		return true;
	}

	bool cbSlottedBox::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		auto mSlots = GetSlots();
		for (const auto& Slot : mSlots)
		{
			if (!Slot)
				continue;

			if (Slot->IsHidden())
				continue;

			if (Slot->IsFocused())
			{
				Slot->OnMouseButtonDown(Mouse);
			}
		}

		return true;
	}

	bool cbSlottedBox::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (!IsFocusable() || !IsEnabled())
			return false;

		if (!IsInside(Mouse.MouseLocation))
			return false;

		if (IsItCulled())
			return false;

		if (!IsFocused())
			SetFocus(true);

		{
			auto mSlots = GetSlots();

			std::vector<cbSlot*> ImmediateFocuses;

			cbSlot* Focus = nullptr;
			for (const auto& Slot : mSlots)
			{
				if (!Slot)
					continue;
				if (Slot->IsHidden())
					continue;

				if (Slot->IsFocusable())
				{
					if (Slot->IsInside(Mouse.MouseLocation))
					{
						if (Slot->GetFocusMode() == eFocusMode::ZOrder)
						{
							Focus = Slot;
						}
					}
				}
			}

			for (const auto& Slot : mSlots)
			{
				if (!Slot)
					continue;

				if (Slot->IsHidden())
					continue;

				if (Slot->IsFocusable() && Slot->GetFocusMode() == eFocusMode::Immediate)
				{
					if (Slot->IsInside(Mouse.MouseLocation))
					{
						ImmediateFocuses.push_back(Slot);
					}
				}
			}

			if (Focus)
			{
				if (!Focus->IsFocused())
					Focus->OnMouseEnter(Mouse);
			}

			for (auto Slot : ImmediateFocuses)
			{
				if (!Slot->IsFocused())
					Slot->OnMouseEnter(Mouse);
			}
		}

		return true;
	}

	bool cbSlottedBox::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		auto mSlots = GetSlots();

		bool bIsSlotStillFocused = false;

		for (const auto& Slot : mSlots)
		{
			if (Slot)
			{
				Slot->OnMouseLeave(Mouse);

				if (Slot->IsFocused() && !bIsSlotStillFocused)
					bIsSlotStillFocused = true;
			}
		}

		if (!bIsSlotStillFocused)
			SetFocus(false);

		return !bIsSlotStillFocused;
	}

	void cbgui::cbSlottedBox::ResetInput()
	{
		SetFocus(false);

		const auto& Components = GetAllComponents();
		for (const auto& Component : Components)
			if (Component)
				Component->ResetInput();

		const std::size_t SlotSize = GetSlotSize();
		for (std::size_t i = 0; i < SlotSize; i++)
		{
			const auto Slot = GetSlot(i);
			if (Slot)
				Slot->ResetInput();
		}
	}

	bool cbSlottedBox::IsItWrapped() const
	{
		return (HasOwner() || bIsItWrapped);
	}

	bool cbSlottedBox::Wrap()
	{
		bIsItWrapped = true;

		const bool VerticalWrap = WrapVertical();
		const bool HorizontalWrap = WrapHorizontal();

		if (VerticalWrap || HorizontalWrap)
		{
			if (VerticalWrap)
				UpdateVerticalAlignment(true);

			if (HorizontalWrap)
				UpdateHorizontalAlignment(true);

			if (HasOwner())
				DimensionUpdated();

			return true;
		}
		return false;
	}

	bool cbSlottedBox::UnWrap()
	{
		if (HasOwner())
		{
			return false;
		}
		else
		{
			if (!bIsItWrapped)
				return false;

			bIsItWrapped = false;
			
			UnWrapHorizontal();
			UnWrapVertical();
			NotifyCanvas_WidgetUpdated();

			UpdateAlignments(true);

			return true;
		}
		return false;
	}

	void cbgui::cbSlottedBox::Notify_DimensionChanged(cbSlot* Sender)
	{
		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();

		OnSlotDimensionUpdated(Sender);
	}

	void cbgui::cbSlottedBox::Notify_RemoveFromParent(cbSlot* Sender)
	{
		OnRemoveSlot(Sender);
	}

	void cbSlottedBox::SlotContentInsertedOrReplaced(cbSlot* Slot)
	{
		ResetInput();

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbgui::cbSlottedBox::Notify_VisibilityChanged(cbSlot* Sender)
	{
		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();

		OnSlotVisibilityChanged(Sender);
	}
}
