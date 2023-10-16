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
#include "cbButton.h"
#include "cbCanvas.h"

namespace cbgui
{
	cbButton::cbButton()
		: Super()
		, Transform(cbTransform(cbDimension(100.0f, 40.0f)))
		, ButtonState(eButtonState::Default)
		, fOnClicked(nullptr)
		, fOnPressed(nullptr)
		, fOnReleased(nullptr)
		, fOnHovered(nullptr)
		, fOnUnhovered(nullptr)
	{}

	cbButton::cbButton(const cbButton& Other, cbSlot* NewOwner)
		: Super(Other, NewOwner)
		, Transform(Other.Transform)
		, ButtonState(Other.ButtonState)
		, VertexColorStyle(Other.VertexColorStyle)
		, fOnClicked(nullptr)
		, fOnPressed(nullptr)
		, fOnReleased(nullptr)
		, fOnHovered(nullptr)
		, fOnUnhovered(nullptr)
	{}

	cbButton::~cbButton()
	{
		fOnClicked = nullptr;
		fOnPressed = nullptr;
		fOnReleased = nullptr;
		fOnHovered = nullptr;
		fOnUnhovered = nullptr;
	}

	cbWidget::SharedPtr cbButton::CloneWidget(cbSlot* NewOwner)
	{
		return cbButton::Create(*this, NewOwner);
	}

	void cbButton::BeginPlay()
	{
		OnBeginPlay();
	}

	void cbButton::Tick(float InDeltaTime)
	{
		if (!IsEnabled())
			return;

		OnTick(InDeltaTime);
	}

	float cbButton::GetRotation() const
	{
		return HasOwner() ? GetOwner()->GetRotation() + Transform.GetRotation() : Transform.GetRotation();
	}

	std::vector<cbGeometryVertexData> cbButton::GetVertexData(const bool LineGeometry) const
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
				GetLocation(), GetRotation(), IsRotated() ? GetOrigin() : cbVector::Zero());
		}

		return cbGeometryFactory::GetAlignedVertexData(cbGeometryFactory::Create4DPlaneVerticesFromRect(GetDimension()),
			   cbGeometryFactory::GeneratePlaneTextureCoordinate(),
			   IsEnabled() ? VertexColorStyle.GetColor(ButtonState) : VertexColorStyle.GetDisabledColor(),
			   GetLocation(), GetRotation(), IsRotated() ? GetOrigin() : cbVector::Zero());
	}

	std::vector<std::uint32_t> cbButton::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return cbGeometryFactory::GeneratePlaneIndices();
	}

	cbGeometryDrawData cbButton::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("Button", static_cast<std::underlying_type_t<eButtonState>>(ButtonState), 4, 6, 6); 
	};

	void cbButton::SetVertexColorStyle(const cbButtonVertexColorStyle& style)
	{
		VertexColorStyle = style;
		NotifyCanvas_WidgetUpdated();
	}

	void cbButton::SetVertexColorStyle(const eButtonState State, const cbColor& Color)
	{
		VertexColorStyle.SetColor(State, Color);
		NotifyCanvas_WidgetUpdated();
	}

	eButtonState cbButton::GetState() const
	{
		return ButtonState;
	}

	void cbButton::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbButton::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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
					NotifyCanvas_WidgetUpdated();
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
					NotifyCanvas_WidgetUpdated();
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
					NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbButton::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbButton::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			NotifyCanvas_WidgetUpdated();
		}
	}

	void cbButton::SetButtonStatus(const eButtonState& ButtonStatus)
	{
		if (ButtonState == ButtonStatus)
			return;

		ButtonState = ButtonStatus;
		NotifyCanvas_WidgetUpdated();
	}

	bool cbButton::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (!IsFocusable() || !IsEnabled())
			return false;

		if (!IsInside(Mouse.MouseLocation))
			return false;

		if (IsItCulled())
			return false;

		if (!IsFocused())
			SetFocus(true);

		if (!IsHovered() && !IsPressed())
			Hovered();

		return true;
	}

	bool cbButton::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		return true;
	}

	bool cbButton::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (IsInside(Mouse.MouseLocation))
		{
			Pressed();
			Released();
			return true;
		}

		return false;
	}

	bool cbButton::OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (!IsPressed())
			return false;

		if (IsInside(Mouse.MouseLocation))
		{
			Released();
			Hovered();
			return true;
		}
		else
		{
			Reset();
			SetFocus(false);
		}

		return true;
	}

	bool cbButton::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (!IsPressed())
			Pressed();

		return true;
	}

	bool cbButton::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (IsPressed())
			return false;

		if (IsFocused())
			SetFocus(false);

		if (IsHovered())
			Unhovered();

		return true;
	}

	void cbButton::ResetInput()
	{
		SetFocus(false);
		Reset();
	}

	void cbButton::Reset()
	{
		if (!IsEnabled() && (!IsPressed() && !IsHovered()))
			return;

		if (IsPressed())
		{
			SetButtonStatus(eButtonState::Default);
			OnReleased();
		}

		if (IsHovered())
			Unhovered();
	}

	bool cbButton::Clicked()
	{
		if (!IsEnabled())
			return false;
		if (fOnClicked)
		{
			fOnClicked();
			return true;
		}
		OnClicked();
		return true;
	}

	bool cbButton::Pressed()
	{
		if (!IsEnabled() || IsPressed())
			return false;
		if (IsHovered())
			Unhovered();
		SetButtonStatus(eButtonState::Pressed);
		if (fOnPressed)
		{
			fOnPressed();
			return true;
		}
		OnPressed();
		return true;
	}

	bool cbButton::Released()
	{
		if (!IsEnabled() || !IsPressed())
			return false;
		SetButtonStatus(eButtonState::Default);
		if (fOnReleased)
		{
			fOnReleased();
			return true;
		}
		OnReleased();
		Clicked();
		return true;
	}

	bool cbButton::Hovered()
	{
		if (!IsEnabled() || IsPressed() || IsHovered())
			return false;
		SetButtonStatus(eButtonState::Hovered);
		if (fOnHovered)
		{
			fOnHovered();
			return true;
		}
		OnHovered();
		return true;
	}

	bool cbButton::Unhovered()
	{
		if (!IsEnabled() || IsPressed() || !IsHovered())
			return false;
		SetButtonStatus(eButtonState::Default);
		if (fOnUnhovered)
		{
			fOnUnhovered();
			return true;
		}
		OnUnhovered();
		return true;
	}

	void cbButton::UpdateVerticalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerHeight = GetOwner()->GetHeight();
			const std::optional<const eVerticalAlignment> CustomAlignment = GetNonAlignedHeight() > OwnerHeight ? std::optional<eVerticalAlignment>(eVerticalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsHeightAligned())
				Transform.ResetHeightAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetVerticalAlignment(), GetOwner()->GetBounds(), GetVerticalAnchor()) || ForceAlign)
			{
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
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetVerticalAlignment() == eVerticalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetY(CanvasOffset.Y);
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsHeightAligned())
			{
				Transform.ResetHeightAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbButton::UpdateHorizontalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerWidth = GetOwner()->GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetHorizontalAlignment(), GetOwner()->GetBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
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
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetX(CanvasOffset.X);
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsWidthAligned())
			{
				Transform.ResetWidthAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbButton::UpdateRotation()
	{
		NotifyCanvas_WidgetUpdated();
	}

	void cbButton::UpdateStatus()
	{
		SetFocus(false);
		SetButtonStatus(IsEnabled() ? eButtonState::Default : eButtonState::Disabled);
	}

	void cbButton::OnAttach()
	{
	}

	void cbButton::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();
			NotifyCanvas_WidgetUpdated();
		}
	}
}
