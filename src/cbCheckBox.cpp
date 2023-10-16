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
#include "cbCheckBox.h"
#include "cbCanvas.h"

namespace cbgui
{
	cbCheckBox::cbCheckBox()
		: cbWidget()
		, Transform(cbTransform(cbDimension(16.0f, 16.0f)))
		, fOnCheckStateChanged(nullptr)
		, CheckBoxState(eCheckBoxState::Unchecked)
		, bIsPressed(false)
		, bIsHovered(false)
	{}

	cbCheckBox::cbCheckBox(const cbCheckBox& Other, cbSlot* NewOwner)
		: Super(Other, NewOwner)
		, Transform(Other.Transform)
		, fOnCheckStateChanged(nullptr)
		, CheckBoxState(Other.CheckBoxState)
		, bIsPressed(Other.bIsPressed)
		, bIsHovered(Other.bIsHovered)
		, VertexColorStyle(Other.VertexColorStyle)
	{}

	cbCheckBox::~cbCheckBox()
	{
		fOnCheckStateChanged = nullptr;
	}

	cbWidget::SharedPtr cbCheckBox::CloneWidget(cbSlot* NewOwner)
	{
		return cbCheckBox::Create(*this, NewOwner);
	}

	void cbCheckBox::BeginPlay()
	{
		OnBeginPlay();
	}

	void cbCheckBox::Tick(float DeltaTime)
	{
		if (!IsEnabled())
			return;

		OnTick(DeltaTime);
	}

	float cbCheckBox::GetRotation() const
	{
		return HasOwner() ? GetOwner()->GetRotation() + Transform.GetRotation() : Transform.GetRotation();
	}

	void cbCheckBox::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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

	void cbCheckBox::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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

	void cbCheckBox::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbCheckBox::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			NotifyCanvas_WidgetUpdated();
		}
	}

	void cbCheckBox::SetCheckBoxState(const eCheckBoxState& InCheckBoxState)
	{
		if (CheckBoxState == InCheckBoxState)
			return;

		CheckBoxState = InCheckBoxState;

		NotifyCanvas_WidgetUpdated();

		if (fOnCheckStateChanged)
		{
			fOnCheckStateChanged(CheckBoxState);
			return;
		}
		
		OnCheckBoxStateChanged(CheckBoxState);
	}

	eCheckBoxState cbCheckBox::GetState() const
	{
		return CheckBoxState;
	}

	void cbCheckBox::UpdateVerticalAlignment(const bool ForceAlign)
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

	void cbCheckBox::UpdateHorizontalAlignment(const bool ForceAlign)
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

	void cbCheckBox::UpdateRotation()
	{
		NotifyCanvas_WidgetUpdated();
	}

	bool cbCheckBox::OnMouseEnter(const cbMouseInput& Mouse)
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

	bool cbCheckBox::OnMouseMove(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		return true;
	}

	bool cbCheckBox::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (IsInside(Mouse.MouseLocation))
		{
			if (IsHovered())
				Unhovered();

			Pressed();
			Clicked();
			Released();

			Pressed();
			Clicked();
			Released();

			if (!IsHovered())
				Hovered();

			return true;
		}

		return false;
	}

	bool cbCheckBox::OnMouseButtonUp(const cbMouseInput& Mouse)
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
			ResetInput();
		}

		return true;
	}

	bool cbCheckBox::OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		if (!IsFocused() || !IsEnabled())
			return false;

		if (!IsPressed())
			Pressed();

		return true;
	}

	bool cbCheckBox::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (IsPressed())
			return false;

		if (IsFocused())
			SetFocus(false);

		if (IsPressed())
			Released();
		else if (IsHovered())
			Unhovered();

		return true;
	}

	void cbCheckBox::ResetInput()
	{
		SetFocus(false);

		if (!IsEnabled() && (!bIsPressed || !bIsHovered))
			return;

		if (IsPressed())
		{
			bIsPressed = false;

			if (CheckBoxState == eCheckBoxState::UndeterminedPressed)
				SetCheckBoxState(eCheckBoxState::Undetermined);
			else if (CheckBoxState == eCheckBoxState::UncheckedPressed)
				SetCheckBoxState(eCheckBoxState::Unchecked);
			else if (CheckBoxState == eCheckBoxState::CheckedPressed)
				SetCheckBoxState(eCheckBoxState::Checked);
		}

		if (IsHovered())
			Unhovered();
	}

	bool cbCheckBox::Clicked()
	{
		if (!IsEnabled())
			return false;

		ToggleCheckBoxState();

		return true;
	}

	bool cbCheckBox::Pressed()
	{
		if (!IsEnabled() && bIsPressed)
			return false;

		bIsPressed = true;

		if (IsHovered())
			Unhovered();

		if (CheckBoxState == eCheckBoxState::Undetermined || CheckBoxState == eCheckBoxState::UndeterminedHovered)
			SetCheckBoxState(eCheckBoxState::UndeterminedPressed);
		else if (CheckBoxState == eCheckBoxState::Unchecked || CheckBoxState == eCheckBoxState::UncheckedHovered)
			SetCheckBoxState(eCheckBoxState::UncheckedPressed);
		else if (CheckBoxState == eCheckBoxState::Checked || CheckBoxState == eCheckBoxState::CheckedHovered)
			SetCheckBoxState(eCheckBoxState::CheckedPressed);

		return true;
	}

	bool cbCheckBox::Released()
	{
		if (!IsEnabled() && !bIsPressed)
			return false;

		bIsPressed = false;

		if (CheckBoxState == eCheckBoxState::UndeterminedPressed)
			SetCheckBoxState(eCheckBoxState::Undetermined);
		else if (CheckBoxState == eCheckBoxState::UncheckedPressed)
			SetCheckBoxState(eCheckBoxState::Unchecked);
		else if (CheckBoxState == eCheckBoxState::CheckedPressed)
			SetCheckBoxState(eCheckBoxState::Checked);

		Clicked();

		return true;
	}

	bool cbCheckBox::Hovered()
	{
		if (!IsEnabled() || bIsPressed || bIsHovered)
			return false;
		bIsHovered = true;

		if (CheckBoxState == eCheckBoxState::Undetermined)
			SetCheckBoxState(eCheckBoxState::UndeterminedHovered);
		else if (CheckBoxState == eCheckBoxState::Unchecked)
			SetCheckBoxState(eCheckBoxState::UncheckedHovered);
		else if (CheckBoxState == eCheckBoxState::Checked)
			SetCheckBoxState(eCheckBoxState::CheckedHovered);

		return true;
	}

	bool cbCheckBox::Unhovered()
	{
		if (!IsEnabled() || bIsPressed || !bIsHovered)
			return false;
		bIsHovered = false;

		if (CheckBoxState == eCheckBoxState::UndeterminedHovered)
			SetCheckBoxState(eCheckBoxState::Undetermined);
		else if (CheckBoxState == eCheckBoxState::UncheckedHovered)
			SetCheckBoxState(eCheckBoxState::Unchecked);
		else if (CheckBoxState == eCheckBoxState::CheckedHovered)
			SetCheckBoxState(eCheckBoxState::Checked);

		return true;
	}

	void cbCheckBox::ToggleCheckBoxState()
	{
		if (CheckBoxState == eCheckBoxState::Undetermined || CheckBoxState == eCheckBoxState::UndeterminedPressed || CheckBoxState == eCheckBoxState::UndeterminedHovered)
			SetCheckBoxState(eCheckBoxState::Unchecked);
		else if (CheckBoxState == eCheckBoxState::Unchecked || CheckBoxState == eCheckBoxState::UncheckedPressed || CheckBoxState == eCheckBoxState::UncheckedHovered)
			SetCheckBoxState(eCheckBoxState::Checked);
		else if (CheckBoxState == eCheckBoxState::Checked || CheckBoxState == eCheckBoxState::CheckedPressed || CheckBoxState == eCheckBoxState::CheckedHovered)
			SetCheckBoxState(eCheckBoxState::Unchecked);
	}

	bool cbCheckBox::IsChecked() const
	{
		if (CheckBoxState == eCheckBoxState::Checked || CheckBoxState == eCheckBoxState::CheckedPressed || CheckBoxState == eCheckBoxState::CheckedHovered)
			return true;
		return false;
	}

	bool cbCheckBox::IsUndetermined() const
	{
		if (CheckBoxState == eCheckBoxState::Undetermined || CheckBoxState == eCheckBoxState::UndeterminedPressed || CheckBoxState == eCheckBoxState::UndeterminedHovered)
			return true;
		return false;
	}

	void cbCheckBox::SetStateToChecked()
	{
		SetCheckBoxState(eCheckBoxState::Checked);
	}

	void cbCheckBox::SetStateToUnchecked()
	{
		SetCheckBoxState(eCheckBoxState::Unchecked);
	}

	void cbCheckBox::SetStateToUndetermined()
	{
		SetCheckBoxState(eCheckBoxState::Undetermined);
	}

	void cbCheckBox::UpdateStatus()
	{
		SetFocus(false);
		if (IsEnabled())
		{
			const auto CheckBoxState = GetState();
			if (CheckBoxState == eCheckBoxState::UndeterminedDisabled)
				SetCheckBoxState(eCheckBoxState::Undetermined);
			else if (CheckBoxState == eCheckBoxState::UncheckedDisabled)
				SetCheckBoxState(eCheckBoxState::Unchecked);
			else if (CheckBoxState == eCheckBoxState::CheckedDisabled)
				SetCheckBoxState(eCheckBoxState::Checked);
		}
		else
		{
			const auto CheckBoxState = GetState();
			if (CheckBoxState == eCheckBoxState::Undetermined || CheckBoxState == eCheckBoxState::UndeterminedHovered)
				SetCheckBoxState(eCheckBoxState::UndeterminedDisabled);
			else if (CheckBoxState == eCheckBoxState::Unchecked || CheckBoxState == eCheckBoxState::UncheckedHovered)
				SetCheckBoxState(eCheckBoxState::UncheckedDisabled);
			else if (CheckBoxState == eCheckBoxState::Checked || CheckBoxState == eCheckBoxState::CheckedHovered)
				SetCheckBoxState(eCheckBoxState::CheckedDisabled);
		}
	}

	void cbCheckBox::OnAttach()
	{
	}

	void cbCheckBox::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();
			NotifyCanvas_WidgetUpdated();
		}
	}

	std::vector<cbGeometryVertexData> cbCheckBox::GetVertexData(const bool LineGeometry) const
	{
		float Rotation = GetRotation();
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
				IsEnabled() ? VertexColorStyle.GetColor(CheckBoxState) : VertexColorStyle.GetDisabledColor(CheckBoxState),
				GetLocation(), Rotation, Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero());
		}

		return cbGeometryFactory::GetAlignedVertexData(cbGeometryFactory::Create4DPlaneVerticesFromRect(GetDimension()),
			cbGeometryFactory::GeneratePlaneTextureCoordinate(),
			IsEnabled() ? VertexColorStyle.GetColor(CheckBoxState) : VertexColorStyle.GetDisabledColor(CheckBoxState),
			GetLocation(), Rotation, Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero());
	}

	std::vector<std::uint32_t> cbCheckBox::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return cbGeometryFactory::GeneratePlaneIndices();
	}

	cbGeometryDrawData cbCheckBox::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("CheckBox", static_cast<std::underlying_type_t<eCheckBoxState>>(CheckBoxState), 4, 6, 6);
	};

	void cbCheckBox::SetVertexColorStyle(const cbCheckBoxVertexColorStyle& style)
	{
		VertexColorStyle = style;
		NotifyCanvas_WidgetUpdated();
	}

	void cbgui::cbCheckBox::SetVertexColorStyle(const eCheckBoxState State, const cbColor& Color)
	{
		VertexColorStyle.SetColor(State, Color);
		NotifyCanvas_WidgetUpdated();
	}
}
