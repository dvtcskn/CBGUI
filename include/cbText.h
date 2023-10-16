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

#include "cbWidget.h"
#include "cbGeometry.h"
#include "cbFont.h"

namespace cbgui
{
	/* The text widget allows you to print text on the screen. */
	class cbText : public cbWidget
	{
		cbClassBody(cbClassConstructor, cbText, cbWidget);
	public:
		cbText(const std::u32string& Text, const cbTextDesc& TextDesc, cbIFontFamily* FontFamily);
		cbText(const cbText& Other, cbSlot* NewOwner = nullptr);
	public:
		virtual ~cbText();

		virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override;

	private:
		virtual void BeginPlay() override final;
		virtual void Tick(float DeltaTime) override final;

		virtual void OnBeginPlay() {}
		virtual void OnTick(float DeltaTime) {}

	public:
		virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
		virtual float GetRotation() const override final;
		virtual cbBounds GetBounds() const override final { return Transform.GetBounds(); }
		virtual cbMargin GetPadding() const override final { return Transform.GetPadding(); }
		virtual cbDimension GetDimension() const override final { return Transform.GetDimension(); }
		virtual float GetWidth() const override final { return Transform.GetWidth(); }
		virtual float GetHeight() const override final { return Transform.GetHeight(); }

		/* Return NonAligned/Wrapped width of the Widget */
		virtual float GetNonAlignedWidth() const override final { return Transform.GetNonAlignedWidth(); }
		/* Return NonAligned/Wrapped height of the Widget */
		virtual float GetNonAlignedHeight() const override final { return Transform.GetNonAlignedHeight(); }

		/* Set the Screen Position of the Widget.*/
		virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false) override final;
		/* Set NonAligned/NonWrapped Dimension of the Widget */
		virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height) override final;
		/* Padding is used to align with offset. The padding only works if attached into slot. */
		virtual void SetPadding(const cbMargin& Padding) override final;
		/* 2D Rotation. Requires Stencil Clipping during rendering, */
		virtual void SetRotation(const float Rotation) override final;

		virtual void UpdateVerticalAlignment(const bool ForceAlign = false) override final;
		virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) override final;
		virtual void UpdateRotation() override final;
		virtual void UpdateStatus() override final;

		virtual bool HasAnyChildren() const override final { return false; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>(); }

		virtual bool HasAnyComponents() const override final { return false; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>(); }

		/* Sets the font and font library. Text library is required for text data. */
		void SetFontFamily(cbIFontFamily* FontFamily);
		const cbIFontFamily* GetFontFamily() const;

		void SetFontType(const eFontType& FontType);
		void SetFontSize(const std::size_t& Size);
		/* Default Font Size */
		std::size_t GetCharSize() const;

		/* Makes the text dimension even. */
		void SetAscenderDescenderPowerOfTwo(const bool Value);

		/* Returns Text. If filtered is enabled, it ignores newline, space, and ignored character. */
		const std::u32string GetText(bool Filtered = false) const;
		/* Deletes the existing text, adds a new one. */
		void SetText(const std::u32string& Text, const std::optional<cbTextDesc> Desc = std::nullopt);
		/* Appends text to the end of existing text. */
		void AppendText(const std::u32string& Text);
		/* Creates Character. If filtered is true, the index location may change based on ignored characters. */
		void AddChar(const char32_t pChar, const std::optional<std::size_t> Index = std::nullopt, bool Filtered = true);
		/* Creates Character. If filtered is true, the index location may change based on ignored characters.*/
		void AddChar(const char32_t* pChar, const std::optional<std::size_t> Index = std::nullopt, bool Filtered = true);
		bool RemoveChar(std::size_t InIndex);
		bool RemoveChars(const std::vector<std::size_t>& Indices);
		bool RemoveLastChar();
		/* Removes all characters. */
		void Clear();

		/* Returns all characters in the line and the bounds of the line. if useTextWidth is true, The line length is equal to the total length of the text. */
		bool GetLineData(std::size_t Index, cbBounds& outLineBounds, std::vector<cbCharacterData>& outCharacterData, bool useTextWidth = true) const;
		/* Returns the bounds of the line. if useTextWidth is true, The line length is equal to the total length of the text. */
		std::optional<cbBounds> GetLineBounds(std::size_t Index, bool useTextWidth = true) const;
		/* Returns the character data at the given index. */
		std::optional<cbCharacterData> GetCharacterData(std::size_t Index) const;

		/* Sets where the text should justify */
		void SetTextJustify(eTextJustify Justify);
		eTextJustify GetTextJustify() const;
		
		/* Checks how many of the given character type are. */
		std::size_t GetCharacterCount(const char32_t ch) const;
		/* Returns text character size. If filtered is enabled, it ignores newline, space, and ignored character. */
		std::size_t GetTextSize(bool Filtered = false) const;
		std::size_t GetLineSize() const;

		/* If Text reaches this Size, the text buffer grows. */
		void SetAutoGrowVerticesSize(const bool value, const std::size_t Size = 24);

		/* A style used to format text. */
		std::vector<cbTextStyle> GetTextStyles() const;
		/* A style used to format text. */
		void AddTextStyle(const cbTextStyle& Style);
		void RemoveTextStyle(const std::size_t& Index);
		/* A style used to format text. */
		void SetTextStylingEnabled(const bool Value);

		/* Sets the length of the space(32) character. */
		void SetDefaultSpaceWidth(const std::optional<std::size_t> Width);
		/*
		* Manipulates Current Line Ascender and Previous Line Descender.
		* Default : 1.0f
		*/
		void SetLineHeightPercentage(const float Percentage);

		virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;
		/* Resets the focus and input state. */
		virtual void ResetInput() override final;

		/* When the line ends, it automatically switches to the next line. */
		void SetAutoWrap(const bool Value);
		bool IsAutoWrapEnabled() const;
		void SetCustomWrapWidth(const std::optional<float> Width);
		float GetAutoWrapWidth() const;
	protected:
		void AutoWrap();

	public:
		bool IsItWrapped() const;
		/* Compresses the Text Widget to the unaligned maximum dimensions of all the text content/characters in it. */
		void Wrap();
		/* Decompresses/UnWrap the text. */
		void UnWrap();
	private:
		bool WrapVertical();
		bool WrapHorizontal();

	protected:
		/* Notifies the canvas and updates the geometry. */
		void GeometryVerticesSizeChanged(std::size_t Size);
	public:
		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

		cbVertexColorStyle GetVertexColorStyle() const { return FontGeometryBuilder->GetVertexColorStyle(); }
		void SetVertexColorStyle(const cbVertexColorStyle& style);

		/*
		//  On the to-do list.
		void SetFontVertexSize(const float& FontSize); // Scale ?

		bool IsShadowEnabled() const;
		void SetShadowColor(const cbColor& Color);
		void SetShadowOffset(const cbVector& Offset);

		bool IsOutlineEnabled() const;
		void SetOutlineSize(const std::size_t Size);
		void SetOutlineColor(const cbColor& Color);

		bool IsStrikeEnabled() const;
		void SetStrikeColor(const cbColor& Color);
		bool IsUnderlineEnabled() const;
		void SetUnderlineColor(const cbColor& Color);
		*/

	private:
		virtual void OnAttach() override final;
		virtual void OnRemoveFromParent() override final;

	public:
		/* Controls the number of letters drawn on the screen. */
		std::optional<std::size_t> LetterDrawCount;

	private:
		cbTransform Transform;
		cbFontGeometry::UniquePtr FontGeometryBuilder;
		bool bIsItWrapped;
	};
}
