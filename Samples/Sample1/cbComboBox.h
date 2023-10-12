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
#pragma once

#include <functional>
#include <cbSlottedBox.h>
#include <cbOverlay.h>
#include <cbButton.h>
#include <cbString.h>
#include <cbSizeBox.h>

using namespace cbgui;

class cbComboBox : public cbgui::cbSlottedBox
{
	cbClassBody(cbClassConstructor, cbComboBox, cbgui::cbSlottedBox);
public:
	class cbComboBoxMenuInterface : public cbOverlay
	{
		cbClassBody(cbClassDefaultProtectedConstructor, cbComboBoxMenuInterface, cbOverlay);
	protected:
		cbComboBoxMenuInterface(const cbOverlay& Other, cbSlot* NewOwner = nullptr)
			: Super(Other, NewOwner)
		{}

	public:
		virtual void SelectOption(cbWidgetObj* Widget) = 0;

		virtual void OnOpen() {}
		virtual void OnClose() {}

		virtual bool IsPressed() { return false; }
		virtual bool IsMenuOpen() { return false; }

		virtual void HideMenuButton(bool Var) {}
	};

public:
	class cbComboBoxMenuSlot : public cbSlot
	{
		cbClassBody(cbClassConstructor, cbComboBoxMenuSlot, cbSlot)
		friend cbComboBox;
	public:
		class cbComboBoxNamedButtonInterface final : public cbComboBoxMenuInterface
		{
			cbClassBody(cbClassConstructor, cbComboBoxNamedButtonInterface, cbComboBoxMenuInterface);
		private:
			class cbComboBoxMenuButton : public cbButton
			{
				cbClassBody(cbClassConstructor, cbComboBoxMenuButton, cbButton);
			public:
				cbComboBoxMenuButton()
					: Super()
				{}

				cbComboBoxMenuButton(const cbComboBoxMenuButton& ComboBoxMenuButton, cbSlot* NewOwner)
					: Super(ComboBoxMenuButton, NewOwner)
				{}

				virtual ~cbComboBoxMenuButton() = default;

				virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override
				{
					return cbComboBoxMenuButton::Create(*this, NewOwner);
				}
			};

		private:
			cbComboBoxMenuButton::SharedPtr BTN;
			cbButtonVertexColorStyle BTNStyle;
			cbString::SharedPtr Text;

		public:
			cbComboBoxNamedButtonInterface()
			{
				SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

				BTN = cbComboBoxMenuButton::Create();
				BTN->SetDimension(cbDimension(1.0f, 1.0f));
				BTN->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				BTN->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				BTN->BindFunctionTo_OnClicked(std::bind(&cbComboBoxNamedButtonInterface::OnClick, this));
				BTN->BindFunctionTo_OnHovered(std::bind(&cbComboBoxNamedButtonInterface::OnHover, this));
				BTN->BindFunctionTo_OnUnhovered(std::bind(&cbComboBoxNamedButtonInterface::OnUnhover, this));
				Insert(BTN);
				Text = cbString::Create(" ");
				Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				Text->SetTextJustify(eTextJustify::Center);
				Text->SetVertexColorStyle(cbColor::Black());
				Text->SetFocusable(false);
				Insert(Text);
			}

			cbComboBoxNamedButtonInterface(const cbComboBoxNamedButtonInterface& Interface, cbSlot* NewOwner)
				: Super(Interface, NewOwner)
			{
				BTNStyle = Interface.BTNStyle;
				BTN = GetSlot(0)->GetSharedContent<cbComboBoxMenuButton>();
				Text = GetSlot(1)->GetSharedContent<cbString>();
			}

			virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override
			{
				return cbComboBoxNamedButtonInterface::Create(*this, NewOwner);
			}

			virtual ~cbComboBoxNamedButtonInterface()
			{
				BTN = nullptr;
				Text = nullptr;
			}

			virtual void SelectOption(cbWidgetObj* Widget) override
			{
				if (auto pText = cbgui::cbCast<cbString>(Widget))
				{
					Text->SetText(pText->GetText());
					Text->SetHeight(12);
					if (auto Root = GetRootOwner<cbComboBoxMenuSlot>(1, false))
						Root->OnSelected(Widget);
				}
			}

			void SetButtonVertexStyle(cbButtonVertexColorStyle Style)
			{
				BTNStyle = Style;
				BTN->SetVertexColorStyle(BTNStyle);
			}

			void SetTextVertexStyle(cbVertexColorStyle Style)
			{
				Text->SetVertexColorStyle(Style);
			}

			virtual void OnOpen() 
			{
				if (BTN->IsVisible())
					BTN->SetVertexColorStyle(BTNStyle.GetColor(eButtonState::Pressed));
			}

			virtual void OnClose()
			{
				if (BTN->IsVisible())
					BTN->SetVertexColorStyle(BTNStyle);
			}

			inline void SetText(std::u32string STR) { Text->SetText(STR); }
			inline void SetFontType(eFontType Type) { Text->SetFontType(Type); }
			inline void SetTextJustify(eTextJustify Justify) { Text->SetTextJustify(Justify); }
			inline void SetFontSize(std::size_t Size) { Text->SetFontSize(Size); Wrap(); }
			inline void SetTextPadding(cbMargin Padding) { Text->SetPadding(Padding); }
			inline void SetTextVerticalAlignment(eVerticalAlignment VA) { Text->SetVerticalAlignment(VA); }
			inline void SetTextHorizontalAlignment(eHorizontalAlignment HA) { Text->SetHorizontalAlignment(HA); }

			virtual void OnClick() { GetRootOwner<cbComboBoxMenuSlot>(2, false)->OnButtonClick(); }
			virtual void OnHover() { GetRootOwner<cbComboBoxMenuSlot>(2, false)->OnButtonHover(); }
			virtual void OnUnhover() { GetRootOwner<cbComboBoxMenuSlot>(2, false)->OnButtonUnhover(); }

			virtual bool IsPressed() override final { return BTN->IsPressed(); }
			virtual bool IsMenuOpen() { return GetRootOwner<cbComboBoxMenuSlot>(2, false)->IsMenuOpen(); }

			virtual void HideMenuButton(bool Var) { BTN->SetVisibilityState(Var ? eVisibility::Invisible : eVisibility::Visible); }
		};

	private:
		bool bIsInserted;
		std::shared_ptr<cbComboBoxMenuInterface> ComboBoxMenuInterface;
		std::shared_ptr<cbSizeBox> Content;

		void Inserted() { bIsInserted = true; OnInserted(); }
		virtual void OnInserted() { }

	public:
		cbComboBoxMenuSlot(cbComboBox* pOwner)
			: Super(pOwner)
			, bIsInserted(false)
			, Content(cbSizeBox::Create())
			, ComboBoxMenuInterface(cbComboBoxNamedButtonInterface::Create())
		{
			Content->SetContent(ComboBoxMenuInterface);
			Content->SetMinMaxWidth(110.0f);
			Content->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Content->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Content->AttachToSlot(this);
		}

		cbComboBoxMenuSlot(const cbComboBoxMenuSlot& Widget, cbSlottedBox* NewOwner)
			: Super(Widget, NewOwner)
			, bIsInserted(false)
			, Content(Widget.Content->Clone<cbSizeBox>())
			, ComboBoxMenuInterface(cbComboBoxNamedButtonInterface::Create())
		{
			Content->AttachToSlot(this);
		}

		virtual ~cbComboBoxMenuSlot()
		{
			ComboBoxMenuInterface = nullptr;
			Content = nullptr;
		}

		virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) override
		{
			return cbComboBoxMenuSlot::Create(*this, NewOwner);
		}

		cbComboBoxMenuInterface* GetMenuInterface() const { return ComboBoxMenuInterface.get(); }
	private:
		void OnButtonClick() { GetOwner<cbComboBox>()->SetIsMenuOpen(); }
		void OnButtonHover() {}
		void OnButtonUnhover() {}

		void OnSelected(cbWidgetObj* Widget) { GetOwner<cbComboBox>()->OnSelected(Widget); }

	public:
		virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }

		virtual void ReplaceContent(const cbWidget::SharedPtr& Content) { }

		void SetMenuMinMaxWidth(std::optional<float> Size) { Content->SetMinMaxWidth(Size); }
		void SetMenuMinMaxHeight(std::optional<float> Size) { Content->SetMinMaxHeight(Size); }
		void SetMenuMinWidth(std::optional<float> Size) { Content->SetMinimumWidth(Size); }
		void SetMenuMaxWidth(std::optional<float> Size) { Content->SetMaximumWidth(Size); }
		void SetMenuMinHeight(std::optional<float> Size) { Content->SetMaximumHeight(Size); }
		void SetMenuMaxHeight(std::optional<float> Size) { Content->SetMaximumHeight(Size); }

		void OnOpen() { ComboBoxMenuInterface->OnOpen(); }
		void OnClose() { ComboBoxMenuInterface->OnClose(); }

		bool IsPressed() { return ComboBoxMenuInterface->IsPressed(); }
		bool IsMenuOpen() { return GetOwner<cbComboBox>()->IsMenuOpen(); }

		void HideMenuButton(bool Var) { ComboBoxMenuInterface->HideMenuButton(Var); }
		void SelectOption(cbWidgetObj* Widget) { ComboBoxMenuInterface->SelectOption(Widget); }

		virtual bool HasContent() const override { return Content != nullptr; }
		virtual cbWidget::SharedPtr GetSharedContent() const override { return Content; }
		virtual cbWidget* GetContent() const override { return Content.get(); }

		virtual bool HasAnyChildren() const { return Content != nullptr; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const { return std::vector<cbWidgetObj*>{ Content.get() }; }

		virtual cbBounds GetBounds() const override { return cbBounds(GetDimension(), GetLocation()); }
		virtual cbVector GetLocation() const override { return GetOwner<cbComboBox>()->GetLocation(); }
		virtual cbDimension GetDimension() const override { return GetOwner<cbComboBox>()->GetSlotDimension(); }
		virtual float GetWidth() const override { return GetDimension().GetWidth(); }
		virtual float GetHeight() const override { return GetDimension().GetHeight(); }

		virtual void Enable() { Content->Enable(); }
		virtual void Disable() { Content->Disable(); }
		virtual void Hidden(bool value) { Content->SetVisibilityState(value ? eVisibility::Hidden : eVisibility::Visible); }

		virtual bool IsFocused() const override { return Content->IsFocused(); }
		virtual bool IsFocusable() const override { return Content->IsFocusable(); }
		virtual eFocusMode GetFocusMode() const override { return Content->GetFocusMode(); }

		inline void UpdateVerticalAlignment() { Content->UpdateVerticalAlignment(); }
		inline void UpdateHorizontalAlignment() { Content->UpdateHorizontalAlignment(); }
		void UpdateRotation() { Content->UpdateRotation(); }

		virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const { return Content->IsInteractableWithKey(KeyCode); }
		virtual bool OnKeyUp(std::uint64_t KeyCode) { return Content->OnKeyUp(KeyCode); }
		virtual bool OnKeyDown(std::uint64_t KeyCode) { return Content->OnKeyDown(KeyCode); }
		virtual bool IsInteractableWithMouse() const { return Content->IsInteractableWithMouse(); }
		virtual bool OnMouseEnter(const cbMouseInput& Mouse) { return Content->OnMouseEnter(Mouse); }
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) { return Content->OnMouseLeave(Mouse); }
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) { return Content->OnMouseWheel(WheelDelta, Mouse); }
		virtual bool OnMouseMove(const cbMouseInput& Mouse) { return Content->OnMouseMove(Mouse); }
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) { return Content->OnMouseButtonDoubleClick(Mouse); }
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) { return Content->OnMouseButtonUp(Mouse); }
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) { return Content->OnMouseButtonDown(Mouse); }
	};

public:
	cbComboBox(const cbgui::eOrientation Orientation = cbgui::eOrientation::Vertical);
	cbComboBox(const cbComboBox& Widget, cbSlot* NewOwner = nullptr);
public:
	virtual ~cbComboBox();

	virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override;

public:
	inline bool IsMenuOpen() const { return bIsOpen; }
	bool SetIsMenuOpen(const std::optional<bool> value = std::nullopt, const bool& Force = false);

	bool Open();
	bool Close();

	bool SelectItem(const std::size_t Index);
	cbSlot* HoverItem(const std::size_t Index);
	cbSlot* UnhoverOption(const std::size_t Index);
	cbSlot* GetCurrentHoveredSlot() const;
	std::int32_t GetCurrentHoveredSlotIndex() const;
	std::size_t GetOptionSize() const;
	cbSlot* GetOptionFromIndex(std::int32_t Index) const;

	template<typename T>
	T* GetMenuInterface() const { return static_cast<T*>(MenuSlot->GetMenuInterface()); }
	cbComboBoxMenuInterface* GetMenuInterface() const { return MenuSlot->GetMenuInterface(); }

	void SetScrollBox_BarVetexStyle(cbVertexColorStyle Style);
	void SetScrollBox_HandleVetexStyle(cbButtonVertexColorStyle Style);
	void SetOptionHoverVetexStyle(cbVertexColorStyle Style);
	void SetListBackgroundVertexStyle(cbVertexColorStyle Style);

private:
	void OnSelected(cbWidgetObj* Widget);

protected:
	virtual void OnOpen();
	virtual void OnClose();

private:
	void SelectSlot(cbSlot* Slot, cbWidget* Option);

	bool IsInsideButton(const cbVector& Location) const;
	bool IsContentFocused() const;
	bool IsInsideContent(const cbVector& Location) const;

	float GetContentVerticalAlignmentLocation();
	float GetContentHorizontalAlignmentLocation();

public:
	virtual bool IsInside(const cbVector& Location) const override final;

	inline void BindFunctionTo_OnSelectionChanged(std::function<void(cbSlot*, cbWidget*)> Function) { fOnSelectionChanged = Function; }
	inline void UnBindFunctionTo_OnSelectionChanged() { fOnSelectionChanged = nullptr; }
private:
	virtual void OnSelectionChanged(cbSlot* Slot, cbWidget* Option) { if (fOnSelectionChanged)fOnSelectionChanged(Slot, Option); }
	virtual void OnOpening() { }

public:
	inline cbComboBox* operator[](const cbWidget::SharedPtr& Widget)
	{
		Insert(Widget);
		return this;
	}

	void Insert(std::u32string Text, cbTextDesc Desc = cbTextDesc(12), cbVertexColorStyle Style = cbVertexColorStyle(cbColor::White()), const std::optional<std::size_t> Index = std::nullopt);
	void Insert(std::string Text, cbTextDesc Desc = cbTextDesc(12), cbVertexColorStyle Style = cbVertexColorStyle(cbColor::White()), const std::optional<std::size_t> Index = std::nullopt);
	void Insert(const cbWidget::SharedPtr& Option, const std::optional<std::size_t> Index = std::nullopt);
	void RemoveOption(std::int32_t Index);

	void HideMenuButton(bool Var);

	virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
	virtual float GetRotation() const override final { return Transform.GetRotation(); }
	virtual cbBounds GetBounds() const override final { return Transform.GetBounds(); }
	virtual cbMargin GetPadding() const override final { return Transform.GetPadding(); }
	virtual cbDimension GetDimension() const override final { return Transform.GetDimension(); }
	virtual float GetWidth() const override final { return Transform.GetWidth(); }
	virtual float GetHeight() const override final { return Transform.GetHeight(); }

	virtual float GetNonAlignedWidth() const override final { return Transform.GetNonAlignedWidth(); }
	virtual float GetNonAlignedHeight() const override final { return Transform.GetNonAlignedHeight(); }

	virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false) override final;
	virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height) override final;
	virtual void SetPadding(const cbMargin& Padding) override final;
	virtual void SetRotation(const float Rotation) override final;

	void SetMenuMinMaxWidth(std::optional<float> Size) { MenuSlot->SetMenuMinMaxWidth(Size); WrapOrUpdateAlignments(); }
	void SetMenuMinMaxHeight(std::optional<float> Size) { MenuSlot->SetMenuMinMaxHeight(Size); WrapOrUpdateAlignments(); }
	void SetMenuMinWidth(std::optional<float> Size) { MenuSlot->SetMenuMinWidth(Size); WrapOrUpdateAlignments(); }
	void SetMenuMaxWidth(std::optional<float> Size) { MenuSlot->SetMenuMaxWidth(Size); WrapOrUpdateAlignments(); }
	void SetMenuMinHeight(std::optional<float> Size) { MenuSlot->SetMenuMinHeight(Size); WrapOrUpdateAlignments(); }
	void SetMenuMaxHeight(std::optional<float> Size) { MenuSlot->SetMenuMaxHeight(Size); WrapOrUpdateAlignments(); }

	void SetListHeight(const float Height);

	inline eOrientation GetOrientation() const { return Orientation; }
	void SetOrientation(const eOrientation Orientation);

	virtual std::vector<cbSlot*> GetSlots() const override;
	virtual cbSlot* GetSlot(const std::size_t Index) const override final;
	virtual std::size_t GetSlotSize(const bool ExcludeHidden = false) const override final { return 2; }

	virtual std::vector<cbWidgetObj*> GetAllChildren() const override;
	virtual bool HasAnyChildren() const { return true; }

	virtual bool HasAnyComponents() const override final { return false; }
	virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>(); }

	virtual bool HasGeometry() const override final { return false; }
	virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final { return std::vector<cbGeometryVertexData>(); };
	virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final { return std::vector<std::uint32_t>(); };
	virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final { return cbGeometryDrawData("NONE", 0, 0, 0, 0); };

	cbSlot* GetOverlappingSlot(const cbBounds& Bounds) const;

protected:
	void WrapOrUpdateAlignments();

public:
	virtual void UpdateVerticalAlignment(const bool ForceAlign = false) override final;
	virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) override final;
	virtual void UpdateRotation() override final;
	virtual void UpdateStatus() override final;

	virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const override;
	virtual bool OnKeyUp(std::uint64_t KeyCode) override;
	virtual bool OnKeyDown(std::uint64_t KeyCode) override;
	virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
	virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;
	virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) override;
	virtual bool OnMouseMove(const cbMouseInput& Mouse) override;
	virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) override;
	virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) override;
	virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) override;

private:
	virtual cbDimension GetSlotDimension() const;

	float GetNextSlotLocation(const eVerticalAlignment& InVAlignment, const std::size_t& Index) const;
	float GetNextSlotLocation(const eHorizontalAlignment& InHAlignment, const std::size_t& Index) const;

	virtual void UpdateSlotVerticalAlignment() override final;
	virtual void UpdateSlotHorizontalAlignment() override final;

	virtual std::size_t GetSlotIndex(const cbSlot* Slot) const override final;

private:
	virtual bool WrapVertical() override final;
	virtual bool WrapHorizontal() override final;
	virtual bool UnWrapVertical() override final;
	virtual bool UnWrapHorizontal() override final;

	virtual void OnSlotVisibilityChanged(cbSlot* Slot);
	virtual void OnSlotDimensionUpdated(cbSlot* Slot);

	virtual bool OnRemoveSlot(cbSlot* Slot) override;

private:
	virtual void OnAttach() override final;
	virtual void OnRemoveFromParent() override final;

private:
	cbTransform Transform;
	eOrientation Orientation;
	bool bIsOpen;

	cbComboBoxMenuSlot::SharedPtr MenuSlot;

	class cbComboBoxListSlot;
	std::shared_ptr<cbComboBoxListSlot> ListSlot;

	std::function<void(cbSlot*, cbWidget*)> fOnSelectionChanged;
};
