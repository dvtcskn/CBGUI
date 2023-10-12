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
#include "cbWidget.h"
#include "cbSlottedBox.h"
#include "cbCanvas.h"
#include "cbComponent.h"
#include "cbSlider.h"

namespace cbgui
{
	cbWidgetObj* cbWidgetObj::GetRootOwner(std::optional<std::size_t> OwnerIndex, const bool SkipSlot) const
	{
		if (!HasOwner())
			return nullptr;

		std::size_t Index = 0;
		cbWidgetObj* TempOwner = GetOwner();

		{
		OwnerSearch:
			if (SkipSlot)
			{
				if (cbgui::cbCast<cbSlot>(TempOwner))
				{
					if (!TempOwner->HasOwner())
						return nullptr;

					TempOwner = TempOwner->GetOwner();
					goto OwnerSearch;
				}
			}

			if (OwnerIndex.has_value())
				if (Index == OwnerIndex.value())
					return TempOwner;

			if (TempOwner)
			{
				if (TempOwner->HasOwner())
				{
					TempOwner = TempOwner->GetOwner();
					Index++;
					goto OwnerSearch;
				}
			}
			else
			{
				return nullptr;
			}
		}

		return TempOwner;
	}

	cbWidget::cbWidget()
		: mVAlignment(eVerticalAlignment::Align_NONE)
		, mHAlignment(eHorizontalAlignment::Align_NONE)
		, CanvasAnchor(eCanvasAnchor::LeftTop)
		, VerticalAnchor(eAnchors::Inside)
		, HorizontalAnchor(eAnchors::Inside)
		, Owner(nullptr)
		, Canvas(nullptr)
		, bIsAlignedToCanvas(false)
		, bIsEnabled(true)
		, Visibility(eVisibility::Visible)
		, bFocusable(true)
		, bFocused(false)
		, FocusMode(eFocusMode::ZOrder)
		, ZOrderMode(eZOrderMode::InOrder)
		, ZOrder(0)
		, Name(std::nullopt)
		, bShouldNotifyCanvas(true)
	{}

	cbgui::cbWidget::cbWidget(const cbWidget& Widget, cbSlot* NewOwner)
		: mVAlignment(Widget.mVAlignment)
		, mHAlignment(Widget.mHAlignment)
		, CanvasAnchor(Widget.CanvasAnchor)
		, VerticalAnchor(Widget.VerticalAnchor)
		, HorizontalAnchor(Widget.HorizontalAnchor)
		, Owner(NewOwner ? NewOwner : Widget.Owner)
		, Canvas(Widget.Canvas)
		, bIsAlignedToCanvas(Widget.bIsAlignedToCanvas)
		, bIsEnabled(Widget.bIsEnabled)
		, Visibility(Widget.Visibility)
		, bFocusable(Widget.bFocusable)
		, bFocused(Widget.bFocused)
		, FocusMode(Widget.FocusMode)
		, ZOrderMode(Widget.ZOrderMode)
		, ZOrder(Widget.ZOrder)
		, Name(Widget.Name)
		, bShouldNotifyCanvas(Widget.bShouldNotifyCanvas)
	{}

	cbWidget::~cbWidget()
	{
		Canvas = nullptr;
		Owner = nullptr;
	}

	void cbWidget::AddToCanvas(cbICanvas* InCanvas)
	{
		if (!InCanvas || Canvas == InCanvas)
			return;

		if (Owner)
		{
			RemoveFromParent();
		}
		else if (Canvas)
		{
			RemoveFromCanvas();
		}
		else
		{
			ResetInput();
		}

		Canvas = InCanvas;
		if (IsItSharedObject())
			Canvas->Add(shared_from_this());
		else
			Canvas->Add(this);

		ResetInput();
		UpdateStatus();
		UpdateRotation();
		UpdateAlignments();

		OnAttachToCanvas();
	}

	void cbWidget::RemoveFromCanvas()
	{
		if (!Canvas)
			return;

		const auto pCanvas = Canvas;
		Canvas = nullptr;

		ResetInput();
		UpdateStatus();
		UpdateRotation();
		UpdateAlignments();

		OnRemoveFromCanvas();

		if (pCanvas)
			pCanvas->RemoveFromCanvas(this);
	}

	void cbWidget::SetCanvasAnchor(const eCanvasAnchor canvasAnchor)
	{
		if (CanvasAnchor == canvasAnchor)
			return;

		CanvasAnchor = canvasAnchor;
		UpdateAlignments();
	}

	void cbWidget::SetAlignToCanvas(bool Value)
	{
		if (bIsAlignedToCanvas == Value)
			return;

		bIsAlignedToCanvas = Value;

		UpdateVerticalAlignment(true);
		UpdateHorizontalAlignment(true);
		UpdateRotation();
	}

	void cbWidget::AlignToCanvas(const bool ForceAlign)
	{
		if (!IsAlignedToCanvas())
			return;

		UpdateVerticalAlignment(ForceAlign);
		UpdateHorizontalAlignment(ForceAlign);
		UpdateRotation();
	}

	void cbWidget::SetZOrderMode(const eZOrderMode Mode)
	{
		if (ZOrderMode == Mode)
			return;

		ZOrderMode = Mode;

		if (HasCanvas())
			GetCanvas()->ZOrderModeUpdated(this);
	}

	void cbWidget::SetZOrder(const std::int32_t& Order)
	{
		ZOrder = Order;

		if (HasCanvas())
			GetCanvas()->ZOrderChanged(this, ZOrder);
	}

	std::int32_t cbWidget::GetZOrder() const
	{
		return Owner ? Owner->GetZOrder() + ZOrder : ZOrder;
	}

	bool cbWidget::IsInside(const cbVector& Location) const
	{
		return IsRotated() ? cbgui::IsInside(GetBounds(), Location, GetRotation(), GetOrigin()) : cbgui::IsInside(GetBounds(), Location);
	}

	bool cbWidget::Intersect(const cbBounds& Other) const
	{
		return IsRotated() ? cbgui::Intersect(GetBounds(), Other, GetRotation(), GetOrigin()) : cbgui::Intersect(GetBounds(), Other);
	}

	bool cbWidget::IsItCulled() const
	{
		if (Owner)
		{
			if (Owner->IsItCulled())
			{
				return true;
			}
			else
			{
				const cbBounds& OwnerRect = Owner->GetBounds();
				const cbBounds& Rect = GetBounds();
				return OwnerRect.Min.X >= Rect.Max.X || OwnerRect.Min.Y >= Rect.Max.Y;
			}
		}
		else if (Canvas && bIsAlignedToCanvas)
		{
			const cbBounds& OwnerRect = Canvas->GetScreenBounds();
			const cbBounds& Rect = GetBounds();
			return OwnerRect.Min.X >= Rect.Max.X || OwnerRect.Min.Y >= Rect.Max.Y;
		}
		return false;
	}

	cbBounds cbWidget::GetCulledBounds() const
	{
		if (Owner)
		{
			if (IsRotated())
				return cbgui::RecalculateBounds(GetDimension(), GetLocation(), GetRotation(), GetOrigin()).Crop(Owner->GetCulledBounds());
			else
				return cbBounds(GetBounds()).Crop(Owner->GetCulledBounds());
		}
		else if (Canvas && bIsAlignedToCanvas)
		{
			if (IsRotated())
				return cbgui::RecalculateBounds(GetDimension(), GetLocation(), GetRotation(), GetOrigin()).Crop(Canvas->GetScreenBounds());
			else
				return cbBounds(GetBounds()).Crop(Canvas->GetScreenBounds());
		}
		return IsRotated() ? cbgui::RecalculateBounds(GetDimension(), GetLocation(), GetRotation(), GetOrigin()) : GetBounds();
	}

	cbVector cbWidget::GetOrigin() const
	{
		if (Owner)
			return Owner->GetOrigin();
		if (HasCanvas())
		{
			if (IsAlignedToCanvas())
				return GetCanvas()->GetCenter();
		}
		return GetLocation();
	}

	void cbWidget::SetVerticalAnchor(const eAnchors& Anchor)
	{
		if (VerticalAnchor == Anchor)
			return;

		VerticalAnchor = Anchor;
		UpdateVerticalAlignment();
	}

	void cbWidget::SetHorizontalAnchor(const eAnchors& Anchor)
	{
		if (HorizontalAnchor == Anchor)
			return;

		HorizontalAnchor = Anchor;
		UpdateHorizontalAlignment();
	}

	void cbWidget::UpdateAlignments(const bool ForceAlign)
	{
		UpdateVerticalAlignment(ForceAlign);
		UpdateHorizontalAlignment(ForceAlign);
	}

	void cbWidget::SetVerticalAlignment(const eVerticalAlignment& InCustomAlignment)
	{
		if (InCustomAlignment == eVerticalAlignment::Align_NONE)
			return;

		mVAlignment = InCustomAlignment;
		UpdateVerticalAlignment();
	}

	void cbWidget::SetHorizontalAlignment(const eHorizontalAlignment& InCustomAlignment)
	{
		if (InCustomAlignment == eHorizontalAlignment::Align_NONE)
			return;

		mHAlignment = InCustomAlignment;
		UpdateHorizontalAlignment();
	}

	bool cbWidget::DimensionUpdated()
	{
		if (Owner)
		{
			Owner->Notify_DimensionChanged();
			return true;
		}
		return false;
	}

	bool cbWidget::NotifyCanvas_WidgetUpdated()
	{
		if (!bShouldNotifyCanvas)
			return false;

		const auto pCanvas = GetCanvas();

		if (pCanvas)
		{
			pCanvas->WidgetUpdated(this);
			return true;
		}
		return false;
	}

	bool cbWidget::HasOwner() const
	{
		return Owner != nullptr;
	}

	cbWidgetObj* cbWidget::GetOwner() const
	{
		return Owner;
	}

	void cbWidget::AttachToSlot(cbSlot* Parent)
	{
		if (Owner == Parent)
			return;

		if (Canvas)
		{
			RemoveFromCanvas();
			SetAlignToCanvas(false);
		}
		else if (Owner)
		{
			RemoveFromParent();
		}
		else
		{
			ResetInput();
		}

		Owner = Parent;

		//if (HasCanvas())
		//	GetCanvas()->NewSlotContentAdded(Owner, this);

		UpdateRotation();
		UpdateStatus();

		if (GetVerticalAlignment() == eVerticalAlignment::Align_NONE)
			SetVerticalAlignment(eVerticalAlignment::Align_Center);
		else
			UpdateVerticalAlignment(true);

		if (GetHorizontalAlignment() == eHorizontalAlignment::Align_NONE)
			SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
		else
			UpdateHorizontalAlignment(true);

		OnAttach();
	}

	void cbWidget::RemoveFromParent(const bool KeepItOnTheCanvas)
	{
		if (!Owner)
			return;

		cbICanvas* pCanvas = nullptr;
		if (KeepItOnTheCanvas)
			pCanvas = GetCanvas();

		auto pOwner = Owner;
		Owner = nullptr;

		ResetInput();

		if (IsItSharedObject())
		{
			if (KeepItOnTheCanvas && pCanvas)
			{
				Canvas = pCanvas;
				Canvas->Add(shared_from_this());
				if (!Canvas->IsWidgetExist(this))
				{
					pOwner->Notify_RemoveFromParent();
					return;
				}
			}

			const long Count = GetSharedObjectUseCount().value();
			if (Count > 1)
			{
				if (KeepItOnTheCanvas && Canvas)
					OnAttachToCanvas();

				pOwner->Notify_RemoveFromParent();
				pOwner = nullptr;

				OnRemoveFromParent();

				UpdateStatus();
				UpdateRotation();
				UpdateAlignments();
			}
			else
			{
				pOwner->Notify_RemoveFromParent();
			}
		}
		else
		{
			if (KeepItOnTheCanvas)
			{
				Canvas = pCanvas;
				Canvas->Add(this);
				OnAttachToCanvas();

				if (Canvas->IsWidgetExist(this))
				{
					pOwner->Notify_RemoveFromParent();
					pOwner = nullptr;

					OnRemoveFromParent();

					UpdateStatus();
					UpdateRotation();
					UpdateAlignments();
				}
				else
				{
					pOwner->Notify_RemoveFromParent();
				}
			}
			else
			{
				pOwner->Notify_RemoveFromParent();
			}

		}
	}

	bool cbWidget::HasCanvas() const
	{
		if (Canvas != nullptr)
			return true;
		else if (Owner)
			if (Owner->HasCanvas())
				return true;
		return false;
	}

	cbICanvas* cbWidget::GetCanvas() const
	{
		if (Canvas != nullptr)
			return Canvas;
		else if (Owner)
			return Owner->GetCanvas();
		return nullptr;
	}

	void cbWidget::SetFocus(const bool value)
	{
		if (bFocused == value)
			return;

		if (bFocused != value && value)
		{
			if (!IsFocusable())
				return;

			if (Visibility == eVisibility::Hidden)
				return;

			bFocused = value;
			OnFocus();
		}
		else if (bFocused != value && !value)
		{
			bFocused = value;
			OnLostFocus();
		}
	}

	bool cbWidget::IsEnabled() const
	{
		if (Owner)
		{
			if (!bIsEnabled)
				return false;

			return Owner->IsEnabled();
		}
		return bIsEnabled;
	}

	bool cbWidget::Enable()
	{
		if (bIsEnabled)
			return false;

		bIsEnabled = true;
		ResetInput();
		UpdateStatus();

		OnEnabled();
		return true;
	}

	bool cbWidget::Disable()
	{
		if (!bIsEnabled)
			return false;

		bIsEnabled = false;
		ResetInput();
		UpdateStatus();

		OnDisabled();
		return true;
	}

	void cbWidget::SetVisibilityState(const eVisibility visibility)
	{
		if (Visibility == visibility)
			return;

		Visibility = visibility;

		if (Visibility == eVisibility::Hidden)
			ResetInput();

		if (Owner)
			Owner->Notify_VisibilityChanged();

		OnVisibilityChanged();

		if (HasCanvas())
			GetCanvas()->VisibilityChanged(this);
	}
}
