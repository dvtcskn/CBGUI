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
#include "cbSizeBox.h"
#include "cbCanvas.h"

namespace cbgui
{
	void cbSizeBox::cbSizeBoxSlot::ReplaceContent(const cbWidget::SharedPtr& pContent)
	{
		if (!pContent)
			return;

		if (Content)
			Content = nullptr;
		Content = pContent;
		Content->AttachToSlot(this);
		OnContentInsertedOrReplaced();
	}

	cbSizeBox::cbSizeBox()
		: Super()
		, Transform(cbTransform(cbDimension(32.0f, 32.0f)))
		, Slot(nullptr)
		, MinimumWidth(std::nullopt)
		, MaximumWidth(std::nullopt)
		, MinimumHeight(std::nullopt)
		, MaximumHeight(std::nullopt)
	{}

	cbgui::cbSizeBox::cbSizeBox(const cbSizeBox& Other, cbSlot* NewOwner)
		: Super(Other, NewOwner)
		, Transform(Other.Transform)
		, Slot(nullptr)
		, MinimumWidth(Other.MinimumWidth)
		, MaximumWidth(Other.MaximumWidth)
		, MinimumHeight(Other.MinimumHeight)
		, MaximumHeight(Other.MaximumHeight)
	{
		if (Other.Slot)
			SetSlot(Other.Slot->Clone<cbSizeBoxSlot>(this));
	}

	cbSizeBox::~cbSizeBox()
	{
		Slot = nullptr;
	}

	cbWidget::SharedPtr cbSizeBox::CloneWidget(cbSlot* NewOwner)
	{
		cbSizeBox::SharedPtr SizeBox = cbSizeBox::Create(*this, NewOwner);
		//SizeBox->SetSlot(Slot->Clone<cbSizeBoxSlot>(SizeBox.get()));
		return SizeBox;
	}

	void cbSizeBox::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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

	void cbSizeBox::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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

	void cbSizeBox::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
		{
			if (Slot)
				Slot->UpdateRotation();
			NotifyCanvas_WidgetUpdated();
		}
	}

	void cbSizeBox::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbSizeBox::SetMinMaxWidth(std::optional<float> inWidth)
	{
		MinimumWidth = inWidth;
		MaximumWidth = inWidth;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbSizeBox::SetMinMaxHeight(std::optional<float> inHeight)
	{
		MinimumHeight = inHeight;
		MaximumHeight = inHeight;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbSizeBox::SetMinimumWidth(std::optional<float> inWidth) 
	{
		MinimumWidth = inWidth;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbSizeBox::SetMinimumHeight(std::optional<float> inHeight)
	{
		MinimumHeight = inHeight;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbSizeBox::SetMaximumWidth(std::optional<float> inWidth)
	{ 
		MaximumWidth = inWidth;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbSizeBox::SetMaximumHeight(std::optional<float> inHeight)
	{
		MaximumHeight = inHeight;

		if (IsItWrapped())
			Wrap();
		else
			UpdateSlotAlignments();
	}

	void cbSizeBox::HideContent(bool value)
	{
		if (!Slot)
			return;

		Slot->Hidden(value);
	}

	void cbSizeBox::UpdateStatus()
	{
		if (!Slot)
			return;

		SetFocus(false);
		Slot->UpdateStatus();
	}

	std::vector<cbWidgetObj*> cbSizeBox::GetAllChildren() const
	{
		return Slot ? std::vector<cbWidgetObj*>{ Slot.get() } : std::vector<cbWidgetObj*>{ };
	}

	std::vector<cbGeometryVertexData> cbSizeBox::GetVertexData(const bool LineGeometry) const
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

	std::vector<std::uint32_t> cbSizeBox::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return std::vector<std::uint32_t>();
	};

	cbGeometryDrawData cbSizeBox::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("NONE", 0, 0, 0, 0);
	};

	void cbSizeBox::OnSlotVisibilityChanged(cbSlot* Slot)
	{
	}

	void cbSizeBox::OnSlotDimensionUpdated(cbSlot* Slot)
	{
	}

	void cbSizeBox::RemoveContent()
	{
		if (!Slot)
			return;

		ResetInput();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->SlotRemoved(this, Slot.get());
		Slot = nullptr;
	}

	bool cbSizeBox::OnRemoveSlot(cbSlot* pSlot)
	{
		if (!Slot)
			return false;

		ResetInput();

		if (cbICanvas* Canvas = GetCanvas())
			Canvas->SlotRemoved(this, Slot.get());
		Slot = nullptr;

		return true;
	}

	void cbSizeBox::UpdateSlotVerticalAlignment()
	{
		if (Slot)
			Slot->UpdateVerticalAlignment();
	}

	void cbSizeBox::UpdateSlotHorizontalAlignment()
	{
		if (Slot)
			Slot->UpdateHorizontalAlignment();
	}

	std::size_t cbSizeBox::GetSlotSize(const bool ExcludeHidden) const
	{
		if (Slot)
		{
			if (ExcludeHidden && Slot->IsHidden())
				return 0;
			return 1;
		}
		return 0;
	}

	void cbSizeBox::SetContent(const cbWidget::SharedPtr& Content)
	{
		if (!Content)
			return;

		if (!Slot)
		{
			SetSlot(cbSizeBoxSlot::Create(this, Content));
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

	void cbSizeBox::SetSlot(const cbSizeBoxSlot::SharedPtr& pSlot)
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

	void cbSizeBox::UpdateVerticalAlignment(const bool ForceAlign)
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

	void cbSizeBox::UpdateHorizontalAlignment(const bool ForceAlign)
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

	void cbSizeBox::UpdateRotation()
	{
		if (HasOwner())
			Transform.SetRollOffset(GetOwner()->GetRotation());
		else if (IsAlignedToCanvas())
			Transform.SetRollOffset(GetCanvas()->GetScreenRotation());
		else
			Transform.SetRollOffset(0.0f);

		if (Slot)
			Slot->UpdateRotation();
		NotifyCanvas_WidgetUpdated();
	}

	bool cbSizeBox::WrapVertical()
	{
		if (!IsItWrapped())
			return false;

		if (!Slot ? true : !Slot->HasContent() || Slot->IsHidden())
		{
			if (MinimumHeight.has_value() || MaximumHeight.has_value())
			{
				float Height = 0.0;
				if (MinimumHeight.has_value() && MaximumHeight.has_value())
				{
					if (MaximumHeight.value() <= MinimumHeight.value())
						Height = Height <= MaximumHeight.value() ? MaximumHeight.value() : Height >= MinimumHeight.value() ? MinimumHeight.value() : Height;
					if (MaximumHeight.value() >= MinimumHeight.value())
						Height = Height <= MinimumHeight.value() ? MinimumHeight.value() : Height >= MaximumHeight.value() ? MaximumHeight.value() : Height;
				}
				else if (MinimumHeight.has_value())
					Height = Height <= MinimumHeight.value() ? MinimumHeight.value() : Height;
				else if (MaximumHeight.has_value())
					Height = Height >= MaximumHeight.value() ? MaximumHeight.value() : Height;

				Transform.CompressHeight(Height);
				return true;
			}
			return UnWrapVertical();
		}

		float Height = 0.0;
		{
			const auto& Content = Slot->GetContent();
			const float SlotHeight = Content->GetNonAlignedHeight();
			const cbMargin& Padding = Content->GetPadding();
			if ((SlotHeight + (Padding.Top + Padding.Bottom)) > Height)
				Height += SlotHeight + (Padding.Top + Padding.Bottom);
		}

		if (MinimumHeight.has_value() && MaximumHeight.has_value())
		{
			if (MaximumHeight.value() <= MinimumHeight.value())
				Height = Height <= MaximumHeight.value() ? MaximumHeight.value() : Height >= MinimumHeight.value() ? MinimumHeight.value() : Height;
			if (MaximumHeight.value() >= MinimumHeight.value())
				Height = Height <= MinimumHeight.value() ? MinimumHeight.value() : Height >= MaximumHeight.value() ? MaximumHeight.value() : Height;
		}
		else if (MinimumHeight.has_value())
			Height = Height <= MinimumHeight.value() ? MinimumHeight.value() : Height;
		else if (MaximumHeight.has_value())
			Height = Height >= MaximumHeight.value() ? MaximumHeight.value() : Height;

		if (Transform.CompressHeight(Height))
		{
			return true;
		}
		return false;
	}

	bool cbSizeBox::WrapHorizontal()
	{
		if (!IsItWrapped())
			return false;

		if (!Slot ? true : !Slot->HasContent() || Slot->IsHidden())
		{
			if (MinimumWidth.has_value() || MaximumWidth.has_value())
			{
				float Width = 0.0f;
				if (MinimumWidth.has_value() && MaximumWidth.has_value())
				{
					if (MaximumWidth.value() <= MinimumWidth.value())
						Width = Width <= MaximumWidth.value() ? MaximumWidth.value() : Width >= MinimumWidth.value() ? MinimumWidth.value() : Width;
					if (MaximumWidth.value() >= MinimumWidth.value())
						Width = Width <= MinimumWidth.value() ? MinimumWidth.value() : Width >= MaximumWidth.value() ? MaximumWidth.value() : Width;
				}
				else if (MinimumWidth.has_value())
					Width = Width <= MinimumWidth.value() ? MinimumWidth.value() : Width;
				else if (MaximumWidth.has_value())
					Width = Width >= MaximumWidth.value() ? MaximumWidth.value() : Width;

				Transform.CompressWidth(Width);
				return true;
			}
			return UnWrapHorizontal();
		}

		float Width = 0.0f;
		{
			const auto& Content = Slot->GetContent();
			const float SlotWidth = Content->GetNonAlignedWidth();
			const cbMargin& Padding = Content->GetPadding();
			if ((SlotWidth + (Padding.Left + Padding.Right)) > Width)
				Width += SlotWidth + (Padding.Left + Padding.Right);
		}

		if (MinimumWidth.has_value() && MaximumWidth.has_value())
		{
			if (MaximumWidth.value() <= MinimumWidth.value())
				Width = Width <= MaximumWidth.value() ? MaximumWidth.value() : Width >= MinimumWidth.value() ? MinimumWidth.value() : Width;
			if (MaximumWidth.value() >= MinimumWidth.value())
				Width = Width <= MinimumWidth.value() ? MinimumWidth.value() : Width >= MaximumWidth.value() ? MaximumWidth.value() : Width;
		}
		else if (MinimumWidth.has_value())
			Width = Width <= MinimumWidth.value() ? MinimumWidth.value() : Width;
		else if (MaximumWidth.has_value())
			Width = Width >= MaximumWidth.value() ? MaximumWidth.value() : Width;

		if (Transform.CompressWidth(Width))
		{
			return true;
		}
		return false;
	}

	bool cbSizeBox::UnWrapVertical()
	{
		if (Transform.IsHeightCompressed())
		{
			Transform.ResetHeightCompressed();
			return true;
		}
		return false;
	}

	bool cbSizeBox::UnWrapHorizontal()
	{
		if (Transform.IsWidthCompressed())
		{
			Transform.ResetWidthCompressed();
			return true;
		}
		return false;
	}

	void cbSizeBox::OnAttach()
	{
		WrapVertical();
		WrapHorizontal();
		NotifyCanvas_WidgetUpdated();
		UpdateSlotAlignments();
	}

	void cbSizeBox::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();
			NotifyCanvas_WidgetUpdated();
		}

		UnWrap();
	}
}
