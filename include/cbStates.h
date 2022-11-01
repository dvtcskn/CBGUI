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

namespace cbgui
{
	enum class eZOrderMode
	{
		/* Draw in the regular order. */
		InOrder,
		/* Put it last in the hierarchy in order of drawing */
		LastInTheHierarchy,
		/* Put it at the end in the order of drawing. */
		Latest,
	};

	enum class eFocusMode
	{
		/* Focus based on ZOrder. */
		ZOrder,
		/* It is always focused. */
		Immediate,
	};

	enum class eVisibility
	{
		/* The Widget is Visible. */
		Visible,
		/* The Widget is Invisible. */
		Invisible,
		/* 
		* The Widget is Hidden/Collapsed. 
		* Hidden/Collapsed widgets cannot be focused.
		*/
		Collapsed,
		/*
		* Hidden = Collapsed
		* The Widget is Hidden/Collapsed.
		* Hidden/Collapsed widgets cannot be focused.
		*/
		Hidden = Collapsed,
	};

	enum class eFontType : unsigned short
	{
		Regular,
		Light,
		LightItalic,
		Italic,
		Bold,
		BoldItalic,
		ExtraType1,
		ExtraType2,
		ExtraType3,
		ExtraType4,
		ExtraType5,
		ExtraType6,
		ExtraType7,
		ExtraType8,
		ExtraType9,
	};

	enum class eAnchors : unsigned short
	{
		/* Inside the Widget. */
		Inside,
		/* On the edge of the canvas/owner according to the alignment. */
		Mid,
		/* Out of canvas/owner's area by alignment. */
		Outside,
	};

	enum class eVerticalAlignment : unsigned short
	{
		/* 
		* Not vertically aligned.
		* It only works with canvas. 
		* Allows vertical movement on the canvas relative to the canvas.
		*/
		Align_NONE,
		/* Aligns to the top of the owner/canvas. */
		Align_Top,
		/* Aligns to the Center of the owner/canvas. */
		Align_Center,
		/* Aligns to the Bottom of the owner/canvas. */
		Align_Bottom,
		/* Vertically fills the entire space of its owner/canvas. */
		Align_Fill,
	};

	enum class eHorizontalAlignment : unsigned short
	{
		/*
		* Not horizontally aligned.
		* It only works with canvas.
		* Allows horizontal movement on the canvas relative to the canvas.
		*/
		Align_NONE,
		/* Aligns to the Left of the owner/canvas. */
		Align_Left,
		/* Aligns to the Center of the owner/canvas. */
		Align_Center,
		/* Aligns to the Right of the owner/canvas. */
		Align_Right,
		/* Horizontally fills the entire space of its owner/canvas. */
		Align_Fill,
	};

	enum class eSlotAlignment : unsigned short
	{
		/* It evenly divides the dimension of the slottedbox. */
		BoundToSlot,
		/* Dimension is calculated based on content. */
		BoundToContent,
	};

	enum class eOrientation : unsigned short
	{
		Vertical,
		Horizontal,
	};

	/* Specifies the Canvas Origin position. */
	enum class eCanvasAnchor : unsigned short
	{
		NONE,
		Center,
		Top,
		Bottom,
		Left,
		Right,
		LeftTop,
		LeftBottom,
		RightTop,
		RightBottom,
	};

	enum class eTextJustify : unsigned short
	{
		Left,
		Center,
		Right,
	};

	enum class eButtonState : unsigned short
	{
		Default,
		Pressed,
		Hovered,
		Disabled,
	};

	enum class eCheckBoxState : unsigned short
	{
		Unchecked,
		UncheckedHovered,
		UncheckedPressed,

		Checked,
		CheckedHovered,
		CheckedPressed,

		Undetermined,
		UndeterminedHovered,
		UndeterminedPressed,

		CheckedDisabled,
		UncheckedDisabled,
		UndeterminedDisabled,
	};
}
