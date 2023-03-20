/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Coşkun.
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
#include "DemoCanvas.h"
#include "cbComboBox.h"
#include "cbScaleBox.h"
#include "cbString.h"
#include <cbgui.h>

class cbLevelData
{
private:
	cbLevelData()
	{
		LevelCompletion.push_back(14);
		LevelCompletion.push_back(2);
		LevelCompletion.push_back(23);
		LevelCompletion.push_back(50);
		LevelCompletion.push_back(15);
		LevelCompletion.push_back(100);
		LevelCompletion.push_back(75);
		LevelCompletion.push_back(90);
	}
	cbLevelData(const cbLevelData&) = delete;
	cbLevelData(cbLevelData&&) = delete;
	cbLevelData& operator=(const cbLevelData&) = delete;
	cbLevelData& operator=(cbLevelData&&) = delete;

public:
	~cbLevelData() = default;

	std::vector<int> LevelCompletion;

	static cbLevelData& Get()
	{
		static cbLevelData instance;
		return instance;
	}
};

class DemoCanvas::cbStartScreen : public cbgui::cbVerticalBox
{
	cbString::SharedPtr Text;
	cbString::SharedPtr Header;
	cbgui::cbBorder::SharedPtr Border;
	cbgui::cbColor Color;
	int Modifier;
	bool bWait;
	float Time;
	bool bStartScreenFadeOut;
public:
	cbStartScreen(cbDimension Dimension)
		: cbVerticalBox()
		, Color(cbgui::cbColor::White())
		, Modifier(1)
		, bWait(false)
		, bStartScreenFadeOut(false)
		, Time(0.0f)
	{
		{
			cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
			Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Empty->SetVerticalAlignment(eVerticalAlignment::Align_Top);
			Empty->SetHeight((Dimension.GetHeight() / 3));
			Insert(Empty, eSlotAlignment::BoundToContent);
		}
		{
			Border = cbBorder::Create();
			Border->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
			Border->SetVerticalAlignment(eVerticalAlignment::Align_Top);
			Border->SetThickness(cbMargin(5));
			Insert(Border, eSlotAlignment::BoundToContent);
			cbTextDesc Desc;
			Desc.CharSize = 144 /*+ 24*/;
			Desc.FontType = cbgui::eFontType::Bold;
			Header = cbString::Create("SAMPLE 1", Desc);
			Header->Wrap();
			Header->SetDefaultSpaceWidth(3);
			Header->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
			Header->SetVerticalAlignment(eVerticalAlignment::Align_Center);
			Header->SetPadding(cbMargin(0, 0, 5, 0));
			//Header->SetLocationY(Dimension.GetHeight() / 2 - 56);
			//Header->SetLocationX(Dimension.GetWidth()/ 2);
			Border->SetContent(Header);
		}
		{
			cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
			Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Empty->SetVerticalAlignment(eVerticalAlignment::Align_Top);
			Empty->SetHeight((Dimension.GetHeight() / 6));
			Insert(Empty, eSlotAlignment::BoundToContent);
		}
		Text = cbString::Create("PRESS [Enter] TO PLAY");
		Text->SetDefaultSpaceWidth(3);
		Text->Wrap();
		Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
		Text->SetVerticalAlignment(eVerticalAlignment::Align_Top);
		//Text->SetLocationY(Dimension.GetHeight() - (Dimension.GetHeight() / 3));
		//Text->SetLocationX(Dimension.GetWidth()) / 2);
		Insert(Text, eSlotAlignment::BoundToContent);
	}

	virtual ~cbStartScreen()
	{
		Border = nullptr;
		Text = nullptr;
		Header = nullptr;
	}

	bool IsStartScreenFadeOut() const { return bStartScreenFadeOut; }

	virtual void OnTick(float DeltaTime) override
	{
		if (!IsEnabled())
			return;

		if (!bWait)
		{
			Color.A -= DeltaTime * Modifier;
			if (bStartScreenFadeOut)
			{
				Border->SetVertexColorStyle(Color);
				Header->SetVertexColorStyle(Color);
			}
			else
				Text->SetVertexColorStyle(Color);
		}

		if (bWait)
			Time += DeltaTime;

		if (!bWait && Color.A <= 0.0f)
		{
			if (bStartScreenFadeOut)
			{
				SetVisibilityState(eVisibility::Hidden);
				Disable();
				GetCanvas<DemoCanvas>()->StartScreenFadeOut();
				return;
			}

			Color.A = 0.0f;
			Modifier = -2;

			bWait = false;
			Time = 0.0f;
		}
		else if (!bWait && Color.A >= 1.0f)
		{
			Color.A = 1.0f;
			Modifier = 1;

			bWait = true;
			Time = 0.0f;
		}

		if (Time >= 0.5f)
		{
			bWait = false;
			Time = 0.0f;
		}
	}

	void FadeOut()
	{
		bStartScreenFadeOut = true;

		{
			Color.A = 1.0f;

			Text->SetVisibilityState(eVisibility::Hidden);
			Text->Disable();
		}
	}
};

class cbAutoScroller : public cbSizeBox
{
	cbClassBody(cbClassConstructor, cbAutoScroller, cbSizeBox)
public:
	cbScrollBox::SharedPtr Scrollbox;
	cbButton::SharedPtr LeftButton;
	cbButton::SharedPtr RightButton;

protected:
	bool bIsLeftButtonHovered;
	bool bIsRightButtonHovered;

public:
	cbAutoScroller(eOrientation Orientation = eOrientation::Vertical)
		: Super()
		, bIsLeftButtonHovered(false)
		, bIsRightButtonHovered(false)
	{
		cbOverlay::SharedPtr Overlay = cbOverlay::Create();
		Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		SetContent(Overlay);

		if (Orientation == eOrientation::Horizontal)
		{
			Scrollbox = cbScrollBox::Create(eOrientation::Horizontal);
			Scrollbox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Scrollbox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Overlay->Insert(Scrollbox);

			{
				LeftButton = cbButton::Create();
				LeftButton->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				LeftButton->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				LeftButton->SetVisibilityState(eVisibility::Invisible);
				LeftButton->BindFunctionTo_OnHovered(std::bind(&cbAutoScroller::LeftBTNHover, this));
				LeftButton->BindFunctionTo_OnUnhovered(std::bind(&cbAutoScroller::LeftBTNUnhover, this));
				LeftButton->SetFocusMode(eFocusMode::Immediate);
				Overlay->Insert(LeftButton);
			}
			{
				RightButton = cbButton::Create();
				RightButton->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				RightButton->SetHorizontalAlignment(eHorizontalAlignment::Align_Right);
				RightButton->SetVisibilityState(eVisibility::Invisible);
				RightButton->BindFunctionTo_OnHovered(std::bind(&cbAutoScroller::RightBTNHover, this));
				RightButton->BindFunctionTo_OnUnhovered(std::bind(&cbAutoScroller::RightBTNUnhover, this));
				RightButton->SetFocusMode(eFocusMode::Immediate);
				Overlay->Insert(RightButton);
			}
		}
		else if (Orientation == eOrientation::Vertical)
		{
			Scrollbox = cbScrollBox::Create(eOrientation::Vertical);
			Scrollbox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Scrollbox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Overlay->Insert(Scrollbox);

			{
				LeftButton = cbButton::Create();
				LeftButton->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				LeftButton->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				LeftButton->SetVisibilityState(eVisibility::Invisible);
				LeftButton->BindFunctionTo_OnHovered(std::bind(&cbAutoScroller::LeftBTNHover, this));
				LeftButton->BindFunctionTo_OnUnhovered(std::bind(&cbAutoScroller::LeftBTNUnhover, this));
				LeftButton->SetFocusMode(eFocusMode::Immediate);
				Overlay->Insert(LeftButton);
			}
			{
				RightButton = cbButton::Create();
				RightButton->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
				RightButton->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				RightButton->SetVisibilityState(eVisibility::Invisible);
				RightButton->BindFunctionTo_OnHovered(std::bind(&cbAutoScroller::RightBTNHover, this));
				RightButton->BindFunctionTo_OnUnhovered(std::bind(&cbAutoScroller::RightBTNUnhover, this));
				RightButton->SetFocusMode(eFocusMode::Immediate);
				Overlay->Insert(RightButton);
			}
		}
	}

	virtual ~cbAutoScroller()
	{
		Scrollbox = nullptr;
		LeftButton = nullptr;
		RightButton = nullptr;
	}

	void Insert(cbWidget::SharedPtr Widget)
	{
		Scrollbox->Insert(Widget);
	}

	virtual void OnTick(float DT) override
	{
		if (bIsLeftButtonHovered)
		{
			auto Percent = Scrollbox->GetPercent();			
			if (!Scrollbox->IsHandlePressed() && Percent > 0.0f)
			{
				Scrollbox->Scroll(Percent - DT);
			}
		}
		else if (bIsRightButtonHovered)
		{
			auto Percent = Scrollbox->GetPercent();
			if (!Scrollbox->IsHandlePressed() && Percent < 1.0f)
			{
				Scrollbox->Scroll(Percent + DT);
			}
		}
	}

	void ResetScrollBar()
	{
		Scrollbox->ResetScrollBar();
		bIsLeftButtonHovered = false;
		bIsRightButtonHovered = false;
	}

	void LeftBTNHover()
	{
		bIsLeftButtonHovered = true;
	}

	void LeftBTNUnhover()
	{
		bIsLeftButtonHovered = false;
	}

	void RightBTNHover()
	{
		bIsRightButtonHovered = true;
	}

	void RightBTNUnhover()
	{
		bIsRightButtonHovered = false;
	}
};

struct cbContainerVariable {};

class cbDemoButtonBase : public cbOverlay
{
	cbClassBody(cbClassConstructor, cbDemoButtonBase, cbOverlay)
protected:
	bool bIsSelected;

public:
	cbButton::SharedPtr Button;
	std::function<void(cbDemoButtonBase*)> fOnSelected;
	cbContainerVariable* Container;

public:
	cbDemoButtonBase()
		: bIsSelected(false)
		, Container(nullptr)
	{
		cbButtonVertexColorStyle ColorStyle;
		ColorStyle.SetDefaultColor(cbColor::Black());
		ColorStyle.SetHoveredColor(cbColor::Red());
		ColorStyle.SetPressedColor(cbColor::Red());
		ColorStyle.SetDisabledColor(cbColor::Black());

		Button = cbButton::Create();
		Button->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Button->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Button->SetVertexColorStyle(ColorStyle);
		Button->BindFunctionTo_OnClicked(std::bind(&cbDemoButtonBase::Select, this));
		Button->BindFunctionTo_OnHovered(std::bind(&cbDemoButtonBase::Hover, this));
		Button->BindFunctionTo_OnUnhovered(std::bind(&cbDemoButtonBase::Unhover, this));
		Insert(Button);
	}

	virtual ~cbDemoButtonBase()
	{
		Button = nullptr;
		fOnSelected = nullptr;

		if (Container)
		{
			delete Container;
			Container = nullptr;
		}
	}

	virtual bool OnMouseButtonUp(const cbMouseInput& Mouse)
	{
		return Super::OnMouseButtonUp(Mouse);
	}

	virtual bool OnMouseButtonDown(const cbMouseInput& Mouse)
	{
		return Super::OnMouseButtonDown(Mouse);
	}

	inline void HideButtonGeometry(bool value) { Button->SetVisibilityState(value ? eVisibility::Invisible : eVisibility::Visible); }
	inline cbDimension GetButtonDimension() const { return Button->GetDimension(); }
	inline void SetButtonVerticalAlignment(eVerticalAlignment VA) { Button->SetVerticalAlignment(VA); }
	inline void SetButtonHorizontalAlignment(eHorizontalAlignment HA) { Button->SetHorizontalAlignment(HA); }
	inline void SetButtonVertexStyle(cbButtonVertexColorStyle Style) const { Button->SetVertexColorStyle(Style); }
	inline void SetButtonVertexStyle(const eButtonState State, const cbColor& Color) const { Button->SetVertexColorStyle(State, Color); }
	inline void SetButtonWidth(float Width) const { Button->SetWidth(Width); }
	inline void SetButtonHeight(float Height) const { Button->SetHeight(Height); }
	inline void SetButtonPadding(cbMargin Padding) const { Button->SetPadding(Padding); }
	inline void ResetButtonStatus() { Button->Reset();	}

	virtual void OnFocus() override
	{
	}

	virtual void OnLostFocus() override
	{
	}

	virtual std::size_t GetIndex() const
	{
		if (HasOwner())
			return GetOwner<cbSlot>()->GetIndex();
		return 0;
	}

	bool IsSelected() const
	{
		return bIsSelected;
	}

	void Select()
	{
		bIsSelected = true;
		OnSelected();
	}

	void Deselect()
	{
		bIsSelected = false;
		OnDeSelected();
	}

	void Hover()
	{
		OnHovered();
	}

	void Unhover()
	{
		OnUnhovered();
	}

	virtual void OnHovered()
	{
	}

	virtual void OnUnhovered()
	{
	}

	virtual void OnSelected()
	{
		if (fOnSelected)
			fOnSelected(this);
	}

	virtual void OnDeSelected()
	{
	}
};

class cbNamedButton : public cbDemoButtonBase
{
	cbClassBody(cbClassConstructor, cbNamedButton, cbDemoButtonBase)
public:
	cbHorizontalBox::SharedPtr HB;
	cbString::SharedPtr Text;

public:
	cbNamedButton(std::u32string STR)
		: Super()
	{
		HB = cbHorizontalBox::Create();
		HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		HB->SetFocusable(false);
		Insert(HB);
		{
			cbTextDesc Desc;
			Desc.CharSize = 16;
			Desc.FontType = eFontType::Bold;
			Text = cbString::Create(STR, Desc);
			Text->SetDefaultSpaceWidth(3);
			Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
			Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
			Text->SetFocusable(false);
			Text->Wrap();
			HB->Insert(Text);
		}
		//Wrap();
	}

	virtual ~cbNamedButton()
	{
		HB = nullptr;
		Text = nullptr;
	}

	inline void SetText(std::u32string STR) { Text->SetText(STR); }
	inline void SetTextJustify(eTextJustify Justify) { Text->SetTextJustify(Justify); }
	inline void SetFontType(eFontType Type) { Text->SetFontType(Type); }
	inline void SetFontSize(std::size_t Size) { Text->SetFontSize(Size); }
	inline void SetTextPadding(cbMargin Padding) { Text->SetPadding(Padding); }
	inline void SetTextVerticalAlignment(eVerticalAlignment VA) { Text->SetVerticalAlignment(VA); }
	inline void SetTextHorizontalAlignment(eHorizontalAlignment HA) { Text->SetHorizontalAlignment(HA); }
	inline void SetTextSlotAlignment(eSlotAlignment SA) { HB->SetSlotAlignment(SA, 0); }
};

class cbMenuButton : public cbNamedButton
{
	cbClassBody(cbClassConstructor, cbMenuButton, cbNamedButton)
private:
	std::function<void(cbMenuButton*)> fCallBack;
public:
	cbMenuButton(std::u32string Name, std::function<void(cbMenuButton*)> pfCallBack = nullptr)
		: Super(Name)
		, fCallBack(pfCallBack)
	{
		cbButtonVertexColorStyle ColorStyle;
		ColorStyle.SetDefaultColor(cbColor::Transparent());
		ColorStyle.SetHoveredColor(cbColor::Red());
		ColorStyle.SetPressedColor(cbColor::Red());
		ColorStyle.SetDisabledColor(cbColor::Transparent());
		SetButtonVertexStyle(ColorStyle);
	}
	virtual ~cbMenuButton() = default;

	virtual void OnHovered() override
	{
	}

	virtual void OnSelected() override
	{
		if (!IsSelected())
			return;

		if (fCallBack)
			fCallBack(this);

		SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
	}

	virtual void OnDeSelected() override
	{
		if (IsSelected())
			return;

		SetButtonVertexStyle(eButtonState::Default, cbColor::Transparent());
	}

	virtual void OnUnhovered() override
	{
	}
};

class cbPopupMenuBase : public cbOverlay
{
	cbClassBody(cbClassConstructor, cbPopupMenuBase, cbOverlay)
protected:
	class cbPopupButton : public cbNamedButton
	{
		cbClassBody(cbClassConstructor, cbPopupButton, cbNamedButton)
	private:
		cbPopupMenuBase* MenuOwner;
	public:
		cbString::SharedPtr EmojiString;
	public:
		cbPopupButton(cbPopupMenuBase* pMenuOwner, std::u32string Name, const char32_t& Char)
			: Super(Name)
			, MenuOwner(pMenuOwner)
		{
			cbSizeBox::SharedPtr SB = cbSizeBox::Create();
			SB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			SB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SB->SetMinimumWidth(24.0f);
			SB->SetPadding(cbMargin(10, 0, 0, 0));
			SB->SetFocusable(false);
			{
				cbTextDesc Desc;
				Desc.CharSize = 16;
				Desc.FontType = eFontType::Bold;
				EmojiString = cbString::Create("", Desc);
				EmojiString->SetDefaultSpaceWidth(3);
				EmojiString->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				EmojiString->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
				EmojiString->AddChar(Char);
				EmojiString->SetFocusable(false);
				EmojiString->Wrap();
				SB->SetContent(EmojiString);
			}
			HB->Insert(SB, 0);

			cbButtonVertexColorStyle ColorStyle;
			ColorStyle.SetDefaultColor(cbColor::Transparent());
			ColorStyle.SetHoveredColor(cbColor::Red());
			ColorStyle.SetPressedColor(cbColor::Red());
			ColorStyle.SetDisabledColor(cbColor::Transparent());

			SetButtonVertexStyle(ColorStyle);
		}
		virtual ~cbPopupButton()
		{
			MenuOwner = nullptr;
			EmojiString = nullptr;
		}

		virtual void OnHovered() override
		{
			MenuOwner->OnOptionHovered(this);
		}

		virtual void OnSelected() override
		{
			MenuOwner->OnOptionSelectd(this);
		}
	};

public:
	cbPopupMenuBase()
		: Super()
	{
	}

	virtual ~cbPopupMenuBase()
	{
	}

	virtual void OnTick(float DT) override
	{
	}

	virtual void OnAttachToCanvas() override
	{
	}

	virtual void OnRemoveFromCanvas() override
	{
	}

	virtual void OnHovered()
	{
	}

	virtual void OnSelected()
	{
	}

private:
	virtual void OnOptionHovered(cbPopupButton* BTN)
	{

	}

	virtual void OnOptionSelectd(cbPopupButton* BTN)
	{
		RemoveFromCanvas();
	}
};

class cbPopupMenuMessageBase : public cbPopupMenuBase
{
	cbClassBody(cbClassConstructor, cbPopupMenuMessageBase, cbPopupMenuBase)
protected:
	cbOverlay::SharedPtr Menu;
	cbVerticalBox::SharedPtr VB;

public:
	cbPopupMenuMessageBase(std::u32string Header, std::u32string Description, cbTextDesc HeaderDesc = cbTextDesc(26), cbTextDesc DescriptionDesc = cbTextDesc(12))
		: Super()
	{
		SetZOrderMode(eZOrderMode::Latest);
		SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		SetAlignToCanvas(true);

		{
			cbgui::cbImage::SharedPtr Image = cbImage::Create();
			Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Image->SetVertexColorStyle(cbColor(0.0f, 0.0f, 0.0, 0.5f));
			Insert(Image);
		}

		{
			Menu = cbOverlay::Create();
			Menu->SetVerticalAlignment(eVerticalAlignment::Align_Center);
			Menu->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
			{
				VB = cbVerticalBox::Create();
				VB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				VB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				{
					cbOverlay::SharedPtr Overlay_Text = cbOverlay::Create();
					Overlay_Text->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Overlay_Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					VB->Insert(Overlay_Text);
					{
						cbgui::cbImage::SharedPtr Image = cbImage::Create();
						Image->SetVertexColorStyle(cbColor(45, 48, 57));
						Image->SetDimension(cbDimension(534.0f, 185));
						Overlay_Text->Insert(Image);
					}
					{
						cbVerticalBox::SharedPtr VB_Text = cbVerticalBox::Create();
						VB_Text->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						VB_Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						{
							cbString::SharedPtr Text = cbString::Create(Header, HeaderDesc);
							Text->SetDefaultSpaceWidth(3);
							Text->SetFontType(eFontType::Bold);
							Text->SetVerticalAlignment(eVerticalAlignment::Align_Top);
							Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							Text->SetPadding(cbMargin(10, 24, 0, 0));
							VB_Text->Insert(Text);
						}
						{
							cbString::SharedPtr Text = cbString::Create(Description, DescriptionDesc);
							Text->SetDefaultSpaceWidth(3);
							Text->SetVerticalAlignment(eVerticalAlignment::Align_Top);
							Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							Text->SetPadding(cbMargin(10, 24, 0, 0));
							VB_Text->Insert(Text);
						}
						Overlay_Text->Insert(VB_Text);
					}
				}
				Menu->Insert(VB);
			}
			Insert(Menu);
		}
	}

	virtual ~cbPopupMenuMessageBase()
	{
		Menu = nullptr;
		VB = nullptr;
	}

	virtual void OnTick(float DT) override
	{
		Super::OnTick(DT);
	}

	virtual void OnAttachToCanvas() override
	{
	}

	virtual void OnRemoveFromCanvas() override
	{
	}

	virtual void OnHovered()
	{
	}

	virtual void OnSelected()
	{
	}

private:
	virtual void OnOptionHovered(cbPopupButton* BTN)
	{

	}

	virtual void OnOptionSelectd(cbPopupButton* BTN)
	{
		RemoveFromCanvas();
	}
};

class cbPopupMenu : public cbPopupMenuMessageBase
{
	cbClassBody(cbClassConstructor, cbPopupMenu, cbPopupMenuMessageBase)
private:
	cbPopupButton::SharedPtr CONFIRM_BTN;
	cbPopupButton::SharedPtr CANCEL_BTN;

public:
	std::function<void()> fCallBack_CONFIRM;
	std::function<void()> fCallBack_CANCEL;

public:
	cbPopupMenu(std::u32string Header, std::u32string Description, cbTextDesc HeaderDesc = cbTextDesc(26), cbTextDesc DescriptionDesc = cbTextDesc(12))
		: Super(Header, Description, HeaderDesc, DescriptionDesc)
	{
		{
			cbOverlay::SharedPtr Overlay_CONFIRM = cbOverlay::Create();
			Overlay_CONFIRM->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Overlay_CONFIRM->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Overlay_CONFIRM->SetPadding(cbMargin(0, 10, 0, 0));
			VB->Insert(Overlay_CONFIRM);
			{
				cbgui::cbImage::SharedPtr Image = cbImage::Create();
				Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Image->SetVertexColorStyle(cbColor(45, 48, 57));
				Image->SetDimension(cbDimension(534.0f, 10));
				Overlay_CONFIRM->Insert(Image);
			}
			{
				CONFIRM_BTN = cbPopupButton::Create(this, U"CONFIRM", char32_t(10145));
				CONFIRM_BTN->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				CONFIRM_BTN->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				CONFIRM_BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				CONFIRM_BTN->SetTextJustify(eTextJustify::Left);
				CONFIRM_BTN->SetTextPadding(cbMargin(5, 0, 0, 0));
				Overlay_CONFIRM->Insert(CONFIRM_BTN);
			}
		}
		{
			cbOverlay::SharedPtr Overlay_CANCEL = cbOverlay::Create();
			Overlay_CANCEL->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Overlay_CANCEL->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Overlay_CANCEL->SetPadding(cbMargin(0, 10, 0, 0));
			VB->Insert(Overlay_CANCEL);
			{
				cbgui::cbImage::SharedPtr Image = cbImage::Create();
				Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Image->SetVertexColorStyle(cbColor(45, 48, 57));
				Image->SetDimension(cbDimension(534.0f, 10));
				Overlay_CANCEL->Insert(Image);
			}
			{
				CANCEL_BTN = cbPopupButton::Create(this, U"CANCEL", char32_t(10799));
				CANCEL_BTN->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				CANCEL_BTN->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				CANCEL_BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				CANCEL_BTN->SetTextJustify(eTextJustify::Left);
				CANCEL_BTN->SetTextPadding(cbMargin(5, 0, 0, 0));
				Overlay_CANCEL->Insert(CANCEL_BTN);
			}
		}
	}

	virtual ~cbPopupMenu()
	{
		Menu = nullptr;
		CONFIRM_BTN = nullptr;
		CANCEL_BTN = nullptr;
		fCallBack_CONFIRM = nullptr;
		fCallBack_CANCEL = nullptr;
	}

	virtual void OnTick(float DT) override
	{
		Super::OnTick(DT);
	}

	virtual void OnAttachToCanvas() override
	{
		CONFIRM_BTN->ResetButtonStatus();
		CANCEL_BTN->ResetButtonStatus();
	}

	virtual void OnRemoveFromCanvas() override
	{
		CONFIRM_BTN->ResetButtonStatus();
		CANCEL_BTN->ResetButtonStatus();
	}

	virtual void OnHovered()
	{
	}

	virtual void OnSelected()
	{
	}

private:
	virtual void OnOptionHovered(cbPopupButton* BTN)
	{

	}

	virtual void OnOptionSelectd(cbPopupButton* BTN)
	{
		if (CONFIRM_BTN.get() == BTN)
		{
			CANCEL_BTN->Deselect();
			CONFIRM_BTN->Deselect();
			if (fCallBack_CONFIRM)
				fCallBack_CONFIRM();
		}
		else if (CANCEL_BTN.get() == BTN)
		{
			CONFIRM_BTN->Deselect();
			CANCEL_BTN->Deselect();
			if (fCallBack_CANCEL)
				fCallBack_CANCEL();
		}
		RemoveFromCanvas();
	}
};

class cbWarningPopupMenu : public cbPopupMenuMessageBase
{
	cbClassBody(cbClassConstructor, cbWarningPopupMenu, cbPopupMenuMessageBase)
private:
	cbPopupButton::SharedPtr OK_BTN;

public:
	std::function<void()> fCallBack_OK;

public:
	cbWarningPopupMenu(std::u32string Header, std::u32string Description, cbTextDesc HeaderDesc = cbTextDesc(26), cbTextDesc DescriptionDesc = cbTextDesc(12))
		: Super(Header, Description, HeaderDesc, DescriptionDesc)
	{
		{
			cbOverlay::SharedPtr Overlay_CANCEL = cbOverlay::Create();
			Overlay_CANCEL->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Overlay_CANCEL->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Overlay_CANCEL->SetPadding(cbMargin(0, 10, 0, 0));
			VB->Insert(Overlay_CANCEL);
			{
				cbgui::cbImage::SharedPtr Image = cbImage::Create();
				Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Image->SetVertexColorStyle(cbColor(45, 48, 57));
				Image->SetDimension(cbDimension(534.0f, 10));
				Overlay_CANCEL->Insert(Image);
			}
			{
				OK_BTN = cbPopupButton::Create(this, U"OK", char32_t(10799));
				OK_BTN->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				OK_BTN->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				OK_BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				OK_BTN->SetTextJustify(eTextJustify::Left);
				OK_BTN->SetTextPadding(cbMargin(5, 0, 0, 0));
				Overlay_CANCEL->Insert(OK_BTN);
			}
		}
	}

	virtual ~cbWarningPopupMenu()
	{
		Menu = nullptr;
		OK_BTN = nullptr;
		fCallBack_OK = nullptr;
	}

	virtual void OnTick(float DT) override
	{
		Super::OnTick(DT);
	}

	virtual void OnAttachToCanvas() override
	{
		OK_BTN->ResetButtonStatus();
	}

	virtual void OnRemoveFromCanvas() override
	{
		OK_BTN->ResetButtonStatus();
	}

	virtual void OnHovered()
	{
	}

	virtual void OnSelected()
	{
	}

private:
	virtual void OnOptionHovered(cbPopupButton* BTN) override
	{

	}

	virtual void OnOptionSelectd(cbPopupButton* BTN) override
	{
		if (OK_BTN.get() == BTN)
		{
			OK_BTN->Deselect();
			if (fCallBack_OK)
				fCallBack_OK();
		}
		RemoveFromCanvas();
	}
};

class cbMultiStringBox : public cbDemoButtonBase
{
	cbClassBody(cbClassConstructor, cbMultiStringBox, cbDemoButtonBase)
public:
	cbOverlay::SharedPtr Text_Overlay;
	cbVerticalBox::SharedPtr Text_VB;
	cbHorizontalBox::SharedPtr Text_HB;
	cbString::SharedPtr Text1;
	cbString::SharedPtr Text2;

public:
	cbMultiStringBox(std::u32string Str1, std::u32string Str2)
		: Super()
	{
		SetDimension(cbDimension(100.0f, 40.0f));

		Text_VB = cbVerticalBox::Create();
		Text_VB->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
		Text_VB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Text_VB->SetFocusable(false);

		Text_Overlay = cbOverlay::Create();
		Text_Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Text_Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Text_Overlay->SetFocusable(false);
		Text_VB->Insert(Text_Overlay);

		{
			Text_HB = cbHorizontalBox::Create();
			Text_HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Text_HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Text_HB->SetPadding(cbMargin(10));
			Text_HB->SetFocusable(false);
			{
				{
					cbVerticalBox::SharedPtr VerticalBox = cbVerticalBox::Create();
					VerticalBox->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					VerticalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					VerticalBox->SetPadding(cbMargin(10, 0, 0, 0));
					{
						Text1 = cbString::Create(Str1, cbTextDesc(8));
						Text1->SetDefaultSpaceWidth(3);
						Text1->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Text1->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Text1->SetVertexColorStyle(cbColor::Black());
						Text1->SetPadding(cbMargin(0, 4, 0, -4));
						VerticalBox->Insert(Text1);
					}
					{
						cbTextDesc Desc(12);
						Desc.FontType = eFontType::Bold;
						Text2 = cbString::Create(Str2, Desc);
						Text2->SetDefaultSpaceWidth(3);
						Text2->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Text2->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Text2->SetVertexColorStyle(cbColor::Black());
						VerticalBox->Insert(Text2);
					}
					Text_HB->Insert(VerticalBox);
				}
			}
			Text_Overlay->Insert(Text_HB);
		}
		Insert(Text_VB);
	}

	virtual ~cbMultiStringBox()
	{
		Text_VB = nullptr;
		Text_Overlay = nullptr;
		Text_HB = nullptr;
		Text1 = nullptr;
		Text2 = nullptr;
	}
};

class cbImageInfo : public cbMultiStringBox
{
	cbClassBody(cbClassConstructor, cbImageInfo, cbMultiStringBox)
public:
	cbImage::SharedPtr BG_Image;
	cbString::SharedPtr EmojiText;
	cbgui::cbImage::SharedPtr EmojiImage;
	bool bIsSelected;

public:
	cbImageInfo(std::u32string Str1, std::u32string Str2, const char32_t Emoji)
		: cbMultiStringBox(Str1, Str2)
		, bIsSelected(false)
	{
		HideButtonGeometry(true);
		BG_Image = cbImage::Create();
		BG_Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		BG_Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		BG_Image->SetVertexColorStyle(cbColor(1.0f, 1.0f, 1.0f, 0.8f));
		Text_Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
		Text_Overlay->Insert(BG_Image, 0);
		{
			cbOverlay::SharedPtr pOverlay = cbOverlay::Create();
			pOverlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			pOverlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			{
				EmojiImage = cbImage::Create();
				EmojiImage->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				EmojiImage->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				EmojiImage->SetVertexColorStyle(cbColor::White());
				pOverlay->Insert(EmojiImage);
			}
			{
				EmojiText = cbString::Create();
				EmojiText->AddChar(Emoji);
				EmojiText->SetDefaultSpaceWidth(3);
				EmojiText->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				EmojiText->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				EmojiText->SetVertexColorStyle(cbColor::Black());
				EmojiText->SetTextJustify(eTextJustify::Center);
				pOverlay->Insert(EmojiText);
			}
			Text_HB->Insert(pOverlay, 0);
		}
	}

	virtual ~cbImageInfo()
	{
		BG_Image = nullptr;
		EmojiText = nullptr;
		EmojiImage = nullptr;
	}

	bool IsSelected() const { return bIsSelected; }

	void HideBackground(bool value)
	{
		BG_Image->SetVisibilityState(value ? eVisibility::Hidden : eVisibility::Visible);
	}

	virtual void Select()
	{
		if (bIsSelected)
			return;
		bIsSelected = true;

		if (BG_Image)
			BG_Image->SetVertexColorStyle(cbColor(1.0f, 0.0f, 0.0f, 1.0f));
		if (EmojiText)
			EmojiText->SetVertexColorStyle(cbColor::White());
		if (EmojiImage)
			EmojiImage->SetVertexColorStyle(cbColor::Black());
		if (Text1)
			Text1->SetVertexColorStyle(cbColor::White());
		if (Text2)
			Text2->SetVertexColorStyle(cbColor::White());
	}
	virtual void Deselect()
	{
		if (!bIsSelected)
			return;
		bIsSelected = false;

		if (BG_Image)
			BG_Image->SetVertexColorStyle(cbColor(1.0f, 1.0f, 1.0f, 0.8f));
		if (EmojiText)
			EmojiText->SetVertexColorStyle(cbColor::Black());
		if (EmojiImage)
			EmojiImage->SetVertexColorStyle(cbColor::White());
		if (Text1)
			Text1->SetVertexColorStyle(cbColor::Black());
		if (Text2)
			Text2->SetVertexColorStyle(cbColor::Black());
	}
};

class cbTargetInfo : public cbPopupMenuBase
{
	cbClassBody(cbClassConstructor, cbTargetInfo, cbPopupMenuBase)
private:
	cbPopupButton::SharedPtr CANCEL_BTN;

public:
	std::function<void()> fCallBack_CANCEL;

public:
	cbTargetInfo()
		: Super()
	{
		SetAlignToCanvas(true);
		SetZOrderMode(eZOrderMode::Latest);
		SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

		{
			auto Image = cbImage::Create();
			Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Image->SetVertexColorStyle(cbColor(0.0f, 0.0f, 0.0f, 0.8f));
			Insert(Image);
		}

		cbSizeBox::SharedPtr Size = cbSizeBox::Create();
		Size->SetVerticalAlignment(eVerticalAlignment::Align_Center);
		Size->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
		Size->SetMinMaxWidth(768.0f);
		Size->SetMinMaxHeight(378.0f);
		Insert(Size);
		cbOverlay::SharedPtr Overlay = cbOverlay::Create();
		Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Size->SetContent(Overlay);

		{
			auto Image = cbImage::Create();
			Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Image->SetVertexColorStyle(cbColor::White());
			Overlay->Insert(Image);
		}
		cbHorizontalBox::SharedPtr HB = cbHorizontalBox::Create();
		HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Overlay->Insert(HB);
		{
			{
				cbOverlay::SharedPtr Overlay_CANCEL = cbOverlay::Create();
				Overlay_CANCEL->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Overlay_CANCEL->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				HB->Insert(Overlay_CANCEL, eSlotAlignment::BoundToSlot);
				{
					cbgui::cbImage::SharedPtr Image = cbImage::Create();
					Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Image->SetVertexColorStyle(cbColor(0.78516f, 0.02026f, 0.05615f));
					Overlay_CANCEL->Insert(Image);
				}
				{
					CANCEL_BTN = cbPopupButton::Create(this, U"CANCEL", char32_t(10799));
					CANCEL_BTN->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
					CANCEL_BTN->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					CANCEL_BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					CANCEL_BTN->SetTextJustify(eTextJustify::Left);
					CANCEL_BTN->Text->SetFontSize(8);
					CANCEL_BTN->Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					CANCEL_BTN->Text->SetPadding(cbMargin(0,2,0,0));
					CANCEL_BTN->EmojiString->SetFontSize(16);
					CANCEL_BTN->SetPadding(cbMargin(15));
					CANCEL_BTN->HideButtonGeometry(true);
					CANCEL_BTN->Button->SetHeight(1.0f);
					Overlay_CANCEL->Insert(CANCEL_BTN);
				}
			}
			{
				cbVerticalBox::SharedPtr VerticalBox = cbVerticalBox::Create();
				VerticalBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				VerticalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				VerticalBox->SetPadding(cbMargin(10, 10, 10, 20));
				HB->Insert(VerticalBox, eSlotAlignment::BoundToSlot);
				{
					cbHorizontalBox::SharedPtr Header_HB = cbHorizontalBox::Create();
					Header_HB->SetVerticalAlignment(eVerticalAlignment::Align_Top);
					Header_HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Header_HB->SetPadding(cbMargin(0, 0, 0, 10));
					VerticalBox->Insert(Header_HB, eSlotAlignment::BoundToContent);

					{
						cbOverlay::SharedPtr Emoji_Overlay = cbOverlay::Create();
						Emoji_Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Emoji_Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Header_HB->Insert(Emoji_Overlay, eSlotAlignment::BoundToContent);

						auto Image = cbImage::Create();
						Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Image->SetVertexColorStyle(cbColor::Black());
						Image->SetDimension(cbDimension(32.0f, 32.0f));
						Emoji_Overlay->Insert(Image);

						auto Text = cbString::Create(U"", cbTextDesc(16));
						Text->AddChar(char32_t(0x23E3));
						Text->SetDefaultSpaceWidth(3);
						Text->SetFontType(eFontType::Bold);
						Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
						Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Text->SetVertexColorStyle(cbColor::White());
						Text->SetTextJustify(eTextJustify::Center);
						Emoji_Overlay->Insert(Text);
					}

					auto Text = cbString::Create(U"TARGET", cbTextDesc(12));
					Text->SetPadding(cbMargin(4, 0, 0, 0));
					Text->SetDefaultSpaceWidth(3);
					Text->SetFontType(eFontType::Bold);
					Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Text->SetVertexColorStyle(cbColor::Black());
					Text->SetTextJustify(eTextJustify::Left);
					Header_HB->Insert(Text, eSlotAlignment::BoundToContent);
				}
				{
					auto Image = cbImage::Create();
					Image->SetVerticalAlignment(eVerticalAlignment::Align_Top);
					Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Image->SetVertexColorStyle(cbColor::Black());
					Image->SetHeight(2.0f);
					Image->SetPadding(cbMargin(0, 0, 0, 15));
					VerticalBox->Insert(Image);
				}
				{
					cbScrollBox::SharedPtr SB = cbScrollBox::Create();
					SB->SetName("Text_SB");
					SB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					SB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					VerticalBox->Insert(SB, eSlotAlignment::BoundToSlot);

					SB->SetScrollBarBackgroundVertexColorStyle(cbColor(180,180,180));
					SB->SetScrollBarHandleVertexColorStyle(cbColor::Black());
					SB->SetScrollBarPadding(cbMargin(4, 0, 0, 0));
					SB->SetScrollBarThickness(4.0f);

					auto Text = cbString::Create(U"	Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam bibendum nisl et volutpat ultrices.\
							 Duis ullamcorper blandit ipsum. Nam congue velit nec tempor porttitor. Vestibulum egestas risus auctor quam placerat, in ultricies ante efficitur.\
							 Nulla eros velit, blandit a nibh in, interdum venenatis odio. Sed fermentum tincidunt sodales. Phasellus a nunc velit. Nunc sed ante eu massa molestie posuere.\
							 Quisque pharetra orci sem. Vestibulum sodales lorem quis nisl tempor semper. Suspendisse tincidunt tincidunt arcu, sed imperdiet tellus sodales a.\
							 In urna velit, ornare vitae odio vel, tempus efficitur augue. Sed eget quam ante. Quisque id odio a dui tincidunt consequat sit amet a tellus.\
							\n	Vivamus mollis tincidunt porttitor.Curabitur eu consectetur lectus, id sodales ante.Vivamus rutrum ligula velit,\
							 vel feugiat eros iaculis quis.Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; \
							Duis mattis dolor vitae enim maximus iaculis.Nullam at erat fermentum, vulputate diam vitae,\
							 pretium tortor.Integer vel eros eu sem mollis placerat sed ac dolor.Pellentesque laoreet, ipsum vitae feugiat bibendum,\
							urna nisl sollicitudin ligula, at lacinia nunc ligula eget ex.Aliquam ultricies venenatis risus, sit amet facilisis tortor tempor\
							 pellentesque.Donec finibus massa vel sollicitudin tempor.Donec ut odio dictum, condimentum odio eu, mattis nisi.Aliquam fringilla,\
							 magna ut semper placerat, augue diam imperdiet magna, eget lobortis sapien felis vitae enim.Morbi cursus in urna vel ultricies.", cbTextDesc(8));
					Text->SetDefaultSpaceWidth(3);
					Text->SetAutoWrap(true);
					Text->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Text->SetVertexColorStyle(cbColor::Black());
					Text->SetTextJustify(eTextJustify::Left);
					SB->Insert(Text);
				}
			}
		}
		Overlay->Insert(HB);
	}

	virtual ~cbTargetInfo()
	{
		CANCEL_BTN = nullptr;
		fCallBack_CANCEL = nullptr;
	}

	virtual void OnTick(float DT) override
	{
		Super::OnTick(DT);
	}

	virtual void OnAttachToCanvas() override
	{
		CANCEL_BTN->ResetButtonStatus();
	}

	virtual void OnRemoveFromCanvas() override
	{
		CANCEL_BTN->ResetButtonStatus();
	}

	virtual void OnHovered()
	{
	}

	virtual void OnSelected()
	{
	}

private:
	virtual void OnOptionHovered(cbPopupButton* BTN)
	{

	}

	virtual void OnOptionSelectd(cbPopupButton* BTN)
	{
		if (CANCEL_BTN.get() == BTN)
		{
			CANCEL_BTN->Deselect();
			if (fCallBack_CANCEL)
				fCallBack_CANCEL();
		}
		RemoveFromCanvas();
	}
};

class cbAnimatedImageInfo : public cbImageInfo
{
	cbClassBody(cbClassConstructor, cbAnimatedImageInfo, cbImageInfo)
public:
	cbString::SharedPtr String;
private:
	cbImage::SharedPtr Image;
	bool bAnimationRunning;
	float Time;
public:
	cbAnimatedImageInfo(std::u32string Str1, std::u32string Str2, const char32_t Emoji)
		: cbImageInfo(Str1, Str2, Emoji)
		, bAnimationRunning(false)
		, Time(0.0f)
	{
		cbOverlay::SharedPtr pOverlay = cbOverlay::Create();
		pOverlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		pOverlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

		Image = cbImage::Create();
		Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Image->SetVertexColorStyle(cbColor(0.0f, 0.0f, 0.0f, 0.5f));
		Image->SetHeight(0.0f);
		pOverlay->Insert(Image);

		String = cbString::Create("Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
			Donec lacinia diam justo, in venenatis tellus mollis quis. In tincidunt, arcu nec luctus sollicitudin, \
			risus sem ullamcorper lectus, blandit varius arcu nisl sit amet mauris.", cbTextDesc(8));

		String->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		String->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		String->SetVertexColorStyle(cbColor::White());
		String->SetAutoWrap(true);
		//String->SetCustomWrapWidth(220);
		String->SetPadding(cbMargin(5, 5, 5, 0));
		String->SetTextJustify(eTextJustify::Center);
		String->SetVisibilityState(eVisibility::Hidden);
		String->Disable();
		pOverlay->Insert(String);

		Text_VB->Insert(pOverlay);
	}

	virtual ~cbAnimatedImageInfo()
	{
		Image = nullptr;
		String = nullptr;
	}

	virtual void OnTick(float DT) override
	{
		if (bAnimationRunning)
		{
			Time += DT;
			if (Time >= (1.0f / 60.0f))
			{
				Image->SetHeight(Image->GetHeight() + 5);

				if (Image->GetHeight() >= 80)
				{
					bAnimationRunning = false;
					String->SetVisibilityState(eVisibility::Visible);
					String->Enable();
				}
				Time = 0.0f;
			}
		}
	}

	virtual void Select()
	{
		if (IsSelected())
			return;

		Super::Select();
		bAnimationRunning = true;
	}

	virtual void Deselect()
	{
		if (!IsSelected())
			return;

		Super::Deselect();
		String->SetVisibilityState(eVisibility::Hidden);
		String->Disable();
		bAnimationRunning = false;
		Time = 0.0f;
		Image->SetHeight(0);
	}
};

class DemoCanvas::cbMainMenu : public cbgui::cbVerticalBox
{
	cbClassBody(cbClassConstructor, cbMainMenu, cbVerticalBox)
private:
	class FeaturedMenuList : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, FeaturedMenuList, cbVerticalBox)
	public:
		std::vector<cbScaleBox*> ScaleBoxes;
		cbImageInfo::SharedPtr ImageInfo;
		cbWarningPopupMenu::SharedPtr WarningPopupMenu;

	public:
		FeaturedMenuList(cbDimension Dimension)
		{
			SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 50, 0));

			cbHorizontalBox::SharedPtr HorizontalBox = cbHorizontalBox::Create();
			HorizontalBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			HorizontalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(HorizontalBox, eSlotAlignment::BoundToSlot);
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				ScaleBox->SetPadding(cbMargin(2));
				{
					cbOverlay::SharedPtr Overlay = cbOverlay::Create();
					Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					{
						cbgui::cbImage::SharedPtr Image = cbImage::Create();
						Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Image->SetVertexColorStyle(cbColor::Random());
						Image->SetWidth(Dimension.GetWidth() / 2 - 100 + 3);
						Overlay->Insert(Image);
					}
					{
						const char32_t Emoji = char32_t(9749);
						cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(U"CAMPAIGN", U"CAMPAIGN1", Emoji);
						ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						ImageInfo->fOnSelected = std::bind(&FeaturedMenuList::Menu_StorySelected, this);
						Overlay->Insert(ImageInfo);
					}
					ScaleBox->SetContent(Overlay);
					ScaleBoxes.push_back(ScaleBox.get());
					ScaleBox->BindFunctionTo_OnFocus(std::bind(&FeaturedMenuList::ScaleBox_Hovered, this, std::placeholders::_1));
				}
				HorizontalBox->Insert(ScaleBox, eSlotAlignment::BoundToSlot);
			}
			{
				cbHorizontalBox::SharedPtr HB_Inner = cbHorizontalBox::Create();
				HB_Inner->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				HB_Inner->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				HorizontalBox->Insert(HB_Inner, eSlotAlignment::BoundToSlot);
				{
					cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
					ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					ScaleBox->SetStayActiveAfterFocusLost(true);
					ScaleBox->SetPadding(cbMargin(2));
					{
						cbOverlay::SharedPtr Overlay = cbOverlay::Create();
						Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						{
							cbgui::cbImage::SharedPtr Image = cbImage::Create();
							Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							Image->SetVertexColorStyle(cbColor::Random());
							Image->SetWidth(Dimension.GetWidth() / 6);
							Overlay->Insert(Image);
						}
						{
							const char32_t Emoji = char32_t(9992);
							cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(U"TARGET", U"TARGET 1", Emoji);
							ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							ImageInfo->fOnSelected = std::bind(&FeaturedMenuList::ShowPopup, this);
							Overlay->Insert(ImageInfo);
						}
						ScaleBox->SetContent(Overlay);
						ScaleBoxes.push_back(ScaleBox.get());
						ScaleBox->BindFunctionTo_OnFocus(std::bind(&FeaturedMenuList::ScaleBox_Hovered, this, std::placeholders::_1));
					}
					HB_Inner->Insert(ScaleBox, eSlotAlignment::BoundToSlot);
				}
				{
					cbVerticalBox::SharedPtr VB = cbVerticalBox::Create();
					VB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					VB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					HB_Inner->Insert(VB, eSlotAlignment::BoundToSlot);
					HB_Inner->SetSlotWeight(2.0f, 1);
					{
						cbHorizontalBox::SharedPtr VB_HorizontalBox = cbHorizontalBox::Create();
						VB_HorizontalBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						VB_HorizontalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						{
							cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
							ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							ScaleBox->SetStayActiveAfterFocusLost(true);
							ScaleBox->SetPadding(cbMargin(2));
							{
								cbOverlay::SharedPtr Overlay = cbOverlay::Create();
								Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								{
									cbgui::cbImage::SharedPtr Image = cbImage::Create();
									Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
									Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
									Image->SetVertexColorStyle(cbColor::Random());
									Image->SetWidth(Dimension.GetWidth() / 6);
									Overlay->Insert(Image);
								}
								{
									const char32_t Emoji = char32_t(10148);
									cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(U"SEASONS", U"SEASON 1", Emoji);
									ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
									ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
									ImageInfo->fOnSelected = std::bind(&FeaturedMenuList::ShowPopup, this);
									Overlay->Insert(ImageInfo);
								}
								ScaleBox->SetContent(Overlay);
								ScaleBoxes.push_back(ScaleBox.get());
								ScaleBox->BindFunctionTo_OnFocus(std::bind(&FeaturedMenuList::ScaleBox_Hovered, this, std::placeholders::_1));
							}
							VB_HorizontalBox->Insert(ScaleBox, eSlotAlignment::BoundToSlot);
						}
						{
							cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
							ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							ScaleBox->SetStayActiveAfterFocusLost(true);
							ScaleBox->SetPadding(cbMargin(2));
							{
							cbOverlay::SharedPtr Overlay = cbOverlay::Create();
							Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							{
								cbgui::cbImage::SharedPtr Image = cbImage::Create();
								Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								Image->SetVertexColorStyle(cbColor::Random());
								Image->SetWidth(Dimension.GetWidth() / 6);
								Overlay->Insert(Image);
							}
							{
								const char32_t Emoji = char32_t(10145);
								cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(U"ANNOUNCEMENT", U"NEWS!", Emoji);
								ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								ImageInfo->fOnSelected = std::bind(&FeaturedMenuList::ShowPopup, this);
								Overlay->Insert(ImageInfo);
							}
							ScaleBox->SetContent(Overlay);
							ScaleBoxes.push_back(ScaleBox.get());
							ScaleBox->BindFunctionTo_OnFocus(std::bind(&FeaturedMenuList::ScaleBox_Hovered, this, std::placeholders::_1));
							}
							VB_HorizontalBox->Insert(ScaleBox, eSlotAlignment::BoundToSlot);
						}
						VB->Insert(VB_HorizontalBox, eSlotAlignment::BoundToSlot);
					}
					{
					cbHorizontalBox::SharedPtr VB_HorizontalBox = cbHorizontalBox::Create();
					VB_HorizontalBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					VB_HorizontalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					{
						cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
						ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						ScaleBox->SetStayActiveAfterFocusLost(true);
						ScaleBox->SetPadding(cbMargin(2));
						{
							cbOverlay::SharedPtr Overlay = cbOverlay::Create();
							Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							{
								cbgui::cbImage::SharedPtr Image = cbImage::Create();
								Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								Image->SetVertexColorStyle(cbColor::Random());
								Image->SetWidth(Dimension.GetWidth() / 6);
								Overlay->Insert(Image);
							}
							{
								const char32_t Emoji = char32_t(9733);
								cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(U"FEATURED", U"TARGET 2!", Emoji);
								ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								ImageInfo->fOnSelected = std::bind(&FeaturedMenuList::ShowPopup, this);
								Overlay->Insert(ImageInfo);
							}
							ScaleBox->SetContent(Overlay);
							ScaleBoxes.push_back(ScaleBox.get());
							ScaleBox->BindFunctionTo_OnFocus(std::bind(&FeaturedMenuList::ScaleBox_Hovered, this, std::placeholders::_1));
						}
						VB_HorizontalBox->Insert(ScaleBox, eSlotAlignment::BoundToSlot);
					}
					{
						cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
						ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						ScaleBox->SetStayActiveAfterFocusLost(true);
						ScaleBox->SetPadding(cbMargin(2));
						{
							cbOverlay::SharedPtr Overlay = cbOverlay::Create();
							Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							{
								cbgui::cbImage::SharedPtr Image = cbImage::Create();
								Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								Image->SetVertexColorStyle(cbColor::Random());
								Image->SetWidth(Dimension.GetWidth() / 6);
								Overlay->Insert(Image);
							}
							{
								const char32_t Emoji = char32_t(9875);
								cbImageInfo::SharedPtr ImageInfo_LOAD = cbImageInfo::Create(U" ", U"LOAD GAME", Emoji);
								ImageInfo_LOAD->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
								ImageInfo_LOAD->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
								ImageInfo_LOAD->fOnSelected = std::bind(&FeaturedMenuList::ShowPopup, this);
								Overlay->Insert(ImageInfo_LOAD);
							}
							ScaleBox->SetContent(Overlay);
							ScaleBoxes.push_back(ScaleBox.get());
							ScaleBox->BindFunctionTo_OnFocus(std::bind(&FeaturedMenuList::ScaleBox_Hovered, this, std::placeholders::_1));
						}
						VB_HorizontalBox->Insert(ScaleBox, eSlotAlignment::BoundToSlot);
					}
					VB->Insert(VB_HorizontalBox, eSlotAlignment::BoundToSlot);
					}
				}
			}
			{
				const char32_t Emoji = char32_t(9733);
				ImageInfo = cbImageInfo::Create(U" ", U" ", Emoji);
				ImageInfo->SetPadding(cbMargin(0, 20, 0, 0));
				ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ImageInfo->SetFocusable(false);
				ImageInfo->Select();
				ImageInfo->SetButtonVertexStyle(cbColor::Transparent());
				ImageInfo->HideButtonGeometry(true);
				ImageInfo->HideBackground(true);
				ImageInfo->EmojiImage->SetDimension(cbDimension(64, 64));
				ImageInfo->EmojiText->SetFontSize(36);
				ImageInfo->Text1->SetFontSize(16);
				ImageInfo->Text2->SetFontSize(24);
				//ImageInfo->SetPadding(cbMargin(0, 0, 0, 32));
				Insert(ImageInfo, eSlotAlignment::BoundToContent);
			}
			{
				//Insert(cbSizeBox::Create(), eSlotAlignment::BoundToContent);
			}
			{
				WarningPopupMenu = cbWarningPopupMenu::Create(U"UNAVAILABLE", U"Unable to load level.");
			}
		}

		virtual ~FeaturedMenuList()
		{
			ImageInfo = nullptr;
			for (auto& ScaleBox : ScaleBoxes)
				ScaleBox = nullptr;
			ScaleBoxes.clear();
			WarningPopupMenu = nullptr;
		}

		void ShowPopup()
		{
			WarningPopupMenu->AddToCanvas(GetCanvas());
		}

		void Menu_StorySelected()
		{
			GetRootOwner<cbMainMenu>(1)->ShowStoryMissionMenu(this);
		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (const auto& pScaleBox : ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(pScaleBox->GetContent())->GetSlot(1)->GetContent());
					pImageInfo->Deselect();
				}
			}

			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(ScaleBox->GetContent())->GetSlot(1)->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}

		void Selected(cbScaleBox* ScaleBox)
		{
			GetRootOwner<cbMainMenu>(1);
		}

		void Select(std::size_t Index)
		{
			ScaleBoxes[Index]->Scale();
			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(ScaleBoxes[Index]->GetContent())->GetSlot(1)->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}
	};

	class cbSelectItemMenu : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, cbSelectItemMenu, cbVerticalBox)
	public:
		std::vector<cbScaleBox*> ScaleBoxes;
		cbImageInfo::SharedPtr ImageInfo;
		cbWidget* Parent;
		cbImageInfo* ItemToChange;

	public:
		cbSelectItemMenu()
			: Super()
			, ItemToChange(nullptr)
			, Parent(nullptr)
		{
			auto GetOption = [&](cbColor Color, std::u32string STR, std::u32string STR2, const char32_t pEmoji) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				ScaleBoxes.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				ScaleBox->SetPadding(cbMargin(2, 2, 2, 2));
				{
					cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(STR, STR2, pEmoji);
					ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					ImageInfo->fOnSelected = std::bind(&cbSelectItemMenu::SelectItem, this, std::placeholders::_1);
					ImageInfo->Button->SetWidth(256);
					ImageInfo->Button->SetHeight(372 / 2);
					ImageInfo->EmojiImage->SetVisibilityState(eVisibility::Hidden);
					ImageInfo->HideButtonGeometry(false);
					ImageInfo->BG_Image->SetVisibilityState(eVisibility::Visible);
					ImageInfo->SetButtonVertexStyle(Color);
					ScaleBox->SetContent(ImageInfo);

					ScaleBoxes.push_back(ScaleBox.get());
					ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbSelectItemMenu::ScaleBox_Hovered, this, std::placeholders::_1));
				}
				return ScaleBox;
			};

			SetVerticalAlignment(eVerticalAlignment::Align_Top);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 50, 0));

			{
				cbImageInfo::SharedPtr LevelInfo = cbImageInfo::Create(U"LEVEL", U"LEVEL 1", 0);
				LevelInfo->SetPadding(cbMargin(0, 18, 0, 0));
				LevelInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				LevelInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				LevelInfo->SetFocusable(false);
				LevelInfo->Select();
				LevelInfo->SetButtonVertexStyle(cbColor::Transparent());
				LevelInfo->HideButtonGeometry(true);
				LevelInfo->HideBackground(true);
				LevelInfo->EmojiImage->SetVisibilityState(eVisibility::Hidden);
				LevelInfo->EmojiText->SetFontSize(24);
				LevelInfo->EmojiText->AddChar(char32_t(0x229A));
				LevelInfo->Text1->SetFontSize(8);
				LevelInfo->Text2->SetFontSize(12);
				Insert(LevelInfo, eSlotAlignment::BoundToContent);
			}

			{
				cbHorizontalBox::SharedPtr MenuBar = cbHorizontalBox::Create();
				MenuBar->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				MenuBar->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				MenuBar->SetPadding(cbMargin(0, 0, 0, 24));
				Insert(MenuBar, eSlotAlignment::BoundToContent);
				{
					auto BTN = cbMenuButton::Create(U"SELECT ITEM", nullptr);
					BTN->SetButtonWidth(200);
					BTN->SetButtonHorizontalAlignment(eHorizontalAlignment::Align_Left);
					BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Left);
					BTN->SetTextPadding(cbMargin(15, 0, 0, 0));
					BTN->Button->SetHeight(50);
					BTN->Select();
					BTN->Text->SetFontSize(12);
					MenuBar->Insert(BTN);
				}
			}

			{
				cbHorizontalBox::SharedPtr MenuBar = cbHorizontalBox::Create();
				MenuBar->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				MenuBar->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				MenuBar->SetPadding(cbMargin(0, 0, 0, 24));
				Insert(MenuBar, eSlotAlignment::BoundToContent);
				{
					auto BTN = cbMenuButton::Create(U"COLORS", nullptr);
					MenuBar->Insert(BTN);
					BTN->SetTextPadding(cbMargin(0, 0, 0, 0));
					BTN->Button->SetWidth(60);
					BTN->Button->SetHeight(16);
					BTN->SetTextSlotAlignment(eSlotAlignment::BoundToSlot);
					BTN->Text->SetFontSize(8);
					BTN->Text->SetTextJustify(eTextJustify::Center);
					BTN->SetButtonHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					BTN->Select();
				}
			}

			{
				cbVerticalBox::SharedPtr VB = cbVerticalBox::Create();
				VB->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				VB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				Insert(VB, eSlotAlignment::BoundToSlot);
				{
					cbHorizontalBox::SharedPtr HorizontalBox = cbHorizontalBox::Create();
					HorizontalBox->SetVerticalAlignment(eVerticalAlignment::Align_Top);
					HorizontalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					{
						HorizontalBox->Insert(GetOption(cbColor(1.0f, 0.0f, 1.0f, 1.0f), U"ITEM", U"PURPLE", char32_t(0x2673)), eSlotAlignment::BoundToContent);
						HorizontalBox->Insert(GetOption(cbColor(1.0f, 1.0f, 0.0f, 1.0f), U"ITEM", U"YELLOW", char32_t(0x2674)), eSlotAlignment::BoundToContent);
					}
					VB->Insert(HorizontalBox, eSlotAlignment::BoundToContent);
				}
				{
					cbHorizontalBox::SharedPtr HorizontalBox = cbHorizontalBox::Create();
					HorizontalBox->SetVerticalAlignment(eVerticalAlignment::Align_Top);
					HorizontalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					{
						HorizontalBox->Insert(GetOption(cbColor::Grey(), U"ITEM", U"GREY", char32_t(0x2675)), eSlotAlignment::BoundToContent);
						HorizontalBox->Insert(GetOption(cbColor::Green(), U"ITEM", U"GREEN", char32_t(0x2676)), eSlotAlignment::BoundToContent);
					}
					VB->Insert(HorizontalBox, eSlotAlignment::BoundToContent);
				}
			}

			{
				const char32_t Emoji = char32_t(9733);
				ImageInfo = cbImageInfo::Create(U" ", U" ", Emoji);
				ImageInfo->SetPadding(cbMargin(0, 20, 0, 0));
				ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ImageInfo->SetFocusable(false);
				ImageInfo->Select();
				ImageInfo->SetButtonVertexStyle(cbColor::Transparent());
				ImageInfo->HideButtonGeometry(true);
				ImageInfo->HideBackground(true);
				ImageInfo->EmojiImage->SetDimension(cbDimension(64, 64));
				ImageInfo->EmojiText->SetFontSize(36);
				ImageInfo->Text1->SetFontSize(16);
				ImageInfo->Text2->SetFontSize(24);
				Insert(ImageInfo, eSlotAlignment::BoundToContent);
			}
		}

		virtual ~cbSelectItemMenu()
		{
			ImageInfo = nullptr;
			for (auto& ScaleBox : ScaleBoxes)
				ScaleBox = nullptr;
			ScaleBoxes.clear();

			Parent = nullptr;
		}

		cbWidget* GetParent() const
		{
			return Parent;
		}

		void SetParent(cbWidget* pParent)
		{
			Parent = pParent;
		}

		void PossessOption(cbImageInfo* pImageInfo)
		{
			ItemToChange = pImageInfo;
		}

		void SelectItem(cbDemoButtonBase* BTN)
		{
			if (ItemToChange && BTN)
			{
				cbImageInfo* BTN_ImageInfo = static_cast<cbImageInfo*>(BTN);

				ItemToChange->EmojiText->SetText(BTN_ImageInfo->EmojiText->GetText());
				ItemToChange->Text1->SetText(BTN_ImageInfo->Text1->GetText());
				ItemToChange->Text2->SetText(BTN_ImageInfo->Text2->GetText());
				ItemToChange->SetButtonVertexStyle(BTN_ImageInfo->Button->GetVertexColorStyle());

				ItemToChange = nullptr;

				GetRootOwner<cbMainMenu>(1)->CloseStoryMissionSelectItemMenu();
			}
		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (const auto& pScaleBox : ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(pScaleBox->GetContent());
					pImageInfo->Deselect();
				}
			}

			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(ScaleBox->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}

		void Select(std::size_t Index)
		{
			ScaleBoxes[Index]->Scale();
			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(ScaleBoxes[Index]->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}
	};

	class cbStoryMissionMenu : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, cbStoryMissionMenu, cbVerticalBox)
	public:
		cbHorizontalBox::SharedPtr MenuBar;

		cbImageInfo::SharedPtr ImageInfo;
		cbWarningPopupMenu::SharedPtr WarningPopupMenu;
		std::vector<cbScaleBox*> Menu_ScaleBoxes;

		cbHorizontalBox::SharedPtr OBJECTIVES_HB;
		std::vector<cbScaleBox*> OBJECTIVES_TAB_ScaleBoxes;
		cbVerticalBox::SharedPtr PLANNING_VB;
		std::vector<cbScaleBox*> PLANNING_TAB_ScaleBoxes;
		cbHorizontalBox::SharedPtr DIFFICULTY_TAB_HB;
		std::vector<cbScaleBox*> DIFFICULTY_TAB_ScaleBoxes;

		std::size_t ActiveMenuIndex;
		cbWidget* Parent;

		cbTargetInfo::SharedPtr TargetInfo1;

	public:
		cbStoryMissionMenu()
			: Super()
			, ActiveMenuIndex(0)
			, Parent(nullptr)
		{
			auto GetOption = [&](std::u32string STR, std::u32string STR2, const char32_t Emoji, std::vector<cbScaleBox*>& pContainer, std::function<void(cbDemoButtonBase*)> fOnSelected) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				pContainer.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				ScaleBox->SetPadding(cbMargin(2, 2, 2, 2));
				ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbStoryMissionMenu::ScaleBox_Hovered, this, std::placeholders::_1));
				{
					cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(STR, STR2, Emoji);
					ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					ImageInfo->fOnSelected = fOnSelected;
					ImageInfo->Button->SetWidth(250);
					ImageInfo->EmojiImage->SetVisibilityState(eVisibility::Hidden);
					ImageInfo->HideButtonGeometry(false);
					ImageInfo->BG_Image->SetVisibilityState(eVisibility::Visible);
					ImageInfo->SetButtonVertexStyle(cbColor::Random());
					ScaleBox->SetContent(ImageInfo);
				}
				return ScaleBox;
			};

			auto GetAnimatedOption = [&](std::u32string STR, std::u32string STR2, const char32_t Emoji, std::vector<cbScaleBox*>& pContainer, std::function<void(cbDemoButtonBase*)> fOnSelected) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				pContainer.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				ScaleBox->SetPadding(cbMargin(2, 2, 2, 2));
				ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbStoryMissionMenu::ScaleBox_Hovered, this, std::placeholders::_1));
				{
					cbAnimatedImageInfo::SharedPtr ImageInfo = cbAnimatedImageInfo::Create(STR, STR2, Emoji);
					ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					ImageInfo->fOnSelected = fOnSelected;
					ImageInfo->Button->SetWidth(250);
					ImageInfo->EmojiImage->SetVisibilityState(eVisibility::Hidden);
					ImageInfo->HideButtonGeometry(false);
					ImageInfo->BG_Image->SetVisibilityState(eVisibility::Visible);
					ImageInfo->SetButtonVertexStyle(cbColor::Random());

					ScaleBox->SetContent(ImageInfo);
				}
				return ScaleBox;
			};

			{
				WarningPopupMenu = cbWarningPopupMenu::Create(U"UNAVAILABLE", U"Unable to load level.");
			}

			{
				TargetInfo1 = cbTargetInfo::Create();
			}

			SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 50, 0));

			{
				cbImageInfo::SharedPtr LevelInfo = cbImageInfo::Create(U"LEVEL", U"LEVEL 1", 0);
				LevelInfo->SetPadding(cbMargin(0, 18, 0, 0));
				LevelInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				LevelInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				LevelInfo->SetFocusable(false);
				LevelInfo->Select();
				LevelInfo->SetButtonVertexStyle(cbColor::Transparent());
				LevelInfo->HideButtonGeometry(true);
				LevelInfo->HideBackground(true);
				LevelInfo->EmojiImage->SetVisibilityState(eVisibility::Hidden);
				LevelInfo->EmojiText->SetFontSize(24);
				LevelInfo->EmojiText->AddChar(char32_t(0x229A));
				LevelInfo->Text1->SetFontSize(8);
				LevelInfo->Text2->SetFontSize(12);
				Insert(LevelInfo, eSlotAlignment::BoundToContent);
			}

			{
				MenuBar = cbHorizontalBox::Create();
				MenuBar->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				MenuBar->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				MenuBar->SetPadding(cbMargin(0, 0, 0, 64));
				Insert(MenuBar, eSlotAlignment::BoundToContent);
				{
					auto GetOption = [&](std::u32string STR, float width) -> cbMenuButton::SharedPtr
					{
						auto BTN = cbMenuButton::Create(STR, std::bind(&cbStoryMissionMenu::MenuBarBTN_Clicked, this, std::placeholders::_1));
						BTN->SetButtonWidth(width);
						BTN->SetButtonHorizontalAlignment(eHorizontalAlignment::Align_Left);
						BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Left);
						BTN->SetTextPadding(cbMargin(15, 0, 0, 0));
						BTN->Button->SetHeight(50);
						return BTN;
					};

					{
						auto Option = GetOption(U"☴ BRIEFING", 300);
						Option->Select();
						MenuBar->Insert(Option);
					}
					{
						auto Option = GetOption(U"☴ CHALLENGES", 300);
						Option->Disable();
						MenuBar->Insert(Option);
					}
					{
						auto Option = GetOption(U"☴ MISSION STORIES", 300);
						Option->Disable();
						MenuBar->Insert(Option);
					}
					{
						auto Option = GetOption(U"☴ LEADERBOARDS", 300);
						Option->Disable();
						MenuBar->Insert(Option);
					}
				}
			}

			cbHorizontalBox::SharedPtr HB_Tab = cbHorizontalBox::Create();
			HB_Tab->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			HB_Tab->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(HB_Tab, eSlotAlignment::BoundToSlot);

			{
				cbVerticalBox::SharedPtr VB = cbVerticalBox::Create();
				VB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				VB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				HB_Tab->Insert(VB, eSlotAlignment::BoundToSlot);
				HB_Tab->SetSlotWeight(0.25f, 0);

				{
					auto Option = GetOption(U"", U"OBJECTIVES", char32_t(0x2620), Menu_ScaleBoxes, nullptr);
					auto Content = static_cast<cbImageInfo*>(Option->GetContent());
					Content->SetButtonVertexStyle(eButtonState::Default, cbColor::White());
					Content->SetButtonVertexStyle(eButtonState::Hovered, cbColor::Red());
					Content->SetButtonVertexStyle(eButtonState::Pressed, cbColor::Red());
					Content->SetButtonVertexStyle(eButtonState::Disabled, cbColor(146, 146, 146));
					Content->BG_Image->SetVisibilityState(eVisibility::Hidden);
					VB->Insert(Option, eSlotAlignment::BoundToSlot);
				}
				{
					auto Option = GetOption(U"", U"PLANNING", char32_t(0x270D), Menu_ScaleBoxes, nullptr);
					auto Content = static_cast<cbImageInfo*>(Option->GetContent());
					Content->SetButtonVertexStyle(eButtonState::Default, cbColor::White());
					Content->SetButtonVertexStyle(eButtonState::Hovered, cbColor::Red());
					Content->SetButtonVertexStyle(eButtonState::Pressed, cbColor::Red());
					Content->SetButtonVertexStyle(eButtonState::Disabled, cbColor(146, 146, 146));
					Content->BG_Image->SetVisibilityState(eVisibility::Hidden);
					VB->Insert(Option, eSlotAlignment::BoundToSlot);
				}
				{
					auto Option = GetOption(U"DIFFICULTY", U"PLAY", char32_t(0x25B6), Menu_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowPopup, this, std::placeholders::_1));
					auto Content = static_cast<cbImageInfo*>(Option->GetContent());
					Content->SetButtonVertexStyle(eButtonState::Default, cbColor::White());
					Content->SetButtonVertexStyle(eButtonState::Hovered, cbColor::Red());
					Content->SetButtonVertexStyle(eButtonState::Pressed, cbColor::Red());
					Content->SetButtonVertexStyle(eButtonState::Disabled, cbColor(146, 146, 146));
					Content->BG_Image->SetVisibilityState(eVisibility::Hidden);
					VB->Insert(Option, eSlotAlignment::BoundToSlot);
				}
			}

			{
				cbOverlay::SharedPtr Overlay = cbOverlay::Create();
				Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				HB_Tab->Insert(Overlay, eSlotAlignment::BoundToSlot);
				{
					OBJECTIVES_HB = cbHorizontalBox::Create();
					OBJECTIVES_HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					OBJECTIVES_HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Overlay->Insert(OBJECTIVES_HB);

					{
						auto Option = GetOption(U"BRIEFING", U"STORY", char32_t(0x2630), OBJECTIVES_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowPopup, this, std::placeholders::_1));
						auto Content = static_cast<cbImageInfo*>(Option->GetContent());
						Content->BG_Image->SetWidth(500.0f);
						OBJECTIVES_HB->Insert(Option, eSlotAlignment::BoundToSlot);
					}
					{
						cbHorizontalBox::SharedPtr HB = cbHorizontalBox::Create();
						HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						OBJECTIVES_HB->Insert(HB, eSlotAlignment::BoundToSlot);
						HB->Insert(GetOption(U"TARGET", U"SOMEONE", char32_t(0x2620), OBJECTIVES_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowTargetInfo1, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(GetOption(U"TARGET", U"ANOTHER GUY", char32_t(0x2620), OBJECTIVES_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowTargetInfo1, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
					}
				}
				{
					PLANNING_VB = cbVerticalBox::Create();
					PLANNING_VB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					PLANNING_VB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Overlay->Insert(PLANNING_VB);

					{
						cbHorizontalBox::SharedPtr HB = cbHorizontalBox::Create();
						HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						PLANNING_VB->Insert(HB, eSlotAlignment::BoundToSlot);

						HB->Insert(GetOption(U"", U"GEAR", char32_t(0x267A), PLANNING_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowSelectItemMenu, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(GetOption(U"", U"WEAPON", char32_t(0x267A), PLANNING_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowSelectItemMenu, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(GetOption(U"", U"LOCATION", char32_t(0x267A), PLANNING_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowSelectItemMenu, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(cbSizeBox::Create(), eSlotAlignment::BoundToSlot);
					}
					{
						cbHorizontalBox::SharedPtr HB = cbHorizontalBox::Create();
						HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						PLANNING_VB->Insert(HB, eSlotAlignment::BoundToSlot);

						HB->Insert(GetOption(U"", U"ITEM 1", char32_t(0x267A), PLANNING_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowSelectItemMenu, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(GetOption(U"", U"ITEM 2", char32_t(0x267A), PLANNING_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowSelectItemMenu, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(GetOption(U"", U"ITEM 3", char32_t(0x267A), PLANNING_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowSelectItemMenu, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						HB->Insert(cbSizeBox::Create(), eSlotAlignment::BoundToSlot);
					}

					PLANNING_VB->SetVisibilityState(eVisibility::Hidden);
					PLANNING_VB->Disable();
				}
				{
					DIFFICULTY_TAB_HB = cbHorizontalBox::Create();
					DIFFICULTY_TAB_HB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					DIFFICULTY_TAB_HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Overlay->Insert(DIFFICULTY_TAB_HB);

					{
						DIFFICULTY_TAB_HB->Insert(GetAnimatedOption(U"DIFFICULTY", U"EASY", char32_t(0x265F), DIFFICULTY_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						DIFFICULTY_TAB_HB->Insert(GetAnimatedOption(U"DIFFICULTY", U"NORMAL", char32_t(0x265B), DIFFICULTY_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
						DIFFICULTY_TAB_HB->Insert(GetAnimatedOption(U"DIFFICULTY", U"HARD", char32_t(0x265A), DIFFICULTY_TAB_ScaleBoxes, std::bind(&cbStoryMissionMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
					}
					DIFFICULTY_TAB_HB->Insert(cbSizeBox::Create(), eSlotAlignment::BoundToSlot);

					DIFFICULTY_TAB_HB->SetVisibilityState(eVisibility::Hidden);
					DIFFICULTY_TAB_HB->Disable();
				}
			}

			{
				ImageInfo = cbImageInfo::Create(U" ", U" ", 0);
				ImageInfo->SetPadding(cbMargin(0, 20, 0, 0));
				ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ImageInfo->SetFocusable(false);
				ImageInfo->Select();
				ImageInfo->SetButtonVertexStyle(cbColor::Transparent());
				ImageInfo->HideButtonGeometry(true);
				ImageInfo->HideBackground(true);
				ImageInfo->EmojiImage->SetDimension(cbDimension(64, 64));
				ImageInfo->EmojiText->SetFontSize(36);
				ImageInfo->Text1->SetFontSize(16);
				ImageInfo->Text2->SetFontSize(24);
				Insert(ImageInfo, eSlotAlignment::BoundToContent);
			}

			Select(0);
		}

		virtual ~cbStoryMissionMenu()
		{
			MenuBar = nullptr;

			ImageInfo = nullptr;
			for (auto& ScaleBox : Menu_ScaleBoxes)
				ScaleBox = nullptr;
			Menu_ScaleBoxes.clear();
			for (auto& ScaleBox : OBJECTIVES_TAB_ScaleBoxes)
				ScaleBox = nullptr;
			OBJECTIVES_TAB_ScaleBoxes.clear();
			for (auto& ScaleBox : PLANNING_TAB_ScaleBoxes)
				ScaleBox = nullptr;
			PLANNING_TAB_ScaleBoxes.clear();
			for (auto& ScaleBox : DIFFICULTY_TAB_ScaleBoxes)
				ScaleBox = nullptr;
			DIFFICULTY_TAB_ScaleBoxes.clear();

			WarningPopupMenu = nullptr;

			OBJECTIVES_HB = nullptr;
			PLANNING_VB = nullptr;
			DIFFICULTY_TAB_HB = nullptr;

			Parent = nullptr;
			TargetInfo1 = nullptr;
		}

		cbWidget* GetParent() const
		{
			return Parent;
		}
		void SetParent(cbWidget* pParent)
		{
			Parent = pParent;
		}

		void ShowPopup(cbDemoButtonBase* BTN)
		{
			WarningPopupMenu->AddToCanvas(GetCanvas());
		}

		void ShowTargetInfo1(cbDemoButtonBase* BTN)
		{
			TargetInfo1->AddToCanvas(GetCanvas());
		}

		void ShowTargetInfo2(cbDemoButtonBase* BTN)
		{
			TargetInfo1->AddToCanvas(GetCanvas());
		}

		void ShowSelectItemMenu(cbDemoButtonBase* BTN)
		{
			GetRootOwner<cbMainMenu>(1)->ShowStoryMissionSelectItemMenu(static_cast<cbImageInfo*>(BTN));
		}

		void MenuBarBTN_Clicked(cbMenuButton* btn)
		{
			if (!btn)
				return;

			const std::size_t Index = btn->GetIndex();

			for (std::size_t i = 0; i < MenuBar->GetSlotSize(); i++)
			{
				const auto MenuButton = MenuBar->GetSlot(i)->GetContent<cbMenuButton>();

				if (MenuButton)
				{
					if (MenuButton->IsSelected() && MenuButton->GetIndex() != Index)
					{
						MenuButton->Deselect();
					}
				}
			}
		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (std::size_t i = 0; i < Menu_ScaleBoxes.size(); i++)
			{
				const auto& pScaleBox = Menu_ScaleBoxes.at(i);
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((pScaleBox->GetContent()));
					pImageInfo->SetButtonVertexStyle(eButtonState::Default, cbColor::White());
					pImageInfo->EmojiText->SetVertexColorStyle(cbColor::Black());
					pImageInfo->Text1->SetVertexColorStyle(cbColor::Black());
					pImageInfo->Text2->SetVertexColorStyle(cbColor::Black());
					pImageInfo->Deselect();
				}
				else
				{
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((pScaleBox->GetContent()));
					pImageInfo->SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
					pImageInfo->EmojiText->SetVertexColorStyle(cbColor::White());
					pImageInfo->Text1->SetVertexColorStyle(cbColor::White());
					pImageInfo->Text2->SetVertexColorStyle(cbColor::White());

					if (i == 0)
					{
						if (PLANNING_VB->IsEnabled())
						{
							PLANNING_VB->Disable();
							PLANNING_VB->SetVisibilityState(eVisibility::Hidden);
						}
						if (DIFFICULTY_TAB_HB->IsEnabled())
						{
							DIFFICULTY_TAB_HB->Disable();
							DIFFICULTY_TAB_HB->SetVisibilityState(eVisibility::Hidden);
						}
						if (!OBJECTIVES_HB->IsEnabled())
						{
							OBJECTIVES_HB->Enable();
							OBJECTIVES_HB->SetVisibilityState(eVisibility::Visible);
							ActiveMenuIndex = 0;
						}
					}
					else  if (i == 1)
					{
						if (OBJECTIVES_HB->IsEnabled())
						{
							OBJECTIVES_HB->Disable();
							OBJECTIVES_HB->SetVisibilityState(eVisibility::Hidden);
						}
						if (DIFFICULTY_TAB_HB->IsEnabled())
						{
							DIFFICULTY_TAB_HB->Disable();
							DIFFICULTY_TAB_HB->SetVisibilityState(eVisibility::Hidden);
						}
						if (!PLANNING_VB->IsEnabled())
						{
							PLANNING_VB->Enable();
							PLANNING_VB->SetVisibilityState(eVisibility::Visible);
							ActiveMenuIndex = 1;
						}
					}
					else  if (i == 2)
					{
						if (OBJECTIVES_HB->IsEnabled())
						{
							OBJECTIVES_HB->Disable();
							OBJECTIVES_HB->SetVisibilityState(eVisibility::Hidden);
						}
						if (PLANNING_VB->IsEnabled())
						{
							PLANNING_VB->Disable();
							PLANNING_VB->SetVisibilityState(eVisibility::Hidden);
						}
						if (!DIFFICULTY_TAB_HB->IsEnabled())
						{
							DIFFICULTY_TAB_HB->Enable();
							DIFFICULTY_TAB_HB->SetVisibilityState(eVisibility::Visible);
							ActiveMenuIndex = 2;
						}
					}
				}
			}

			for (const auto& pScaleBox : OBJECTIVES_TAB_ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((pScaleBox->GetContent()));
					pImageInfo->Deselect();
				}
				else
				{
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((ScaleBox->GetContent()));
					if (pImageInfo)
					{
						pImageInfo->Select();
						ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
						ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
						ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
					}
					{
						const auto& pMenuScaleBox = Menu_ScaleBoxes.at(ActiveMenuIndex);
						cbImageInfo* pMenuImageInfo = static_cast<cbImageInfo*>((pMenuScaleBox->GetContent()));
						pMenuImageInfo->SetButtonVertexStyle(eButtonState::Default, cbColor(146, 146, 146));
						pMenuImageInfo->EmojiText->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
						pMenuImageInfo->Text1->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
						pMenuImageInfo->Text2->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
					}
				}
			}

			for (const auto& pScaleBox : PLANNING_TAB_ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((pScaleBox->GetContent()));
					pImageInfo->Deselect();
				}
				else
				{
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((ScaleBox->GetContent()));
					if (pImageInfo)
					{
						pImageInfo->Select();
						ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
						ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
						ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
					}
					{
						const auto& pMenuScaleBox = Menu_ScaleBoxes.at(ActiveMenuIndex);
						cbImageInfo* pMenuImageInfo = static_cast<cbImageInfo*>((pMenuScaleBox->GetContent()));
						pMenuImageInfo->SetButtonVertexStyle(eButtonState::Default, cbColor(146, 146, 146));
						pMenuImageInfo->EmojiText->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
						pMenuImageInfo->Text1->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
						pMenuImageInfo->Text2->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
					}
				}
			}
			for (const auto& pScaleBox : DIFFICULTY_TAB_ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((pScaleBox->GetContent()));
					pImageInfo->Deselect();
				}
				else
				{
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((ScaleBox->GetContent()));
					if (pImageInfo)
					{
						pImageInfo->Select();
						ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
						ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
						ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
					}
					{
						const auto& pMenuScaleBox = Menu_ScaleBoxes.at(ActiveMenuIndex);
						cbImageInfo* pMenuImageInfo = static_cast<cbImageInfo*>((pMenuScaleBox->GetContent()));
						pMenuImageInfo->SetButtonVertexStyle(eButtonState::Default, cbColor(146, 146, 146));
						pMenuImageInfo->EmojiText->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
						pMenuImageInfo->Text1->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
						pMenuImageInfo->Text2->SetVertexColorStyle(cbColor(0.66f, 0.66f, 0.66f));
					}
				}
			}
		}

		void Select(std::size_t Index)
		{
			Menu_ScaleBoxes[Index]->Scale();

			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>((Menu_ScaleBoxes[Index]->GetContent()));
			if (pImageInfo)
			{
				pImageInfo->Select();
				if (std::find(DIFFICULTY_TAB_ScaleBoxes.begin(), DIFFICULTY_TAB_ScaleBoxes.end(), Menu_ScaleBoxes[Index]) == DIFFICULTY_TAB_ScaleBoxes.end())
				{
					pImageInfo->SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
				}
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}
	};

	class cbStoryMissionListMenu : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, cbStoryMissionListMenu, cbVerticalBox)
	public:
		std::vector<cbScaleBox*> ScaleBoxes;
		cbImageInfo::SharedPtr ImageInfo;
		cbWarningPopupMenu::SharedPtr WarningPopupMenu;
	public:
		cbStoryMissionListMenu()
		{
			auto GetOption = [&](std::u32string STR, std::u32string STR2, const char32_t Emoji, std::function<void(cbDemoButtonBase*)> fOnSelected) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				ScaleBoxes.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				{
					cbOverlay::SharedPtr Overlay = cbOverlay::Create();
					Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					{
						cbgui::cbImage::SharedPtr Image = cbImage::Create();
						Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Image->SetVertexColorStyle(cbColor::Random());
						Image->SetWidth(450);
						Image->SetHeight(372);
						Overlay->Insert(Image);
					}
					{
						cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(STR, STR2, Emoji);
						ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						ImageInfo->fOnSelected = fOnSelected;
						Overlay->Insert(ImageInfo);
					}
					ScaleBox->SetContent(Overlay);
					ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbStoryMissionListMenu::ScaleBox_Hovered, this, std::placeholders::_1));
				}
				return ScaleBox;
			};

			{
				WarningPopupMenu = cbWarningPopupMenu::Create(U"UNAVAILABLE", U"Unable to load level.");
			}

			SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 50, 0));

			auto pScrollBox = cbAutoScroller::Create(eOrientation::Horizontal);
			pScrollBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			pScrollBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			pScrollBox->SetPadding(cbMargin(50, 0, 50, 0));

			pScrollBox->Scrollbox->SetScrollBarPadding(cbMargin(50, 10, 50, 5));
			pScrollBox->Scrollbox->SetScrollBarBackgroundVertexColorStyle(cbColor(0.0097f, 0.0116f, 0.01685f));

			cbButtonVertexColorStyle ColorStyle;
			ColorStyle.SetDefaultColor(cbColor::Red());
			ColorStyle.SetHoveredColor(ColorStyle.GetColor(eButtonState::Default));
			ColorStyle.SetPressedColor(ColorStyle.GetColor(eButtonState::Default));
			pScrollBox->Scrollbox->SetScrollBarHandleVertexColorStyle(ColorStyle);

			Insert(pScrollBox, eSlotAlignment::BoundToSlot);

			{
				pScrollBox->Insert(GetOption(U"", U"STORY 1", char32_t(0x2630), std::bind(&cbStoryMissionListMenu::Story1_Selected, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 2", char32_t(0x265B), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 3", char32_t(0x2600), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 4", char32_t(0x2692), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 5", char32_t(0x2620), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 6", char32_t(0x2620), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 7", char32_t(0x2620), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
				pScrollBox->Insert(GetOption(U"", U"STORY 8", char32_t(0x2620), std::bind(&cbStoryMissionListMenu::ShowPopup, this, std::placeholders::_1)));
			}

			{
				ImageInfo = cbImageInfo::Create(U" ", U" ", 0);
				ImageInfo->SetPadding(cbMargin(0, 20, 0, 0));
				ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ImageInfo->SetFocusable(false);
				ImageInfo->Select();
				ImageInfo->SetButtonVertexStyle(cbColor::Transparent());
				ImageInfo->HideButtonGeometry(true);
				ImageInfo->HideBackground(true);
				ImageInfo->EmojiImage->SetDimension(cbDimension(64, 64));
				ImageInfo->EmojiText->SetFontSize(36);
				ImageInfo->Text1->SetFontSize(16);
				ImageInfo->Text2->SetFontSize(24);
				ImageInfo->SetPadding(cbMargin(0,0,0,32));
				Insert(ImageInfo, eSlotAlignment::BoundToContent);
			}

			Select(0);
		}

		virtual ~cbStoryMissionListMenu()
		{
			ImageInfo = nullptr;
			for (auto& ScaleBox : ScaleBoxes)
				ScaleBox = nullptr;
			ScaleBoxes.clear();

			WarningPopupMenu = nullptr;
		}

		void ShowPopup(cbDemoButtonBase* BTN)
		{
			WarningPopupMenu->AddToCanvas(GetCanvas());
		}

		void Story1_Selected(cbDemoButtonBase* BTN)
		{
			GetRootOwner<cbMainMenu>(1)->ShowStoryMissionMenu(this);
		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (const auto& pScaleBox : ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(pScaleBox->GetContent())->GetSlot(1)->GetContent());
					pImageInfo->Deselect();
				}
			}

			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(ScaleBox->GetContent())->GetSlot(1)->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}

		void Selected(cbScaleBox* ScaleBox)
		{
			//GetRootOwner<cbMainMenu>(1);
		}

		void Select(std::size_t Index)
		{
			ScaleBoxes[Index]->Scale();
			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(ScaleBoxes[Index]->GetContent())->GetSlot(1)->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}
	};

	class cbOptions : public cbgui::cbHorizontalBox
	{
		cbClassBody(cbClassConstructor, cbOptions, cbHorizontalBox)
	private:
		class cbOptionVariable : public cbDemoButtonBase
		{
			cbClassBody(cbClassConstructor, cbOptionVariable, cbDemoButtonBase)
		private:
			cbHorizontalBox::SharedPtr HB;
			cbString::SharedPtr Text;
			std::u32string Info;

		public:
			cbOptionVariable(std::u32string Name)
				: Super()
			{
				SetButtonVertexStyle(eButtonState::Default, cbColor::Transparent());
				SetButtonVertexStyle(eButtonState::Disabled, cbColor::Transparent());

				HB = cbHorizontalBox::Create();
				HB->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				HB->SetFocusable(true);
				HB->SetFocusMode(eFocusMode::Immediate);
				{
					cbTextDesc Desc;
					Desc.CharSize = 12;
					Desc.FontType = eFontType::Bold;
					Text = cbString::Create(Name, Desc);
					Text->SetDefaultSpaceWidth(3);
					Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					Text->Wrap();
					HB->Insert(Text);
				}
				{
					cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
					Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					Empty->SetWidth(50);
					HB->Insert(Empty, eSlotAlignment::BoundToSlot);
				}
				Insert(HB);
				HB->SetPadding(cbMargin(10, 0, 10, 0));
			}

			virtual ~cbOptionVariable()
			{
				HB = nullptr;
				Text = nullptr;
			}

			std::function<void(cbOptionVariable*)> fCallBack_Hovered;
			std::function<void(cbOptionVariable*)> fCallBack_Selected;

			void SetInfo(std::u32string inInfo) { Info = inInfo; }
			std::u32string GetInfo() const { return Info; }

			inline void SetText(std::u32string STR) { Text->SetText(STR); }
			inline void SetFontType(eFontType Type) { Text->SetFontType(Type); }
			inline void SetFontSize(std::size_t Size) { Text->SetFontSize(Size); }
			inline void SetTextPadding(cbMargin Padding) { Text->SetPadding(Padding); }
			inline void SetTextVerticalAlignment(eVerticalAlignment VA) { Text->SetVerticalAlignment(VA); }
			inline void SetTextHorizontalAlignment(eHorizontalAlignment HA) { Text->SetHorizontalAlignment(HA); }

			virtual std::size_t GetIndex() const
			{
				return GetOwner<cbSlot>()->GetIndex();
			}

			virtual void OnHovered() override
			{
				if (fCallBack_Hovered)
					fCallBack_Hovered(this);
			}

			virtual void OnSelected() override
			{
				if (!IsSelected())
					return;

				if (fCallBack_Selected)
					fCallBack_Selected(this);

				SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
			}

			virtual void OnUnhovered() override
			{
			}

			virtual void OnDeSelected() override
			{
				if (IsSelected())
					return;

				SetButtonVertexStyle(eButtonState::Default, cbColor::Transparent());
			}

			void AddContext(cbWidget::SharedPtr Widget, eSlotAlignment Alignment = eSlotAlignment::BoundToContent)
			{
				HB->Insert(Widget, Alignment);
			}

			virtual void RestoreDefault() {}
		};

		class cbOptionSubMenuButtonVar : public cbOptionVariable
		{
			cbClassBody(cbClassConstructor, cbOptionSubMenuButtonVar, cbOptionVariable)
		private:
			std::optional<std::size_t> Index;

		public:
			cbOptionSubMenuButtonVar(std::u32string Name, std::optional<std::size_t> inIndex)
				: Super(Name)
				, Index(inIndex)
			{}

			virtual ~cbOptionSubMenuButtonVar()
			{}

			std::function<void(cbOptionSubMenuButtonVar*)> fCallback_OnSelect;

			std::optional<std::size_t> GetSubMenuIndex() const
			{
				return Index;
			}

			virtual void OnSelected() override
			{
				Super::OnSelected();
				if (fCallback_OnSelect)
					fCallback_OnSelect(this);
			}
		};

		class cbOptionSubMenuPopupVar : public cbOptionSubMenuButtonVar
		{
			cbClassBody(cbClassConstructor, cbOptionSubMenuPopupVar, cbOptionSubMenuButtonVar)
		private:
			cbPopupMenu::SharedPtr Popup;

		public:
			cbOptionSubMenuPopupVar(std::u32string Name, std::u32string PopupHeader, std::u32string PopupDesc)
				: Super(Name, std::nullopt)
			{
				Popup = cbPopupMenu::Create(PopupHeader, PopupDesc);
			}

			virtual ~cbOptionSubMenuPopupVar()
			{
				Popup = nullptr;
			}

			void BindFunction_CONFIRM(std::function<void()> pfcb)
			{
				Popup->fCallBack_CONFIRM = pfcb;
			}

			void BindFunction_CANCEL(std::function<void()> pfcb)
			{
				Popup->fCallBack_CANCEL = pfcb;
			}

			virtual void OnSelected() override
			{
				Super::OnSelected();
				Popup->AddToCanvas(GetCanvas());
			}
		};

		class cbOptionPopup : public cbOptionVariable
		{
			cbClassBody(cbClassConstructor, cbOptionPopup, cbOptionVariable)
		private:
			cbPopupMenu::SharedPtr PopupMenu;

		public:
			cbOptionPopup(std::u32string Name, std::u32string PopupHeader, std::u32string PopupDesc)
				: Super(Name)
			{
				PopupMenu = cbPopupMenu::Create(PopupHeader, PopupDesc);
			}

			virtual ~cbOptionPopup()
			{
				PopupMenu = nullptr;
			}

			void BindFunction_CONFIRM(std::function<void()> pfcb)
			{
				PopupMenu->fCallBack_CONFIRM = pfcb;
			}

			void BindFunction_CANCEL(std::function<void()> pfcb)
			{
				PopupMenu->fCallBack_CANCEL = pfcb;
			}

			virtual void OnSelected() override
			{
				Super::OnSelected();

				PopupMenu->AddToCanvas(GetCanvas());

				Deselect();
			}
		};

		class cbSliderVar : public cbOptionVariable
		{
			cbClassBody(cbClassConstructor, cbSliderVar, cbOptionVariable)
		private:
			cbSlider::SharedPtr Slider;
			cbString::SharedPtr SliderText;
			float DefaultValue;
			std::function<void(std::size_t)> fCallBack;
		public:
			cbSliderVar(std::u32string Name, std::function<void(std::size_t)> pfCallBack)
				: Super(Name)
				, DefaultValue(0.0f)
				, fCallBack(pfCallBack)
			{
				SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

				Slider = cbSlider::Create();
				Slider->SetName("SliderVar");
				Slider->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				Slider->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				Slider->SetIntegerOnly(true);
				Slider->SetMaximumValue(20.0f);
				Slider->BindFunctionTo_OnValueChanged(std::bind(&cbSliderVar::OnSliderValueChanged, this, std::placeholders::_1));
				Slider->SetBarHorizontalFill(true);
				Slider->SetBarVerticalFill(true);
				Slider->SetHandleFillMode(true);
				Slider->SetHandleFillThickness(true);
				Slider->SetHandlePadding(1);
				Slider->SetHandleVertexColorStyle(cbColor::Black());
				AddContext(Slider, eSlotAlignment::BoundToContent);
				{
					cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
					Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Right);
					Empty->SetMinimumWidth(65.0f);
					AddContext(Empty, eSlotAlignment::BoundToContent);
					{
						SliderText = cbString::Create("[0]", cbTextDesc(12));
						SliderText->SetDefaultSpaceWidth(3);
						SliderText->SetVerticalAlignment(eVerticalAlignment::Align_Center);
						SliderText->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						SliderText->SetTextJustify(eTextJustify::Right);
						Empty->SetContent(SliderText);
					}
				}
			}

			virtual ~cbSliderVar()
			{
				Slider = nullptr;
			}

			void OnSliderValueChanged(float value)
			{
				if (Slider->IsIntegerOnly())
					SliderText->SetString("[" + std::to_string((int)value) + "]");
				else
					SliderText->SetString("[" + std::to_string(value) + "]");

				if (fCallBack)
					fCallBack((std::size_t)value);
			}

			void SetPercent(float Percent) { Slider->SetSliderValueByPercent(Percent); }
			void SetValue(float Value) { Slider->SetSliderValue(Value); }

			void SetMaximumValue(float Value)
			{
				Slider->SetMaximumValue(Value);
			}

			void SetDefaultValue(float Value)
			{
				DefaultValue = Value;
				Slider->SetSliderValue(DefaultValue);
			}

			virtual void RestoreDefault() override
			{
				Slider->SetSliderValue(DefaultValue);
			}
		};

		class cbBoolSliderVar : public cbOptionVariable
		{
			cbClassBody(cbClassConstructor, cbBoolSliderVar, cbOptionVariable)
		private:
			cbSlider::SharedPtr Slider;
			cbString::SharedPtr SliderText;
			float DefaultValue;
			bool bIsSelected;
			int modifier;
			std::function<void(bool)> fCallBack;
		public:
			cbBoolSliderVar(std::u32string Name, std::function<void(bool)> pfCallBack)
				: Super(Name)
				, DefaultValue(0.0f)
				, bIsSelected(false)
				, modifier(4)
				, fCallBack(pfCallBack)
			{
				SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

				Slider = cbSlider::Create();
				Slider->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				Slider->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				Slider->SetMaximumValue(1.0f);
				Slider->SetBarLength(25.0f);
				Slider->BindFunctionTo_OnValueChanged(std::bind(&cbBoolSliderVar::OnSliderValueChanged, this, std::placeholders::_1));
				Slider->SetBarHorizontalFill(true);
				Slider->SetBarVerticalFill(true);
				Slider->SetHandleFillThickness(true);
				Slider->SetHandlePadding(1);
				Slider->SetHandleVertexColorStyle(cbColor::Black());
				Slider->SetFocusable(false);
				AddContext(Slider, eSlotAlignment::BoundToContent);
				{
					cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
					Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Right);
					Empty->SetMinimumWidth(40.0f);
					AddContext(Empty, eSlotAlignment::BoundToContent);
					{
						SliderText = cbString::Create("[0]", cbTextDesc(12));
						SliderText->SetDefaultSpaceWidth(3);
						SliderText->SetVerticalAlignment(eVerticalAlignment::Align_Center);
						SliderText->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						SliderText->SetTextJustify(eTextJustify::Right);
						Empty->SetContent(SliderText);
					}
				}
			}

			virtual ~cbBoolSliderVar()
			{
				Slider = nullptr;
			}

			virtual void OnTick(float DT) override
			{
				if (bIsSelected)
				{
					Slider->SetSliderValue(Slider->GetValue() + DT * modifier);
				}
			}

			void OnSliderValueChanged(float value)
			{
				if (!(value == 0.0f || value == 1.0f))
					return;

				if (value == 0.0f)
					modifier = 4;
				else if (value == (1.0f))
					modifier = -4;

				bIsSelected = false;

				SliderText->SetString("[" + std::to_string((int)value) + "]");

				if (fCallBack)
					fCallBack(value <= 0.0f ? false : value >= 1.0f ? true : false);
			}

			void SetPercent(float Percent) { Slider->SetSliderValueByPercent(Percent); }
			void SetValue(float Value) { Slider->SetSliderValue(Value); }

			void SetDefaultValue(float Value)
			{
				DefaultValue = Value;
				Slider->SetSliderValue(DefaultValue);
			}

			virtual void RestoreDefault() override
			{
				Slider->SetSliderValue(DefaultValue);
			}

			virtual void OnSelected() override
			{
				Super::OnSelected();
				bIsSelected = true;
			}
		};

		class IOptionNamedButtonVar : public cbOptionVariable
		{
			cbClassBody(cbClassNoDefaults, IOptionNamedButtonVar, cbOptionVariable)
		public:
			IOptionNamedButtonVar(std::u32string Name)
				: Super(Name)
			{}

			virtual ~IOptionNamedButtonVar()
			{}

			virtual cbVerticalBox::SharedPtr GetContext() const = 0;
		};

		template<typename T>
		class cbOptionNamedButtonVar : public IOptionNamedButtonVar
		{
			cbClassBody(cbClassConstructor, cbOptionNamedButtonVar, IOptionNamedButtonVar)
		private:
			struct sButtonResult
			{
				std::u32string String;
				T Result;

				sButtonResult(std::u32string inString, T inResult)
					: String(inString)
					, Result(inResult)
				{}
			};

			class cbOptionNamedButtonVar_TabVB : public cbgui::cbVerticalBox
			{
				cbClassBody(cbClassConstructor, cbOptionNamedButtonVar_TabVB, cbgui::cbVerticalBox)
			private:
				cbOptionNamedButtonVar* OwnedOption;
				std::vector<sButtonResult> Results;
			public:
				cbOptionNamedButtonVar_TabVB(cbOptionNamedButtonVar* pOwnedOption)
					: Super()
					, OwnedOption(pOwnedOption)
				{
					SetVerticalAlignment(eVerticalAlignment::Align_Top);
					SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				}

				virtual ~cbOptionNamedButtonVar_TabVB() = default;

				void AddOption(std::u32string String, T result)
				{
					Results.push_back(sButtonResult(String, result));

					std::shared_ptr<cbOptionVariable> NamedButton = cbOptionVariable::Create(String);

					NamedButton->SetVerticalAlignment(eVerticalAlignment::Align_Top);
					NamedButton->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

					NamedButton->fCallBack_Hovered = std::bind(&cbOptionNamedButtonVar_TabVB::OnOptionHovered, this, std::placeholders::_1);
					NamedButton->fCallBack_Selected = std::bind(&cbOptionNamedButtonVar_TabVB::OnOptionSelected, this, std::placeholders::_1);

					Insert(NamedButton);
				}

				std::size_t GetOptionSize() const
				{
					return Results.size();
				}

				std::u32string GetOptionSTR(std::size_t Index) const
				{
					return Results.at(Index).String;
				}

				sButtonResult GetOptionResult(std::size_t Index) const
				{
					return Results.at(Index);
				}

				void OnOptionHovered(cbOptionVariable* Selected)
				{

				}

				void SelectOption(std::size_t Index)
				{
					for (std::size_t i = 0; i < GetSlotSize(); i++)
					{
						const auto ButtonBase = GetSlot(i)->GetContent<cbOptionVariable>();

						if (ButtonBase && Index == i)
						{
							ButtonBase->Select();
						}
					}
				}

				void OnOptionSelected(cbOptionVariable* Selected)
				{
					for (std::size_t i = 0; i < GetSlotSize(); i++)
					{
						const auto ButtonBase = GetSlot(i)->GetContent<cbOptionVariable>();

						if (ButtonBase)
						{
							if (ButtonBase->IsSelected() && ButtonBase != Selected)
							{
								ButtonBase->Deselect();
							}
						}
					}
					OwnedOption->OnOptionSelected(Results.at(Selected->GetIndex()));
				}
			};

		private:
			std::shared_ptr<cbOptionNamedButtonVar_TabVB> Tab;
			cbString::SharedPtr Text;
			std::size_t defaultIndex;

		public:
			cbOptionNamedButtonVar(std::u32string Name)
				: Super(Name)
				, Text(cbString::Create("", cbTextDesc(12, eTextJustify::Right)))
				, defaultIndex(0)
			{
				SetVerticalAlignment(eVerticalAlignment::Align_Center);
				SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

				Tab = cbOptionNamedButtonVar_TabVB::Create(this);

				Text->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				Text->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				Text->SetFocusable(false);
				//Text->SetPadding(cbMargin(0, 0, 5, 0));

				AddContext(Text);
			}

			virtual ~cbOptionNamedButtonVar()
			{
				Text = nullptr;
				Tab = nullptr;

				fCallback_OnSelect = nullptr;
				fCallback_OnOptionSelected = nullptr;
				fCallback_OnOptionDeSelected = nullptr;
				fResultCallback = nullptr;
			}

			std::function<void(IOptionNamedButtonVar*)> fCallback_OnSelect;
			std::function<void(IOptionNamedButtonVar*)> fCallback_OnOptionSelected;
			std::function<void(IOptionNamedButtonVar*)> fCallback_OnOptionDeSelected;
			std::function<void(T)> fResultCallback;

			virtual cbVerticalBox::SharedPtr GetContext() const override final { return Tab; }

			void AddOption(std::u32string String, T result)
			{
				Tab->AddOption(String, result);
				Text->SetText(Tab->GetOptionSTR(0));
			}

			void SetDefault(std::size_t Index)
			{
				if (defaultIndex >= Tab->GetOptionSize())
					return;
				defaultIndex = Index;
				Tab->SelectOption(defaultIndex);
			}

			virtual void RestoreDefault() override
			{
				if (defaultIndex < Tab->GetOptionSize())
					Tab->SelectOption(defaultIndex);
			}

			void OnOptionSelected(sButtonResult Result)
			{
				Text->SetText(Result.String);
				if (fCallback_OnOptionSelected)
					fCallback_OnOptionSelected(this);
				if (fResultCallback)
					fResultCallback(Result.Result);
			}

			virtual void OnSelected() override
			{
				Super::OnSelected();
				if (fCallback_OnSelect)
					fCallback_OnSelect(this);
			}
			virtual void OnDeSelected() override
			{
				Super::OnDeSelected();
				if (fCallback_OnOptionDeSelected)
					fCallback_OnOptionDeSelected(this);
			}
		};

		template<typename T>
		class cbOptionComboBoxVar : public cbOptionVariable
		{
			cbClassBody(cbClassConstructor, cbOptionComboBoxVar, cbOptionVariable)
		private:
			std::map<std::size_t, T> Results;
			cbComboBox::SharedPtr ComboBox;
			std::function<void(T)> fCallback;
			std::size_t defaultIndex;

		public:
			cbOptionComboBoxVar(std::u32string Name, std::function<void(T)> Callback)
				: Super(Name)
				, fCallback(Callback)
				, defaultIndex(0)
			{
				SetVerticalAlignment(eVerticalAlignment::Align_Center);
				SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

				ComboBox = cbComboBox::Create();

				ComboBox->HideMenuButton(true);

				{
					cbButtonVertexColorStyle ScrollBox_HandleVetexStyle;
					ScrollBox_HandleVetexStyle.SetDefaultColor(cbColor::Red());
					ScrollBox_HandleVetexStyle.SetHoveredColor(ScrollBox_HandleVetexStyle.GetColor(eButtonState::Default));
					ScrollBox_HandleVetexStyle.SetPressedColor(ScrollBox_HandleVetexStyle.GetColor(eButtonState::Default));

					auto NamedButtonInterface = ComboBox->GetMenuInterface<cbComboBox::cbComboBoxMenuSlot::cbComboBoxNamedButtonInterface>();
					NamedButtonInterface->SetTextJustify(eTextJustify::Right);
					NamedButtonInterface->SetFontSize(12);
					NamedButtonInterface->SetTextVertexStyle(cbColor::White());
					NamedButtonInterface->SetButtonVertexStyle(cbButtonVertexColorStyle(cbColor::White(), cbColor::White(), cbColor::White(), cbColor::White()));
					ComboBox->SetOptionHoverVetexStyle(cbColor::Grey());
					ComboBox->SetScrollBox_BarVetexStyle(cbColor(0.5f, 0.5f, 0.5f));
					ComboBox->SetScrollBox_HandleVetexStyle(ScrollBox_HandleVetexStyle);
					ComboBox->SetListBackgroundVertexStyle(cbColor(0.0097f, 0.0116f, 0.01685f));
				}

				ComboBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ComboBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Right);

				ComboBox->BindFunctionTo_OnSelectionChanged(std::bind(&cbOptionComboBoxVar::OnSelectionChanged, this, std::placeholders::_1, std::placeholders::_2));
				//ComboBox->SetPadding(cbMargin(0, 0, 5, 0));
				AddContext(ComboBox);
			}

			virtual ~cbOptionComboBoxVar()
			{
				ComboBox = nullptr;
				fCallback = nullptr;
				Results.clear();
			}

			void SetDefault(std::size_t Index)
			{
				defaultIndex = Index;
				ComboBox->SelectItem(defaultIndex);
			}

			virtual void RestoreDefault() override
			{
				ComboBox->SelectItem(defaultIndex);
			}

			void OnSelectionChanged(cbSlot* SelectedOption, cbWidget* SelectedContent)
			{
				if (fCallback)
					fCallback(Results.at(SelectedOption->GetIndex()));
			}

			void AddOption(std::u32string STR, T result)
			{
				Results.insert({ Results.size(), result });

				cbTextDesc Desc;
				Desc.CharSize = 12;
				Desc.TextJustify = eTextJustify::Right;

				ComboBox->Insert(STR, Desc, cbColor::White());
			}

			void AddOption(std::string STR, T result)
			{
				Results.insert({ Results.size(), result });

				cbTextDesc Desc;
				Desc.CharSize = 12;
				Desc.TextJustify = eTextJustify::Right;

				ComboBox->Insert(STR, Desc, cbColor::White());
			}

			void SetActiveOption(std::size_t Index)
			{
				ComboBox->SelectItem(Index);
			}
		};

		class cbOptionTabHB : public cbHorizontalBox
		{
			cbClassBody(cbClassConstructor, cbOptionTabHB, cbHorizontalBox)
		private:
			class cbOptionTabHB_ScrollBox : public cbAutoScroller
			{
				cbClassBody(cbClassConstructor, cbOptionTabHB_ScrollBox, cbAutoScroller)
			public:
				cbOptionTabHB_ScrollBox()
					: Super(eOrientation::Vertical)
				{
					SetVerticalAlignment(eVerticalAlignment::Align_Top);
					SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Scrollbox->ShowScrollBarIfScrollable(true);

					cbButtonVertexColorStyle ColorStyle;
					ColorStyle.SetDefaultColor(cbColor::Red());
					ColorStyle.SetHoveredColor(ColorStyle.GetColor(eButtonState::Default));
					ColorStyle.SetPressedColor(ColorStyle.GetColor(eButtonState::Default));
					Scrollbox->SetScrollBarHandleVertexColorStyle(ColorStyle);
					Scrollbox->SetScrollBarBackgroundVertexColorStyle(cbColor(0.0097f, 0.0116f, 0.01685f));

					Scrollbox->SetScrollBarThickness(2.5f);
					Scrollbox->SetScrollBarPadding(cbMargin(5, 0, 5, 0));
				}

				virtual ~cbOptionTabHB_ScrollBox() = default;

				virtual void OnTick(float InDeltaTime) override
				{
					Super::OnTick(InDeltaTime);
				}

				virtual void OnOptionSelected(cbOptionVariable* Selected)
				{
					for (std::size_t i = 0; i < Scrollbox->GetSlotSize(); i++)
					{
						const auto ButtonBase = Scrollbox->GetSlot(i)->GetContent<cbOptionVariable>();

						if (ButtonBase)
						{
							if (ButtonBase->IsSelected() && ButtonBase != Selected)
							{
								ButtonBase->Deselect();
							}
						}
					}
				}

				void RestoreDefaults()
				{
					for (std::size_t i = 0; i < Scrollbox->GetSlotSize(); i++)
					{
						const auto ButtonBase = Scrollbox->GetSlot(i)->GetContent<cbOptionVariable>();
						ButtonBase->RestoreDefault();
					}
					ResetScrollBar();
				}

				void AddOption(cbOptionVariable::SharedPtr Context)
				{
					Context->fCallBack_Hovered = std::bind(&cbOptionTabHB_ScrollBox::OnOptionHovered, this, std::placeholders::_1);
					Context->fCallBack_Selected = std::bind(&cbOptionTabHB_ScrollBox::OnOptionSelected, this, std::placeholders::_1);
					Insert(Context);
				}

				void OnOptionHovered(cbOptionVariable* Context)
				{
					GetRootOwner<cbOptionTabHB>(1, false)->OnOptionHovered(Context);
				}

				virtual void Deselect()
				{
					for (std::size_t i = 0; i < Scrollbox->GetSlotSize(); i++)
					{
						const auto ButtonBase = Scrollbox->GetSlot(i)->GetContent<cbOptionVariable>();

						if (ButtonBase)
						{
							ButtonBase->Deselect();
						}
					}
				}
			};

		private:
			cbOptionTabHB_ScrollBox::SharedPtr Context;
			cbVerticalBox::SharedPtr OptionInfo;
			cbString::SharedPtr InfoText;
			cbWidget::SharedPtr OptionContext;

		public:
			cbOptionTabHB()
				: Super()
			{
				{
					SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

					Context = cbOptionTabHB_ScrollBox::Create();
					Insert(Context, eSlotAlignment::BoundToSlot);

					OptionInfo = cbVerticalBox::Create();
					OptionInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					OptionInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					OptionInfo->SetPadding(cbMargin(25, 0, 0, 0));

					InfoText = cbString::Create("", cbTextDesc(16));
					InfoText->SetDefaultSpaceWidth(3);
					InfoText->SetFocusable(false);
					InfoText->SetVerticalAlignment(eVerticalAlignment::Align_Top);
					InfoText->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					InfoText->SetAutoWrap(true);
					InfoText->SetAutoGrowVerticesSize(true, 376);

					cbTextStyle TextStyle(U"Size_8_Style");
					TextStyle.FontSize = 8;
					InfoText->AddTextStyle(TextStyle);
					InfoText->SetTextStylingEnabled(true);

					OptionInfo->Insert(InfoText, eSlotAlignment::BoundToSlot);
					{
						cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
						Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Empty->SetWidth(50);
						OptionInfo->Insert(Empty, eSlotAlignment::BoundToSlot);
					}
				}
				Insert(OptionInfo, eSlotAlignment::BoundToSlot);
			}

			virtual ~cbOptionTabHB()
			{
				InfoText = nullptr;
				OptionInfo = nullptr;
				Context = nullptr;
			}

			void Reset()
			{
				Context->Deselect();
				InfoText->Clear();
			}

			void OnOptionHovered(cbOptionVariable* Context)
			{
				InfoText->SetText(Context->GetInfo());
			}

			void RestoreDefaults()
			{
				Context->RestoreDefaults();
			}

			void OnOptionSelected(cbOptionVariable* Selected)
			{
			}

			void OptionNamedButtonVarOptionSelected(IOptionNamedButtonVar* Var)
			{
				if (OptionContext)
					OptionContext->RemoveFromParent();
				OptionContext = nullptr;
				Reset();
			}

			void OptionNamedButtonVarSelected(IOptionNamedButtonVar* Var)
			{
				auto VarContext = Var->GetContext();
				if (OptionContext != VarContext)
				{
					if (OptionContext)
						OptionContext->RemoveFromParent();
					OptionContext = nullptr;
					OptionContext = VarContext;
					OptionInfo->Insert(OptionContext);
				}
				else if (VarContext == nullptr)
				{
					if (OptionContext)
						OptionContext->RemoveFromParent();
					OptionContext = nullptr;
				}
			}

			void OnOptionDeSelected(IOptionNamedButtonVar* Var)
			{
				if (OptionContext)
					OptionContext->RemoveFromParent();
				OptionContext = nullptr;
			}

			cbSliderVar* CreateSliderVar(std::u32string Name, std::function<void(std::size_t)> fCallBack)
			{
				cbSliderVar::SharedPtr SliderVar = cbSliderVar::Create(Name, fCallBack);
				Context->AddOption(SliderVar);
				return SliderVar.get();
			}

			cbBoolSliderVar* CreateBoolSliderVar(std::u32string Name, std::function<void(bool)> fCallBack)
			{
				cbBoolSliderVar::SharedPtr SliderVar = cbBoolSliderVar::Create(Name, fCallBack);
				Context->AddOption(SliderVar);
				return SliderVar.get();
			}

			cbOptionVariable* CreateOptionPopupButtonVar(std::u32string Name, std::u32string PopupHeader, std::u32string PopupDesc, std::function<void()> fCallBack)
			{
				cbOptionPopup::SharedPtr Popup = cbOptionPopup::Create(Name, PopupHeader, PopupDesc);
				Context->AddOption(Popup);
				Popup->BindFunction_CONFIRM(fCallBack);
				return Popup.get();
			}

			cbOptionSubMenuButtonVar* CreateOptionSubMenuButtonVar(std::u32string Name, std::size_t Index, std::function<void(cbOptionSubMenuButtonVar*)> fCallBack)
			{
				std::shared_ptr<cbOptionSubMenuButtonVar> SubMenu = cbOptionSubMenuButtonVar::Create(Name, Index);
				SubMenu->fCallback_OnSelect = fCallBack;
				Context->AddOption(SubMenu);
				return SubMenu.get();
			}

			template<typename T>
			cbOptionComboBoxVar<T>* CreateComboBoxVar(std::u32string Name, std::function<void(T)> Callback)
			{
				std::shared_ptr<cbOptionComboBoxVar<T>> ComboBox = cbOptionComboBoxVar<T>::Create(Name, Callback);
				Context->AddOption(ComboBox);
				return ComboBox.get();
			}

			template<typename T>
			cbOptionNamedButtonVar<T>* CreateOptionNamedButtonVar(std::u32string Name, std::function<void(T)> fCallBack = nullptr)
			{
				std::shared_ptr<cbOptionNamedButtonVar<T>> OptionNamedButtonVar = cbOptionNamedButtonVar<T>::Create(Name);
				OptionNamedButtonVar->fCallback_OnSelect = std::bind(&cbOptionTabHB::OptionNamedButtonVarSelected, this, std::placeholders::_1);
				OptionNamedButtonVar->fCallback_OnOptionSelected = std::bind(&cbOptionTabHB::OptionNamedButtonVarOptionSelected, this, std::placeholders::_1);
				OptionNamedButtonVar->fCallback_OnOptionDeSelected = std::bind(&cbOptionTabHB::OnOptionDeSelected, this, std::placeholders::_1);
				OptionNamedButtonVar->fResultCallback = fCallBack;
				Context->AddOption(OptionNamedButtonVar);
				return OptionNamedButtonVar.get();
			}
		};

	private:
		cbVerticalBox::SharedPtr OptionList;
		cbOverlay::SharedPtr OptionContext;

	public:
		cbOptions()
		{
			SetVerticalAlignment(eVerticalAlignment::Align_Top);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 50, 0));

			{
				OptionList = cbVerticalBox::Create();
				OptionList->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				OptionList->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);

				auto CreateOptionSubMenuButtonVar = [&](std::u32string Name, std::optional<std::size_t> Index, std::function<void(cbOptionSubMenuButtonVar*)> fCallBack) -> cbOptionSubMenuButtonVar*
				{
					std::shared_ptr<cbOptionSubMenuButtonVar> SubMenu = cbOptionSubMenuButtonVar::Create(Name, Index);
					SubMenu->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Left);
					SubMenu->SetPadding(cbMargin(0, 0, 50, 0));
					SubMenu->fCallback_OnSelect = fCallBack;
					OptionList->Insert(SubMenu);
					return SubMenu.get();
				};

				CreateOptionSubMenuButtonVar(U"\x2634 GAMEPLAY", 0, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1));
				CreateOptionSubMenuButtonVar(U"☴ CONTROLS", 2, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1))->Disable();
				CreateOptionSubMenuButtonVar(U"☴ MOUSE & KEYBOARD", 3, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1))->Disable();
				CreateOptionSubMenuButtonVar(U"\x2B23 GRAPHICS", 4, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1));
				CreateOptionSubMenuButtonVar(U"♬ AUDIO", 5, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1));
				CreateOptionSubMenuButtonVar(U"☴ CREDITS", std::nullopt, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1));
				{
					std::shared_ptr<cbOptionSubMenuPopupVar> SubMenu = cbOptionSubMenuPopupVar::Create(U"\x2716 QUIT GAME", U"QUIT GAME", U"Are you sure you want to quit the game?");
					SubMenu->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Left);
					SubMenu->SetPadding(cbMargin(0, 0, 50, 0));
					SubMenu->fCallback_OnSelect = std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1);
					SubMenu->BindFunction_CONFIRM(std::bind(&cbOptions::QuitGameButton, this));
					OptionList->Insert(SubMenu);
				}
				Insert(OptionList, eSlotAlignment::BoundToContent);

				{
					OptionContext = cbOverlay::Create();
					OptionContext->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					OptionContext->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					Insert(OptionContext, eSlotAlignment::BoundToSlot);

					OptionContext->SetVisibilityState(eVisibility::Hidden);
					OptionContext->Disable();
				}
				{
					cbOptionTabHB::SharedPtr HB = cbOptionTabHB::Create();
					OptionContext->Insert(HB);
					{
						auto OptionNamedButtonVar = HB->CreateOptionSubMenuButtonVar(U"GENERAL", 1, std::bind(&cbOptions::SelectSubMenu, this, std::placeholders::_1));
						OptionNamedButtonVar->SetInfo(U"General");

						{
							cbOptionTabHB::SharedPtr HB_Sub = cbOptionTabHB::Create();
							OptionContext->Insert(HB_Sub);
							{
								auto OptionNamedButtonVar = HB_Sub->CreateOptionNamedButtonVar<bool>(U"SubMenu Item 1");
								OptionNamedButtonVar->AddOption(U"[OFF]", false);
								OptionNamedButtonVar->AddOption(U"[ON]", true);
								OptionNamedButtonVar->SetInfo(U"SubMenu Item 1");
								OptionNamedButtonVar->Disable();
							}
							{
								auto OptionNamedButtonVar = HB_Sub->CreateOptionNamedButtonVar<bool>(U"SubMenu Item 2");
								OptionNamedButtonVar->AddOption(U"[OFF]", false);
								OptionNamedButtonVar->AddOption(U"[ON]", true);
								OptionNamedButtonVar->SetInfo(U"SubMenu Item 2");
								OptionNamedButtonVar->Disable();
							}
							{
								auto OptionNamedButtonVar = HB_Sub->CreateOptionNamedButtonVar<bool>(U"SubMenu Item 3");
								OptionNamedButtonVar->AddOption(U"[OFF]", false);
								OptionNamedButtonVar->AddOption(U"[ON]", true);
								OptionNamedButtonVar->SetInfo(U"SubMenu Item 3");
								OptionNamedButtonVar->Disable();
							}
							{
								auto OptionNamedButtonVar = HB_Sub->CreateOptionNamedButtonVar<bool>(U"SubMenu Item 4");
								OptionNamedButtonVar->AddOption(U"[OFF]", false);
								OptionNamedButtonVar->AddOption(U"[ON]", true);
								OptionNamedButtonVar->SetInfo(U"SubMenu Item 4");
								OptionNamedButtonVar->Disable();
							}
							{
								auto OptionNamedButtonVar = HB_Sub->CreateOptionNamedButtonVar<short>(U"SubMenu Item 5");
								OptionNamedButtonVar->AddOption(U"[HOLD]", 0);
								OptionNamedButtonVar->AddOption(U"[TOGGLE]", 1);
								OptionNamedButtonVar->SetInfo(U"SubMenu Item 5");
								OptionNamedButtonVar->Disable();
							}
							{
								auto OptionNamedButtonVar = HB_Sub->CreateOptionPopupButtonVar(U"RESTORE TO DEFAULTS", U"RESTORE DEFAULTS", U"Are you sure you want to restore to defaults?", std::bind(&cbOptionTabHB::RestoreDefaults, HB_Sub.get()));
								OptionNamedButtonVar->SetInfo(U"RESTORE TO DEFAULTS");
							}
						}
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<bool>(U"SubMenu Item 0");
						OptionNamedButtonVar->AddOption(U"[OFF]", false);
						OptionNamedButtonVar->AddOption(U"[ON]", true);
						OptionNamedButtonVar->SetInfo(U"SubMenu Item 0");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionPopupButtonVar(U"RESTORE TO DEFAULTS", U"RESTORE DEFAULTS", U"Are you sure you want to restore to defaults?", std::bind(&cbOptionTabHB::RestoreDefaults, HB.get()));
						OptionNamedButtonVar->SetInfo(U"RESTORE TO DEFAULTS");
					}
				}
				{
					cbOptionTabHB::SharedPtr HB = cbOptionTabHB::Create();
					OptionContext->Insert(HB);
				}
				{
					cbOptionTabHB::SharedPtr HB = cbOptionTabHB::Create();
					OptionContext->Insert(HB);
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<bool>(U"INVERT MOUSE Y");
						OptionNamedButtonVar->AddOption(U"[OFF]", false);
						OptionNamedButtonVar->AddOption(U"[ON]", true);
						OptionNamedButtonVar->SetInfo(U"INVERT MOUSE Y");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<bool>(U"MOUSE SENSITIVITY"); // Slider
						OptionNamedButtonVar->SetInfo(U"MOUSE SENSITIVITY");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<bool>(U"KEY BINDINGS"); // Slider
						OptionNamedButtonVar->SetInfo(U"KEY BINDINGS");
						OptionNamedButtonVar->Disable();
					}
				}
				{
					cbOptionTabHB::SharedPtr HB = cbOptionTabHB::Create();
					OptionContext->Insert(HB);
					{
						auto ComboBox = HB->CreateComboBoxVar<cbDimension>(U"FULLSCREEN RESOLUTION", std::bind(&cbOptions::ResolutionDimensionChanged, this, std::placeholders::_1));
						ComboBox->AddOption(U"[1280X720]", cbDimension(1280, 728));
						ComboBox->AddOption(U"[1366X768]", cbDimension(1366, 768));
						ComboBox->AddOption(U"[1600X900]", cbDimension(1600, 900));
						ComboBox->AddOption(U"[1920X1080]", cbDimension(1920, 1080));
						ComboBox->SetActiveOption(1);
						ComboBox->SetDefault(1);
						ComboBox->SetInfo(U"Fullscreen Resolution\n<Size_8_Style>Set this to the resolution of your monitor. If you experience performance issues, try a lower resolution.</>");
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"DISPLAY MODE", std::bind(&cbOptions::DisplayModeChanged, this, std::placeholders::_1));
						OptionNamedButtonVar->AddOption(U"[FULLSCREEN]", 0);
						OptionNamedButtonVar->AddOption(U"[BORDERLESS WINDOWED]", 1);
						OptionNamedButtonVar->AddOption(U"[WINDOWED]", 2);
						OptionNamedButtonVar->SetDefault(1);
						OptionNamedButtonVar->SetInfo(U"Display Mode\n<Size_8_Style>Play in window, exclusive full screen or full screen.</>");
					}
					{
						auto OptionNamedButtonVar = HB->CreateBoolSliderVar(U"VSYNC", std::bind(&cbOptions::VSYNC, this, std::placeholders::_1));
						OptionNamedButtonVar->SetInfo(U"VSYNC");
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"VSYNC INTERVAL", std::bind(&cbOptions::VSYNC_Interval, this, std::placeholders::_1));
						OptionNamedButtonVar->AddOption(U"[1]", 1);
						OptionNamedButtonVar->AddOption(U"[2]", 2);
						OptionNamedButtonVar->SetDefault(0);
						OptionNamedButtonVar->SetInfo(U"VSYNC INTERVAL");
					}
					{
						auto OptionNamedButtonVar = HB->CreateSliderVar(U"Graphics Item 1", nullptr);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 1");
						OptionNamedButtonVar->SetMaximumValue(2.0f);
						OptionNamedButtonVar->SetValue(1.0f);
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateSliderVar(U"Graphics Item 2", nullptr);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 2");
						OptionNamedButtonVar->SetMaximumValue(2.0f);
						OptionNamedButtonVar->SetValue(1.0f);
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 3");
						OptionNamedButtonVar->AddOption(U"[LOW]", 0);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 1);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 2);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 3");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 4");
						OptionNamedButtonVar->AddOption(U"[LOW]", 0);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 1);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 2);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 4");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 5");
						OptionNamedButtonVar->AddOption(U"[4X]", 0);
						OptionNamedButtonVar->AddOption(U"[8X]", 1);
						OptionNamedButtonVar->AddOption(U"[16X]", 2);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 5");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 6");
						OptionNamedButtonVar->AddOption(U"[LOW]", 0);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 1);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 2);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 6");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 7");
						OptionNamedButtonVar->AddOption(U"[LOW]", 0);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 1);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 2);
						OptionNamedButtonVar->AddOption(U"[ULTRA]", 3);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 7");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 8");
						OptionNamedButtonVar->AddOption(U"[OFF]", 0);
						OptionNamedButtonVar->AddOption(U"[LOW]", 1);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 2);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 3);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 8");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 9");
						OptionNamedButtonVar->AddOption(U"[OFF]", 0);
						OptionNamedButtonVar->AddOption(U"[LOW]", 1);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 2);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 3);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 9");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 10");
						OptionNamedButtonVar->AddOption(U"[OFF]", 0);
						OptionNamedButtonVar->AddOption(U"[LOW]", 1);
						OptionNamedButtonVar->AddOption(U"[MEDIUM]", 2);
						OptionNamedButtonVar->AddOption(U"[HIGH]", 3);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 10");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"Graphics Item 11");
						OptionNamedButtonVar->AddOption(U"[BASE]", 0);
						OptionNamedButtonVar->AddOption(U"[BEST]", 1);
						OptionNamedButtonVar->SetInfo(U"Graphics Item 11");
						OptionNamedButtonVar->Disable();
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionPopupButtonVar(U"RESTORE TO DEFAULTS", U"RESTORE DEFAULTS", U"Are you sure you want to restore to defaults?", std::bind(&cbOptionTabHB::RestoreDefaults, HB.get()));
						OptionNamedButtonVar->SetInfo(U"RESTORE TO DEFAULTS");
					}
				}
				{
					cbOptionTabHB::SharedPtr HB = cbOptionTabHB::Create();
					OptionContext->Insert(HB);

					{
						auto OptionNamedButtonVar = HB->CreateSliderVar(U"MASTER VOLUME", std::bind(&cbOptions::Volume, this, std::placeholders::_1));
						OptionNamedButtonVar->SetInfo(U"MASTER VOLUME");
					}
					{
						auto OptionNamedButtonVar = HB->CreateSliderVar(U"EFFECTS VOLUME", std::bind(&cbOptions::Volume, this, std::placeholders::_1));
						OptionNamedButtonVar->SetInfo(U"EFFECTS VOLUME");
					}
					{
						auto OptionNamedButtonVar = HB->CreateSliderVar(U"MUSIC VOLUME", std::bind(&cbOptions::Volume, this, std::placeholders::_1));
						OptionNamedButtonVar->SetInfo(U"MUSIC VOLUME");
					}
					{
						auto OptionNamedButtonVar = HB->CreateSliderVar(U"DIALOGUE VOLUME", std::bind(&cbOptions::Volume, this, std::placeholders::_1));
						OptionNamedButtonVar->SetInfo(U"DIALOGUE VOLUME");
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"OUTPUT VOLUME");
						OptionNamedButtonVar->AddOption(U"[SYSTEM]", 0);
						OptionNamedButtonVar->AddOption(U"[STERIO]", 1);
						OptionNamedButtonVar->AddOption(U"[HEADPHONE]", 2);
						OptionNamedButtonVar->SetInfo(U"OUTPUT VOLUME");
						OptionNamedButtonVar->SetDefault(0);
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"QUALITY");
						OptionNamedButtonVar->AddOption(U"[BASE]", 0);
						OptionNamedButtonVar->AddOption(U"[BETTER]", 1);
						OptionNamedButtonVar->AddOption(U"[BEST]", 2);
						OptionNamedButtonVar->SetInfo(U"QUALITY");
						OptionNamedButtonVar->SetDefault(0);
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionNamedButtonVar<short>(U"DYNAMIC RANGE");
						OptionNamedButtonVar->AddOption(U"[CINEMATIC]", 0);
						OptionNamedButtonVar->AddOption(U"[NORMAL]", 1);
						OptionNamedButtonVar->AddOption(U"[INTIMATE]", 2);
						OptionNamedButtonVar->SetInfo(U"DYNAMIC RANGE");
						OptionNamedButtonVar->SetDefault(0);
					}
					{
						auto OptionNamedButtonVar = HB->CreateOptionPopupButtonVar(U"RESTORE TO DEFAULTS", U"RESTORE DEFAULTS", U"Are you sure you want to restore to defaults?", std::bind(&cbOptionTabHB::RestoreDefaults, HB.get()));
						OptionNamedButtonVar->SetInfo(U"RESTORE TO DEFAULTS");
					}
				}
			}
		}

		virtual ~cbOptions()
		{
			OptionList = nullptr;
			OptionContext = nullptr;
		}

		void SelectSubMenu(cbOptionSubMenuButtonVar* BTN)
		{
			if (BTN->GetSubMenuIndex().has_value())
			{
				std::size_t Index = BTN->GetSubMenuIndex().value();
				for (std::size_t i = 0; i < OptionList->GetSlotSize(); i++)
				{
					const auto SubMenu = OptionList->GetSlot(i)->GetContent<cbOptionSubMenuButtonVar>();

					if (SubMenu)
					{
						if (SubMenu->IsSelected() && SubMenu->GetSubMenuIndex() != Index)
						{
							SubMenu->Deselect();
						}
					}
				}

				if (OptionContext->IsHidden())
					OptionContext->SetVisibilityState(eVisibility::Visible);
				if (!OptionContext->IsEnabled())
					OptionContext->Enable();

				for (std::size_t i = 0; i < OptionContext->GetSlotSize(); i++)
				{
					const auto SubMenu = OptionContext->GetSlot(i)->GetContent();

					if (SubMenu)
					{
						SubMenu->SetVisibilityState(eVisibility::Hidden);
						SubMenu->Disable();
					}
				}

				OptionContext->GetSlot(Index)->GetContent()->SetVisibilityState(eVisibility::Visible);
				OptionContext->GetSlot(Index)->GetContent()->Enable();

				OptionContext->GetSlot(Index)->GetContent<cbOptionTabHB>()->Reset();
			}
			else
			{
				for (std::size_t i = 0; i < OptionList->GetSlotSize(); i++)
				{
					const auto SubMenu = OptionList->GetSlot(i)->GetContent<cbOptionSubMenuButtonVar>();

					if (SubMenu)
					{
						if (SubMenu->IsSelected() && SubMenu != BTN)
						{
							SubMenu->Deselect();
						}
					}
				}
				OptionContext->SetVisibilityState(eVisibility::Hidden);
				OptionContext->Disable();
			}
		}

		void ResolutionDimensionChanged(cbDimension Dimension)
		{
			std::cout << Dimension.ToString() << std::endl;

			if (HasCanvas())
				GetCanvas<ICanvas>()->GetPlatformOwner()->ResizeWindow((std::size_t)Dimension.Width, (std::size_t)Dimension.Height);
		}

		void DisplayModeChanged(short Index)
		{
			std::cout << "DisplayModeChanged : " << Index << std::endl;

			if (HasCanvas())
				GetCanvas<ICanvas>()->GetPlatformOwner()->WindowMode(Index);
		}

		void VSYNC(bool val)
		{
			std::cout << "VSYNC : " << val << std::endl;

			if (HasCanvas())
				GetCanvas<ICanvas>()->GetPlatformOwner()->Vsync(val);
		}

		void VSYNC_Interval(short Value)
		{
			std::cout << "VSYNC_Interval : " << Value << std::endl;

			if (HasCanvas())
				GetCanvas<ICanvas>()->GetPlatformOwner()->VsyncInterval(Value);
		}

		void Volume(std::size_t Value)
		{
			std::cout << "Volume : " << Value << std::endl;
		}

		void Credits()
		{

		}

		void QuitGameButton()
		{
			PostQuitMessage(0);
		}
	};
	class cbDestinationsMenu : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, cbDestinationsMenu, cbVerticalBox)
	public:
		cbOverlay::SharedPtr MissionSelectOverlay;
		cbAutoScroller::SharedPtr pDestinationScrollBox;
		cbString::SharedPtr MissionPercentString;
		std::vector<cbScaleBox*> ScaleBoxes;
		cbWarningPopupMenu::SharedPtr WarningPopupMenu;

	public:
		cbDestinationsMenu()
			: Super()
		{
			auto GetOption = [&](std::u32string STR, std::function<void(cbDemoButtonBase*)> fOnSelected) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				ScaleBoxes.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				ScaleBox->SetScale(cbMargin(5, 4, 5, 4));
				{
					cbgui::cbSizeBox::SharedPtr SizeBox = cbSizeBox::Create();
					SizeBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					SizeBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					SizeBox->SetMinimumWidth(250.0f);
					SizeBox->SetMaximumHeight(50.0f);
					{
						cbButtonVertexColorStyle ColorStyle;
						ColorStyle.SetDefaultColor(cbColor::White());
						ColorStyle.SetHoveredColor(cbColor::Red());
						ColorStyle.SetPressedColor(cbColor::Red());
						ColorStyle.SetDisabledColor(cbColor::White());

						cbMultiStringBox::SharedPtr MultiStringBox = cbMultiStringBox::Create(U"Level", STR);
						MultiStringBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						MultiStringBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						MultiStringBox->SetButtonVertexStyle(ColorStyle);
						MultiStringBox->fOnSelected = fOnSelected;
						//MultiStringBox->BG_Image->SetVertexColorStyle(cbColor::White());
						SizeBox->SetContent(MultiStringBox);
					}
					ScaleBox->SetContent(SizeBox);
				}

				ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbDestinationsMenu::ScaleBox_Hovered, this, std::placeholders::_1));
				return ScaleBox;
			};

			{
				WarningPopupMenu = cbWarningPopupMenu::Create(U"UNAVAILABLE", U"Unable to load level.");
			}

			SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

			MissionSelectOverlay = cbOverlay::Create();
			MissionSelectOverlay->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
			MissionSelectOverlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(MissionSelectOverlay);

			{
				pDestinationScrollBox = cbAutoScroller::Create(eOrientation::Horizontal);
				pDestinationScrollBox->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				pDestinationScrollBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				pDestinationScrollBox->Scrollbox->SetScrollBarPadding(cbMargin(50, 10, 50, 5));
				pDestinationScrollBox->Scrollbox->SetScrollBarThickness(2);
				pDestinationScrollBox->Scrollbox->ShowScrollBarIfScrollable(true);
				pDestinationScrollBox->Scrollbox->SetScrollBarBackgroundVertexColorStyle(cbColor(0.0097f, 0.0116f, 0.01685f));

				cbButtonVertexColorStyle ColorStyle;
				ColorStyle.SetDefaultColor(cbColor::Red());
				ColorStyle.SetHoveredColor(ColorStyle.GetColor(eButtonState::Default));
				ColorStyle.SetPressedColor(ColorStyle.GetColor(eButtonState::Default));
				pDestinationScrollBox->Scrollbox->SetScrollBarHandleVertexColorStyle(ColorStyle);

				{
					cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
					Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					Empty->SetWidth(50);
					Empty->Disable();
					pDestinationScrollBox->Insert(Empty);
				}
				{
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 1", std::bind(&cbDestinationsMenu::DESTINATION1_Selected, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 2", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 3", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 4", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 5", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 6", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 7", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
					pDestinationScrollBox->Insert(GetOption(U"DESTINATION 8", std::bind(&cbDestinationsMenu::ShowPopup, this, std::placeholders::_1)));
				}
				{
					cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
					Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
					Empty->SetWidth(50);
					Empty->Disable();
					pDestinationScrollBox->Insert(Empty);
				}
				MissionSelectOverlay->Insert(pDestinationScrollBox);
			}
			{
				cbHorizontalBox::SharedPtr MissionPercentHB = cbHorizontalBox::Create();
				MissionPercentHB->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				MissionPercentHB->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				MissionPercentHB->SetPadding(cbMargin(50, 0, 0, 0));
				Insert(MissionPercentHB);
				{
					cbString::SharedPtr String = cbString::Create("COMPLETION ", cbTextDesc(10));
					String->SetDefaultSpaceWidth(3);
					String->SetFontType(eFontType::Bold);
					String->SetVertexColorStyle(cbColor::White());
					MissionPercentHB->Insert(String);
				}
				{
					MissionPercentString = cbString::Create(std::to_string(cbLevelData::Get().LevelCompletion.at(0)) + "%", cbTextDesc(10));
					MissionPercentString->SetDefaultSpaceWidth(3);
					MissionPercentString->SetFontType(eFontType::Bold);
					MissionPercentString->SetVertexColorStyle(cbColor::White());
					MissionPercentHB->Insert(MissionPercentString);
				}
			}
			Select(0);
		}

		virtual ~cbDestinationsMenu()
		{
			MissionSelectOverlay = nullptr;
			pDestinationScrollBox = nullptr;
			MissionPercentString = nullptr;
			WarningPopupMenu = nullptr;

			for (auto& ScaleBox : ScaleBoxes)
				ScaleBox = nullptr;
			ScaleBoxes.clear();
		}

		void ShowPopup(cbDemoButtonBase* BTN)
		{
			WarningPopupMenu->AddToCanvas(GetCanvas());
		}

		void DESTINATION1_Selected(cbDemoButtonBase* BTN)
		{
			GetRootOwner<cbMainMenu>(1)->ShowStoryMissionMenu(this);
		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (std::size_t i = 0; i < ScaleBoxes.size(); i++)
			{
				const auto& pScaleBox = ScaleBoxes[i];
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbMultiStringBox* MultiStringBox = static_cast<cbMultiStringBox*>(static_cast<cbSizeBox*>(pScaleBox->GetContent())->GetContent());
					MultiStringBox->SetButtonVertexStyle(eButtonState::Default, cbColor::White());
					MultiStringBox->Button->Unhovered();
					if (MultiStringBox->Text1)
						MultiStringBox->Text1->SetVertexColorStyle(cbColor::Black());
					if (MultiStringBox->Text2)
						MultiStringBox->Text2->SetVertexColorStyle(cbColor::Black());
				}
				else
				{
					MissionPercentString->SetString(std::to_string(cbLevelData::Get().LevelCompletion.at(i)) + "%", cbTextDesc(10));
				}
			}

			cbMultiStringBox* MultiStringBox = static_cast<cbMultiStringBox*>(static_cast<cbSizeBox*>(ScaleBox->GetContent())->GetContent());
			if (MultiStringBox)
			{
				MultiStringBox->SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
				if (MultiStringBox->Text1)
					MultiStringBox->Text1->SetVertexColorStyle(cbColor::White());
				if (MultiStringBox->Text2)
					MultiStringBox->Text2->SetVertexColorStyle(cbColor::White());
			}
		}

		void Selected(cbScaleBox* ScaleBox)
		{
			//GetRootOwner<cbMainMenu>(1);
		}

		void Select(std::size_t Index)
		{
			ScaleBoxes[Index]->Scale();
			cbMultiStringBox* MultiStringBox = static_cast<cbMultiStringBox*>(static_cast<cbSizeBox*>(ScaleBoxes[Index]->GetContent())->GetContent());
			if (MultiStringBox)
			{
				MultiStringBox->SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
				if (MultiStringBox->Text1)
					MultiStringBox->Text1->SetVertexColorStyle(cbColor::White());
				if (MultiStringBox->Text2)
					MultiStringBox->Text2->SetVertexColorStyle(cbColor::White());
			}
			MissionPercentString->SetString(std::to_string(cbLevelData::Get().LevelCompletion.at(0)) + "%", cbTextDesc(10));
		}
	};
	class cbCampaigns : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, cbCampaigns, cbVerticalBox)
	public:
		cbOverlay::SharedPtr MissionSelectOverlay;
		cbHorizontalBox::SharedPtr pDestinationHB;
		std::vector<cbScaleBox*> ScaleBoxes;
		cbWarningPopupMenu::SharedPtr WarningPopupMenu;

	public:
		cbCampaigns()
			: Super()
		{
			auto GetOption = [&](std::u32string STR, std::function<void(cbDemoButtonBase*)> fOnSelected) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				ScaleBoxes.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				ScaleBox->SetScale(cbMargin(5, 4, 5, 4));
				{
					cbgui::cbSizeBox::SharedPtr SizeBox = cbSizeBox::Create();
					SizeBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					SizeBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					SizeBox->SetMinimumWidth(180.0f);
					SizeBox->SetMaximumHeight(50.0f);
					{
						cbButtonVertexColorStyle ColorStyle;
						ColorStyle.SetDefaultColor(cbColor::White());
						ColorStyle.SetHoveredColor(cbColor::Red());
						ColorStyle.SetPressedColor(cbColor::Red());
						ColorStyle.SetDisabledColor(cbColor::White());

						cbMultiStringBox::SharedPtr MultiStringBox = cbMultiStringBox::Create(U"Level", STR);
						MultiStringBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						MultiStringBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						MultiStringBox->SetButtonVertexStyle(ColorStyle);
						MultiStringBox->fOnSelected = fOnSelected;
						SizeBox->SetContent(MultiStringBox);
					}
					ScaleBox->SetContent(SizeBox);
				}

				ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbCampaigns::ScaleBox_Hovered, this, std::placeholders::_1));
				return ScaleBox;
			};

			{
				WarningPopupMenu = cbWarningPopupMenu::Create(U"UNAVAILABLE", U"Unable to load level.");
			}

			SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 0, 35));

			MissionSelectOverlay = cbOverlay::Create();
			MissionSelectOverlay->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
			MissionSelectOverlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(MissionSelectOverlay);

			{
				pDestinationHB = cbHorizontalBox::Create();
				pDestinationHB->SetVerticalAlignment(eVerticalAlignment::Align_Top);
				pDestinationHB->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);

				{
					pDestinationHB->Insert(GetOption(U"PROLOGUE", std::bind(&cbCampaigns::ShowPopup, this, std::placeholders::_1)));
					pDestinationHB->Insert(GetOption(U"STORY 1", std::bind(&cbCampaigns::STORY1_Selected, this, std::placeholders::_1)));
					pDestinationHB->Insert(GetOption(U"STORY 2", std::bind(&cbCampaigns::ShowPopup, this, std::placeholders::_1)));
					pDestinationHB->Insert(GetOption(U"STORY 3", std::bind(&cbCampaigns::ShowPopup, this, std::placeholders::_1)));
					pDestinationHB->Insert(GetOption(U"SIDE MISSION", std::bind(&cbCampaigns::ShowPopup, this, std::placeholders::_1)));
				}
				MissionSelectOverlay->Insert(pDestinationHB);
			}
			Select(0);
		}

		virtual ~cbCampaigns()
		{
			MissionSelectOverlay = nullptr;
			pDestinationHB = nullptr;
			WarningPopupMenu = nullptr;

			for (auto& ScaleBox : ScaleBoxes)
				ScaleBox = nullptr;
			ScaleBoxes.clear();
		}

		void ShowPopup(cbDemoButtonBase* BTN)
		{
			WarningPopupMenu->AddToCanvas(GetCanvas());
		}

		void STORY1_Selected(cbDemoButtonBase* BTN)
		{
			auto MainMenu = GetRootOwner<cbMainMenu>(1);
			if (MainMenu)
			{
				MainMenu->ShowStoryMissionListMenu();
			}
		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (std::size_t i = 0; i < ScaleBoxes.size(); i++)
			{
				const auto& pScaleBox = ScaleBoxes[i];
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbMultiStringBox* MultiStringBox = static_cast<cbMultiStringBox*>(static_cast<cbSizeBox*>(pScaleBox->GetContent())->GetContent());
					MultiStringBox->SetButtonVertexStyle(eButtonState::Default, cbColor::White());
					MultiStringBox->Button->Unhovered();
					if (MultiStringBox->Text1)
						MultiStringBox->Text1->SetVertexColorStyle(cbColor::Black());
					if (MultiStringBox->Text2)
						MultiStringBox->Text2->SetVertexColorStyle(cbColor::Black());
				}
			}

			cbMultiStringBox* MultiStringBox = static_cast<cbMultiStringBox*>(static_cast<cbSizeBox*>(ScaleBox->GetContent())->GetContent());
			if (MultiStringBox)
			{
				MultiStringBox->SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
				if (MultiStringBox->Text1)
					MultiStringBox->Text1->SetVertexColorStyle(cbColor::White());
				if (MultiStringBox->Text2)
					MultiStringBox->Text2->SetVertexColorStyle(cbColor::White());
			}
		}

		void Selected(cbScaleBox* ScaleBox)
		{
			//GetRootOwner<cbMainMenu>(1);
		}

		void Select(std::size_t Index)
		{
			ScaleBoxes[Index]->Scale();
			cbMultiStringBox* MultiStringBox = static_cast<cbMultiStringBox*>(static_cast<cbSizeBox*>(ScaleBoxes[Index]->GetContent())->GetContent());
			if (MultiStringBox)
			{
				MultiStringBox->SetButtonVertexStyle(eButtonState::Default, cbColor::Red());
				if (MultiStringBox->Text1)
					MultiStringBox->Text1->SetVertexColorStyle(cbColor::White());
				if (MultiStringBox->Text2)
					MultiStringBox->Text2->SetVertexColorStyle(cbColor::White());
			}
		}
	};
	class cbCareerMenu : public cbVerticalBox
	{
		cbClassBody(cbClassConstructor, cbCareerMenu, cbVerticalBox)
	public:
		std::vector<cbScaleBox*> ScaleBoxes;
		cbImageInfo::SharedPtr ImageInfo;
		cbWarningPopupMenu::SharedPtr WarningPopupMenu;
	public:
		cbCareerMenu()
		{
			auto GetOption = [&](std::u32string STR, std::u32string STR2, const char32_t Emoji, std::function<void(cbDemoButtonBase*)> fOnSelected) -> cbScaleBox::SharedPtr
			{
				cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
				ScaleBoxes.push_back(ScaleBox.get());
				ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				ScaleBox->SetStayActiveAfterFocusLost(true);
				{
					cbOverlay::SharedPtr Overlay = cbOverlay::Create();
					Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					{
						cbgui::cbImage::SharedPtr Image = cbImage::Create();
						Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						Image->SetVertexColorStyle(cbColor::Random());
						Image->SetHeight(372);
						Overlay->Insert(Image);
					}
					{
						cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(STR, STR2, Emoji);
						ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						ImageInfo->fOnSelected = fOnSelected;
						Overlay->Insert(ImageInfo);
					}
					ScaleBox->SetContent(Overlay);
					ScaleBox->BindFunctionTo_OnFocus(std::bind(&cbCareerMenu::ScaleBox_Hovered, this, std::placeholders::_1)); 
				}
				return ScaleBox;
			};

			{
				WarningPopupMenu = cbWarningPopupMenu::Create(U"UNAVAILABLE", U"Unable to load level.");
			}

			SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			SetPadding(cbMargin(50, 0, 50, 0));

			cbHorizontalBox::SharedPtr HorizontalBox = cbHorizontalBox::Create();
			HorizontalBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			HorizontalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(HorizontalBox, eSlotAlignment::BoundToSlot);

			{
				HorizontalBox->Insert(GetOption(U"", U"PLAYER PROFILE", char32_t(0x2630), std::bind(&cbCareerMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
				HorizontalBox->Insert(GetOption(U"", U"CHALLENGES", char32_t(0x265B), std::bind(&cbCareerMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
				HorizontalBox->Insert(GetOption(U"", U"MASTERY", char32_t(0x2600), std::bind(&cbCareerMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
				HorizontalBox->Insert(GetOption(U"", U"INVENTORY", char32_t(0x2692), std::bind(&cbCareerMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
				HorizontalBox->Insert(GetOption(U"", U"TARGETS", char32_t(0x2620), std::bind(&cbCareerMenu::ShowPopup, this, std::placeholders::_1)), eSlotAlignment::BoundToSlot);
			}

			{
				ImageInfo = cbImageInfo::Create(U" ", U" ", 0);
				ImageInfo->SetPadding(cbMargin(0, 20, 0, 0));
				ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				ImageInfo->SetFocusable(false);
				ImageInfo->Select();
				ImageInfo->SetButtonVertexStyle(cbColor::Transparent());
				ImageInfo->HideButtonGeometry(true);
				ImageInfo->HideBackground(true);
				ImageInfo->EmojiImage->SetDimension(cbDimension(64, 64));
				ImageInfo->EmojiText->SetFontSize(36);
				ImageInfo->Text1->SetFontSize(16);
				ImageInfo->Text2->SetFontSize(24);
				Insert(ImageInfo, eSlotAlignment::BoundToContent);
			}

			Select(0);
		}

		virtual ~cbCareerMenu()
		{
			ImageInfo = nullptr;
			for (auto& ScaleBox : ScaleBoxes)
				ScaleBox = nullptr;
			ScaleBoxes.clear();

			WarningPopupMenu = nullptr;
		}

		void ShowPopup(cbDemoButtonBase* BTN)
		{
			WarningPopupMenu->AddToCanvas(GetCanvas());
		}

		void Option_Selected()
		{

		}

		void ScaleBox_Hovered(cbScaleBox* ScaleBox)
		{
			if (!ScaleBox)
				return;

			for (const auto& pScaleBox : ScaleBoxes)
			{
				if (pScaleBox != ScaleBox)
				{
					pScaleBox->Scale(false);
					cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(pScaleBox->GetContent())->GetSlot(1)->GetContent());
					pImageInfo->Deselect();
				}
			}

			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(ScaleBox->GetContent())->GetSlot(1)->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}

		void Selected(cbScaleBox* ScaleBox)
		{
			//GetRootOwner<cbMainMenu>(1);
		}

		void Select(std::size_t Index)
		{
			ScaleBoxes[Index]->Scale();
			cbImageInfo* pImageInfo = static_cast<cbImageInfo*>(static_cast<cbOverlay*>(ScaleBoxes[Index]->GetContent())->GetSlot(1)->GetContent());
			if (pImageInfo)
			{
				pImageInfo->Select();
				ImageInfo->EmojiText->SetText(pImageInfo->EmojiText->GetText());
				ImageInfo->Text1->SetText(pImageInfo->Text1->GetText());
				ImageInfo->Text2->SetText(pImageInfo->Text2->GetText());
			}
		}
	};

private:
	cbOverlay::SharedPtr MenuContextOverlay;
	cbAutoScroller::SharedPtr MenuAutoScroller;

	FeaturedMenuList::SharedPtr pFeaturedMenuList;
	cbDestinationsMenu::SharedPtr DestinationsMenu;
	cbCampaigns::SharedPtr pCampaignsMenu;
	cbCareerMenu::SharedPtr pCareerMenu;
	cbStoryMissionListMenu::SharedPtr pStoryMissionListMenu;
	cbStoryMissionMenu::SharedPtr pStoryMissionMenu;

	cbAutoScroller::SharedPtr pStoreScrollBox;

	cbOptions::SharedPtr Options;
	cbBorder::SharedPtr BackButton;

	cbSelectItemMenu::SharedPtr SelectItemMenu;

protected:
	cbWidget* ActiveMenu;

public:
	cbMainMenu(cbDimension Dimension)
		: cbVerticalBox()
		, ActiveMenu(nullptr)
	{
		{
			MenuAutoScroller = cbAutoScroller::Create(eOrientation::Horizontal);
			MenuAutoScroller->SetVerticalAlignment(eVerticalAlignment::Align_Top);
			MenuAutoScroller->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			MenuAutoScroller->Scrollbox->HideScrollBar(true);
			MenuAutoScroller->SetHeight(64);
			MenuAutoScroller->SetPadding(cbMargin(0, 76, 0, 64));
			Insert(MenuAutoScroller, eSlotAlignment::BoundToContent);
			{
				cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
				Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				Empty->SetWidth(50);
				MenuAutoScroller->Insert(Empty);
			}
			{
				auto GetOption = [&](std::u32string STR, float width) -> cbMenuButton::SharedPtr
				{
					auto BTN = cbMenuButton::Create(STR, std::bind(&cbMainMenu::MenuBarBTN_Clicked, this, std::placeholders::_1));
					BTN->SetButtonWidth(width);
					BTN->SetButtonHorizontalAlignment(eHorizontalAlignment::Align_Left);
					BTN->SetTextHorizontalAlignment(eHorizontalAlignment::Align_Left);
					BTN->SetTextPadding(cbMargin(15, 0, 0, 0));
					return BTN;
				};

				{
					auto Option = GetOption(U"\x272F FEATURED", Dimension.GetWidth() / 5.0f);
					Option->Select();
					MenuAutoScroller->Insert(Option);
				}
				MenuAutoScroller->Insert(GetOption(U"\x2708 DESTINATIONS", Dimension.GetWidth() / 5.0f));
				MenuAutoScroller->Insert(GetOption(U"\x265C CAMPAIGNS", Dimension.GetWidth() / 5.0f));
				{
					auto Option = GetOption(U"\x273E GAME MODES", Dimension.GetWidth() / 5.0f);
					Option->Disable();
					MenuAutoScroller->Insert(Option);
				}
				MenuAutoScroller->Insert(GetOption(U"\x2618 CAREER", Dimension.GetWidth() / 5.0f));
				MenuAutoScroller->Insert(GetOption(U"\x26C1 STORE", Dimension.GetWidth() / 5.0f));
				MenuAutoScroller->Insert(GetOption(U"\x2707 OPTIONS", Dimension.GetWidth() / 5.0f));
			}
			{
				cbgui::cbSizeBox::SharedPtr Empty = cbSizeBox::Create();
				Empty->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Empty->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				Empty->SetWidth(50);
				MenuAutoScroller->Insert(Empty);
			}
		}
		{
			MenuContextOverlay = cbOverlay::Create();
			MenuContextOverlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			MenuContextOverlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(MenuContextOverlay, eSlotAlignment::BoundToSlot);
		}

		{
			pFeaturedMenuList = FeaturedMenuList::Create(Dimension);
			pFeaturedMenuList->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			pFeaturedMenuList->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			pFeaturedMenuList->Select(0);
			ActiveMenu = pFeaturedMenuList.get();
			MenuContextOverlay->Insert(pFeaturedMenuList);
		}
		{
			DestinationsMenu = cbDestinationsMenu::Create();
			DestinationsMenu->Disable();
			DestinationsMenu->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(DestinationsMenu);
		}
		{
			pCampaignsMenu = cbCampaigns::Create();
			pCampaignsMenu->Disable();
			pCampaignsMenu->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(pCampaignsMenu);
		}

		{
			pStoreScrollBox = cbAutoScroller::Create(eOrientation::Horizontal);
			pStoreScrollBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			pStoreScrollBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			pStoreScrollBox->SetPadding(cbMargin(0, 0, 0, 32));

			pStoreScrollBox->Scrollbox->SetScrollBarPadding(cbMargin(100, 10, 100, 5));
			pStoreScrollBox->Scrollbox->SetScrollBarBackgroundVertexColorStyle(cbColor(0.0097f, 0.0116f, 0.01685f));

			cbButtonVertexColorStyle ColorStyle;
			ColorStyle.SetDefaultColor(cbColor::Red());
			ColorStyle.SetHoveredColor(ColorStyle.GetColor(eButtonState::Default));
			ColorStyle.SetPressedColor(ColorStyle.GetColor(eButtonState::Default));
			pStoreScrollBox->Scrollbox->SetScrollBarHandleVertexColorStyle(ColorStyle);

			{
				auto GetStoreOption = [](std::u32string STR, std::u32string STR2, char32_t Emoji) -> cbScaleBox::SharedPtr
				{
					cbScaleBox::SharedPtr ScaleBox = cbScaleBox::Create();
					ScaleBox->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
					ScaleBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					{
						cbOverlay::SharedPtr Overlay = cbOverlay::Create();
						Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						{
							cbgui::cbImage::SharedPtr Image = cbImage::Create();
							Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							Image->SetVertexColorStyle(cbColor::Random());
							Image->SetWidth(512);
							Image->SetHeight(372);
							Overlay->Insert(Image);
						}
						{
							cbImageInfo::SharedPtr ImageInfo = cbImageInfo::Create(STR, STR2, Emoji);
							ImageInfo->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
							ImageInfo->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
							Overlay->Insert(ImageInfo);
						}
						ScaleBox->SetContent(Overlay);
						ScaleBox->Disable();
					}
					return ScaleBox;
				};

				{
					const auto StoreOption = GetStoreOption(U"UNAVAILABLE", U"STORE ITEM 1", char32_t(0x26C3));
					StoreOption->SetPadding(cbMargin(50, 0, 0, 0));
					pStoreScrollBox->Insert(StoreOption);
				}
				pStoreScrollBox->Insert(GetStoreOption(U"UNAVAILABLE", U"STORE ITEM 2", char32_t(0x26C3)));
				pStoreScrollBox->Insert(GetStoreOption(U"UNAVAILABLE", U"STORE ITEM 3", char32_t(0x26C3)));
				pStoreScrollBox->Insert(GetStoreOption(U"UNAVAILABLE", U"STORE ITEM 4", char32_t(0x26C3)));
				pStoreScrollBox->Insert(GetStoreOption(U"UNAVAILABLE", U"STORE ITEM 5", char32_t(0x26C3)));
				{
					const auto StoreOption = GetStoreOption(U"UNAVAILABLE", U"STORE ITEM 6", char32_t(0x26C3));
					StoreOption->SetPadding(cbMargin(0, 0, 50, 0));
					pStoreScrollBox->Insert(StoreOption);
				}
			}

			pStoreScrollBox->Disable();
			pStoreScrollBox->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(pStoreScrollBox);
		}
		{
			Options = cbOptions::Create();
			Options->Disable();
			Options->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(Options);
		}
		{
			pCareerMenu = cbCareerMenu::Create();
			pCareerMenu->Disable();
			pCareerMenu->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(pCareerMenu);
		}
		{
			cbOverlay::SharedPtr MainOverlay = cbOverlay::Create();
			MainOverlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			MainOverlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Insert(MainOverlay);

			{
				BackButton = cbBorder::Create();
				BackButton->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				BackButton->SetHorizontalAlignment(eHorizontalAlignment::Align_Left);
				BackButton->SetPadding(cbMargin(175, 10, 0, 10));
				BackButton->SetThickness(cbMargin(1));
				MainOverlay->Insert(BackButton);

				BackButton->Disable();
				BackButton->SetVisibilityState(eVisibility::Hidden);

				auto BackButton_Overlay = cbOverlay::Create();
				BackButton_Overlay->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				BackButton_Overlay->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				BackButton_Overlay->SetPadding(cbMargin(4, 2, 4, 2));
				BackButton->SetContent(BackButton_Overlay);

				cbButton::SharedPtr Button = cbButton::Create();
				Button->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
				Button->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
				Button->SetWidth(1.0f);
				Button->SetHeight(1.0f);
				Button->SetVisibilityState(eVisibility::Invisible);
				Button->BindFunctionTo_OnClicked(std::bind(&cbMainMenu::Back, this));
				BackButton_Overlay->Insert(Button);

				cbString::SharedPtr Back = cbString::Create(U"\x2190 Back", cbTextDesc(10));
				Back->SetDefaultSpaceWidth(3);
				Back->SetVerticalAlignment(eVerticalAlignment::Align_Center);
				Back->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
				Back->SetVertexColorStyle(cbColor::White());
				Back->SetTextJustify(eTextJustify::Left);
				Back->SetFocusable(false);
				BackButton_Overlay->Insert(Back);
			}
			
			{
				cbHorizontalBox::SharedPtr HB = cbHorizontalBox::Create();
				HB->SetVerticalAlignment(eVerticalAlignment::Align_Bottom);
				HB->SetHorizontalAlignment(eHorizontalAlignment::Align_Right);
				MainOverlay->Insert(HB);

				HB->SetPadding(cbMargin(0, 10, 175, 10));

				{
					cbString::SharedPtr String = cbString::Create("", cbTextDesc(24));
					String->SetDefaultSpaceWidth(3);
					String->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					String->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					String->SetVertexColorStyle(cbColor::White());
					String->SetTextJustify(eTextJustify::Left);
					String->AddChar(char32_t(9876));
					HB->Insert(String);
				}
				{
					cbgui::cbImage::SharedPtr Image = cbImage::Create();
					Image->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Center);
					Image->SetVertexColorStyle(cbColor::White());
					Image->SetPadding(cbMargin(5, 0, 5, 0));
					Image->SetWidth(1);
					Image->SetHeight(32);
					HB->Insert(Image);
				}
				{
					cbVerticalBox::SharedPtr VerticalBox = cbVerticalBox::Create();
					VerticalBox->SetVerticalAlignment(eVerticalAlignment::Align_Center);
					VerticalBox->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
					HB->Insert(VerticalBox);
					{
						cbString::SharedPtr String = cbString::Create("Player", cbTextDesc(12));
						String->SetDefaultSpaceWidth(3);
						String->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						String->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						String->SetVertexColorStyle(cbColor::White());
						String->SetTextJustify(eTextJustify::Left);
						VerticalBox->Insert(String);
					}
					{
						cbString::SharedPtr String = cbString::Create("Offline", cbTextDesc(8));
						String->SetDefaultSpaceWidth(3);
						String->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
						String->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
						String->SetVertexColorStyle(cbColor::Red());
						String->SetTextJustify(eTextJustify::Left);
						VerticalBox->Insert(String);
					}
				}
			}
		}
		{
			pStoryMissionListMenu = cbStoryMissionListMenu::Create();
			pStoryMissionListMenu->Disable();
			pStoryMissionListMenu->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(pStoryMissionListMenu);
		}
		{
			pStoryMissionMenu = cbStoryMissionMenu::Create();
			pStoryMissionMenu->Disable();
			pStoryMissionMenu->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(pStoryMissionMenu);
		}
		{
			SelectItemMenu = cbSelectItemMenu::Create();
			SelectItemMenu->Disable();
			SelectItemMenu->SetVisibilityState(eVisibility::Hidden);
			MenuContextOverlay->Insert(SelectItemMenu);
		}
	}

	virtual ~cbMainMenu()
	{
		BackButton = nullptr;
		ActiveMenu = nullptr;

		MenuAutoScroller = nullptr;
		DestinationsMenu = nullptr;
		pCampaignsMenu = nullptr;

		pFeaturedMenuList = nullptr;
		pStoreScrollBox = nullptr;

		Options = nullptr;

		pStoryMissionListMenu = nullptr;
		pStoryMissionMenu = nullptr;
	}

	virtual void OnTick(float DeltaTime) override
	{
	}

	void HideMenu()
	{
		MenuAutoScroller->SetVisibilityState(eVisibility::Hidden);
		MenuAutoScroller->Disable();
	}

	void ShowMenu()
	{
		MenuAutoScroller->SetVisibilityState(eVisibility::Visible);
		MenuAutoScroller->Enable();
	}

	void ShowStoryMissionListMenu()
	{
		if (pFeaturedMenuList)
		{
			if (pFeaturedMenuList->IsEnabled())
			{
				pFeaturedMenuList->Disable();
				pFeaturedMenuList->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (DestinationsMenu)
		{
			if (DestinationsMenu->IsEnabled())
			{
				DestinationsMenu->Disable();
				DestinationsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pCampaignsMenu)
		{
			if (pCampaignsMenu->IsEnabled())
			{
				pCampaignsMenu->Disable();
				pCampaignsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pCareerMenu)
		{
			if (pCareerMenu->IsEnabled())
			{
				pCareerMenu->Disable();
				pCareerMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoreScrollBox)
		{
			if (pStoreScrollBox->IsEnabled())
			{
				pStoreScrollBox->Disable();
				pStoreScrollBox->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (Options)
		{
			if (Options->IsEnabled())
			{
				Options->Disable();
				Options->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoryMissionMenu)
		{
			if (pStoryMissionMenu->IsEnabled())
			{
				pStoryMissionMenu->Disable();
				pStoryMissionMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (SelectItemMenu)
		{
			if (SelectItemMenu->IsEnabled())
			{
				SelectItemMenu->Disable();
				SelectItemMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pStoryMissionListMenu)
		{
			{
				pStoryMissionListMenu->Enable();
				pStoryMissionListMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pStoryMissionListMenu.get();
			}
		}
	}

	void ShowStoryMissionMenu(cbWidget* Parent)
	{
		if (pFeaturedMenuList)
		{
			if (pFeaturedMenuList->IsEnabled())
			{
				pFeaturedMenuList->Disable();
				pFeaturedMenuList->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (DestinationsMenu)
		{
			if (DestinationsMenu->IsEnabled())
			{
				DestinationsMenu->Disable();
				DestinationsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pCampaignsMenu)
		{
			if (pCampaignsMenu->IsEnabled())
			{
				pCampaignsMenu->Disable();
				pCampaignsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pCareerMenu)
		{
			if (pCareerMenu->IsEnabled())
			{
				pCareerMenu->Disable();
				pCareerMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoreScrollBox)
		{
			if (pStoreScrollBox->IsEnabled())
			{
				pStoreScrollBox->Disable();
				pStoreScrollBox->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (Options)
		{
			if (Options->IsEnabled())
			{
				Options->Disable();
				Options->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoryMissionListMenu)
		{
			if (pStoryMissionListMenu->IsEnabled())
			{
				pStoryMissionListMenu->Disable();
				pStoryMissionListMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (SelectItemMenu)
		{
			if (SelectItemMenu->IsEnabled())
			{
				SelectItemMenu->Disable();
				SelectItemMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pStoryMissionMenu)
		{
			{
				HideMenu();
				pStoryMissionMenu->Enable();
				pStoryMissionMenu->SetVisibilityState(eVisibility::Visible);
				pStoryMissionMenu->SetParent(Parent);
				ActiveMenu = pStoryMissionMenu.get();

				BackButton->Enable();
				BackButton->SetVisibilityState(eVisibility::Visible);
			}
		}
	}

	void ShowStoryMissionSelectItemMenu(cbImageInfo* pImageInfo)
	{
		if (pFeaturedMenuList)
		{
			if (pFeaturedMenuList->IsEnabled())
			{
				pFeaturedMenuList->Disable();
				pFeaturedMenuList->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (DestinationsMenu)
		{
			if (DestinationsMenu->IsEnabled())
			{
				DestinationsMenu->Disable();
				DestinationsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pCampaignsMenu)
		{
			if (pCampaignsMenu->IsEnabled())
			{
				pCampaignsMenu->Disable();
				pCampaignsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pCareerMenu)
		{
			if (pCareerMenu->IsEnabled())
			{
				pCareerMenu->Disable();
				pCareerMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoreScrollBox)
		{
			if (pStoreScrollBox->IsEnabled())
			{
				pStoreScrollBox->Disable();
				pStoreScrollBox->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (Options)
		{
			if (Options->IsEnabled())
			{
				Options->Disable();
				Options->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoryMissionListMenu)
		{
			if (pStoryMissionListMenu->IsEnabled())
			{
				pStoryMissionListMenu->Disable();
				pStoryMissionListMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pStoryMissionMenu)
		{
			if (pStoryMissionMenu->IsEnabled())
			{
				pStoryMissionMenu->Disable();
				pStoryMissionMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (SelectItemMenu)
		{
			{
				SelectItemMenu->Enable();
				SelectItemMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = SelectItemMenu.get();

				SelectItemMenu->SetParent(pStoryMissionMenu.get());
				SelectItemMenu->PossessOption(pImageInfo);

				BackButton->Enable();
				BackButton->SetVisibilityState(eVisibility::Visible);
			}
		}
	}

	void CloseStoryMissionSelectItemMenu()
	{
		if (pFeaturedMenuList)
		{
			if (pFeaturedMenuList->IsEnabled())
			{
				pFeaturedMenuList->Disable();
				pFeaturedMenuList->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (DestinationsMenu)
		{
			if (DestinationsMenu->IsEnabled())
			{
				DestinationsMenu->Disable();
				DestinationsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pCampaignsMenu)
		{
			if (pCampaignsMenu->IsEnabled())
			{
				pCampaignsMenu->Disable();
				pCampaignsMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pCareerMenu)
		{
			if (pCareerMenu->IsEnabled())
			{
				pCareerMenu->Disable();
				pCareerMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoreScrollBox)
		{
			if (pStoreScrollBox->IsEnabled())
			{
				pStoreScrollBox->Disable();
				pStoreScrollBox->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (Options)
		{
			if (Options->IsEnabled())
			{
				Options->Disable();
				Options->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (pStoryMissionListMenu)
		{
			if (pStoryMissionListMenu->IsEnabled())
			{
				pStoryMissionListMenu->Disable();
				pStoryMissionListMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}
		if (SelectItemMenu)
		{
			if (SelectItemMenu->IsEnabled())
			{
				SelectItemMenu->Disable();
				SelectItemMenu->SetVisibilityState(eVisibility::Hidden);
			}
		}

		if (pStoryMissionMenu)
		{
			if (!pStoryMissionMenu->IsEnabled())
			{
				pStoryMissionMenu->Enable();
				pStoryMissionMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pStoryMissionMenu.get();
			}
		}
	}

	void Back()
	{
		if (ActiveMenu == pStoryMissionMenu.get())
		{
			pStoryMissionMenu->Disable();
			pStoryMissionMenu->SetVisibilityState(eVisibility::Hidden);

			ShowMenu();

			auto Parent = pStoryMissionMenu->GetParent();
			if (Parent == pStoryMissionListMenu.get())
			{
				pStoryMissionListMenu->Enable();
				pStoryMissionListMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pStoryMissionListMenu.get();

				BackButton->Disable();
				BackButton->SetVisibilityState(eVisibility::Hidden);
			}
			else if (Parent == DestinationsMenu.get())
			{
				DestinationsMenu->Enable();
				DestinationsMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = DestinationsMenu.get();

				BackButton->Disable();
				BackButton->SetVisibilityState(eVisibility::Hidden);
			}
			else if (Parent == pFeaturedMenuList.get())
			{
				pFeaturedMenuList->Enable();
				pFeaturedMenuList->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pFeaturedMenuList.get();

				BackButton->Disable();
				BackButton->SetVisibilityState(eVisibility::Hidden);
			}
		}
		else if (ActiveMenu == SelectItemMenu.get())
		{
			SelectItemMenu->Disable();
			SelectItemMenu->SetVisibilityState(eVisibility::Hidden);

			pStoryMissionMenu->Enable();
			pStoryMissionMenu->SetVisibilityState(eVisibility::Visible);

			ActiveMenu = pStoryMissionMenu.get();
		}
	}

	void MenuBarBTN_Clicked(cbMenuButton* btn)
	{
		if (!btn)
			return;

		const std::size_t Index = btn->GetIndex();

		for (std::size_t i = 0; i < MenuAutoScroller->Scrollbox->GetSlotSize(); i++)
		{
			const auto MenuButton = MenuAutoScroller->Scrollbox->GetSlot(i)->GetContent<cbMenuButton>();

			if (MenuButton)
			{
				if (MenuButton->IsSelected() && MenuButton->GetIndex() != Index)
				{
					MenuButton->Deselect();
				}
			}
		}
		SelectMenu(Index);
	}

	void SelectMenu(std::size_t Index)
	{
		if (Index == 1)
		{
			if (pFeaturedMenuList)
			{
				pFeaturedMenuList->Enable();
				pFeaturedMenuList->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pFeaturedMenuList.get();
			}
		}
		else
		{
			if (pFeaturedMenuList)
			{
				if (pFeaturedMenuList->IsEnabled())
				{
					pFeaturedMenuList->Disable();
					pFeaturedMenuList->SetVisibilityState(eVisibility::Hidden);
				}
			}
		}

		if (Index == 2)
		{
			if (DestinationsMenu)
			{
				DestinationsMenu->Enable();
				DestinationsMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = DestinationsMenu.get();
			}
		}
		else
		{
			if (DestinationsMenu)
			{
				if (DestinationsMenu->IsEnabled())
				{
					DestinationsMenu->Disable();
					DestinationsMenu->SetVisibilityState(eVisibility::Hidden);
				}
			}
		}

		if (Index == 3)
		{
			if (pCampaignsMenu)
			{
				pCampaignsMenu->Enable();
				pCampaignsMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pCampaignsMenu.get();
			}
		}
		else
		{
			if (pCampaignsMenu)
			{
				if (pCampaignsMenu->IsEnabled())
				{
					pCampaignsMenu->Disable();
					pCampaignsMenu->SetVisibilityState(eVisibility::Hidden);
				}
			}
		}

		if (Index == 5)
		{
			if (pCareerMenu)
			{
				pCareerMenu->Enable();
				pCareerMenu->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pCareerMenu.get();
			}
		}
		else
		{
			if (pCareerMenu)
			{
				if (pCareerMenu->IsEnabled())
				{
					pCareerMenu->Disable();
					pCareerMenu->SetVisibilityState(eVisibility::Hidden);
				}
			}
		}

		if (Index == 6)
		{
			if (pStoreScrollBox)
			{
				pStoreScrollBox->Enable();
				pStoreScrollBox->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = pStoreScrollBox.get();
			}
		}
		else
		{
			if (pStoreScrollBox)
			{
				if (pStoreScrollBox->IsEnabled())
				{
					pStoreScrollBox->Disable();
					pStoreScrollBox->SetVisibilityState(eVisibility::Hidden);
				}
			}
		}

		if (Index == 7)
		{
			if (Options)
			{
				Options->Enable();
				Options->SetVisibilityState(eVisibility::Visible);
				ActiveMenu = Options.get();
			}
		}
		else
		{
			if (Options)
			{
				if (Options->IsEnabled())
				{
					Options->Disable();
					Options->SetVisibilityState(eVisibility::Hidden);
				}
			}
		}
		if (pStoryMissionListMenu)
		{
			if (pStoryMissionListMenu->IsEnabled())
			{
				pStoryMissionListMenu->Disable();
				pStoryMissionListMenu->SetVisibilityState(eVisibility::Hidden);
				ActiveMenu = pStoryMissionListMenu.get();
			}
		}

		if (pStoryMissionMenu)
		{
			if (pStoryMissionMenu->IsEnabled())
			{
				pStoryMissionMenu->Disable();
				pStoryMissionMenu->SetVisibilityState(eVisibility::Hidden);
				ActiveMenu = pStoryMissionMenu.get();
			}
		}
	}
};

DemoCanvas::DemoCanvas(WindowsPlatform* WPlatformOwner)
	: DemoCanvasBase(WPlatformOwner, cbgui::cbDimension((float)WPlatformOwner->GetWindowWidth(), (float)WPlatformOwner->GetWindowHeight()))
	, PopupMenu(nullptr)
{
	{
		cbgui::cbImage::SharedPtr Image = std::make_shared<cbgui::cbImage>();
		Image->SetName("Start Screen Image");
		Image->AddToCanvas(this);

		Image->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		Image->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		Image->SetAlignToCanvas(true);
	}

	{
		StartScreen = std::make_shared<cbStartScreen>(cbDimension((float)WPlatformOwner->GetWindowWidth(), (float)WPlatformOwner->GetWindowHeight()));
		StartScreen->AddToCanvas(this);
		StartScreen->Wrap();

		StartScreen->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		StartScreen->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		StartScreen->SetAlignToCanvas(true);
	}

	{
		MainMenu = std::make_shared<cbMainMenu>(cbDimension((float)WPlatformOwner->GetWindowWidth(), (float)WPlatformOwner->GetWindowHeight()));
		MainMenu->AddToCanvas(this);
		MainMenu->Wrap();

		MainMenu->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
		MainMenu->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
		MainMenu->SetAlignToCanvas(true);
		MainMenu->SetVisibilityState(eVisibility::Hidden);
		MainMenu->Disable();

		Focus = MainMenu.get();
	}
}

DemoCanvas::~DemoCanvas()
{
	StartScreen = nullptr;
	PopupMenu = nullptr;
	MainMenu = nullptr;
	Focus = nullptr;
}

void DemoCanvas::ResizeWindow(std::size_t Width, std::size_t Height)
{
	Super::ResizeWindow(Width, Height);
	if (StartScreen->IsEnabled())
		StartScreen->UpdateAlignments(true);
	if (MainMenu->IsEnabled())
		MainMenu->UpdateAlignments(true);
}

void DemoCanvas::Add(const std::shared_ptr<cbWidget>& Object)
{
	Super::Add(Object);

	if (auto Popup = cbgui::cbCast<cbPopupMenuBase>(Object.get()))
	{
		PopupMenu = Popup;
		Focus = PopupMenu;
	}
}

void DemoCanvas::RemoveFromCanvas(cbWidget* Object)
{
	if (PopupMenu)
	{
		if (auto Popup = cbgui::cbCast<cbPopupMenuBase>(Object))
		{
			PopupMenu = nullptr;
			Focus = MainMenu.get();
		}
	}

	Super::RemoveFromCanvas(Object);
}

void DemoCanvas::SetMaterial(WidgetHierarchy* pWP)
{
	DemoCanvasBase::SetMaterial(pWP);

	if (pWP->Widget->GetName() == "Start Screen Image")
	{
		pWP->MaterialName = "StartScreen";
	}
}

void DemoCanvas::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void DemoCanvas::InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	auto GetMouseLocation = [&]() -> cbVector
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);

		return cbVector(static_cast<float>(point.x), static_cast<float>(point.y));
	};

	if (uMsg)
	{
		switch (uMsg)
		{
		case WM_SYSKEYUP:
			break;
		case WM_SYSKEYDOWN:
			break;
		case WM_KEYDOWN:
		{
			int iKeyPressed = static_cast<int>(wParam);

			if (StartScreen)
			{
				if (!StartScreen->IsStartScreenFadeOut())
					StartScreen->FadeOut();
			}

			break;
		}
		case WM_MOUSEWHEEL:
		{
			if (Focus)
			{
				if (Focus->IsEnabled())
				{
					cbMouseInput MouseInput;
					const float SpinFactor = 1.0f / 120.0f;
					const SHORT WheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

					MouseInput.MouseLocation = GetMouseLocation();

					if (Focus->IsInside(MouseInput.MouseLocation))
					{
						Focus->OnMouseWheel(static_cast<float>(WheelDelta) * SpinFactor, MouseInput);
					}
				}

				return;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			cbMouseInput MouseInput;
			MouseInput.State = cbMouseState::Moving;
			MouseInput.MouseLocation = GetMouseLocation();

			if (Focus)
			{
				if (!Focus->IsFocused())
					Focus->OnMouseEnter(MouseInput);
				else
					Focus->OnMouseMove(MouseInput);
			}
		}
		break;

		case WM_LBUTTONDOWN:
		{
			cbMouseInput MouseInput;
			MouseInput.Buttons["Left"] = cbMouseButtonState::Pressed;
			MouseInput.MouseLocation = GetMouseLocation();
			if (Focus->IsFocused())
			{
				Focus->OnMouseButtonDown(MouseInput);
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			cbMouseInput MouseInput;
			MouseInput.Buttons["Left"] = cbMouseButtonState::Released;
			MouseInput.MouseLocation = GetMouseLocation();

			if (Focus->IsFocused())
			{
				Focus->OnMouseButtonUp(MouseInput);
			}
			break;
		}
		}
	}
}

void DemoCanvas::StartScreenFadeOut()
{
	std::cout << "StartScreenFadeOut" << std::endl;

	MainMenu->SetVisibilityState(eVisibility::Visible);
	MainMenu->Enable();
}
