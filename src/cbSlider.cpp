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
#include "cbSlider.h"
#include "cbCanvas.h"

namespace cbgui
{
	cbSlider::cbSliderHandleComponent::cbSliderHandleComponent(cbSlider* pOwner)
		: Super(pOwner)
		, Length(10.0f)
		, Thickness(6.0f)
		, ButtonState(eButtonState::Default)
		, Offset(0.0f)
		, bIsItFocused(false)
		, bFillMode(false)
		, Padding(0)
		, bFillThickness(false)
	{}

	cbSlider::cbSliderHandleComponent::cbSliderHandleComponent(const cbSliderHandleComponent& eComponent, cbSlider* NewOwner)
		: Super(NewOwner)
		, Length(eComponent.Length)
		, Thickness(eComponent.Thickness)
		, ButtonState(eComponent.ButtonState)
		, Offset(eComponent.Offset)
		, bIsItFocused(eComponent.bIsItFocused)
		, bFillMode(eComponent.bFillMode)
		, Padding(eComponent.Padding)
		, bFillThickness(eComponent.bFillThickness)
		, VertexColorStyle(eComponent.VertexColorStyle)
	{
		SetName(eComponent.GetName());
	}

	cbSlider::cbSliderHandleComponent::~cbSliderHandleComponent()
	{
	}

	void cbSlider::cbSliderHandleComponent::OnPressed()
	{
		SetButtonStatus(eButtonState::Pressed);
	}

	void cbSlider::cbSliderHandleComponent::OnReleased()
	{
		SetButtonStatus(eButtonState::Default);
	}

	void cbSlider::cbSliderHandleComponent::OnHovered()
	{
		if (IsPressed())
			return;
		SetButtonStatus(eButtonState::Hovered);
	}

	void cbSlider::cbSliderHandleComponent::OnUnhovered()
	{
		if (IsPressed())
			return;
		SetButtonStatus(eButtonState::Default);
	}

	void cbSlider::cbSliderHandleComponent::UpdateStatus()
	{
		SetFocus(false);
		SetButtonStatus(IsEnabled() ? eButtonState::Default : eButtonState::Disabled);
	}

	cbVector cbSlider::cbSliderHandleComponent::GetLocation() const
	{
		return GetOwner<cbSlider>()->GetOrientation() == eOrientation::Vertical ? GetOwner<cbSlider>()->GetLocation() + cbVector(0.0f, Offset)
			: GetOwner<cbSlider>()->GetLocation() + cbVector(Offset, 0.0f);
	}

	cbDimension cbSlider::cbSliderHandleComponent::GetDimension() const
	{
		return cbDimension(GetWidth(), GetHeight());
	}

	float cbSlider::cbSliderHandleComponent::GetWidth() const
	{
		return GetOwner<cbSlider>()->GetOrientation() == eOrientation::Vertical ? GetThickness() : GetLength();
	}

	float cbSlider::cbSliderHandleComponent::GetHeight() const
	{
		return GetOwner<cbSlider>()->GetOrientation() == eOrientation::Vertical ? GetLength() : GetThickness();
	}

	bool cbSlider::cbSliderHandleComponent::SetOffset(const float value)
	{
		if (Offset != value)
		{
			Offset = value;
			if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
				NotifyCanvas_WidgetUpdated();
			return true;
		}
		return false;
	}

	bool cbSlider::cbSliderHandleComponent::SetThickness(const float value)
	{
		if (Thickness != value)
		{
			Thickness = value;
			if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
				NotifyCanvas_WidgetUpdated();
			return true;
		}
		return false;
	}

	bool cbSlider::cbSliderHandleComponent::SetLength(const float value)
	{
		if (Length != value)
		{
			Length = value;
			if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
				NotifyCanvas_WidgetUpdated();
			return true;
		}
		return false;
	}

	void cbSlider::cbSliderHandleComponent::UpdateRotation()
	{
		if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	float cbgui::cbSlider::cbSliderHandleComponent::GetThickness() const
	{
		if (bFillThickness)
		{
			switch (GetOwner<cbSlider>()->GetOrientation())
			{
			case eOrientation::Vertical:
				return GetOwner()->GetWidth() - Padding.GetWidth();
				break;
			case eOrientation::Horizontal:
				return GetOwner()->GetHeight() - Padding.GetHeight();
				break;
			}
		}
		return Thickness;
	}

	void cbgui::cbSlider::cbSliderHandleComponent::SetFillMode(bool FillMode)
	{
		bFillMode = FillMode;
		UpdateVerticalAlignment();
	}

	void cbgui::cbSlider::cbSliderHandleComponent::SetFillThickness(const bool value)
	{
		bFillThickness = value;
		if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	bool cbSlider::cbSliderHandleComponent::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (!GetOwner<cbSlider>()->IsFocusable() || !IsEnabled())
			return false;

		if (IsItCulled())
			return false;

		if (!IsFocused())
			SetFocus(true);

		if (!IsHovered() && !IsPressed())
			OnHovered();

		return true;
	}

	bool cbSlider::cbSliderHandleComponent::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (IsPressed())
			GetOwner<cbSlider>()->Slide(cbgui::RotateVectorAroundPoint((Mouse.MouseLocation), GetOrigin(), GetRotation() * (-1.0f)));
		return true;
	}

	bool cbSlider::cbSliderHandleComponent::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;
		return false;
	}

	bool cbSlider::cbSliderHandleComponent::OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (!IsPressed())
			return false;

		OnReleased();
		if (IsInside(Mouse.MouseLocation))
			OnHovered();
		else
			SetFocus(false);

		return true;
	}

	bool cbSlider::cbSliderHandleComponent::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (!IsPressed())
			OnPressed();
		return true;
	}

	void cbSlider::cbSliderHandleComponent::ResetInput()
	{
		bIsItFocused = false;
		OnReleased();
	}

	bool cbSlider::cbSliderHandleComponent::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!IsEnabled())
			return false;

		if (IsPressed())
			return false;

		if (IsFocused())
			SetFocus(false);

		if (IsPressed())
			OnReleased();
		if (IsHovered())
			OnUnhovered();

		return true;
	}

	void cbSlider::cbSliderHandleComponent::Slide(const float Percent)
	{
		const auto Slider = GetOwner<cbSlider>();
		const auto OwnerBounds = Slider->GetBounds();

		switch (Slider->GetOrientation())
		{
		case eOrientation::Vertical:
		{
			if (bFillMode)
			{
				Length = (OwnerBounds.GetBottom() - OwnerBounds.GetTop() - Padding.GetHeight()) * Percent;
				SetOffset(OwnerBounds.GetTop() + (Padding.GetHeight() / 2.0f) + (Length / 2.0f) - OwnerBounds.GetCenter().Y);
			}
			else
			{
				SetOffset(OwnerBounds.GetTop() - (OwnerBounds.GetCenter().Y - (Padding.GetHeight() / 2.0f)) + (Length / 2.0f) + ((OwnerBounds.GetHeight() - (Padding.GetHeight())- Length) * Percent));
			}
		}
		break;
		case eOrientation::Horizontal:
		{
			if (bFillMode)
			{
				Length = (OwnerBounds.GetRight() - OwnerBounds.GetLeft() - Padding.GetWidth()) * Percent;
				SetOffset(OwnerBounds.GetLeft() + (Padding.GetWidth() / 2.0f) + (Length / 2.0f) - OwnerBounds.GetCenter().X);
			}
			else
			{
				SetOffset(OwnerBounds.GetLeft() - (OwnerBounds.GetCenter().X - (Padding.GetWidth() / 2.0f)) + (Length / 2.0f) + ((OwnerBounds.GetWidth() - (Padding.GetWidth()) - Length) * Percent));
			}
		}
		break;
		}
	}

	void cbSlider::cbSliderHandleComponent::UpdateVerticalAlignment()
	{
		const auto Owner = GetOwner<cbSlider>();

		switch (Owner->GetOrientation())
		{
		case eOrientation::Vertical:
			Slide(Owner->GetPercent());
			break;
		case eOrientation::Horizontal:
			break;
		}
		if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	void cbSlider::cbSliderHandleComponent::UpdateHorizontalAlignment()
	{
		const auto Owner = GetOwner<cbSlider>();

		switch (Owner->GetOrientation())
		{
		case eOrientation::Vertical:
			break;
		case eOrientation::Horizontal:
			Slide(Owner->GetPercent());
			break;
		}
		if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	std::vector<cbGeometryVertexData> cbSlider::cbSliderHandleComponent::GetVertexData(const bool LineGeometry) const
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

	std::vector<std::uint32_t> cbSlider::cbSliderHandleComponent::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return cbGeometryFactory::GeneratePlaneIndices();
	}

	cbGeometryDrawData cbSlider::cbSliderHandleComponent::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("Button", static_cast<std::underlying_type_t<eButtonState>>(ButtonState), 4, 6, 6);
	};

	void cbSlider::cbSliderHandleComponent::SetVertexColorStyle(const cbButtonVertexColorStyle& style)
	{
		VertexColorStyle = style;
		if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	void cbSlider::cbSliderHandleComponent::SetButtonStatus(const eButtonState& ButtonStatus)
	{
		ButtonState = ButtonStatus;
		if (GetOwner<cbSlider>()->bShouldNotifyCanvas)
			NotifyCanvas_WidgetUpdated();
	}

	cbSlider::cbSlider(eOrientation orientation)
		: cbWidget()
		, Transform(cbTransform(cbDimension(100.0f, 8.0f)))
		, bIntegerOnly(false)
		, Percent(0.0f)
		, Orientation(orientation)
		, Value(0.0f)
		, MinValue(0.0f)
		, MaxValue(100.0f)
		, StepSize(5.0f)
		, bUseStepOnly(false)
		, Handle(cbSliderHandleComponent::CreateUnique(this))
		, Thickness(2.5f)
		, bHorizontalFill(false)
		, bVerticalFill(false)
		, fOnValueChanged(nullptr)
	{
		ChangeOrientation(Orientation);
	}

	cbgui::cbSlider::cbSlider(const cbSlider& Other, cbSlot* NewOwner)
		: cbWidget(Other, NewOwner)
		, Transform(Other.Transform)
		, bIntegerOnly(Other.bIntegerOnly)
		, Percent(Other.Percent)
		, Orientation(Other.Orientation)
		, Value(Other.Value)
		, MinValue(Other.MinValue)
		, MaxValue(Other.MaxValue)
		, StepSize(Other.StepSize)
		, bUseStepOnly(Other.bUseStepOnly)
		, Handle(cbSliderHandleComponent::CreateUnique(*Other.Handle.get(), this))
		, Thickness(Other.Thickness)
		, bHorizontalFill(Other.bHorizontalFill)
		, bVerticalFill(bVerticalFill)
		, VertexColorStyle(Other.VertexColorStyle)
		, fOnValueChanged(nullptr)
	{
		ChangeOrientation(Other.GetOrientation());
	}

	cbSlider::~cbSlider()
	{
		fOnValueChanged = nullptr;
		Handle = nullptr;
	}

	cbWidget::SharedPtr cbSlider::CloneWidget(cbSlot* NewOwner)
	{
		return cbSlider::Create(*this);
	}

	void cbSlider::BeginPlay()
	{
		Handle->BeginPlay();
		OnBeginPlay();
	}

	void cbSlider::Tick(float InDeltaTime)
	{
		if (!IsEnabled())
			return;

		Handle->Tick(InDeltaTime); 
		OnTick(InDeltaTime);
	}

	float cbSlider::GetRotation() const
	{
		return HasOwner() ? GetOwner()->GetRotation() + Transform.GetRotation() : Transform.GetRotation();
	}

	void cbSlider::UpdateStatus()
	{
		SetFocus(false);
		Handle->UpdateStatus();
		NotifyCanvas_WidgetUpdated();
	}

	void cbSlider::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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
				Handle->UpdateAlignments();
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
				Handle->UpdateHorizontalAlignment();
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
				Handle->UpdateVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbSlider::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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
					Handle->UpdateAlignments();
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
					Handle->UpdateHorizontalAlignment();
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
					Handle->UpdateVerticalAlignment();
					NotifyCanvas_WidgetUpdated();
				}
			}
		}
	}

	void cbSlider::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbSlider::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			Handle->UpdateRotation();
			NotifyCanvas_WidgetUpdated();
		}
	}

	void cbSlider::SetBarLength(const float value)
	{
		switch (Orientation)
		{
		case eOrientation::Vertical:
		{
			if (Transform.SetHeight(value))
			{
				Handle->UpdateVerticalAlignment();
				NotifyCanvas_WidgetUpdated();

				if (HasOwner())
					DimensionUpdated();
				else if (IsAlignedToCanvas())
					AlignToCanvas();
			}
		}
		break;
		case eOrientation::Horizontal:
		{
			if (Transform.SetWidth(value))
			{
				Handle->UpdateHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();

				if (HasOwner())
					DimensionUpdated();
				else if (IsAlignedToCanvas())
					AlignToCanvas();
			}
		}
		break;
		}

		SetSliderValueByPercent(Percent);
	}

	void cbSlider::SetBarThickness(const float value)
	{
		if (Thickness == value)
			return;

		Thickness = value;
		NotifyCanvas_WidgetUpdated();
	}

	float cbSlider::GetBarLength() const
	{
		return Orientation == eOrientation::Vertical ? bVerticalFill ? GetHeight() : GetHeight() - Handle->GetLength()
			   : bHorizontalFill ? GetWidth() : GetWidth() - Handle->GetLength();
	}

	float cbSlider::GetBarThickness() const
	{
		return Orientation == eOrientation::Vertical ? bVerticalFill ? GetWidth() : Thickness 
			   : bHorizontalFill ? GetHeight() : Thickness;
	}

	void cbSlider::SetBarVerticalFill(const bool VerticalFill)
	{
		bVerticalFill = VerticalFill;
		NotifyCanvas_WidgetUpdated();
	}

	void cbSlider::SetBarHorizontalFill(const bool HorizontalFill)
	{
		bHorizontalFill = HorizontalFill;
		NotifyCanvas_WidgetUpdated();
	}

	void cbgui::cbSlider::SetHandleFillMode(const bool FillMode)
	{
		Handle->SetFillMode(FillMode);
		SetSliderValueByPercent(0.0f);
	}

	void cbgui::cbSlider::SetHandlePadding(const std::int32_t value)
	{
		Handle->SetPadding(value);
	}

	void cbSlider::UpdateRotation()
	{
		Handle->UpdateRotation();
		NotifyCanvas_WidgetUpdated();
	}

	void cbSlider::UpdateVerticalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerHeight = GetOwner()->GetHeight();
			const std::optional<const eVerticalAlignment> CustomAlignment = GetNonAlignedHeight() > OwnerHeight ? std::optional<eVerticalAlignment>(eVerticalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsHeightAligned())
				Transform.ResetHeightAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetVerticalAlignment(), GetOwner()->GetBounds(), GetVerticalAnchor()) || ForceAlign)
			{
				Handle->UpdateVerticalAlignment();
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
				Handle->UpdateVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}			
			else if (GetVerticalAlignment() == eVerticalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetY(CanvasOffset.Y);
				Handle->UpdateVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsHeightAligned())
			{
				Transform.ResetHeightAlignment();
				Handle->UpdateVerticalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else
			{
				Handle->UpdateVerticalAlignment();
			}
		}
	}

	void cbSlider::UpdateHorizontalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerWidth = GetOwner()->GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetHorizontalAlignment(), GetOwner()->GetBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
				Handle->UpdateHorizontalAlignment();
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
				Handle->UpdateHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetX(CanvasOffset.X);
				Handle->UpdateHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsWidthAligned())
			{
				Transform.ResetWidthAlignment();
				Handle->UpdateHorizontalAlignment();
				NotifyCanvas_WidgetUpdated();
			}
			else
			{
				Handle->UpdateHorizontalAlignment();
			}
		}
	}

	void cbSlider::ValueChanged(const float value)
	{
		const float OldValue = Value;
		Value = value;
		if (Value < MinValue)
			Value = MinValue;
		if (Value > MaxValue)
			Value = MaxValue;

		if (OldValue != Value)
		{
			if (fOnValueChanged)
			{
				fOnValueChanged(value);
			}
			else
			{
				OnValueChanged(Value);
			}
		}
	}

	std::vector<cbGeometryVertexData> cbSlider::GetVertexData(const bool LineGeometry) const
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
				cbDimension(GetBarThickness(), GetBarLength()) : cbDimension(GetBarLength(), GetBarThickness()));
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
														cbDimension(GetBarThickness(), GetBarLength()) : cbDimension(GetBarLength(), GetBarThickness())),
			cbGeometryFactory::GeneratePlaneTextureCoordinate(),
			IsEnabled() ? VertexColorStyle.GetColor() : VertexColorStyle.GetDisabledColor(),
			GetLocation(), GetRotation(), IsRotated() ? GetOrigin() : cbVector::Zero());
	}

	std::vector<std::uint32_t> cbSlider::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, };
		return cbGeometryFactory::GeneratePlaneIndices();
	}

	cbGeometryDrawData cbSlider::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 8, 16, 16);
		return cbGeometryDrawData("Plane", 0, 4, 6, 6);
	};

	void cbSlider::SetBarVertexColorStyle(const cbVertexColorStyle& style)
	{
		VertexColorStyle = style;
		NotifyCanvas_WidgetUpdated();
	}

	void cbSlider::SetHandleVertexColorStyle(const cbButtonVertexColorStyle& style)
	{
		Handle->SetVertexColorStyle(style);
	}

	void cbgui::cbSlider::SetHandleFillThickness(const bool value)
	{
		Handle->SetFillThickness(value);
	}

	void cbSlider::SetHandleThickness(const float value)
	{
		Handle->SetThickness(Orientation == eOrientation::Vertical ? value > GetWidth() ? GetWidth() : value
			: value > GetHeight() ? GetHeight() : value);
	}

	void cbSlider::SetHandleLength(const float value)
	{
		Handle->SetLength(value);
		SetSliderValueByPercent(Percent);
	}

	void cbSlider::Slide(const cbVector& MouseLocation)
	{
		if (Orientation == eOrientation::Horizontal)
			SetSliderValueByPercent(Handle->IsFillModeEnabled() ? (MouseLocation.X - GetBounds().GetLeft() + (Handle->GetPadding().GetWidth() / 2.0f)) / (GetWidth() - Handle->GetPadding().GetWidth())
				: (MouseLocation.X - GetBounds().GetLeft() - (Handle->GetPadding().GetWidth() / 2.0f) - (Handle->GetLength() / 2.0f)) / (GetWidth() - Handle->GetPadding().GetWidth() - Handle->GetLength()));
		else if (Orientation == eOrientation::Vertical)
			SetSliderValueByPercent(Handle->IsFillModeEnabled() ? (MouseLocation.Y - GetBounds().GetTop() + (Handle->GetPadding().GetHeight() / 2.0f)) / (GetHeight() - Handle->GetPadding().GetHeight())
				: (MouseLocation.Y - GetBounds().GetTop() - (Handle->GetPadding().GetHeight() / 2.0f) - (Handle->GetLength() / 2.0f)) / (GetHeight() - Handle->GetPadding().GetHeight() - Handle->GetLength()));
	}

	void cbSlider::SetSliderValue(float value)
	{
		const float val = value < MinValue ? MinValue : value > MaxValue ? MaxValue : value;

		SetSliderValueByPercent((val - MinValue) / (MaxValue - MinValue));
	}

	void cbSlider::SetSliderValueByPercent(float percent)
	{
		float TempPercent = (percent / 1.0f) < 0.0f ? 0.0f : (percent / 1.0f) > 1.0f ? 1.0f : (percent / 1.0f);

		float Result = cbgui::Lerp(MinValue, MaxValue, TempPercent);

		if (bUseStepOnly)
		{
			const float StepPercent = StepSize >= (MaxValue - MinValue) ? 1.0f : StepSize / (MaxValue - MinValue);
			TempPercent = std::round((TempPercent) / StepPercent) * StepPercent;
			Result = cbgui::Lerp(MinValue, MaxValue, TempPercent);
		}

		if (bIntegerOnly)
		{
			float val = std::round(Result);
			if (val != Result)
			{
				val = val < MinValue ? MinValue : val > MaxValue ? MaxValue : val;

				TempPercent = (val - MinValue) / (MaxValue - MinValue);
				TempPercent = (TempPercent / 1.0f) < 0.0f ? 0.0f : (TempPercent / 1.0f) > 1.0f ? 1.0f : (TempPercent / 1.0f);
				Result = cbgui::Lerp(MinValue, MaxValue, TempPercent);
			}
		}

		Percent = TempPercent;

		Handle->Slide(Percent);

		if (Result <= 0.00009999f)
			Result = std::round(Result);

		if (Result != Value)
			ValueChanged(Result);
	}

	void cbSlider::Slide(float WheelDelta)
	{
		if (!IsEnabled())
			return;

		if (WheelDelta == 0.0f)
			return;

		float Result = Value + (StepSize * WheelDelta);
		SetSliderValue(Result);
	}

	void cbSlider::SetMinimumValue(const float value)
	{
		MinValue = value;
		if (Value < MinValue)
			Value = MinValue;
		if (Value > MaxValue)
			Value = MaxValue;
		SetSliderValueByPercent(Percent);
	}

	void cbSlider::SetMaximumValue(const float value)
	{
		MaxValue = value;
		SetSliderValueByPercent(Percent);
	}

	void cbSlider::ChangeOrientation(const eOrientation& orientation)
	{
		if (Orientation == orientation)
			return;

		Orientation = orientation;

		Transform.SetDimension(cbDimension(GetHeight(), GetWidth()));

		NotifyCanvas_WidgetUpdated();

		UpdateAlignments(true);
	}

	void cbSlider::OnAttach()
	{
	}

	void cbSlider::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();

			Handle->UpdateAlignments();
			NotifyCanvas_WidgetUpdated();
		}
	}

	bool cbSlider::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (!IsEnabled())
			return false;

		if (!IsInside(Mouse.MouseLocation))
			return false;

		if (IsItCulled())
			return false;

		if (!IsFocused())
			SetFocus(true);

		return true;
	}

	bool cbSlider::OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		Slide(WheelDelta);

		if (!Handle->IsInside(Mouse.MouseLocation))
		{
			Handle->OnMouseLeave(Mouse);
		}

		return true;
	}

	void cbSlider::ResetInput()
	{
		SetFocus(false);
		Handle->ResetInput();
	}

	bool cbSlider::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (Handle->IsFocused() && Handle->IsPressed())
		{
			Handle->OnMouseMove(Mouse);
			return true;
		}

		if (Handle->IsInside(Mouse.MouseLocation))
		{
			Handle->OnMouseEnter(Mouse);
		}
		else if (IsInside(Mouse.MouseLocation))
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

	bool cbSlider::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (Handle->IsInside(Mouse.MouseLocation) && !IsHandleFillModeEnabled())
		{
			if (Handle->IsFocused())
			{
				Handle->OnMouseButtonDown(Mouse);
			}
		}
		else if (IsInside(Mouse.MouseLocation))
		{
			Slide(cbgui::RotateVectorAroundPoint((Mouse.MouseLocation), GetOrigin(), GetRotation() * (-1.0f)));
			if (Handle->IsInside(Mouse.MouseLocation))
			{
				if (Handle->OnMouseEnter(Mouse))
				{
					if (Handle->IsFocused())
						Handle->OnMouseButtonDown(Mouse);
				}
			}
		}

		return false;
	}

	bool cbSlider::OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (Handle->IsPressed())
			Handle->OnMouseButtonUp(Mouse);

		if (!IsInside(Mouse.MouseLocation))
			OnMouseLeave(Mouse);

		return true;
	}

	bool cbSlider::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (Handle->IsInside(Mouse.MouseLocation) && !IsHandleFillModeEnabled())
		{
			if (Handle->IsFocused())
			{
				Handle->OnMouseButtonDown(Mouse);
			}
		}
		else if (IsInside(Mouse.MouseLocation))
		{
			Slide(cbgui::RotateVectorAroundPoint((Mouse.MouseLocation), GetOrigin(), GetRotation() * (-1.0f)));
			if (Handle->IsInside(Mouse.MouseLocation))
			{
				if (Handle->OnMouseEnter(Mouse))
				{
					if (Handle->IsFocused())
						Handle->OnMouseButtonDown(Mouse);
				}
			}
		}

		return true;
	}

	bool cbSlider::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!IsFocusable())
			return false;

		if (Handle->IsPressed())
			return false;

		if (IsFocused())
			SetFocus(false);

		if (Handle->IsFocused())
			Handle->OnMouseLeave(Mouse);

		return true;
	}
}
