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
#include "cbComboBox.h"
#include <cbScrollBox.h>
#include <cbString.h>
#include <cbOverlay.h>
#include <cbImage.h>

using namespace cbgui;

class cbComboBox::cbComboBoxListSlot final : public cbSlot
{
	cbClassBody(cbClassConstructor, cbComboBoxListSlot, cbSlot)
	friend cbComboBox;
public:
	class cbComboBoxList final : public cbScrollBox
	{
		cbClassBody(cbClassConstructor, cbComboBoxList, cbScrollBox)
	public:
		class cbComboBoxOptionSlot : public cbScrollBox::cbScrollBoxSlot
		{
			cbClassBody(cbClassConstructor, cbComboBoxOptionSlot, cbScrollBox::cbScrollBoxSlot)
		private:
			bool bIsSelected;
			bool bIsPressed;
			bool bIsHovered;

			cbImage::SharedPtr Highlight;
			cbWidget::SharedPtr Option;

		public:
			cbComboBoxOptionSlot(cbComboBoxList* pOwner, const cbWidget::SharedPtr& pContent)
				: Super(pOwner, cbOverlay::Create())
				, Option(pContent)
				, bIsPressed(false)
				, bIsHovered(false)
				, bIsSelected(false)
				, Highlight(cbImage::Create())
			{
				Highlight->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Highlight->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				Highlight->SetVisibilityState(eVisibility::Invisible);
				Highlight->SetWidth(1.0f);
				Highlight->SetHeight(1.0f);

				auto Overlay = static_cast<cbOverlay*>(GetContent());
				Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				Overlay->AttachToSlot(this);
				Overlay->Insert(Highlight);
				Overlay->Insert(Option);

				SetSelectionVertexColorStyle(GetOwner<cbComboBoxList>()->GetOptionsVertexStyle());
			}

			cbComboBoxOptionSlot(const cbScrollBoxSlot& Widget, cbSlottedBox* NewOwner)
				: Super(Widget, NewOwner)
				, Option(nullptr)
				, bIsPressed(false)
				, bIsHovered(false)
				, bIsSelected(false)
				, Highlight(nullptr)
			{
				auto Overlay = static_cast<cbOverlay*>(GetContent());
				Highlight = Overlay->GetSlot(0)->GetSharedContent<cbImage>();
				Option = Overlay->GetSlot(1)->GetSharedContent();

				Overlay->AttachToSlot(this);

				SetSelectionVertexColorStyle(GetOwner<cbComboBoxList>()->GetOptionsVertexStyle());
			}

		public:
			virtual ~cbComboBoxOptionSlot()
			{
				Option = nullptr;
				Highlight = nullptr;
			}

			virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) override
			{
				return cbComboBoxOptionSlot::Create(*this, NewOwner);
			}

		public:
			cbWidget* GetOption() const { return Option.get(); }

			virtual bool IsPressed() const final { return bIsPressed; }
			virtual bool IsHovered() const final { return bIsHovered; }
			virtual bool IsFocused() const override { return bIsHovered; }

			virtual eZOrderMode GetZOrderMode() const { return eZOrderMode::Latest; }
			virtual eFocusMode GetFocusMode() const override { return eFocusMode::ZOrder; }

			eButtonState GetButtonState() const { return eButtonState::Default; }

			bool IsSelected() const { return bIsSelected; }
			void Select()
			{
				bIsSelected = true;
				GetOwner<cbComboBoxList>()->OptionSelected(this);
			}
			void Deselect() { bIsSelected = false; }

			virtual void Pressed() { bIsPressed = true; }
			virtual void Released()
			{
				if (!bIsPressed)
					return;

				bIsPressed = false;
			}
			virtual void Hovered()
			{
				bIsHovered = true;
				Highlight->SetVisibilityState(eVisibility::Visible);
				GetOwner<cbComboBoxList>()->OnOptionHovered(this);
			}
			virtual void Unhovered() { bIsHovered = false; Highlight->SetVisibilityState(eVisibility::Invisible); }

			virtual void OnOpen() {}
			virtual void OnClose() { Unhovered(); }

			virtual void ToggleMenu() { GetOwner<cbComboBoxList>()->ToggleMenu(); }

			void SetTextVertexColorStyle(const cbVertexColorStyle& style)
			{
				if (auto Text = cbgui::cbCast<cbString>(GetContent()))
					Text->SetVertexColorStyle(style);
			}

			void SetSelectionVertexColorStyle(const cbVertexColorStyle& style)
			{
				Highlight->SetVertexColorStyle(style);
			}

		public:
			virtual bool OnMouseEnter(const cbMouseInput& Mouse)
			{
				if (!IsFocusable() || !IsEnabled())
					return false;

				if (IsItCulled())
					return false;

				{
					Hovered();
					Super::OnMouseEnter(Mouse);
				}

				return true;
			}

			virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) override
			{
				if (!IsFocused())
					return false;

				if (!bIsPressed)
					return false;

				Released();
				Select();

				return true;
			}

			virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) override
			{
				if (!IsFocused())
					return false;

				if (bIsPressed)
					return false;

				Pressed();

				return true;
			}

			virtual bool OnMouseLeave(const cbMouseInput& Mouse)
			{
				if (!IsFocusable() || !IsEnabled() || !IsFocused())
					return false;

				Unhovered();

				Super::OnMouseLeave(Mouse);

				return true;
			}
		};

		friend cbComboBoxOptionSlot;
	private:
		cbVertexColorStyle OptionsVertexStyle;

	public:
		cbComboBoxList()
			: Super()
			, OptionsVertexStyle(cbVertexColorStyle(cbColor::Black()))
		{
			ShowScrollBarIfScrollable(true);
			SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetZOrderMode(eZOrderMode::Latest);
		}

		cbComboBoxList(const cbComboBoxList& ComboBoxList)
			: Super(ComboBoxList)
			, OptionsVertexStyle(ComboBoxList.OptionsVertexStyle)
		{
		}

	public:
		virtual ~cbComboBoxList() = default;

		virtual cbWidget::SharedPtr CloneWidget() override
		{
			cbComboBoxList::SharedPtr ScrollBox = cbComboBoxList::Create(*this);

			/*std::size_t SlotSize = GetSlotSize();
			for (std::size_t i = 0; i < SlotSize; i++)
			{
				cbComboBoxOptionSlot::SharedPtr Slot = GetSlot(i)->Clone<cbComboBoxOptionSlot>(ScrollBox.get());
				ScrollBox->Insert(Slot, i);
			}

			ScrollBox->Scroll(0.0f);*/
			return ScrollBox;
		}

	public:
		//virtual std::optional<cbBounds> GetScissorRect() const override final { return cbgui::GetScissorRect(GetDimension(), GetLocation(), GetRotation(), GetOrigin()); }
		virtual bool IsItCulled() const override { return GetOwner()->IsItCulled(); }
		virtual cbBounds GetCulledBounds() const override final { return GetBounds(); }

		bool IsScrollBarHandleHovered() const { return GetScrollBar()->GetHandle()->IsHovered(); }
		bool IsScrollBarHandlePressed() const { return GetScrollBar()->GetHandle()->IsPressed(); }

		void OnOpen()
		{
			//Enable();
			//SetVisibilityState(eVisibility::Visible);
			ResetScrollBar();

			if (!IsFocused())
				SetFocus(true);
		}

		void OnClose()
		{
			//Disable();
			//SetVisibilityState(eVisibility::Hidden);
			SetFocus(false);
			ResetScrollBar();

			const std::size_t Size = GetSlotSize();
			for (std::size_t i = 0; i < Size; i++)
			{
				static_cast<cbComboBoxOptionSlot*>(GetSlot(i))->OnClose();
			}
		}

		bool IsInsideSlots(const cbVector& Location) const
		{
			const bool Inside = Super::IsInside(Location);

			if (!Inside)
			{
				const std::size_t Size = GetSlotSize();
				for (std::size_t i = 0; i < Size; i++)
				{
					const auto& Slot = GetSlot(i);
					if (Slot->IsInside(Location) && !Slot->IsItCulled())
						return true;
				}
			}

			return Inside;
		}

	public:
		void InsertOption(cbWidget* Content, const std::optional<std::size_t> SlotIndex)
		{
			const auto& CB_Slot = static_cast<cbComboBoxOptionSlot*>(Super::Insert(cbComboBoxOptionSlot::Create(this, cbWidget::SharedPtr(Content)), SlotIndex.has_value() ? SlotIndex.value() : GetNextSlotIndex()));
			CB_Slot->SetSelectionVertexColorStyle(OptionsVertexStyle);
		}

		void InsertOption(const cbWidget::SharedPtr& Content, const std::optional<std::size_t> SlotIndex)
		{
			const auto& CB_Slot = static_cast<cbComboBoxOptionSlot*>(Super::Insert(cbComboBoxOptionSlot::Create(this, Content), SlotIndex.has_value() ? SlotIndex.value() : GetNextSlotIndex()));
			CB_Slot->SetSelectionVertexColorStyle(OptionsVertexStyle);
		}

		void InsertOption(const std::u32string& Text, cbTextDesc Desc, cbVertexColorStyle Style, const std::optional<std::size_t> Index)
		{
			cbString::SharedPtr pUIText = cbString::Create(Text, Desc);
			pUIText->SetVertexColorStyle(Style);
			pUIText->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			pUIText->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			pUIText->SetZOrderMode(eZOrderMode::Latest);
			pUIText->SetFocusMode(eFocusMode::Immediate);
			//pUIText->SetMaterialStyle(FontStyle);
			InsertOption(pUIText, Index);
		}

		cbVertexColorStyle GetOptionsVertexStyle() const { return OptionsVertexStyle; }
		void SetOptionHoverVertexStyle(const cbVertexColorStyle& VertexColorStyle)
		{
			OptionsVertexStyle = VertexColorStyle;

			const std::size_t Size = GetSlotSize();
			for (std::size_t i = 0; i < Size; i++)
			{
				auto pSlot = static_cast<cbComboBoxOptionSlot*>(GetSlot(i));
				pSlot->SetSelectionVertexColorStyle(OptionsVertexStyle);
			}
		}

		void Close() { static_cast<cbComboBox*>(GetOwner()->GetOwner())->Close(); }
		void ToggleMenu() { static_cast<cbComboBox*>(GetOwner()->GetOwner())->SetIsMenuOpen(); }

		void OptionSelected(cbComboBoxOptionSlot* Slot)
		{
			const std::size_t Size = GetSlotSize();
			for (std::size_t i = 0; i < Size; i++)
			{
				const auto& mSlot = GetSlot(i);
				if (mSlot == Slot)
					continue;
				auto pSlot = static_cast<cbComboBoxOptionSlot*>(mSlot);
				if (pSlot->IsSelected())
				{
					pSlot->Deselect();
				}
			}
			GetRootOwner<cbComboBox::cbComboBoxListSlot>(2, false)->SelectSlot(Slot);
			//static_cast<cbComboBox::cbComboBoxListSlot*>(GetOwner()->GetOwner()->GetOwner())->SelectSlot(Slot);
		}

		void OnOptionHovered(cbComboBoxOptionSlot* pSlot)
		{
			const std::size_t Size = GetSlotSize();
			for (std::size_t i = 0; i < Size; i++)
			{
				const auto& Slot = GetSlot<cbComboBoxOptionSlot>(i);
				if (Slot == pSlot)
					continue;

				Slot->Unhovered();
			}
		}

		cbSlot* GetCurrentHoveredSlot() const
		{
			const std::size_t Size = GetSlotSize();
			for (std::size_t i = 0; i < Size; i++)
			{
				const auto pSlot = GetSlot<cbComboBoxOptionSlot>(i);
				if (pSlot->IsHovered())
				{
					return pSlot;
				}
			}
			return nullptr;
		}

		std::int32_t GetCurrentHoveredSlotIndex() const
		{
			const std::size_t Size = GetSlotSize();
			for (std::size_t i = 0; i < Size; i++)
			{
				const auto pSlot = GetSlot<cbComboBoxOptionSlot>(i);
				if (pSlot->IsHovered())
				{
					return std::int32_t(pSlot->GetIndex());
				}
			}
			return -1;
		}

		cbSlot* HoverOption(std::size_t Index)
		{
			auto HoveredOption = GetSlot<cbComboBoxOptionSlot>(Index);
			if (HoveredOption)
			{
				HoveredOption->Hovered();
				ScrollSlotIntoView(Index);
				return HoveredOption;
			}
			return nullptr;
		}

		cbSlot* UnhoverOption(std::size_t Index)
		{
			if (IsIndexExist(Index))
			{
				auto Slot = GetSlot<cbComboBoxOptionSlot>(Index);
				{
					if (Slot->GetIndex() == Index)
					{
						Slot->Unhovered();
						return Slot;
					}
				}
			}
			return nullptr;
		}

		bool SelectOption(std::size_t Index) const
		{
			if (IsIndexExist(Index))
			{
				const auto Slot = GetSlot<cbComboBoxOptionSlot>(Index);
				Slot->Select();
				return true;
			}
			return false;
		}

		cbSlot* GetOptionFromIndex(std::size_t Index) const
		{
			if (IsIndexExist(Index))
			{
				return GetSlot(Index);
			}
			return nullptr;
		}
	};

private:
	bool bIsInserted;
	cbOverlay::SharedPtr Content;
	cbImage::SharedPtr BG;
	cbComboBoxList::SharedPtr ComboBoxList;
	cbVector Location;
	float Height;

	void Inserted() { bIsInserted = true; OnInserted(); }
	virtual void OnInserted() { }

public:
	cbComboBoxListSlot(cbComboBox* pOwner)
		: Super(pOwner)
		, bIsInserted(false)
		, BG(cbImage::Create())
		, ComboBoxList(cbComboBoxList::Create())
		, Height(128.0f)
		, Location(cbVector::Zero())
	{
		BG->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		BG->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

		Content = cbOverlay::Create();
		Content->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Content->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Content->Insert(BG);
		Content->Insert(ComboBoxList);
		Content->SetVisibilityState(eVisibility::Hidden);
		Content->AttachToSlot(this);
	}

	cbComboBoxListSlot(const cbComboBoxListSlot& ComboBoxListSlot, cbSlottedBox* pOwner)
		: Super(ComboBoxListSlot, pOwner)
		, bIsInserted(false)
		, Height(ComboBoxListSlot.Height)
		, Location(ComboBoxListSlot.Location)
	{
		Content = ComboBoxListSlot.Content->Clone<cbOverlay>();
		BG = Content->GetSlot(0)->GetSharedContent<cbImage>();
		ComboBoxList = Content->GetSlot(1)->GetSharedContent<cbComboBoxList>();
		Content->AttachToSlot(this);
	}

	virtual ~cbComboBoxListSlot()
	{
		BG = nullptr;
		ComboBoxList = nullptr;
		Content = nullptr;
	}

	virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) override
	{
		return cbComboBoxListSlot::Create(*this, NewOwner);
	}

	virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }

	void BeginPlay() { if (HasContent()) GetContent()->BeginPlay(); }
	void Tick(float DeltaTime) { if (HasContent()) GetContent()->Tick(DeltaTime); }

	virtual void ReplaceContent(const cbWidget::SharedPtr& Content) {}

	cbComboBoxList* GetComboBoxList() const { return ComboBoxList.get(); }

	virtual cbVector GetLocation() const override { return Location; }

	virtual bool HasContent() const override { return Content != nullptr; }
	virtual cbWidget::SharedPtr GetSharedContent() const override { return Content; }
	virtual cbWidget* GetContent() const override { return Content.get(); }

	virtual void Hidden(bool value) { Content->SetVisibilityState(value ? eVisibility::Hidden : eVisibility::Visible); }

	virtual bool HasAnyChildren() const { return Content != nullptr; }
	virtual std::vector<cbWidgetObj*> GetAllChildren() const { return std::vector<cbWidgetObj*>{ Content.get() }; }

	bool IsInsideSlots(const cbVector& Location) const { return ComboBoxList->IsInsideSlots(Location); }

	virtual eZOrderMode GetZOrderMode() const { return eZOrderMode::Latest; }
	virtual std::int32_t GetZOrder() const override { return GetOwner()->GetZOrder(); }

	void SetListHeight(const float inHeight)
	{
		Height = inHeight;
		UpdateVerticalAlignment();
		UpdateHorizontalAlignment();
	}

	virtual void OnOpen()
	{
		Content->Enable();
		Content->SetVisibilityState(eVisibility::Visible);
		UpdateVerticalAlignment();
		UpdateHorizontalAlignment();
		ComboBoxList->OnOpen();
	}

	virtual void OnClose()
	{
		Content->Disable();
		Content->SetVisibilityState(eVisibility::Hidden);
		ComboBoxList->OnClose();
	}

	bool IsScrollBarHandlePressed() const { return ComboBoxList->IsScrollBarHandlePressed(); }
	cbSlot* HoverOption(std::size_t Index) { return ComboBoxList->HoverOption(Index); }
	cbSlot* UnhoverOption(std::size_t Index) { return ComboBoxList->UnhoverOption(Index); }
	bool SelectOption(std::size_t Index) { return ComboBoxList->SelectOption(Index); }
	cbSlot* GetOptionFromIndex(std::size_t Index) const { return ComboBoxList->GetOptionFromIndex(Index); }
	cbSlot* GetCurrentHoveredSlot() const { return ComboBoxList->GetCurrentHoveredSlot(); }
	std::int32_t GetCurrentHoveredSlotIndex() const { return ComboBoxList->GetCurrentHoveredSlotIndex(); }

	void SelectSlot(cbComboBox::cbComboBoxListSlot::cbComboBoxList::cbComboBoxOptionSlot* Option) { GetOwner<cbComboBox>()->SelectSlot(Option, Option->GetOption()); }

	void UpdateVerticalAlignment()
	{
		if (!HasContent() || !HasOwner())
			return;

		const auto pOwner = GetOwner<cbComboBox>();
		switch (pOwner->GetOrientation())
		{
		case eOrientation::Vertical:
			Location.Y = pOwner->GetNextSlotLocation(eVerticalAlignment::Align_Top, this->GetIndex()) + this->GetHeight() / 2.0f;
		break;
		case eOrientation::Horizontal:
			Location.Y = pOwner->GetContentVerticalAlignmentLocation();
		break;
		}

		Content->UpdateVerticalAlignment();
	}

	void UpdateHorizontalAlignment()
	{
		if (!HasContent() || !HasOwner())
			return;

		const auto pOwner = GetOwner<cbComboBox>();
		switch (pOwner->GetOrientation())
		{
		case eOrientation::Vertical:
			Location.X = pOwner->GetContentHorizontalAlignmentLocation();
		break;
		case eOrientation::Horizontal:
			Location.X = pOwner->GetNextSlotLocation(eHorizontalAlignment::Align_Right, this->GetIndex()) + this->GetWidth() / 2.0f;
		break;
		}

		Content->UpdateHorizontalAlignment();
	}

	virtual cbDimension GetDimension() const override final
	{
		const float width = GetContent()->GetNonAlignedWidth() < GetOwner()->GetWidth() ? GetOwner()->GetWidth() : GetContent()->GetNonAlignedWidth();

		if (Height > GetContent()->GetNonAlignedHeight())
			return cbDimension(width, GetContent()->GetNonAlignedHeight());

		return cbDimension(width, Height);
	}

	virtual float GetWidth() const override final { return GetDimension().GetWidth(); }
	virtual float GetHeight() const override final { return GetDimension().GetHeight(); }
	virtual cbBounds GetBounds() const { return cbBounds(GetDimension(), GetLocation()); }
	virtual cbBounds GetCulledBounds() const override final { return GetBounds(); }
	virtual bool IsItCulled() const override final { return !GetOwner<cbComboBox>()->IsMenuOpen(); }

	void UpdateStatus()
	{
		Content->UpdateStatus();
	}

	void UpdateRotation()
	{
		Content->UpdateRotation();
	}

	void InsertOption(const std::u32string& Text, cbTextDesc Desc, cbVertexColorStyle Style, const std::optional<std::size_t> Index)
	{
		ComboBoxList->InsertOption(Text, Desc, Style, Index);
	}

	void InsertOption(cbWidget* Obj, const std::optional<std::size_t> InIndex)
	{
		ComboBoxList->InsertOption(Obj, InIndex);
	}

	void InsertOption(const cbWidget::SharedPtr& Obj, const std::optional<std::size_t> InIndex)
	{
		ComboBoxList->InsertOption(Obj, InIndex);
	}

	void RemoveOption(std::int32_t Index)
	{
		ComboBoxList->RemoveSlot(Index);
	}

	void SetOptionHoverVertexStyle(const cbVertexColorStyle& VertexColorStyle) { ComboBoxList->SetOptionHoverVertexStyle(VertexColorStyle); }
	void SetScrollBarBackgroundVertexColorStyle(const cbVertexColorStyle& style) { ComboBoxList->SetScrollBarBackgroundVertexColorStyle(style); }
	void SetScrollBarHandleVertexColorStyle(const cbButtonVertexColorStyle& style) { ComboBoxList->SetScrollBarHandleVertexColorStyle(style); }
	void SetBackgroundVertexColorStyle(const cbVertexColorStyle& style) { BG->SetVertexColorStyle(style); }

	virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const { return GetContent()->IsInteractableWithKey(KeyCode); }
	virtual bool OnKeyUp(std::uint64_t KeyCode) { return GetContent()->OnKeyUp(KeyCode); }
	virtual bool OnKeyDown(std::uint64_t KeyCode) { return GetContent()->OnKeyDown(KeyCode); }
	virtual bool IsInteractableWithMouse() const { return GetContent()->IsInteractableWithMouse(); }
	virtual bool OnMouseEnter(const cbMouseInput& Mouse) { return GetContent()->OnMouseEnter(Mouse); }
	virtual bool OnMouseLeave(const cbMouseInput& Mouse) { return GetContent()->OnMouseLeave(Mouse); }
	virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) { return GetContent()->OnMouseWheel(WheelDelta, Mouse); }
	virtual bool OnMouseMove(const cbMouseInput& Mouse) { return GetContent()->OnMouseMove(Mouse); }
	virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) { return GetContent()->OnMouseButtonDoubleClick(Mouse); }
	virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) { return GetContent()->OnMouseButtonUp(Mouse); }
	virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) { return GetContent()->OnMouseButtonDown(Mouse); }
};

cbComboBox::cbComboBox(const cbgui::eOrientation InOrientation)
	: Super()
	, Orientation(InOrientation)
	, bIsOpen(false)
	, MenuSlot(cbComboBoxMenuSlot::Create(this))
	, ListSlot(cbComboBoxListSlot::Create(this))
{
	MenuSlot->Inserted();
	ListSlot->Inserted();
	SetDimension(cbDimension(100.0f, 40.0f));
	SetVerticalAlignment(eVerticalAlignment::Align_Fill);
	SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
}

cbComboBox::cbComboBox(const cbComboBox& Widget)
	: Super(Widget)
	, Transform(Widget.Transform)
	, Orientation(Widget.Orientation)
	, bIsOpen(false)
	, MenuSlot(Widget.MenuSlot->Clone<cbComboBoxMenuSlot>(this))
	, ListSlot(Widget.ListSlot->Clone<cbComboBoxListSlot>(this))
	, fOnSelectionChanged(Widget.fOnSelectionChanged)
{
	MenuSlot->Inserted();
	ListSlot->Inserted();
}

cbComboBox::~cbComboBox()
{
	MenuSlot = nullptr;
	ListSlot = nullptr;
}

cbWidget::SharedPtr cbComboBox::CloneWidget()
{
	return cbComboBox::Create(*this);
}

cbDimension cbComboBox::GetSlotDimension() const
{
	return cbDimension(GetWidth(), GetHeight());
}

void cbComboBox::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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

void cbComboBox::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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

void cbComboBox::SetRotation(const float Rotation)
{
	if (Transform.Rotate2D(Rotation))
	{
		SetIsMenuOpen(false);
		MenuSlot->UpdateRotation();
		ListSlot->UpdateRotation();
		NotifyCanvas_WidgetUpdated();
	}
}

void cbComboBox::SetPadding(const cbMargin& Padding)
{
	if (Transform.SetPadding(Padding))
	{
		if (HasOwner())
			DimensionUpdated();
		else if (IsAlignedToCanvas())
			AlignToCanvas();
	}
}

void cbComboBox::UpdateVerticalAlignment(const bool ForceAlign)
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

void cbComboBox::UpdateHorizontalAlignment(const bool ForceAlign)
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

void cbComboBox::UpdateStatus()
{
	SetFocus(false);
	MenuSlot->UpdateStatus();
	ListSlot->UpdateStatus();
}

void cbComboBox::UpdateRotation()
{
	SetIsMenuOpen(false);
	MenuSlot->UpdateRotation();
	ListSlot->UpdateRotation();
	NotifyCanvas_WidgetUpdated();
}

void cbComboBox::OnAttach()
{
	WrapVertical();
	WrapHorizontal();
	UpdateSlotAlignments();
}

void cbComboBox::OnRemoveFromParent()
{
	if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
	{
		Transform.ResetWidthAlignment();
		Transform.ResetHeightAlignment();
		NotifyCanvas_WidgetUpdated();
	}

	UnWrap();
}

bool cbComboBox::WrapVertical()
{
	if (!IsItWrapped() && !MenuSlot)
		return false;

	float Height = 0.0;

	const auto& Content = MenuSlot->GetContent();
	const float SlotHeight = Content->GetNonAlignedHeight();
	const cbMargin& Padding = Content->GetPadding();
	if ((SlotHeight + (Padding.Top + Padding.Bottom)) > Height)
		Height += SlotHeight + (Padding.Top + Padding.Bottom);

	return Transform.CompressHeight(Height);
}

bool cbComboBox::WrapHorizontal()
{
	if (!IsItWrapped() && !MenuSlot)
		return false;

	float Width = 0.0f;

	const auto& Content = MenuSlot->GetContent();
	const float SlotWidth = Content->GetNonAlignedWidth();
	const cbMargin& Padding = Content->GetPadding();
	if ((SlotWidth + (Padding.Left + Padding.Right)) > Width)
		Width += SlotWidth + (Padding.Left + Padding.Right);

	return Transform.CompressWidth(Width);
}

bool cbComboBox::UnWrapVertical()
{
	if (Transform.IsHeightCompressed())
	{
		Transform.ResetHeightCompressed();
		return true;
	}
	return false;
}

bool cbComboBox::UnWrapHorizontal()
{
	if (Transform.IsWidthCompressed())
	{
		Transform.ResetWidthCompressed();
		return true;
	}
	return false;
}

float cbComboBox::GetContentVerticalAlignmentLocation()
{
	if (!HasCanvas())
		return GetBounds().GetTop() + ListSlot->GetHeight() / 2.0f;

	const float LocationY = GetLocation().Y;
	const float ContentHeight = ListSlot->GetHeight();

	if ((LocationY + ContentHeight) > GetCanvas()->GetScreenDimension().GetHeight())
	{
		return MenuSlot->GetBounds().GetBottom() - ContentHeight / 2.0f;
	}
	return GetBounds().GetTop() + ListSlot->GetHeight() / 2.0f;
}

float cbComboBox::GetContentHorizontalAlignmentLocation()
{
	return GetBounds().GetLeft() + ListSlot->GetWidth() / 2.0f;
}

void cbComboBox::Insert(std::u32string Text, cbTextDesc Desc, cbVertexColorStyle Style, const std::optional<std::size_t> Index)
{
	ListSlot->InsertOption(Text, Desc, Style, Index);
}

void cbComboBox::Insert(std::string Text, cbTextDesc Desc, cbVertexColorStyle Style, const std::optional<std::size_t> Index)
{
	Insert(std::u32string(Text.begin(), Text.end()), Desc, Style, Index);
}

void cbComboBox::Insert(const cbWidget::SharedPtr& Obj, const std::optional<std::size_t> Index)
{
	ListSlot->InsertOption(Obj, Index);
}

bool cbComboBox::SetIsMenuOpen(const std::optional<bool> value, const bool& Force)
{
	const bool oldValue = bIsOpen;
	bIsOpen = value.has_value() ? value.value() : !bIsOpen;
	if (oldValue != bIsOpen || Force)
	{
		if (bIsOpen)
		{
			OnOpen();
			return true;
		}
		else
		{
			OnClose();
			return false;
		}
	}

	return false;
}

void cbComboBox::OnOpen()
{
	MenuSlot->OnOpen();
	ListSlot->OnOpen();

	OnOpening();
}

void cbComboBox::OnClose()
{
	MenuSlot->OnClose();
	ListSlot->OnClose();
}

bool cbComboBox::IsInside(const cbVector& Location) const
{
	bool Inside = Super::IsInside(Location);

	if (IsMenuOpen() && !Inside)
	{
		Inside = ListSlot->IsInsideSlots(Location);
	}

	return Inside;
}

bool cbComboBox::IsInsideButton(const cbVector& Location) const
{
	return MenuSlot->IsInside(Location);
}

bool cbComboBox::IsContentFocused() const
{
	return ListSlot->IsFocused();
}

bool cbComboBox::IsInsideContent(const cbVector& Location) const
{
	return ListSlot->IsInside(Location);
}

std::vector<cbSlot*> cbComboBox::GetSlots() const
{
	return std::vector<cbSlot*>{ MenuSlot.get(), ListSlot.get() };
}

cbSlot* cbComboBox::GetSlot(const std::size_t Index) const
{
	if (Index == 0)
		return MenuSlot.get();
	else if (Index == 1)
		return ListSlot.get();
	return nullptr;
}

std::vector<cbWidgetObj*> cbComboBox::GetAllChildren() const
{
	return std::vector<cbWidgetObj*>{ MenuSlot.get(), ListSlot.get() };
}

void cbComboBox::OnSlotVisibilityChanged(cbSlot* Slot)
{
}

void cbComboBox::OnSlotDimensionUpdated(cbSlot* Slot)
{
}

bool cbComboBox::OnRemoveSlot(cbSlot* pSlot)
{
	return false;
}

cbSlot* cbComboBox::GetOverlappingSlot(const cbBounds& Bounds) const
{
	if (MenuSlot->Intersect(Bounds))
	{
		return MenuSlot.get();
	}
	else if (ListSlot->Intersect(Bounds))
	{
		return ListSlot.get();
	}
	return nullptr;
}

void cbComboBox::SetOrientation(const eOrientation InOrientation)
{
	Orientation = InOrientation;
	UpdateAlignments();
}

void cbComboBox::UpdateSlotVerticalAlignment()
{
	MenuSlot->UpdateVerticalAlignment();
	ListSlot->UpdateVerticalAlignment();
}

void cbComboBox::UpdateSlotHorizontalAlignment()
{
	MenuSlot->UpdateHorizontalAlignment();
	ListSlot->UpdateHorizontalAlignment();
}

std::size_t cbComboBox::GetSlotIndex(const cbSlot* Slot) const
{
	return MenuSlot.get() == Slot ? 0 : 1;
}

cbSlot* cbComboBox::GetOptionFromIndex(std::int32_t Index) const
{
	return ListSlot->GetOptionFromIndex(Index);
}

void cbComboBox::SetScrollBox_BarVetexStyle(cbVertexColorStyle Style)
{
	ListSlot->SetScrollBarBackgroundVertexColorStyle(Style);
}

void cbComboBox::SetScrollBox_HandleVetexStyle(cbButtonVertexColorStyle Style)
{
	ListSlot->SetScrollBarHandleVertexColorStyle(Style);
}

void cbComboBox::SetOptionHoverVetexStyle(cbVertexColorStyle Style)
{
	ListSlot->SetOptionHoverVertexStyle(Style);
}

void cbComboBox::SetListBackgroundVertexStyle(cbVertexColorStyle Style)
{
	ListSlot->SetBackgroundVertexColorStyle(Style);
}

void cbComboBox::OnSelected(cbWidgetObj* Widget)
{
}

void cbComboBox::SelectSlot(cbSlot* Slot, cbWidget* Option)
{
	if (Slot == nullptr)
		return;
	if (!Option)
		return;

	MenuSlot->SelectOption(Option);

	Close();
	OnSelectionChanged(Slot, Option);
}

void cbComboBox::HideMenuButton(bool Var)
{
	MenuSlot->HideMenuButton(Var);
}

float cbComboBox::GetRotation() const
{
	return HasOwner() ? GetOwner()->GetRotation() + Transform.GetRotation() : Transform.GetRotation();
}

bool cbComboBox::SelectItem(const std::size_t Index)
{
	return ListSlot->GetComboBoxList()->SelectOption(Index);
}

cbSlot* cbComboBox::HoverItem(const std::size_t Index)
{
	if (!IsMenuOpen())
		return nullptr;

	return ListSlot->HoverOption(Index);
}

cbSlot* cbComboBox::UnhoverOption(const std::size_t Index)
{
	if (!IsMenuOpen())
		return nullptr;

	return ListSlot->UnhoverOption(Index);
}

cbSlot* cbComboBox::GetCurrentHoveredSlot() const
{
	if (!IsMenuOpen())
		return nullptr;

	return ListSlot->GetCurrentHoveredSlot();
}

std::int32_t cbComboBox::GetCurrentHoveredSlotIndex() const
{
	if (!IsMenuOpen())
		return -1;

	return ListSlot->GetCurrentHoveredSlotIndex();
}

std::size_t cbComboBox::GetOptionSize() const
{
	return ListSlot->GetComboBoxList()->GetSlotSize();
}

bool cbComboBox::Open()
{
	if (!IsFocusable() || !IsEnabled())
		return false;

	if (IsMenuOpen())
		return false;

	if (IsItCulled())
		return false;

	if (!IsFocused())
		SetFocus(true);

	ListSlot->OnOpen();

	SetIsMenuOpen(true, true);
	return true;
}

bool cbComboBox::Close()
{
	if (!IsFocusable() || !IsEnabled() || !IsFocused())
		return false;

	if (!IsMenuOpen())
		return false;

	ListSlot->OnClose();

	SetIsMenuOpen(false, true);
	return true;
}

void cbComboBox::RemoveOption(std::int32_t Index)
{
	ListSlot->RemoveOption(Index);
}

void cbComboBox::SetListHeight(const float Height)
{
	ListSlot->SetListHeight(Height);
}

float cbComboBox::GetNextSlotLocation(const eVerticalAlignment& InVAlignment, const std::size_t& Index) const
{
	if (!HasCanvas())
	{
		if (Index == 0)
			return GetBounds().GetTop();
		return MenuSlot->GetBounds().GetBottom();
	}

	if (Index == 0)
		return GetBounds().GetTop();

	const float LocationY = GetLocation().Y;
	const float ContentHeight = ListSlot->GetHeight();

	if ((LocationY + ContentHeight) > GetCanvas()->GetScreenDimension().GetHeight())
	{
		return MenuSlot->GetBounds().GetTop() - ContentHeight;
	}
	return MenuSlot->GetBounds().GetBottom();
}

float cbComboBox::GetNextSlotLocation(const eHorizontalAlignment& InHAlignment, const std::size_t& Index) const
{
	if (!HasCanvas())
	{
		if (Index == 0)
			return GetBounds().GetLeft();
		return MenuSlot->GetBounds().GetRight();
	}

	if (Index == 0)
		return GetBounds().GetLeft();

	const float LocationX = GetLocation().X;
	const float ContentWidth = ListSlot->GetWidth();

	if ((LocationX + ContentWidth) > GetCanvas()->GetScreenDimension().GetWidth())
	{
		return MenuSlot->GetBounds().GetLeft() - ContentWidth;
	}
	return MenuSlot->GetBounds().GetRight();
}

void cbComboBox::WrapOrUpdateAlignments()
{
	if (IsItWrapped())
	{
		Wrap();
	}
	else
	{
		if (IsAlignedToCanvas())
			AlignToCanvas(true);
		else
			UpdateSlotAlignments();
	}
}

bool cbComboBox::IsInteractableWithKey(std::optional<std::uint64_t> KeyCode) const
{
	return true;
}

bool cbComboBox::OnKeyUp(std::uint64_t KeyCode)
{
	if (!IsFocused() || !IsEnabled())
		return false;

	ListSlot->OnKeyUp(KeyCode);

	return false;
}

bool cbComboBox::OnKeyDown(std::uint64_t KeyCode)
{
	if (!IsFocused() || !IsEnabled())
		return false;

	ListSlot->OnKeyDown(KeyCode);

	return false;
}

bool cbComboBox::OnMouseEnter(const cbMouseInput& Mouse)
{
	if (!IsFocusable() || !IsEnabled())
		return false;

	if (!IsInside(Mouse.MouseLocation))
		return false;

	if (IsItCulled())
		return false;

	if (!IsFocused())
		SetFocus(true);

	if (MenuSlot->IsInside(Mouse.MouseLocation))
		MenuSlot->OnMouseEnter(Mouse);
	else if (ListSlot->IsInside(Mouse.MouseLocation))
		ListSlot->OnMouseEnter(Mouse);

	return true;
}

bool cbComboBox::OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse)
{
	if (!IsFocused() || !IsEnabled())
		return false;

	ListSlot->OnMouseWheel(WheelDelta, Mouse);

	return true;
}

bool cbComboBox::OnMouseMove(const cbMouseInput& Mouse)
{
	if (!IsFocused() || !IsEnabled())
		return false;

	if (MenuSlot->IsPressed())
	{
		MenuSlot->OnMouseMove(Mouse);
	}
	else if (MenuSlot->IsInside(Mouse.MouseLocation))
	{
		if (ListSlot->IsFocused())
		{
			ListSlot->OnMouseLeave(Mouse);
		}
		if (MenuSlot->IsFocused())
			MenuSlot->OnMouseMove(Mouse);
		else
			MenuSlot->OnMouseEnter(Mouse);
	}
	else if (MenuSlot->IsFocused())
	{
		MenuSlot->OnMouseLeave(Mouse);
	}
	else if (IsInside(Mouse.MouseLocation))
	{
		if (ListSlot->IsFocused())
			ListSlot->OnMouseMove(Mouse);
		else
			ListSlot->OnMouseEnter(Mouse);
	}
	else if (ListSlot->IsFocused())
	{
		if (ListSlot->IsScrollBarHandlePressed())
			ListSlot->OnMouseMove(Mouse);
		else
			ListSlot->OnMouseLeave(Mouse);
	}

	return true;
}

bool cbComboBox::OnMouseButtonDoubleClick(const cbMouseInput& Mouse)
{
	if (!IsFocused() || !IsEnabled())
		return false;

	return true;
}

bool cbComboBox::OnMouseButtonUp(const cbMouseInput& Mouse)
{
	if (!IsFocused())
		return false;

	if (MenuSlot->IsPressed())
	{
		MenuSlot->OnMouseButtonUp(Mouse);
	}
	else if (ListSlot->IsFocused())
	{
		ListSlot->OnMouseButtonUp(Mouse);
	}

	return true;
}

bool cbComboBox::OnMouseButtonDown(const cbMouseInput& Mouse)
{
	if (!IsFocused() || !IsEnabled())
		return false;

	if (MenuSlot->IsInside(Mouse.MouseLocation))
		MenuSlot->OnMouseButtonDown(Mouse);
	else if (IsInside(Mouse.MouseLocation))
		ListSlot->OnMouseButtonDown(Mouse);
	else if (IsMenuOpen())
	{
		if (!IsInside(Mouse.MouseLocation))
		{
			SetIsMenuOpen(false, true);

			if (IsFocused())
				SetFocus(false);

			if (MenuSlot->IsFocused())
				MenuSlot->OnMouseLeave(Mouse);

			if (ListSlot->IsFocused())
				ListSlot->OnMouseLeave(Mouse);
		}
		return false;
	}

	return true;
}

bool cbComboBox::OnMouseLeave(const cbMouseInput& Mouse)
{
	if (!IsFocused())
		return false;

	if (IsMenuOpen())
	{
		//if (ListSlot->IsFocused())
		//	ListSlot->OnMouseLeave(Mouse);
		return false;
	}

	if (MenuSlot->IsPressed())
		return false;

	bool bIsSlotStillFocused = false;

	const auto& Slots = GetSlots();

	if (MenuSlot->IsFocused())
		if (!MenuSlot->OnMouseLeave(Mouse))
			bIsSlotStillFocused = true;

	if (ListSlot->IsFocused())
		if (!ListSlot->OnMouseLeave(Mouse))
			bIsSlotStillFocused = true;

	if (IsFocused() && !bIsSlotStillFocused)
		SetFocus(false);

	return !bIsSlotStillFocused;
}
