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
#include "cbBorder.h"
#include "cbCanvas.h"

namespace cbgui
{
	void cbBorder::cbBorderSlot::ReplaceContent(const cbWidget::SharedPtr& pContent)
	{
		if (!pContent)
			return;

		if (Content)
			Content = nullptr;
		Content = pContent;
		Content->AttachToSlot(this);
		OnContentInsertedOrReplaced();
	}

	cbBorder::cbBorder()
		: Super()
		, Transform(cbTransform(cbDimension(100.0f, 40.0f)))
		, BorderThickness(cbMargin(5))
		, Slot(nullptr)
	{}

	cbBorder::cbBorder(const cbBorder& Widget, cbSlot* NewOwner)
		: Super(Widget, NewOwner)
		, Transform(Widget.Transform)
		, BorderThickness(Widget.BorderThickness)
		, Slot(nullptr)
		, VertexColorStyle(Widget.VertexColorStyle)
	{
		if (Widget.Slot)
		{
			SetSlot(Widget.Slot->Clone<cbBorderSlot>(this));
		}
	}

	cbBorder::~cbBorder()
	{
		Slot = nullptr;
	}

	cbWidget::SharedPtr cbBorder::CloneWidget(cbSlot* NewOwner)
	{
		cbBorder::SharedPtr Border = cbBorder::Create(*this, NewOwner);
		//Border->SetSlot(Slot->Clone<cbBorderSlot>(Border.get()));
		return Border;
	}

	float cbBorder::GetRotation() const
	{
		return HasOwner() ? GetOwner()->GetRotation() + Transform.GetRotation() : Transform.GetRotation();
	}

	void cbBorder::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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

	void cbBorder::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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

	void cbBorder::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			if (Slot)
				Slot->UpdateRotation();

			NotifyCanvas_WidgetUpdated();
		}
	}

	void cbBorder::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbBorder::SetThickness(const cbMargin& Thickness)
	{
		BorderThickness = Thickness;
		UpdateSlotAlignments();
		NotifyCanvas_WidgetUpdated();
	}

	void cbBorder::SetThickness(const std::int32_t Thickness)
	{
		BorderThickness = cbMargin(Thickness);
		UpdateSlotAlignments();
		NotifyCanvas_WidgetUpdated();
	}

	cbDimension cbBorder::GetSlotDimension() const
	{
		return cbDimension(GetSlotWidth(), GetSlotHeight());
	}

	float cbBorder::GetSlotWidth() const
	{
		return (GetWidth() - BorderThickness.Left - BorderThickness.Right);
	}

	float cbBorder::GetSlotHeight() const
	{
		return (GetHeight() - BorderThickness.Top - BorderThickness.Bottom);
	}

	void cbBorder::HideContent(bool value)
	{
		if (!Slot)
			return;

		Slot->Hidden(value);
	}

	void cbBorder::UpdateStatus()
	{
		if (!Slot)
			return;

		SetFocus(false);
		Slot->UpdateStatus();

		NotifyCanvas_WidgetUpdated();
	}

	std::vector<cbWidgetObj*> cbBorder::GetAllChildren() const
	{
		if (Slot)
			return std::vector<cbWidgetObj*>{ Slot.get() };
		return std::vector<cbWidgetObj*>{ };
	}

	std::vector<cbGeometryVertexData> cbBorder::GetVertexData(const bool LineGeometry) const
	{
		if (LineGeometry)
		{
			cbBounds Bounds(GetDimension());
			std::vector<cbVector4> Data;
			Data.push_back(cbVector4(Bounds.GetCorner(0), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(1), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(2), 0.0f, 1.0f));
			Data.push_back(cbVector4(Bounds.GetCorner(3), 0.0f, 1.0f));

			cbBounds BarBounds(GetSlotDimension());
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

		return cbGeometryFactory::GetAlignedVertexData(cbGeometryFactory::GenerateBorderVertices(Transform.GetDimension(), BorderThickness), 
			   cbGeometryFactory::GenerateBorderTextureCoordinate(), 
			   IsEnabled() ? VertexColorStyle.Color : VertexColorStyle.GetDisabledColor(),
			   GetLocation(), GetRotation(), IsRotated() ? GetOrigin() : cbVector::Zero());
	}

	std::vector<std::uint32_t> cbBorder::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, };
		return cbGeometryFactory::GenerateBorderIndices();
	}

	cbGeometryDrawData cbBorder::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 8, 16, 16);
		return cbGeometryDrawData("Border", 0, 40, 48, 48);
	};

	void cbBorder::SetVertexColorStyle(const cbVertexColorStyle& style)
	{
		VertexColorStyle = style;
		NotifyCanvas_WidgetUpdated();
	}

	void cbBorder::OnSlotVisibilityChanged(cbSlot* Slot)
	{
	}

	void cbBorder::OnSlotDimensionUpdated(cbSlot* Slot)
	{
	}

	void cbBorder::RemoveContent()
	{
		if (!Slot)
			return;

		ResetInput();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->SlotRemoved(this, Slot.get());
		Slot = nullptr;
	}

	bool cbBorder::OnRemoveSlot(cbSlot* pSlot)
	{
		if (!Slot)
			return false;

		ResetInput();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->SlotRemoved(this, Slot.get());
		Slot = nullptr;

		return true;
	}

	void cbBorder::SetContent(const cbWidget::SharedPtr& Content)
	{
		if (!Content)
			return;

		if (!Slot)
		{
			SetSlot(cbBorderSlot::Create(this, Content));
		}
		else
		{
			cbWidget::SharedPtr Old = Slot->GetSharedContent();
			Slot->ReplaceContent(Content);

			ResetInput();

			if (cbICanvas* Canvas = GetCanvas())
				Canvas->SlotContentReplaced(Slot.get(), Old.get(), Content.get());
		}
	}

	void cbBorder::SetSlot(const cbBorderSlot::SharedPtr& pSlot)
	{
		if (!pSlot)
			return;

		Slot = nullptr;
		Slot = pSlot;
		Slot->Inserted();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->NewSlotAdded(this, Slot.get());

		UpdateSlotAlignments();
		if (IsItWrapped())
			Wrap();

		Slot->UpdateRotation();
		Slot->UpdateStatus();
	}

	std::size_t cbBorder::GetSlotSize(const bool ExcludeHidden) const
	{
		if (Slot)
		{
			if (ExcludeHidden && Slot->IsHidden())
				return 0;
			return 1;
		}
		return 0;
	}

	void cbBorder::UpdateSlotVerticalAlignment()
	{
		if (Slot)
			Slot->UpdateVerticalAlignment();
	}

	void cbBorder::UpdateSlotHorizontalAlignment()
	{
		if (Slot)
			Slot->UpdateHorizontalAlignment();
	}

	void cbBorder::UpdateVerticalAlignment(const bool ForceAlign)
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

	void cbBorder::UpdateHorizontalAlignment(const bool ForceAlign)
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

	void cbBorder::UpdateRotation()
	{
		if (Slot)
			Slot->UpdateRotation();

		NotifyCanvas_WidgetUpdated();
	}

	bool cbBorder::WrapVertical()
	{
		if (!IsItWrapped())
			return false;

		if (!Slot ? true : !Slot->HasContent() || Slot->IsHidden())
		{
			if (Transform.CompressHeight(1.0f))
			{
				return true;
			}
			return false;
		}

		float Height = 0.0;
		{
			const auto& Content = Slot->GetContent();
			const float SlotHeight = Content->GetNonAlignedHeight();
			const cbMargin& Padding = Content->GetPadding();
			if ((SlotHeight + (Padding.Top + Padding.Bottom)) > Height)
				Height += SlotHeight + (Padding.Top + Padding.Bottom);
		}

		if (Transform.CompressHeight(Height + (BorderThickness.Top + BorderThickness.Bottom)))
		{
			return true;
		}
		return false;
	}

	bool cbBorder::WrapHorizontal()
	{
		if (!IsItWrapped())
			return false;

		if (!Slot ? true : !Slot->HasContent() || Slot->IsHidden())
		{
			if (Transform.CompressWidth(1.0f))
			{
				return true;
			}
			return false;
		}

		float Width = 0.0f;
		{
			const auto& Content = Slot->GetContent();
			const float SlotWidth = Content->GetNonAlignedWidth();
			const cbMargin& Padding = Content->GetPadding();
			if ((SlotWidth + (Padding.Left + Padding.Right)) > Width)
				Width += SlotWidth + (Padding.Left + Padding.Right);
		}

		if (Transform.CompressWidth(Width + (BorderThickness.Left + BorderThickness.Right)))
		{
			return true;
		}
		return false;
	}

	bool cbBorder::UnWrapVertical()
	{
		if (Transform.IsHeightCompressed())
		{
			Transform.ResetHeightCompressed();
			return true;
		}
		return false;
	}

	bool cbBorder::UnWrapHorizontal()
	{
		if (Transform.IsWidthCompressed())
		{
			Transform.ResetWidthCompressed();
			return true;
		}
		return false;
	}

	void cbBorder::OnAttach()
	{
		WrapVertical();
		WrapHorizontal();
		NotifyCanvas_WidgetUpdated();
		UpdateSlotAlignments();
	}

	void cbBorder::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();
			NotifyCanvas_WidgetUpdated();
		}

		UnWrap();
	}

	float cbBorder::GetVerticalSlotLocation() const
	{
		const cbBounds& Bounds = GetBounds();
		return ((Bounds.GetTop() + BorderThickness.Top) + (Bounds.GetBottom() - BorderThickness.Bottom)) / 2.0f;
	}

	float cbBorder::GetHorizontalSlotLocation() const
	{
		const cbBounds& Bounds = GetBounds();
		return ((Bounds.GetLeft() + BorderThickness.Left) + (Bounds.GetRight() - BorderThickness.Right)) / 2.0f;
	}
}
