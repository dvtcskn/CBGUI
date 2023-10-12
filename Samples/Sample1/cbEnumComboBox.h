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
#include "cbComboBox.h"

class cbCanvasAnchorComboBox : public cbComboBox
{
	cbClassBody(cbClassConstructor, cbCanvasAnchorComboBox, cbComboBox)
private:
	std::map<eCanvasAnchor, std::u32string> AnchorStrings;

public:
	cbCanvasAnchorComboBox()
		: Super()
	{
		AnchorStrings.insert({ eCanvasAnchor::NONE, (U"NONE") });
		AnchorStrings.insert({ eCanvasAnchor::Center,(U"Center") });
		AnchorStrings.insert({ eCanvasAnchor::Top,(U"Top") });
		AnchorStrings.insert({ eCanvasAnchor::Bottom,(U"Bottom") });
		AnchorStrings.insert({ eCanvasAnchor::Left,(U"Left") });
		AnchorStrings.insert({ eCanvasAnchor::Right,(U"Right") });
		AnchorStrings.insert({ eCanvasAnchor::LeftTop,(U"LeftTop") });
		AnchorStrings.insert({ eCanvasAnchor::LeftBottom,(U"LeftBottom") });
		AnchorStrings.insert({ eCanvasAnchor::RightTop,(U"RightTop") });
		AnchorStrings.insert({ eCanvasAnchor::RightBottom,(U"RightBottom") });

		for (const auto& AnchorString : AnchorStrings)
			Insert(cbString::Create(AnchorString.second, cbTextDesc(8)));

		SelectItem(0);
	}

	cbCanvasAnchorComboBox(const cbCanvasAnchorComboBox& Widget, cbSlot* NewOwner)
		: Super(Widget, NewOwner)
		, AnchorStrings(Widget.AnchorStrings)
		, fCallBack(Widget.fCallBack)
	{}

	std::function<void(eCanvasAnchor)> fCallBack;

	virtual ~cbCanvasAnchorComboBox()
	{
		fCallBack = nullptr;
		AnchorStrings.clear();
	}

	virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override
	{
		return cbCanvasAnchorComboBox::Create(*this, NewOwner);
	}

	virtual void OnSelectionChanged(cbSlot* SelectedOption) 
	{
		if (fCallBack)
			fCallBack(static_cast<eCanvasAnchor>(SelectedOption->GetIndex()));
	}

	void SelectOption(eCanvasAnchor Anchor)
	{
		SelectItem(static_cast<std::size_t>(Anchor));
	}
};

class cbVerticalAlignmentComboBox : public cbComboBox
{
	cbClassBody(cbClassConstructor, cbVerticalAlignmentComboBox, cbComboBox)
private:
	std::map<eVerticalAlignment, std::u32string> AnchorStrings;

public:
	cbVerticalAlignmentComboBox()
		: Super()
	{
		AnchorStrings.insert({ eVerticalAlignment::Align_NONE, (U"NONE") });
		AnchorStrings.insert({ eVerticalAlignment::Align_Top, (U"Align_Top") });
		AnchorStrings.insert({ eVerticalAlignment::Align_Center, (U"Align_Center") });
		AnchorStrings.insert({ eVerticalAlignment::Align_Bottom, (U"Align_Bottom") });
		AnchorStrings.insert({ eVerticalAlignment::Align_Fill, (U"Align_Fill") });

		for (const auto& AnchorString : AnchorStrings)
			Insert(cbString::Create(AnchorString.second, cbTextDesc(8)));

		SelectItem(0);
	}

	cbVerticalAlignmentComboBox(const cbVerticalAlignmentComboBox& Widget, cbSlot* NewOwner)
		: Super(Widget, NewOwner)
		, AnchorStrings(Widget.AnchorStrings)
		, fCallBack(Widget.fCallBack)
	{}

	std::function<void(eVerticalAlignment)> fCallBack;

	virtual ~cbVerticalAlignmentComboBox()
	{
		fCallBack = nullptr;
		AnchorStrings.clear();
	}

	virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override
	{
		return cbVerticalAlignmentComboBox::Create(*this, NewOwner);
	}

	virtual void OnSelectionChanged(cbSlot* SelectedOption)
	{
		if (fCallBack)
			fCallBack(static_cast<eVerticalAlignment>(SelectedOption->GetIndex()));
	}

	void SelectOption(eVerticalAlignment Alignment)
	{
		SelectItem(static_cast<std::size_t>(Alignment));
	}
};

class cbHorizontalAlignmentComboBox : public cbComboBox
{
	cbClassBody(cbClassConstructor, cbHorizontalAlignmentComboBox, cbComboBox)
private:
	std::map<eHorizontalAlignment, std::u32string> AnchorStrings;

public:
	cbHorizontalAlignmentComboBox()
		: Super()
	{
		AnchorStrings.insert({ eHorizontalAlignment::Align_NONE, (U"NONE") });
		AnchorStrings.insert({ eHorizontalAlignment::Align_Left, (U"Align_Left") });
		AnchorStrings.insert({ eHorizontalAlignment::Align_Center, (U"Align_Center") });
		AnchorStrings.insert({ eHorizontalAlignment::Align_Right, (U"Align_Right") });
		AnchorStrings.insert({ eHorizontalAlignment::Align_Fill, (U"Align_Fill") });

		for (const auto& AnchorString : AnchorStrings)
			Insert(cbString::Create(AnchorString.second, cbTextDesc(8)));

		SelectItem(0);
	}

	cbHorizontalAlignmentComboBox(const cbHorizontalAlignmentComboBox& Widget, cbSlot* NewOwner)
		: Super(Widget, NewOwner)
		, AnchorStrings(Widget.AnchorStrings)
		, fCallBack(Widget.fCallBack)
	{}

	std::function<void(eHorizontalAlignment)> fCallBack;

	virtual ~cbHorizontalAlignmentComboBox()
	{
		fCallBack = nullptr;
		AnchorStrings.clear();
	}

	virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override
	{
		return cbHorizontalAlignmentComboBox::Create(*this, NewOwner);
	}

	virtual void OnSelectionChanged(cbSlot* SelectedOption)
	{
		if (fCallBack)
			fCallBack(static_cast<eHorizontalAlignment>(SelectedOption->GetIndex()));
	}

	void SelectOption(eHorizontalAlignment Alignment)
	{
		SelectItem(static_cast<std::size_t>(Alignment));
	}
};

class cbAnchorComboBox : public cbComboBox
{
	cbClassBody(cbClassConstructor, cbAnchorComboBox, cbComboBox)
private:
	std::map<eAnchors, std::u32string> AnchorStrings;

public:
	cbAnchorComboBox()
		: Super()
	{
		AnchorStrings.insert({ eAnchors::Inside, (U"Inside") });
		AnchorStrings.insert({ eAnchors::Mid, (U"Mid") });
		AnchorStrings.insert({ eAnchors::Outside, (U"Outside") });

		for (const auto& AnchorString : AnchorStrings)
			Insert(cbString::Create(AnchorString.second, cbTextDesc(8)));

		SelectItem(0);
	}

	cbAnchorComboBox(const cbAnchorComboBox& Widget, cbSlot* NewOwner)
		: Super(Widget, NewOwner)
		, AnchorStrings(Widget.AnchorStrings)
		, fCallBack(Widget.fCallBack)
	{}

	std::function<void(eAnchors)> fCallBack;

	virtual ~cbAnchorComboBox()
	{
		fCallBack = nullptr;
		AnchorStrings.clear();
	}

	virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override
	{
		return cbAnchorComboBox::Create(*this, NewOwner);
	}

	virtual void OnSelectionChanged(cbSlot* SelectedOption)
	{
		if (fCallBack)
			fCallBack(static_cast<eAnchors>(SelectedOption->GetIndex()));
	}

	void SelectOption(eAnchors Anchor)
	{
		SelectItem(static_cast<std::size_t>(Anchor));
	}
};
