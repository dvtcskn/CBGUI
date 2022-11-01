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
#include <iostream>
#include "cbScrollBox.h"
#include "cbCanvas.h"

namespace cbgui
{
	cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::cbScrollBarHandleComponent(cbScrollBarComponent* pOwner)
		: Super(pOwner)
		, ButtonState(eButtonState::Default)
		, mouseOffset(std::nullopt)
		, Percent(0.0f)
		, Length(0.0f)
		, Thickness(10.0f)
		, bIsScrollable(false)
		, LocationOffset(0.0f)
		, bIsItFocused(false)
	{}

	cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::~cbScrollBarHandleComponent()
	{
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::SetButtonStatus(const eButtonState ButtonStatus)
	{
		ButtonState = ButtonStatus;
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnPressed()
	{
		SetButtonStatus(eButtonState::Pressed);
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnReleased()
	{
		mouseOffset = std::nullopt;
		SetButtonStatus(eButtonState::Default);
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnHovered()
	{
		if (IsPressed())
			return;

		SetButtonStatus(eButtonState::Hovered);
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnUnhovered()
	{
		if (IsPressed())
			return;

		SetButtonStatus(eButtonState::Default);
	}

	cbVector cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetLocation() const
	{
		auto ScrollBar = GetComponentOwner<cbScrollBarComponent>();
		return ScrollBar->GetOrientation() == eOrientation::Vertical ? ScrollBar->GetLocation() + cbVector(0.0f, LocationOffset)
			: ScrollBar->GetOrientation() == eOrientation::Horizontal ? ScrollBar->GetLocation() + cbVector(LocationOffset, 0.0f)
			: ScrollBar->GetLocation();
	}

	cbDimension cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetDimension() const
	{
		return cbDimension(GetWidth(), GetHeight());
	}

	float cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetWidth() const
	{
		return GetComponentOwner<cbScrollBarComponent>()->GetOrientation() == eOrientation::Vertical ? GetThickness()
			: GetComponentOwner<cbScrollBarComponent>()->GetOrientation() == eOrientation::Horizontal ? GetLength()
			: 0.0f;
	}

	float cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetHeight() const
	{
		return GetComponentOwner<cbScrollBarComponent>()->GetOrientation() == eOrientation::Vertical ? GetLength()
			: GetComponentOwner<cbScrollBarComponent>()->GetOrientation() == eOrientation::Horizontal ? GetThickness()
			: 0.0f;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::Slide(const float InPercent)
	{
		Percent = InPercent > 1.0f ? 1.0f : InPercent < 0.0f ? 0.0f : InPercent;

		const auto ScrollBar = GetComponentOwner<cbScrollBarComponent>();

		switch (ScrollBar->GetOrientation())
		{
		case eOrientation::Vertical:
		{
			const auto OwnerBounds = ScrollBar->GetBounds();
			if (SetLocationOffset(OwnerBounds.GetTop() - OwnerBounds.GetCenter().Y + (Length / 2.0f) + ((OwnerBounds.GetHeight() - Length) * Percent)))
			{
				ScrollBar->OnScroll();
				return true;
			}
		}
		break;
		case eOrientation::Horizontal:
		{
			const auto OwnerBounds = ScrollBar->GetBounds();
			if (SetLocationOffset(OwnerBounds.GetLeft() - OwnerBounds.GetCenter().X + (Length / 2.0f) + ((OwnerBounds.GetWidth() - Length) * Percent)))
			{
				ScrollBar->OnScroll();
				return true;
			}
		}
		break;
		}
		return false;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::Slide(const cbVector& pLocation)
	{
		const auto ScrollBar = GetComponentOwner<cbScrollBarComponent>();
		const auto OwnerBounds = ScrollBar->GetBounds();
		switch (ScrollBar->GetOrientation())
		{
		case eOrientation::Vertical:
			return Slide(ScrollBar->GetHeight() == GetLength() ? 0.0f
				: ((mouseOffset.has_value() ? (pLocation.Y - mouseOffset.value().Y) : pLocation.Y) - ScrollBar->GetBounds().GetTop() - (GetLength() / 2.0f)) / (ScrollBar->GetHeight() - GetLength()));
			break;
		case eOrientation::Horizontal:
			return Slide(ScrollBar->GetWidth() == GetLength() ? 0.0f
				: ((mouseOffset.has_value() ? (pLocation.X - mouseOffset.value().X) : pLocation.X) - ScrollBar->GetBounds().GetLeft() - (GetLength() / 2.0f)) / (ScrollBar->GetWidth() - GetLength()));
			break;
		}
		return false;
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::SetThickness(const float InThickness)
	{
		if (InThickness >= 0.0f)
		{
			Thickness = InThickness;
			if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
				NotifyCanvas_WidgetUpdated();
		}
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::UpdateVerticalAlignment()
	{
		const cbScrollBarComponent* ScrollBar = GetComponentOwner<cbScrollBarComponent>();

		if (ScrollBar->GetOrientation() == eOrientation::Vertical)
		{
			const cbScrollBox* ScrollBox = ScrollBar->GetOwner<cbScrollBox>();
			const auto OverflowingHeight = ScrollBox->GetOverflowingExtent(true);
			if (OverflowingHeight.has_value())
			{
				const float Height = ScrollBar->GetHeight() / ((OverflowingHeight.value() + ScrollBar->GetHeight()) / ScrollBar->GetHeight());
				SetLength(Height < 1.0f ? 1.0f : Height);
			}
			else
			{
				SetLength(ScrollBar->GetHeight());
			}
		}
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::UpdateHorizontalAlignment()
	{
		const cbScrollBarComponent* ScrollBar = GetComponentOwner<cbScrollBarComponent>();

		if (ScrollBar->GetOrientation() == eOrientation::Horizontal)
		{
			const cbScrollBox* ScrollBox = ScrollBar->GetOwner<cbScrollBox>();
			const auto OverflowingWidth = ScrollBox->GetOverflowingExtent(true);
			if (OverflowingWidth.has_value())
			{
				const float Width = ScrollBar->GetWidth() / ((OverflowingWidth.value() + ScrollBar->GetWidth()) / ScrollBar->GetWidth());
				SetLength(Width < 1.0f ? 1.0f : Width);
			}
			else
			{
				SetLength(ScrollBar->GetWidth());
			}
		}
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::UpdateRotation()
	{
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	bool cbgui::cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::SetLength(float lenght)
	{
		if (Length != lenght && lenght > 0.0f)
		{
			Length = lenght;

			const auto ScrollBar = GetComponentOwner<cbScrollBarComponent>();

			if (Length >= ScrollBar->GetLength())
			{
				Length = ScrollBar->GetLength();
				Percent = 1.0f;
				SetLocationOffset(0.0f);
				
				if (bIsScrollable)
				{
					bIsScrollable = false;
					ScrollBar->OnScrollable();
				}
			}
			else if (Length < ScrollBar->GetLength())
			{
				if (!bIsScrollable)
				{
					bIsScrollable = true;
					Percent = 0.0f;
					ScrollBar->OnScrollable();
				}

				const auto OwnerBounds = ScrollBar->GetBounds();

				if (ScrollBar->GetOrientation() == eOrientation::Vertical)
					SetLocationOffset(OwnerBounds.GetTop() - OwnerBounds.GetCenter().Y + (Length / 2.0f) + ((OwnerBounds.GetHeight() - Length) * Percent));
				else if (ScrollBar->GetOrientation() == eOrientation::Horizontal)
					SetLocationOffset(OwnerBounds.GetLeft() - OwnerBounds.GetCenter().X + (Length / 2.0f) + ((OwnerBounds.GetWidth() - Length) * Percent));

				ScrollBar->OnScroll();
			}

			if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
				NotifyCanvas_WidgetUpdated();

			return true;
		}
		return false;
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::Enable()
	{
		SetFocus(false);
		SetButtonStatus(eButtonState::Default);
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::Disable()
	{
		SetFocus(false);
		SetButtonStatus(eButtonState::Disabled);
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::UpdateStatus()
	{
		SetFocus(false);
		SetButtonStatus(IsEnabled() ? eButtonState::Default : eButtonState::Disabled);
	}

	std::vector<cbGeometryVertexData> cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetVertexData(const bool LineGeometry) const
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
				IsEnabled() ? VertexColorStyle.GetColor(ButtonState) : VertexColorStyle.GetDisabledColor(),
				GetLocation(), GetRotation(), GetRotation() != 0.0f ? GetOrigin() : cbVector::Zero());
		}

		return cbGeometryFactory::GetAlignedVertexData(cbGeometryFactory::Create4DPlaneVerticesFromRect(GetDimension()),
			cbGeometryFactory::GeneratePlaneTextureCoordinate(),
			IsEnabled() ? VertexColorStyle.GetColor(ButtonState) : VertexColorStyle.GetDisabledColor(),
			GetLocation(), GetRotation(), GetRotation() != 0.0f ? GetOrigin() : cbVector::Zero());
	}

	std::vector<std::uint32_t> cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return cbGeometryFactory::GeneratePlaneIndices();
	}

	cbGeometryDrawData cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("Button", static_cast<std::underlying_type_t<eButtonState>>(ButtonState), 4, 6, 6);
	};

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::SetVertexColorStyle(const cbButtonVertexColorStyle& style)
	{
		VertexColorStyle = style;
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::SetLocationOffset(const float value)
	{
		if (LocationOffset != value)
		{
			LocationOffset = value;
			if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
				NotifyCanvas_WidgetUpdated();
			return true;
		}
		return false;
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::SetMouseOffset(cbVector Location)
	{
		mouseOffset = cbgui::RotateVectorAroundPoint(Location, GetOrigin(), GetRotation() * (-1.0f)) - GetBounds().GetCenter();
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (!GetOwner<cbScrollBox>()->IsFocusable() || !IsEnabled())
			return false;

		if (IsItCulled())
			return false;

		if (!IsFocused())
			SetFocus(true);

		if (!IsHovered() && !IsPressed())
			OnHovered();

		return true;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (IsPressed())
			GetComponentOwner<cbScrollBarComponent>()->Scroll(Mouse.MouseLocation, mouseOffset);
		return true;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		return false;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (!IsPressed())
			return false;

		OnReleased();
		if (IsInside(Mouse.MouseLocation))
		{
			OnHovered();
		}
		else
		{
			SetFocus(false);
			ClearMouseOffset();
		}

		return true;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		SetMouseOffset(Mouse.MouseLocation);

		if (!IsPressed())
			OnPressed();

		return true;
	}

	bool cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!GetOwner<cbScrollBox>()->IsFocusable() || !IsEnabled())
			return false;

		if (IsPressed())
			return false;

		if (IsFocused())
			SetFocus(false);

		if (IsPressed())
			OnReleased();
		if (IsHovered())
			OnUnhovered();

		ClearMouseOffset();

		return true;
	}

	void cbScrollBox::cbScrollBarComponent::cbScrollBarHandleComponent::ResetInput()
	{
		bIsItFocused = false;
		OnReleased();
	}

	void cbScrollBox::cbScrollBarComponent::SetThickness(const float value)
	{
		Thickness = value;
		if (Thickness < Handle->GetThickness())
			Handle->SetThickness(Thickness);
		if (Thickness < BarThickness)
			SetBarThickness(Thickness);
		UpdateAlignments();
	}

	void cbScrollBox::cbScrollBarComponent::SetHandleThickness(const float value)
	{
		if (value < Thickness)
		{
			Handle->SetThickness(value);
		}
		else
		{
			SetThickness(value);
			Handle->SetThickness(value);
		}
	}

	void cbScrollBox::cbScrollBarComponent::SetBarThickness(const float value)
	{
		if (value < Thickness)
		{
			BarThickness = value;
		}
		else
		{
			BarThickness = Thickness;
		}
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	float cbScrollBox::cbScrollBarComponent::GetThickness() const
	{
		return GetOrientation() == eOrientation::Vertical ? Thickness + Padding.GetWidth()
			: GetOrientation() == eOrientation::Horizontal ? Thickness + Padding.GetHeight()
			: 0.0f;
	}

	float cbScrollBox::cbScrollBarComponent::GetLength() const
	{
		return GetOrientation() == eOrientation::Vertical ? GetOwner<cbScrollBox>()->GetHeight() == 0.0f ? 1.0f : GetOwner<cbScrollBox>()->GetHeight() - Padding.GetHeight()
			: GetOrientation() == eOrientation::Horizontal ? GetOwner<cbScrollBox>()->GetWidth() == 0.0f ? 1.0f : GetOwner<cbScrollBox>()->GetWidth() - Padding.GetWidth()
			: 0.0f;
	}

	float cbScrollBox::cbScrollBarComponent::GetWidth() const
	{
		return GetOrientation() == eOrientation::Vertical ? GetThickness()
			: GetOrientation() == eOrientation::Horizontal ? GetLength()
			: 0.0f;
	}

	float cbScrollBox::cbScrollBarComponent::GetHeight() const
	{
		return GetOrientation() == eOrientation::Vertical ? GetLength()
			: GetOrientation() == eOrientation::Horizontal ? GetThickness()
			: 0.0f;
	}

	cbVector cbScrollBox::cbScrollBarComponent::GetLocation() const
	{
		const auto Bound = GetOwner()->GetBounds();
		return GetOrientation() == eOrientation::Vertical ? cbVector(Bound.GetRight() - (GetWidth() / 2.0f) + (Padding.Left - Padding.Right) / 2.0f, Bound.GetCenter().Y + (Padding.Top - Padding.Bottom) / 2.0f)
			: GetOrientation() == eOrientation::Horizontal ? cbVector(Bound.GetCenter().X + (Padding.Left - Padding.Right) / 2.0f, Bound.GetBottom() - (GetHeight() / 2.0f) + (Padding.Top - Padding.Bottom) / 2.0f)
			: cbVector::Zero();
	}

	void cbScrollBox::cbScrollBarComponent::UpdateVerticalAlignment()
	{
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
		Handle->UpdateVerticalAlignment();
	}

	void cbScrollBox::cbScrollBarComponent::UpdateHorizontalAlignment()
	{
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
		Handle->UpdateHorizontalAlignment();
	}

	void cbScrollBox::cbScrollBarComponent::UpdateRotation()
	{
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
		Handle->UpdateRotation();
	}

	void cbScrollBox::cbScrollBarComponent::UpdateStatus()
	{
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
		Handle->UpdateStatus();
	}

	std::vector<cbGeometryVertexData> cbScrollBox::cbScrollBarComponent::GetVertexData(const bool LineGeometry) const
	{
		if (LineGeometry)
		{
			cbBounds Bounds(GetDimension());
			std::vector<cbVector4> Data;
			Data.push_back(cbVector4(Bounds.GetCorner(0), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(1), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(2), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(3), 0.0f, 1.0f));

			cbBounds BarBounds(GetOrientation() == eOrientation::Vertical ?
				cbDimension(BarThickness, GetLength()) : cbDimension(GetLength(), BarThickness));
			Data.push_back(cbVector4(BarBounds.GetCorner(0), 0.0f, 1.0f));
			Data.push_back(cbVector4(BarBounds.GetCorner(1), 0.0f, 1.0f));
			Data.push_back(cbVector4(BarBounds.GetCorner(2), 0.0f, 1.0f));
			Data.push_back(cbVector4(BarBounds.GetCorner(3), 0.0f, 1.0f));

			std::vector<cbVector> TC;
			cbBounds Rect(cbDimension(1.0f, 1.0f), cbVector(0.5f, 0.5f));
			const std::array<unsigned int, 8> Edges = { 0,1,1,2,2,3,3,0 };
			for (std::size_t i = 0; i < 8; i++)
				TC.push_back(Rect.GetCorner(Edges[i]));

			return cbGeometryFactory::GetAlignedVertexData(Data, TC,
				IsEnabled() ? VertexColorStyle.GetColor() : VertexColorStyle.GetDisabledColor(),
				GetLocation(), GetRotation(), GetRotation() != 0.0f ? GetOrigin() : cbVector::Zero());
		}

		return cbGeometryFactory::GetAlignedVertexData(cbGeometryFactory::Create4DPlaneVerticesFromRect(GetOrientation() == eOrientation::Vertical ? 
													   cbDimension(BarThickness, GetLength()) : cbDimension(GetLength(), BarThickness)),
			   cbGeometryFactory::GeneratePlaneTextureCoordinate(),
			   IsEnabled() ? VertexColorStyle.GetColor() : VertexColorStyle.GetDisabledColor(),
			   GetLocation(), GetRotation(), GetRotation() != 0.0f ? GetOrigin() : cbVector::Zero());
	}

	std::vector<std::uint32_t> cbScrollBox::cbScrollBarComponent::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, };
		return cbGeometryFactory::GeneratePlaneIndices();
	}

	cbGeometryDrawData cbScrollBox::cbScrollBarComponent::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 8, 16, 16);
		return cbGeometryDrawData("Plane", 0, 4, 6, 6);
	};

	void cbScrollBox::cbScrollBarComponent::SetBarVertexColorStyle(const cbVertexColorStyle& style)
	{
		VertexColorStyle = style;
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	void cbScrollBox::cbScrollBarComponent::SetPadding(cbMargin InPadding)
	{
		Padding = InPadding;
		if (GetOwner<cbScrollBox>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
		UpdateAlignments();
		ResetScrollBar();
	}

	bool cbScrollBox::cbScrollBarComponent::SetScrollAmount(float value)
	{
		if (ScrollAmount != value)
		{
			ScrollAmount = value;

			return true;
		}
		return false;
	}

	bool cbScrollBox::cbScrollBarComponent::IsVisible() const
	{
		if (bIsHidden)
			return false;

		if (bShowScrollBarIfScrollable)
		{
			if (IsScrollable())
				return true;
			else
				return false;
		}

		return true;
	}

	void cbScrollBox::cbScrollBarComponent::Hidden(bool Hide)
	{
		if (bIsHidden != Hide)
		{
			bIsHidden = Hide;
			GetOwner<cbScrollBox>()->UpdateSlotAlignments();
		}
	}

	void cbScrollBox::cbScrollBarComponent::ShowScrollBarIfScrollable(bool Hide)
	{
		if (bShowScrollBarIfScrollable != Hide)
		{
			bShowScrollBarIfScrollable = Hide;
			GetOwner<cbScrollBox>()->UpdateSlotAlignments();
		}
	}

	void cbScrollBox::cbScrollBarComponent::Scroll(const cbVector& InMouseLocation, std::optional<cbVector> MouseOffset)
	{
		Handle->Slide(cbgui::RotateVectorAroundPoint((InMouseLocation), GetOrigin(), GetRotation() * (-1.0f)));
	}

	void cbScrollBox::cbScrollBarComponent::ScrollWheel(float WheelDelta)
	{
		float Percent = Handle->GetPercent();

		switch (GetOrientation())
		{
		case eOrientation::Vertical:
		{
			if (WheelDelta > 0.0f)
			{
				const float MaxPercent = (GetHeight() - (Handle->GetLength() / 2.0f)) / GetHeight();
				Percent = Percent - ScrollStepPercent;
			}
			if (WheelDelta < 0.0f)
			{
				const float MinPercent = (Handle->GetLength() / 2.0f) / GetHeight();
				Percent = Percent + ScrollStepPercent;
			}
		}
		break;
		case eOrientation::Horizontal:
		{
			if (WheelDelta > 0.0f)
			{
				const float MaxPercent = (GetWidth() - (Handle->GetLength() / 2.0f)) / GetWidth();
				Percent = Percent - ScrollStepPercent;
			}
			if (WheelDelta < 0.0f)
			{
				const float MinPercent = (Handle->GetLength() / 2.0f) / GetWidth();
				Percent = Percent + ScrollStepPercent;
			}
		}
		break;
		}

		Scroll(Percent > 1.0f ? 1.0f : Percent < 0.0f ? 0.0f : Percent);
	}

	void cbScrollBox::cbScrollBarComponent::Scroll(const float Percent)
	{
		Handle->Slide(Percent > 1.0f ? 1.0f : Percent < 0.0f ? 0.0f : Percent);
	}

	void cbScrollBox::cbScrollBarComponent::OnScroll()
	{
		const float Percent = Handle->GetPercent();

		if (GetOrientation() == eOrientation::Vertical)
		{
			const float Height = GetHeight();
			float TotalHeight = GetOwner<cbScrollBox>()->GetTotalAreaHeight(true);
			TotalHeight = TotalHeight > Height ? TotalHeight : Height;

			if (SetScrollAmount((TotalHeight * Percent) - ((Height + Padding.GetHeight()) * Percent)))
				GetOwner<cbScrollBox>()->UpdateSlotVerticalAlignment();
		}
		else if (GetOrientation() == eOrientation::Horizontal)
		{
			const float Width = GetWidth();
			float TotalWidth = GetOwner<cbScrollBox>()->GetTotalAreaWidth(true);
			TotalWidth = TotalWidth > Width ? TotalWidth : Width;

			if (SetScrollAmount((TotalWidth * Percent) - ((Width + Padding.GetWidth()) * Percent)))
				GetOwner<cbScrollBox>()->UpdateSlotHorizontalAlignment();
		}
	}

	float cbScrollBox::cbScrollBarComponent::GetScrollAmount() const
	{
		return ScrollAmount;
	}

	void cbScrollBox::cbScrollBarComponent::OnScrollable()
	{
		if (SetScrollAmount(0.0f))
		{
			switch (GetOrientation())
			{
			case eOrientation::Vertical:
			{
				GetOwner<cbScrollBox>()->UpdateSlotHorizontalAlignment();
			}
			break;
			case eOrientation::Horizontal:
			{
				GetOwner<cbScrollBox>()->UpdateSlotVerticalAlignment();
			}
			break;
			}
		}
	}

	void cbScrollBox::cbScrollBarComponent::ResetScrollBar()
	{
		Scroll(0.0f);
	}

	void cbgui::cbScrollBox::cbScrollBarComponent::ResetInput()
	{
		Handle->ResetInput();
	}

	cbScrollBox::cbScrollBoxSlot::cbScrollBoxSlot(cbScrollBox* InOwner, const cbWidget::SharedPtr& pContent)
		: Super(InOwner)
		, bIsInserted(false)
		, Content(pContent)
		, Location(cbVector::Zero())
		, Dimension(cbDimension())
	{
		UpdateVerticalDimension();
		UpdateHorizontalDimension();
		UpdateVerticalLocation();
		UpdateHorizontalLocation();

		if (Content)
			Content->AttachToSlot(this);
	}

	void cbScrollBox::cbScrollBoxSlot::UpdateHorizontalDimension()
	{
		const cbScrollBox* ScrollBox = GetOwner<cbScrollBox>();

		if (ScrollBox->GetOrientation() == eOrientation::Vertical)
		{
			const float Thickness = ScrollBox->GetScrollBarThickness();
			const cbBounds Bounds = ScrollBox->GetBounds();
			Dimension.Width = ScrollBox->GetWidth() - Thickness;
		}
		else if (ScrollBox->GetOrientation() == eOrientation::Horizontal)
		{
			Dimension.Width = Content ? Content->GetNonAlignedWidth() + Content->GetPadding().GetWidth() : 0.0f;
		}
	}

	void cbScrollBox::cbScrollBoxSlot::UpdateVerticalDimension()
	{
		const cbScrollBox* ScrollBox = GetOwner<cbScrollBox>();

		if (ScrollBox->GetOrientation() == eOrientation::Vertical)
		{
			Dimension.Height = Content ? Content->GetNonAlignedHeight() + Content->GetPadding().GetHeight() : 0.0f;
		}
		else if (ScrollBox->GetOrientation() == eOrientation::Horizontal)
		{
			const float Thickness = ScrollBox->GetScrollBarThickness();
			const cbBounds Bounds = ScrollBox->GetBounds();

			Dimension.Height = ScrollBox->GetHeight() - Thickness;
		}
	}

	void cbScrollBox::cbScrollBoxSlot::UpdateHorizontalLocation()
	{
		const cbScrollBox* ScrollBox = GetOwner<cbScrollBox>();

		if (ScrollBox->GetOrientation() == eOrientation::Vertical)
		{
			const float Thickness = ScrollBox->GetScrollBarThickness();
			const cbBounds Bounds = ScrollBox->GetBounds();
			Location.X = (Bounds.GetCenter().X - ((Thickness) / 2.0f));
		}
		else if (ScrollBox->GetOrientation() == eOrientation::Horizontal)
		{
			Location.X = (ScrollBox->GetNextSlotLocation(eHorizontalAlignment::Align_Left, this) + GetWidth() / 2.0f);
		}
	}

	void cbScrollBox::cbScrollBoxSlot::UpdateVerticalLocation()
	{
		const cbScrollBox* ScrollBox = GetOwner<cbScrollBox>();

		if (ScrollBox->GetOrientation() == eOrientation::Vertical)
		{
			Location.Y = (ScrollBox->GetNextSlotLocation(eVerticalAlignment::Align_Top, this) + GetHeight() / 2.0f);
		}
		else if (ScrollBox->GetOrientation() == eOrientation::Horizontal)
		{
			const float Thickness = ScrollBox->GetScrollBarThickness();
			const cbBounds Bounds = ScrollBox->GetBounds();

			Location.Y = (Bounds.GetCenter().Y - ((Thickness) / 2.0f));
		}
	}

	void cbScrollBox::cbScrollBoxSlot::ReplaceContent(const cbWidget::SharedPtr& pContent)
	{
		if (!pContent)
			return;

		if (Content)
			Content = nullptr;
		Content = pContent;
		Content->AttachToSlot(this);
		OnContentInsertedOrReplaced();
		GetOwner<cbScrollBox>()->OnScrollBoxSlotContentInsertedOrReplaced(this);
	}

	cbDimension cbScrollBox::cbScrollBoxSlot::GetDimension() const
	{
		return Dimension;
	}

	float cbScrollBox::cbScrollBoxSlot::GetWidth() const
	{
		return Dimension.Width;
	}

	float cbScrollBox::cbScrollBoxSlot::GetHeight() const
	{
		return Dimension.Height;
	}

	void cbScrollBox::cbScrollBoxSlot::UpdateVerticalAlignment()
	{
		UpdateVerticalDimension();
		UpdateVerticalLocation();

		if (Content)
			Content->UpdateVerticalAlignment();
	}

	void cbScrollBox::cbScrollBoxSlot::UpdateHorizontalAlignment()
	{
		UpdateHorizontalDimension();
		UpdateHorizontalLocation();

		if (Content)
			Content->UpdateHorizontalAlignment();
	}

	cbScrollBox::cbScrollBox(const eOrientation orientation)
		: Super()
		, Transform(cbTransform(cbDimension(256.0f, 256.0f)))
		, Orientation(orientation)
		, slotsize(0)
		, ScrollBar(cbScrollBarComponent::CreateUnique(this))
	{
		SetScrollBar_HandleThickness(10.0f);
		SetScrollBarThickness(10.0f);
	}

	cbScrollBox::~cbScrollBox()
	{
		ScrollBar = nullptr;
		for (auto& Slot : mSlots)
			Slot = nullptr;
		mSlots.clear();
		slotsize = 0;
	}

	void cbScrollBox::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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
				ScrollBar->UpdateAlignments();
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
				ScrollBar->UpdateHorizontalAlignment();
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
				ScrollBar->UpdateVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbScrollBox::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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
					ScrollBar->UpdateAlignments();
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
					ScrollBar->UpdateHorizontalAlignment();
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
					ScrollBar->UpdateVerticalAlignment();
					NotifyCanvas_WidgetUpdated();
				}
			}
		}
	}

	void cbScrollBox::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbgui::cbScrollBox::SetScrollBarPadding(const cbMargin& Padding)
	{
		ScrollBar->SetPadding(Padding);

		UpdateSlotAlignments();
		if (IsItWrapped())
			Wrap();
	}

	void cbScrollBox::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			ScrollBar->UpdateRotation();
			NotifyCanvas_WidgetUpdated();

			for (const auto& Slot : mSlots)
				if (Slot)
					Slot->UpdateRotation();
		}
	}

	bool cbScrollBox::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (Super::OnMouseEnter(Mouse))
		{
			return true;
		}
		return false;
	}

	bool cbScrollBox::OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse)
	{
		if (Super::OnMouseWheel(WheelDelta, Mouse))
		{
			if (!ScrollBar->IsFocused())
				return false;

			ScrollBar->ScrollWheel(WheelDelta);

			auto Handle = ScrollBar->GetHandle();

			if (Handle->IsInside(Mouse.MouseLocation))
			{
				Handle->OnMouseEnter(Mouse);
			}
			else if (ScrollBar->IsInside(Mouse.MouseLocation))
			{
				if (Handle->IsFocused())
				{
					Handle->OnMouseLeave(Mouse);
				}
			}
			else
			{
				if (Handle->IsFocused())
					Handle->OnMouseLeave(Mouse);
			}

			return true;
		}
		return false;
	}

	bool cbScrollBox::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (Super::OnMouseMove(Mouse))
		{
			if (ScrollBar->IsInside(Mouse.MouseLocation) || ScrollBar->Handle->IsPressed())
			{
				if (!ScrollBar->IsFocused())
				{
					if (!ScrollBar->IsEnabled() || ScrollBar->IsItCulled())
						return false;

					if (!ScrollBar->IsInside(Mouse.MouseLocation))
						return false;

					return true;
				}
				auto Handle = ScrollBar->GetHandle();

				if (Handle->IsFocused() && Handle->IsPressed())
				{
					Handle->OnMouseMove(Mouse);
					return true;
				}

				if (Handle->IsInside(Mouse.MouseLocation))
				{
					Handle->OnMouseEnter(Mouse);
				}
				else if (ScrollBar->IsInside(Mouse.MouseLocation))
				{
					if (Handle->IsFocused())
					{
						Handle->OnMouseLeave(Mouse);
					}
				}
				else
				{
					if (Handle->IsFocused())
						Handle->OnMouseLeave(Mouse);
				}
			}
			else
			{
				if (ScrollBar->IsFocused())
				{
					if (!ScrollBar->IsEnabled())
						return false;

					auto Handle = ScrollBar->GetHandle();

					if (Handle->IsPressed())
						return false;

					if (Handle->IsFocused())
						Handle->OnMouseLeave(Mouse);
				}
			}
			return true;
		}
		return false;
	}

	bool cbScrollBox::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (Super::OnMouseButtonDoubleClick(Mouse))
		{
			if (!ScrollBar->IsFocused())
				return false;

			auto Handle = ScrollBar->GetHandle();

			if (Handle->IsInside(Mouse.MouseLocation))
			{
				if (Handle->IsFocused())
				{
					Handle->OnMouseButtonDown(Mouse);
				}
			}
			else if (ScrollBar->IsInside(Mouse.MouseLocation))
			{
				ScrollBar->Scroll(Mouse.MouseLocation);
				{
					if (Handle->IsInside(Mouse.MouseLocation))
					{
						if (Handle->OnMouseEnter(Mouse))
						{
							if (Handle->IsFocused())
								Handle->OnMouseButtonDown(Mouse);
						}
					}
				}
			}

			return true;
		}
		return false;
	}

	bool cbScrollBox::OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		if (Super::OnMouseButtonUp(Mouse))
		{
			if (!ScrollBar->IsFocused())
				return false;

			auto Handle = ScrollBar->GetHandle();

			if (Handle->IsPressed())
				Handle->OnMouseButtonUp(Mouse);

			if (!IsInside(Mouse.MouseLocation))
				OnMouseLeave(Mouse);

			return true;
		}
		return false;
	}

	bool cbScrollBox::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (Super::OnMouseButtonDown(Mouse))
		{
			if (!ScrollBar->IsFocused() || !ScrollBar->IsVisible())
				return false;

			auto Handle = ScrollBar->GetHandle();

			if (Handle->IsInside(Mouse.MouseLocation))
			{
				if (Handle->IsFocused())
				{
					Handle->OnMouseButtonDown(Mouse);
				}
			}
			else if (ScrollBar->IsInside(Mouse.MouseLocation))
			{
				ScrollBar->Scroll(Mouse.MouseLocation);
				{
					if (Handle->IsInside(Mouse.MouseLocation))
					{
						if (Handle->OnMouseEnter(Mouse))
						{
							if (Handle->IsFocused())
								Handle->OnMouseButtonDown(Mouse);
						}
					}
				}
			}

			return true;
		}
		return false;
	}

	bool cbScrollBox::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (ScrollBar->Handle->IsPressed())
			return false;

		if (Super::OnMouseLeave(Mouse))
		{
			if (!ScrollBar->IsFocusable() || !ScrollBar->IsEnabled())
				return false;

			auto Handle = ScrollBar->GetHandle();

			if (Handle->IsPressed())
				return false;

			if (Handle->IsFocused())
				Handle->OnMouseLeave(Mouse);

			return true;
		}
		return false;
	}

	void cbScrollBox::ResetScrollBar()
	{
		ScrollBar->ResetScrollBar();
	}

	float cbScrollBox::GetScrollBarThickness() const
	{
		return ScrollBar->IsVisible() ? ScrollBar->GetThickness() : 0.0f;
	}

	void cbScrollBox::SetScrollBarThickness(const float value)
	{
		ScrollBar->SetThickness(value);
		UpdateSlotAlignments();
	}

	void cbScrollBox::SetScrollBar_BarThickness(const float value)
	{
		ScrollBar->SetBarThickness(value);
		UpdateSlotAlignments();
	}

	void cbScrollBox::SetScrollBar_HandleThickness(const float value)
	{
		ScrollBar->SetHandleThickness(value);
		UpdateSlotAlignments();
	}

	cbSlot* cbScrollBox::Insert(const cbWidget::SharedPtr& Content, const std::optional<std::size_t> SlotIndex)
	{
		if (!Content)
			return nullptr;

		return Insert(cbScrollBoxSlot::Create(this, Content), SlotIndex.has_value() ? SlotIndex.value() : GetNextSlotIndex());
	}

	cbSlot* cbScrollBox::Insert(const cbScrollBoxSlot::SharedPtr& Slot, const std::size_t Index)
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

		ScrollBar->UpdateAlignments();

		return mSlots.at(Index).get();
	}

	float cbScrollBox::GetNextSlotLocation(const eVerticalAlignment InVAlignment, const cbScrollBoxSlot* pSlot, const float ScrollWeight) const
	{
		switch (InVAlignment)
		{
		case eVerticalAlignment::Align_Top:
			{
				cbSlot* Slot = GetPreviousSlot(pSlot);
				if (!Slot)
					return GetBounds().GetTop() - (ScrollBar->ScrollAmount * ScrollWeight);
				return Slot->GetBounds().GetBottom();
			}
			case eVerticalAlignment::Align_Bottom:
			{
				cbSlot* Slot = GetPreviousSlot(pSlot);
				if (!Slot)
					return GetBounds().GetBottom() + (ScrollBar->ScrollAmount * ScrollWeight);
				return Slot->GetBounds().GetTop();
			}
		}

		return 0.0f;
	}

	float cbScrollBox::GetNextSlotLocation(const eHorizontalAlignment InHAlignment, const cbScrollBoxSlot* pSlot, const float ScrollWeight) const
	{
		switch (InHAlignment)
		{
			case eHorizontalAlignment::Align_Left:
			{
				cbSlot* Slot = GetPreviousSlot(pSlot);
				if (!Slot)
					return GetBounds().GetLeft() - (ScrollBar->ScrollAmount * ScrollWeight);
				return Slot->GetBounds().GetRight();
			}
			case eHorizontalAlignment::Align_Right:
			{
				cbSlot* Slot = GetPreviousSlot(pSlot);
				if (!Slot)
					return GetBounds().GetRight() + (ScrollBar->ScrollAmount * ScrollWeight);
				return Slot->GetBounds().GetLeft();
			}
		}

		return 0.0f;
	}

	bool cbScrollBox::CanScrollUp()
	{
		if (GetOrientation() == eOrientation::Horizontal || GetSlotSize() == NULL)
			return false;

		const auto Slot = GetSlot(GetSlotSize() - 1);
		if (GetBounds().GetBottom() < Slot->GetBounds().GetBottom())
		{
			return true;
		}
		return false;
	}

	bool cbScrollBox::CanScrollDown()
	{
		if (GetOrientation() == eOrientation::Horizontal || GetSlotSize() == NULL)
			return false;

		const auto Slot = GetSlot(0);
		if (GetBounds().GetTop() > Slot->GetBounds().GetTop())
		{
			return true;
		}
		return false;
	}

	bool cbScrollBox::CanScrollLeft()
	{
		if (GetOrientation() == eOrientation::Vertical || GetSlotSize() == NULL)
			return false;

		const auto Slot = GetSlot(GetSlotSize() - 1);
		if (GetBounds().GetRight() < Slot->GetBounds().GetRight())
		{
			return true;
		}
		return false;
	}

	bool cbScrollBox::CanScrollRight()
	{
		if (GetOrientation() == eOrientation::Vertical || GetSlotSize() == NULL)
			return false;

		const auto Slot = GetSlot(0);
		if (GetBounds().GetLeft() > Slot->GetBounds().GetLeft())
		{
			return true;
		}
		return false;
	}

	bool cbScrollBox::SetOrientation(const eOrientation orientation)
	{
		if (Orientation == orientation)
			return false;

		Orientation = orientation;
		UpdateAlignments(true);

		return true;
	}

	void cbScrollBox::Scroll(const float Percent)
	{
		switch (GetOrientation())
		{
			case eOrientation::Vertical:
			{
				if (!ScrollBar)
					return;

				if (Percent > ScrollBar->GetCurrentPercent())
					if (!CanScrollUp())
						return;
				if (Percent < ScrollBar->GetCurrentPercent())
					if (!CanScrollDown())
						return;

				if (ScrollBar->GetHeight() == 0.0f)
					return;

				ScrollBar->Scroll(Percent > 1.0f ? 1.0f : Percent < 0.0f ? 0.0f : Percent); 
			}
			break;
			case eOrientation::Horizontal:
			{
				if (!ScrollBar)
					return;

				if (Percent > ScrollBar->GetCurrentPercent())
					if (!CanScrollLeft())
						return;
				if (Percent < ScrollBar->GetCurrentPercent())
					if (!CanScrollRight())
						return;

				if (ScrollBar->GetWidth() == 0.0f)
					return;

				ScrollBar->Scroll(Percent > 1.0f ? 1.0f : Percent < 0.0f ? 0.0f : Percent);
			}
			break;
		}
	}

	void cbScrollBox::ScrollSlotIntoView(const std::size_t Index)
	{
		cbSlot* Slot = GetSlot(Index);

		if (!Slot)
			return;

		switch (Orientation)
		{
		case eOrientation::Vertical:
		{
			if (GetLocation().Y > Slot->GetLocation().Y)
			{
				if (!IsInside(Slot->GetBounds().Min))
				{
					const float TotalHeight = GetTotalAreaHeight(true);
					{
						const float Slot_Offset = Slot->GetBounds().Min.Y - GetBounds().Min.Y + ScrollBar->ScrollAmount;

						const float Result = Slot_Offset / (TotalHeight - GetHeight());

						ScrollBar->Scroll(Result);
					}
				}
			}
			else
			{
				if (!IsInside(Slot->GetBounds().Max))
				{
					const float TotalHeight = GetTotalAreaHeight(true);
					{
						const float Slot_Offset = Slot->GetBounds().Max.Y - GetBounds().Max.Y + ScrollBar->ScrollAmount;

						const float Result = Slot_Offset / (TotalHeight - GetHeight());

						ScrollBar->Scroll(Result);
					}
				}
			}
		}
		break;
		case eOrientation::Horizontal:
		{
			if (GetLocation().X > Slot->GetLocation().X)
			{
				if (!IsInside(Slot->GetBounds().Min))
				{
					const float TotalWidth = GetTotalAreaWidth(true);
					{
						const float Slot_Offset = Slot->GetBounds().Min.X - GetBounds().Min.X + ScrollBar->ScrollAmount;

						const float Result = Slot_Offset / (TotalWidth - GetHeight());

						ScrollBar->Scroll(Result);
					}
				}
			}
			else
			{
			if (!IsInside(Slot->GetBounds().Max))
			{
				const float TotalWidth = GetTotalAreaWidth(true);
				{
					const float Slot_Offset = Slot->GetBounds().Min.X - GetBounds().Min.X + ScrollBar->ScrollAmount;

					const float Result = Slot_Offset / (TotalWidth - GetHeight());

					ScrollBar->Scroll(Result);
				}
			}
			}
		}
		break;
		}
	}

	float cbScrollBox::GetTotalAreaWidth(const bool ExcludeHidden) const
	{
		float Width = 0.0f;
		const std::size_t Size = GetSlotSize();
		for (std::size_t i = 0; i < Size; i++)
		{
			const auto& Slot = GetSlot(i);

			if (ExcludeHidden && Slot->IsHidden())
				continue;
			const auto Content = Slot->GetContent();
			Width = (Width + (Content->GetNonAlignedWidth() + Content->GetPadding().GetWidth()));
		}
		return Width;
	}

	float cbScrollBox::GetTotalAreaHeight(const bool ExcludeHidden) const
	{
		float Height = 0.0f;
		const std::size_t Size = GetSlotSize();
		for (std::size_t i = 0; i < Size; i++)
		{
			const auto& Slot = GetSlot(i);

			if (ExcludeHidden && Slot->IsHidden())
				continue;
			const auto Content = Slot->GetContent();
			Height = (Height + (Content->GetNonAlignedHeight() + Content->GetPadding().GetHeight()));
		}
		return Height;
	}

	std::optional<float> cbScrollBox::GetOverflowingExtent(const bool ExcludeHidden) const
	{
		std::optional<float> Extent;
		std::size_t Size = GetSlotSize();
		for (std::size_t i = 0; i < Size; i++)
		{
			const auto& Slot = GetSlot(i);

			if (ExcludeHidden && Slot->IsHidden())
				continue;

			const auto Content = Slot->GetContent();

			if (Orientation == eOrientation::Vertical)
			{
				if (Extent.has_value())
					Extent = Extent.value() + (Content->GetNonAlignedHeight() + Content->GetPadding().GetHeight());
				else
					Extent = Content->GetNonAlignedHeight() + Content->GetPadding().GetHeight();
			}
			else if (Orientation == eOrientation::Horizontal)
			{
				if (Extent.has_value())
					Extent = Extent.value() + Content->GetNonAlignedWidth() + Content->GetPadding().GetWidth();
				else
					Extent = Content->GetNonAlignedWidth() + Content->GetPadding().GetWidth();
			}
		}

		if (Orientation == eOrientation::Vertical)
		{
			if (Extent.has_value())
			{
				if (Extent.value() > GetHeight())
					return Extent.value() - GetHeight();
				else
					return std::nullopt;
			}
		}
		else if (Orientation == eOrientation::Horizontal)
		{
			if (Extent.has_value())
			{
				if (Extent.value() > GetWidth())
					return Extent.value() - GetWidth();
				else
					return std::nullopt;
			}
		}
		return std::nullopt;
	}

	void cbScrollBox::UpdateVerticalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerHeight = GetOwner()->GetHeight();
			const std::optional<const eVerticalAlignment> CustomAlignment = GetNonAlignedHeight() > OwnerHeight ? std::optional<eVerticalAlignment>(eVerticalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsHeightAligned())
				Transform.ResetHeightAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetVerticalAlignment(), GetOwner()->GetBounds(), GetVerticalAnchor()) || ForceAlign)
			{
				ScrollBar->UpdateVerticalAlignment();
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
				ScrollBar->UpdateVerticalAlignment();
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetVerticalAlignment() == eVerticalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetY(CanvasOffset.Y);
				ScrollBar->UpdateVerticalAlignment();
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsHeightAligned())
			{
				Transform.ResetHeightAlignment();
				ScrollBar->UpdateVerticalAlignment();
				UpdateSlotVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else
			{
				ScrollBar->UpdateVerticalAlignment();
				UpdateSlotVerticalAlignment();
			}
		}
	}

	void cbScrollBox::UpdateHorizontalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerWidth = GetOwner()->GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetHorizontalAlignment(), GetOwner()->GetBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
				ScrollBar->UpdateHorizontalAlignment();
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
				ScrollBar->UpdateHorizontalAlignment();
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetX(CanvasOffset.X);
				ScrollBar->UpdateHorizontalAlignment();
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsWidthAligned())
			{
				Transform.ResetWidthAlignment();
				ScrollBar->UpdateHorizontalAlignment();
				UpdateSlotHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else
			{
				ScrollBar->UpdateHorizontalAlignment();
				UpdateSlotHorizontalAlignment();
			}
		}
	}

	void cbScrollBox::UpdateRotation()
	{
		if (HasOwner())
			Transform.SetRollOffset(GetOwner()->GetRotation());
		else if (IsAlignedToCanvas())
			Transform.SetRollOffset(GetCanvas()->GetScreenRotation());
		else
			Transform.SetRollOffset(0.0f);
		ScrollBar->UpdateRotation();
		NotifyCanvas_WidgetUpdated();

		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateRotation();
	}

	bool cbScrollBox::WrapVertical()
	{
		if (!IsItWrapped())
			return false;

		if (GetSlotSize() == 0)
		{
			switch (GetOrientation())
			{
				case eOrientation::Vertical:
					return Transform.CompressHeight(1.0f);
					break;
				case eOrientation::Horizontal:
				{
					const float Thickness = ScrollBar->GetThickness();
					return Transform.CompressHeight(Thickness);
					break;
				}
			}
			return false;
		}

		const float Thickness = ScrollBar->GetThickness();
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

			switch (GetOrientation())
			{
			case eOrientation::Vertical:
				Height += SlotHeight + (Padding.Top + Padding.Bottom);
				break;
			case eOrientation::Horizontal:
				if (((SlotHeight + Thickness) + (Padding.Top + Padding.Bottom)) > Height)
					Height = SlotHeight + Thickness + (Padding.Top + Padding.Bottom);
				break;
			}
		}

		if (Transform.CompressHeight(Height))
		{
			ScrollBar->UpdateVerticalAlignment();
			return true;
		}
		return false;
	}

	bool cbScrollBox::WrapHorizontal()
	{
		if (!IsItWrapped())
			return false;

		if (GetSlotSize() == 0)
		{
			switch (GetOrientation())
			{
				case eOrientation::Vertical:
				{
					const float Thickness = ScrollBar->GetThickness();
					return Transform.CompressWidth(Thickness);
				}
					break;
				case eOrientation::Horizontal:
					return Transform.CompressHeight(1.0f);
					break;
			}
			return false;
		}

		const float Thickness = ScrollBar->GetThickness();
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

			switch (GetOrientation())
			{
			case eOrientation::Vertical:
				if (((SlotWidth + Thickness) + (Padding.Left + Padding.Right)) > Width)
					Width = SlotWidth + Thickness + (Padding.Left + Padding.Right);
				break;
			case eOrientation::Horizontal:
				Width += SlotWidth + (Padding.Left + Padding.Right);
				break;
			}
		}

		if (Transform.CompressWidth(Width))
		{
			ScrollBar->UpdateHorizontalAlignment();
			return true;
		}
		return false;
	}

	bool cbScrollBox::UnWrapVertical()
	{
		if (Transform.IsHeightCompressed())
		{
			Transform.ResetHeightCompressed();
			ScrollBar->UpdateVerticalAlignment();
			return true;
		}
		return false;
	}

	bool cbScrollBox::UnWrapHorizontal()
	{
		if (Transform.IsWidthCompressed())
		{
			Transform.ResetWidthCompressed();
			ScrollBar->UpdateHorizontalAlignment();
			return true;
		}
		return false;
	}

	void cbScrollBox::OnAttach()
	{
		WrapVertical();
		WrapHorizontal();
		NotifyCanvas_WidgetUpdated();
		UpdateSlotAlignments();
	}

	void cbScrollBox::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();
			ScrollBar->UpdateAlignments();
			NotifyCanvas_WidgetUpdated();
		}

		UnWrap();
	}

	void cbScrollBox::OnScrollBoxSlotContentInsertedOrReplaced(cbSlot* Slot)
	{
		ScrollBar->UpdateAlignments();
	}

	void cbScrollBox::OnSlotVisibilityChanged(cbSlot* Slot)
	{
		ScrollBar->UpdateAlignments();
	}

	void cbScrollBox::OnSlotDimensionUpdated(cbSlot* Slot)
	{
		ScrollBar->UpdateAlignments();
	}

	void cbScrollBox::UpdateSlotVerticalAlignment()
	{
		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateVerticalAlignment();
	}

	void cbScrollBox::UpdateSlotHorizontalAlignment()
	{
		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateHorizontalAlignment();
	}

	void cbScrollBox::UpdateStatus()
	{
		SetFocus(false);
		ScrollBar->UpdateStatus();
		for (const auto& Slot : mSlots)
			if (Slot)
				Slot->UpdateStatus();
	}

	cbSlot* cbScrollBox::GetOverlappingSlot(const cbBounds& Bounds) const
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

	bool cbScrollBox::ReplaceSlotContent(std::size_t Index, const cbWidget::SharedPtr& New)
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

	bool cbScrollBox::OnRemoveSlot(cbSlot* Slot)
	{
		if (!Slot || slotsize == 0)
		{
			return false;
		}

		cbScrollBoxSlot::SharedPtr pSlot = nullptr;
		std::vector<cbScrollBoxSlot::SharedPtr>::iterator it = mSlots.begin();
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

		ScrollBar->UpdateAlignments();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->SlotRemoved(this, Slot);

		pSlot = nullptr;

		return true;
	}

	bool cbScrollBox::RemoveSlot(const std::size_t SlotIndex)
	{
		if (SlotIndex < 0 || SlotIndex >= mSlots.size() || slotsize == 0)
			return false;

		cbSlot::SharedPtr Slot = mSlots[SlotIndex];
		auto Content = Slot->GetContent();
		Content->RemoveFromParent();

		ResetInput();

		return true;
	}

	std::size_t cbScrollBox::GetSlotIndex(const cbSlot* Slot) const
	{
		return std::distance(mSlots.begin(), std::find_if(mSlots.begin(), mSlots.end(), [&](const cbSlot::SharedPtr& Ptr)
			{
				return Ptr.get() == Slot;
			}));
	}

	bool cbScrollBox::SwapSlots(const std::size_t sourceIndex, const std::size_t destIndex)
	{
		if (sourceIndex >= slotsize || destIndex >= slotsize)
			return false;

		std::swap(mSlots[sourceIndex], mSlots[destIndex]);

		UpdateSlotAlignments();
		ResetInput();

		return true;
	}

	bool cbScrollBox::SwapSlotWithFront(const std::size_t index)
	{
		if ((index + 1) >= slotsize)
			return false;

		std::swap(mSlots[index], mSlots[index + 1]);

		UpdateSlotAlignments();
		ResetInput();

		return true;
	}

	bool cbScrollBox::SwapSlotWithBack(const std::size_t index)
	{
		if (index >= slotsize || index == 0)
			return false;

		std::swap(mSlots[index], mSlots[index - 1]);

		UpdateSlotAlignments();
		ResetInput();

		return true;
	}

	std::size_t cbScrollBox::GetNextSlotIndex() const
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

	cbSlot* cbScrollBox::GetNextSlot(const cbScrollBoxSlot* pSlot, const bool ExcludeHidden) const
	{
		cbSlot* Slot = nullptr;
		bool found = false;
		for (const auto& pSB_Slot : mSlots)
		{
			if (pSB_Slot.get() == pSlot)
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
				if (!pSB_Slot->IsHidden())
				{
					Slot = pSB_Slot.get();
					break;
				}
			}
			else
			{
				Slot = pSB_Slot.get();
				break;
			}
		}

		return Slot;
	}

	cbSlot* cbScrollBox::GetPreviousSlot(const cbScrollBoxSlot* pSlot, const bool ExcludeHidden) const
	{
		cbSlot* Slot = nullptr;
		for (const auto& pSB_Slot : mSlots)
		{
			if (pSB_Slot.get() == pSlot)
			{
				break;
			}
			if (ExcludeHidden)
			{
				if (!pSB_Slot->IsHidden())
					Slot = pSB_Slot.get();
			}
			else
			{
				Slot = pSB_Slot.get();
			}
		}

		return Slot;
	}

	std::size_t cbScrollBox::GetSlotSize(const bool ExcludeHidden) const
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

	std::vector<cbSlot*> cbScrollBox::GetSlots() const
	{
		std::vector<cbSlot*> Result;
		Result.reserve(mSlots.size());
		std::transform(mSlots.cbegin(), mSlots.cend(), std::back_inserter(Result), [](auto& ptr) { return ptr.get(); });
		return Result;
	}

	bool cbScrollBox::HasAnyChildren() const
	{
		return mSlots.size() > 0 || HasAnyComponents();
	}

	std::vector<cbWidgetObj*> cbScrollBox::GetAllChildren() const
	{
		std::vector<cbWidgetObj*> Children;

		const auto& Components = GetAllComponents();
		Children.reserve(mSlots.size() + Components.size());
		std::transform(Components.cbegin(), Components.cend(), std::back_inserter(Children), [](auto& ptr) { return ptr; });
		std::transform(mSlots.cbegin(), mSlots.cend(), std::back_inserter(Children), [](auto& ptr) { return ptr.get(); });

		return Children;
	}

	std::vector<cbGeometryVertexData> cbScrollBox::GetVertexData(const bool LineGeometry) const
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

	std::vector<std::uint32_t> cbScrollBox::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return std::vector<std::uint32_t>();
	};

	cbGeometryDrawData cbScrollBox::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("NONE", 0, 0, 0, 0);
	};

	bool cbScrollBox::HideSlot(const std::size_t Index, const bool value)
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
