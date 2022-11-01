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
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "cbClassBody.h"

#include "cbGeometry.h"
#include "cbVertexColorStyle.h"

namespace cbgui
{
	struct cbFontDesc
	{
		struct cbFontLoadDesc
		{
			/*
			* Font pointer for loading from memory.
			* If the font pointer is set, FontName is not required.
			*/
			unsigned char* pFont;
			// Font pointer Size
			std::size_t pFontSize;

			/*
			* This is used if no Font pointer is specified.
			* If "DefaultFontLocation" is set, it can be a name with an extension.
			*/
			std::string FontPath;

			cbFORCEINLINE cbFontLoadDesc()
				: pFont(nullptr)
				, pFontSize(0)
				, FontPath("")
			{}
			cbFORCEINLINE cbFontLoadDesc(std::string InFontPath)
				: pFont(nullptr)
				, pFontSize(0)
				, FontPath(InFontPath)
			{}
			cbFORCEINLINE cbFontLoadDesc(unsigned char* FontPTR, std::size_t FontSize)
				: pFont(FontPTR)
				, pFontSize(FontSize)
				, FontPath("")
			{}
			cbFORCEINLINE ~cbFontLoadDesc()
			{
				pFont = nullptr;
			}
		};
		std::string FontFamilyName;

		/*
		* Optional
		* Default Font Location
		* Default : Empty("")
		*/
		std::string DefaultFontLocation;

		// TO DO
		//cbFontTypeDesc VariableFont;

		// Fonts to load
		std::map<eFontType, cbFontLoadDesc> Fonts;

		/*
		* Optional
		* Set only if the Font Ascender value (or baseline) is incorrect.
		* Font Size, Ascender
		*/
		std::map<std::size_t, int> DefaultAscender;
		/*
		* Optional
		* Set only if the Font Descender value (or baseline) is incorrect.
		* Descender is negative.
		* Font Size, Descender
		*/
		std::map<std::size_t, int> DefaultDescender;

		// dafault : 24
		std::size_t FontSize;
		/*
		* character resolution
		* dafault : 96
		*/
		std::size_t DPI;
		/*
		* Affects characters on 'Texture Atlas'
		* dafault : 7.0f
		*/
		cbVector SpaceBetweenChars;
		// Align characters on 'Texture Atlas'
		bool UseVerticalAligment;
		/*
		* Affects the texture coordinates of the characters.
		* Use for special conditions only.
		* dafault : 0
		*/
		cbMargin CharPadding;
		// dafault : 96
		std::size_t Numchars;

		/*
		* Called when new Glyph is added.
		*/
		std::function<void(const void*, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t)> fFontTextureUpdate_Callback;

		// dafault : 2048
		std::uint32_t AtlasWidth;
		// dafault : 2048
		std::uint32_t AtlasHeight;

		/*
		* First Character Position
		* important for Vertical Aligment
		* dafault : (0, FontSize)
		*/
		cbVector StartPosition;

		// Experimental, do not use 
		bool SDF;
		std::int32_t Spread;
		std::int32_t Overlaps;

		cbFORCEINLINE cbFontDesc(const std::string& FontName, const std::size_t CharSize = 24, const bool VerticalAligment = false)
			: FontFamilyName(FontName)
			, DefaultFontLocation("")
			, AtlasWidth(2048)
			, AtlasHeight(2048)
			, StartPosition(cbVector(0.0f, VerticalAligment ? CharSize : 0.0f))
			, Numchars(96)
			, FontSize(CharSize)
			, DPI(96)
			, SpaceBetweenChars(cbVector(7.0f, 7.0f))
			, UseVerticalAligment(VerticalAligment)
			, CharPadding(cbMargin())
			, fFontTextureUpdate_Callback(nullptr)
			, SDF(false)
			, Spread(4)
			, Overlaps(0)
		{}

		cbFORCEINLINE ~cbFontDesc()
		{
			Fonts.clear();
			fFontTextureUpdate_Callback = nullptr;
		}
	};

	/* A style used to format text. */
	struct cbTextStyle
	{
		/* Format/Tag name. */
		std::u32string Name;

		eFontType FontType;
		std::size_t FontSize;

		std::optional<cbColor> Color;

		/*bool bIsShadowEnabled;
		cbColor ShadowColor;
		cbVector ShadowOffset;

		bool bIsOutlineEnabled;
		std::size_t OutlineSize;
		cbColor OutlineColor;

		bool bIsStrikeEnabled;
		cbColor StrikeColor;
		bool bIsUnderlineEnabled;
		cbColor UnderlineColor;*/

		cbFORCEINLINE std::u32string GetStyleStartTag() const
		{
			return U"<"+ Name + U">";
		}

		cbFORCEINLINE std::u32string GetStyleEndTag() const
		{
			return U"</>";
		}

		cbFORCEINLINE cbTextStyle(const std::u32string& name, eFontType fontType = eFontType::Regular, std::size_t fontSize = 24, std::optional<cbColor> color = std::nullopt)
			: Name(name)
			, FontType(fontType)
			, FontSize(fontSize)
			, Color(color)
			/*, bIsStrikeEnabled(false)
			, StrikeColor(cbColor::Black())
			, bIsUnderlineEnabled(false)
			, UnderlineColor(cbColor::Black())
			, bIsOutlineEnabled(false)
			, OutlineSize(0)
			, OutlineColor(cbColor::Black())
			, bIsShadowEnabled(false)
			, ShadowOffset(cbVector::Zero())
			, ShadowColor(cbColor::Black())*/
		{}
		~cbTextStyle() = default;
	};

	/* Text properties. */
	struct cbTextDesc
	{
		std::size_t CharSize;
		eFontType FontType;
		bool bEnableAutoGrowVerticesSize;
		std::size_t AutoGrowVerticesSize;
		eTextJustify TextJustify;

		cbFORCEINLINE constexpr cbTextDesc(std::size_t pSize = 24, eTextJustify InTextJustify = eTextJustify::Left,
			eFontType FontType = eFontType::Regular, bool bEnableAutoGrowVerticesSize = false, std::size_t AutoGrowVerticesSize = 0)
			: CharSize(pSize)
			, FontType(FontType)
			, bEnableAutoGrowVerticesSize(bEnableAutoGrowVerticesSize)
			, AutoGrowVerticesSize(AutoGrowVerticesSize)
			, TextJustify(InTextJustify)
		{}
	};

	/* Text Data. */
	struct cbTextMetrics
	{
		struct cbCharacter
		{
			char32_t Character;
			std::size_t CharacterOrder;
			std::size_t WordOrder;
			std::size_t LineOrder;
			std::size_t CharacterSize;
			eFontType Type;
			int Ascender;
			int Descender;
			int xadvance;
			int BearingX;

			cbBounds Bounds;
			cbBounds TextureCoordinates;

			bool ignore;

			float Kerning;
			float AlignedKerning;
			std::optional<cbColor> Color;

			cbFORCEINLINE constexpr cbCharacter()
				: Character(0)
				, CharacterOrder(0)
				, WordOrder(0)
				, LineOrder(0)
				, CharacterSize(24)
				, Type(eFontType::Regular)
				, Ascender(0)
				, Descender(0)
				, xadvance(0)
				, BearingX(0)
				, Bounds(cbBounds())
				, TextureCoordinates(cbBounds())
				, ignore(false)
				, Kerning(0.0f)
				, AlignedKerning(0.0f)
				, Color(std::nullopt)
			{}
			cbFORCEINLINE constexpr cbCharacter(char32_t Char, std::size_t characterSize = 24,
									eFontType Type = eFontType::Regular, std::size_t characterOrder = 0, bool ignore = false)
				: Character(Char)
				, CharacterOrder(characterOrder)
				, WordOrder(0)
				, LineOrder(0)
				, CharacterSize(characterSize)
				, Type(Type)
				, Ascender(0)
				, Descender(0)
				, xadvance(0)
				, BearingX(0)
				, Bounds(cbBounds())
				, TextureCoordinates(cbBounds())
				, ignore(ignore)
				, Kerning(0.0f)
				, AlignedKerning(0.0f)
				, Color(std::nullopt)
			{}
			cbFORCEINLINE cbCharacter(char32_t InCharacter, std::size_t InCharacterOrder, std::size_t InWordOrder, std::size_t InLineOrder,
									std::size_t InCharacterSize, const eFontType InType, int InAscender, int InDescender, int Inxadvance, int InBearingX,
									const cbBounds& InBounds, const cbBounds& InTextureCoordinates, bool Inignore, float InKerning, const std::optional<cbColor> InColor)
				: Character(InCharacter)
				, CharacterOrder(InCharacterOrder)
				, WordOrder(InWordOrder)
				, LineOrder(InLineOrder)
				, CharacterSize(InCharacterSize)
				, Type(InType)
				, Ascender(InAscender)
				, Descender(InDescender)
				, xadvance(Inxadvance)
				, BearingX(InBearingX)
				, Bounds(InBounds)
				, TextureCoordinates(InTextureCoordinates)
				, ignore(Inignore)
				, Kerning(InKerning)
				, AlignedKerning(InKerning)
				, Color(InColor)
			{}

			cbFORCEINLINE ~cbCharacter() = default;

			cbFORCEINLINE constexpr bool IsNewLine() const
			{
				return Character == '\n';
			}
			cbFORCEINLINE constexpr bool IsSpace() const
			{
				return Character == 32;
			}
			cbFORCEINLINE constexpr bool IsValid() const
			{
				return (Character != '\n') && Character >= 32 && !ignore;
			}
		};

		struct cbLineMetaData
		{
			std::size_t Index;
			float Width;
			int Ascender;
			int Descender;
			float Baseline;
			float HorizontalAlignment;

			cbFORCEINLINE constexpr cbLineMetaData()
				: Index(0)
				, Width(0.0f)
				, Ascender(0)
				, Descender(0)
				, Baseline(0.0f)
				, HorizontalAlignment(0.0f)
			{}
			cbFORCEINLINE constexpr cbLineMetaData(std::size_t InIndex, float InWidth, int InAscender, int InDescender, float InBaseline, float InHorizontalAlignment)
				: Index(InIndex)
				, Width(InWidth)
				, Ascender(InAscender)
				, Descender(InDescender)
				, Baseline(InBaseline)
				, HorizontalAlignment(InHorizontalAlignment)
			{}

			cbFORCEINLINE ~cbLineMetaData() = default;

			cbFORCEINLINE constexpr int GetHeight() const { return Ascender - Descender; }
			cbFORCEINLINE constexpr cbDimension GetDimension() const { return cbDimension(Width, (float)GetHeight()); }
			cbFORCEINLINE constexpr cbBounds GetBounds() const { return cbBounds(GetDimension(), cbVector((Width / 2.0f) + HorizontalAlignment, (Baseline - Ascender + (GetHeight() / 2.0f)))); }
		};

		struct cbWordMetaData
		{
			std::size_t Index;
			float Width;
			float HorizontalAlignment;

			cbFORCEINLINE constexpr cbWordMetaData()
				: Index(0)
				, Width(0.0f)
				, HorizontalAlignment(0.0f)
			{}
			cbFORCEINLINE constexpr cbWordMetaData(std::size_t InIndex, float InWidth, float InHorizontalAlignment)
				: Index(InIndex)
				, Width(InWidth)
				, HorizontalAlignment(InHorizontalAlignment)
			{}

			cbFORCEINLINE ~cbWordMetaData() = default;

			cbFORCEINLINE constexpr float GetLeft() const { return HorizontalAlignment + (Width / 2.0f); }
		};

		std::vector<cbCharacter> Characters;

		std::vector<cbLineMetaData> LineMetaData;
		std::vector<cbWordMetaData> WordMetaData;

		std::string FontName;

		eFontType FontType;
		std::size_t FontSize;

		/*bool bIsShadowEnabled;
		cbColor ShadowColor;
		cbVector ShadowOffset;

		bool bIsOutlineEnabled;
		std::size_t OutlineSize;
		cbColor OutlineColor;

		bool bIsStrikeEnabled;
		cbColor StrikeColor;
		bool bIsUnderlineEnabled;
		cbColor UnderlineColor;*/

		bool bIsAscenderDescenderPowerOfTwo;
		bool bIsTextStylingEnabled;

		std::vector<cbTextStyle> TextStyles;

		cbFORCEINLINE cbTextMetrics()
			: FontName("")
			, FontType(eFontType::Regular)
			, FontSize(24)
			/*, bIsStrikeEnabled(false)
			, StrikeColor(cbColor::Black())
			, bIsUnderlineEnabled(false)
			, UnderlineColor(cbColor::Black())
			, bIsOutlineEnabled(false)
			, OutlineSize(0)
			, OutlineColor(cbColor::Black())
			, bIsShadowEnabled(false)
			, ShadowOffset(cbVector::Zero())
			, ShadowColor(cbColor::Black())*/
			, bIsAscenderDescenderPowerOfTwo(true)
			, bIsTextStylingEnabled(false)
		{}

		cbFORCEINLINE ~cbTextMetrics()
		{
			Release();
			TextStyles.clear();
		}

		cbFORCEINLINE void Release()
		{
			Characters.clear();
			LineMetaData.clear();
			WordMetaData.clear();
		}

		cbFORCEINLINE std::size_t LineCount() const
		{
			if (Characters.size() == 0)
				return 0;

			std::size_t LineCount = 1;
			for (const auto& Character : Characters)
				if (Character.Character == 10)
					LineCount++;

			return LineCount;
		}

		/* Ignores new line, space and ignored character */
		cbFORCEINLINE std::size_t GetFilteredSize() const
		{
			std::size_t FilteredSize = 0;
			for (const auto& Character : Characters)
				if (Character.IsValid() && !Character.IsSpace())
					FilteredSize++;

			return FilteredSize;
		}

		cbFORCEINLINE std::u32string GetText() const
		{
			std::u32string Text;
			for (auto& Character : Characters)
				Text.push_back(Character.Character);
			return Text;
		}

		/* Ignores new line, space and ignored character */
		cbFORCEINLINE std::u32string GetFilteredText() const
		{
			std::u32string Text;
			for (auto& Character : Characters)
				if (Character.IsValid())
					Text.push_back(Character.Character);
			return Text;
		}

		cbFORCEINLINE std::optional<cbTextStyle> GetTextStyle(const std::size_t& StyleIndex) const
		{
			if (StyleIndex < TextStyles.size())
				return TextStyles.at(StyleIndex);
			return std::nullopt;
		}
	};

	/* Simple Character Data. */
	struct cbCharacterData
	{
		char32_t Character;
		std::size_t CharacterOrder;
		bool Ignored;
		cbBounds Bounds;
		std::size_t LineOrder;

		cbFORCEINLINE constexpr cbCharacterData()
			: Character(0)
			, CharacterOrder(0)
			, Ignored(false)
			, Bounds(cbBounds())
			, LineOrder(0)
		{}

		cbFORCEINLINE constexpr cbCharacterData(cbTextMetrics::cbCharacter CharacterData, float Baseline)
			: Character(CharacterData.Character)
			, CharacterOrder(CharacterData.CharacterOrder)
			, Ignored(CharacterData.ignore)
			, LineOrder(CharacterData.LineOrder)
			, Bounds(cbBounds(cbVector(CharacterData.Bounds.Min.X + CharacterData.Kerning, CharacterData.Bounds.Min.Y + Baseline),
				cbVector(CharacterData.Bounds.Max.X + CharacterData.Kerning, CharacterData.Bounds.Max.Y + Baseline)))
		{}

		cbFORCEINLINE constexpr bool IsNewLine() const
		{
			return Character == '\n';
		}
		cbFORCEINLINE constexpr bool IsSpace() const
		{
			return Character == 32;
		}
		cbFORCEINLINE constexpr bool IsValid() const
		{
			return (Character != '\n') && Character >= 32 && !Ignored;
		}
	};

	/* Glyph pixel data. */
	struct cbFontTextureGlyphData
	{
		cbFORCEINLINE cbFontTextureGlyphData()
			: PenLocation(cbVector::Zero())
			, Width(0)
			, Height(0)
			, Pitch(0)
			, VerticalAligment(0)
			, UseVerticalAligment(false)
		{}
		cbFORCEINLINE cbFontTextureGlyphData(std::size_t InWidth, std::size_t InHeight, std::size_t InPitch, const cbVector& InPenLocation,
												bool InUseVerticalAligment, std::int32_t InVerticalAligment)
			: PenLocation(InPenLocation)
			, Width(InWidth)
			, Height(InHeight)
			, Pitch(InPitch)
			, VerticalAligment(InVerticalAligment)
			, UseVerticalAligment(InUseVerticalAligment)
		{}
		cbFORCEINLINE ~cbFontTextureGlyphData()
		{
			Pixels.clear();
		}
		cbFORCEINLINE constexpr const cbBounds GenerateTextureCoordinate() const
		{
			cbBounds Rect(cbVector(0.0f + PenLocation.X, 0.0f + PenLocation.Y), cbVector(Width + PenLocation.X, Height + PenLocation.Y));

			if (UseVerticalAligment)
			{
				Rect.Min.Y -= VerticalAligment;
				Rect.Max.Y -= VerticalAligment;
			}
			return Rect;
		}

		cbFORCEINLINE bool IsValid() const
		{
			if (Pixels.size() == NULL || Width == NULL || Height == NULL || Pitch == NULL)
				return false;
			return true;
		}

		std::size_t Width;
		std::size_t Height;
		std::size_t Pitch;
		cbVector PenLocation;
		std::int32_t VerticalAligment;
		bool UseVerticalAligment;
		std::vector<unsigned char> Pixels;
	};

	/* Glyph data. */
	struct cbGlyph
	{
		unsigned int Width;
		unsigned int Height;
		int BearingX;
		int BearingY;
		int xadvance;
		int LeftSideBitmapBearing;
		int TopSideBitmapBearing;

		/* Pixel data. (Shared) */
		cbFontTextureGlyphData* FontTextureGlyphData;

		cbFORCEINLINE constexpr cbGlyph()
			: Width(NULL)
			, Height(NULL)
			, BearingX(NULL)
			, BearingY(NULL)
			, xadvance(NULL)
			, LeftSideBitmapBearing(NULL)
			, TopSideBitmapBearing(NULL)
			, FontTextureGlyphData(nullptr)
		{}
		cbFORCEINLINE constexpr cbGlyph(unsigned int InWidth, unsigned int InHeight, int InBearingX, int InBearingY,
							int Inxadvance, int InLeftSideBitmapBearing, int InTopSideBitmapBearing,
							cbFontTextureGlyphData* InFontTextureGlyphData)
			: Width(InWidth)
			, Height(InHeight)
			, BearingX(InBearingX)
			, BearingY(InBearingY)
			, xadvance(Inxadvance)
			, LeftSideBitmapBearing(InLeftSideBitmapBearing)
			, TopSideBitmapBearing(InTopSideBitmapBearing)
			, FontTextureGlyphData(InFontTextureGlyphData)
		{}
		cbFORCEINLINE ~cbGlyph()
		{
			Width = NULL;
			Height = NULL;
			BearingX = NULL;
			BearingY = NULL;
			xadvance = NULL;
			LeftSideBitmapBearing = NULL;
			TopSideBitmapBearing = NULL;
			//if (FontTextureGlyphData)
			//	delete FontTextureGlyphData;
			FontTextureGlyphData = nullptr;
		}
	};

	/* Font library interface. */
	class cbIFontFamily
	{
		cbBaseClassBody(cbClassDefaultProtectedConstructor, cbIFontFamily)
	public:
		/* Creates Character. */
		virtual bool CreateChar(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character) = 0;
		virtual bool IsCharExist(const eFontType& FontType, const std::size_t& Size, const char32_t& Character) = 0;
		/* Creates Character. */
		virtual const cbGlyph* FindOrAdd(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character) = 0;
		/* Returns Character Glyph. */
		virtual const cbGlyph* GetGlyph(const eFontType& FontType, const std::size_t& Size, const char32_t& ch) const = 0;
		/* Returns the length of the space between characters. */
		virtual std::int32_t GetKerning(const int& ch1, const int& ch2, const std::size_t& Size, const eFontType& Type) const = 0;
		virtual std::optional<cbBounds> GetTextureCoordinate(const eFontType& FontType, const std::size_t& Size, const char32_t& Character) = 0;

		/* Returns the maximum length above the baseline. */
		virtual int GetAscender(const std::size_t& Size, const eFontType& Type) const = 0;
		/* Returns the maximum length below the baseline. */
		virtual int GetDescender(const std::size_t& Size, const eFontType& Type) const = 0;

		/* Returns the length of the space between the lines. */
		virtual std::size_t GetLineDistance(const std::size_t& Size, const eFontType& FontType) = 0;

		virtual const std::string GetFontFamilyName() const = 0;
		virtual std::size_t GetDefaultFontSize() const = 0;
		virtual std::size_t GetTextureWidth() const = 0;
		virtual std::size_t GetTextureHeight() const = 0;
		virtual const cbFontDesc& GetDesc() const = 0;

		/* Returns Font Texture. */
		virtual const std::vector<unsigned char> GetTexture() const = 0;
	};

	/* FreeType Font library interface. */
	class cbFreeTypeFontFamily : public cbIFontFamily
	{
		cbClassBody(cbClassConstructor, cbFreeTypeFontFamily, cbIFontFamily)
	public:
		cbFreeTypeFontFamily(const cbFontDesc& FontDesc);
	public:
		virtual ~cbFreeTypeFontFamily();

		/* Creates Character. */
		virtual bool CreateChar(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character) override;
		virtual bool IsCharExist(const eFontType& FontType, const std::size_t& Size, const char32_t& Character) override;
		/* Creates Character. */
		virtual const cbGlyph* FindOrAdd(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character) override;
		/* Returns Character Glyph. */
		virtual const cbGlyph* GetGlyph(const eFontType& FontType, const std::size_t& Size, const char32_t& ch) const override;
		/* Returns the length of the space between characters. */
		virtual std::int32_t GetKerning(const int& ch1, const int& ch2, const std::size_t& Size, const eFontType& Type) const override;
		virtual std::optional<cbBounds> GetTextureCoordinate(const eFontType& FontType, const std::size_t& Size, const char32_t& Character) override;

		/* Returns the maximum length above the baseline. */
		virtual int GetAscender(const std::size_t& Size, const eFontType& Type) const override;
		/* Returns the maximum length below the baseline. */
		virtual int GetDescender(const std::size_t& Size, const eFontType& Type) const override;

		/* Returns the length of the space between the lines. */
		virtual std::size_t GetLineDistance(const std::size_t& Size, const eFontType& FontType) override;

		virtual const std::string GetFontFamilyName() const override { return Desc.FontFamilyName; }
		virtual std::size_t GetDefaultFontSize() const override { return Desc.FontSize; }
		virtual std::size_t GetTextureWidth() const override { return Desc.AtlasWidth; }
		virtual std::size_t GetTextureHeight() const override { return Desc.AtlasHeight; }
		virtual const cbFontDesc& GetDesc() const override { return Desc; }

		/* Returns Font Texture. */
		virtual const std::vector<unsigned char> GetTexture() const override;

	private:
		cbFontDesc Desc;
		class FreeTypeFont;
		FreeTypeFont* FreeTypeFontManager;
	};

	/* Required function to load FreeTypeFont library. */
	cbFreeTypeFontFamily* CreateFreeTypeFont(const cbFontDesc& FontDesc);

	class cbText;

	/* Font Geometry builder. */
	class cbFontGeometry
	{
		cbBaseClassBody(cbClassConstructor, cbFontGeometry)
	public:
		cbFontGeometry(cbText* pOwner, cbIFontFamily* FontFamily);
		cbFontGeometry(cbText* pOwner, const std::u32string& inText, const cbTextDesc& TextDesc, cbIFontFamily* FontFamily);

		virtual ~cbFontGeometry();

		const cbIFontFamily* GetFontFamily() const { return FontFamily; }
		void SetFontFamily(cbIFontFamily* FontFamily);

		/* Deletes the existing text, adds a new one. */
		void SetText(const std::u32string& Text, const std::optional<cbTextDesc> Desc);
		/* Appends text to the end of existing text. */
		void AppendText(const std::u32string& Text);
		/* Destroys all characters. */
		void Clear();

		/* Build or Rebuilds character data. */
		void BuildText();
	private:
		bool BuildCharacter(cbTextMetrics::cbCharacter* Character);
		/* Generates character data and apply Text Style if available. */
		void ParseCharacters(const std::u32string& Text);

		const cbGlyph* GetGlyph(const eFontType& Type, const std::size_t& Size, const char32_t& ch) const;

	public:
		inline std::vector<cbVector4> GetVertices() const { return Vertices; }
		inline std::vector<cbColor> GetVertexColors() const { return VertexColors; }
		inline std::vector<cbVector> GetTextureCoordinate() const { return TextureCoordinates; }
		std::vector<std::uint32_t> GenerateIndices() const;

		inline const std::size_t GetDrawCount() const { return (GetTextSize() * 6); }
		inline const std::size_t GetVerticesSize() const { return Vertices.size(); }
		inline const std::size_t GetIndexCount() const { return (Vertices.size() / 4) * 6; }

		//virtual void SetFontVertexSize(const float& FontSize) override;

		inline constexpr std::size_t GetDefaultCharSize() const { return TextDesc.CharSize; }
		/* Creates Character. */
		void AddChar(const char32_t pChar, const std::optional<std::size_t> Index = std::nullopt, bool Filtered = true);
		bool RemoveChar(const std::size_t& Index);
		bool RemoveChars(const std::vector<std::size_t>& Indices);

		/* Returns the total dimensions of the text. */
		inline constexpr cbBounds GetGeometryBounds() const { return CachedBounds; }
		/* Returns the total sizes of individual rows. */
		std::vector<cbBounds> GetLineBounds() const;

		/* If Text reaches this Size, the text buffer grows. */
		void SetAutoGrowVerticesSize(const bool value, const std::size_t Size = 24);
		void GrowTextGeometryData();
		inline constexpr std::size_t GetVerticesGrowSize() const { return TextDesc.AutoGrowVerticesSize; }
		inline constexpr bool IsAutoGrowVerticesSizeEnabled() const { return TextDesc.bEnableAutoGrowVerticesSize; }

		/* When the line ends, it automatically switches to the next line. */
		void SetAutoWrapText(const bool Value);
		bool IsAutoWrapTextEnabled() const;
		void SetCustomWrapWidth(const std::optional<float> Width);
		float GetAutoWrapWidth() const;

		inline constexpr eTextJustify GetTextJustify() const { return TextDesc.TextJustify; }
		void SetTextJustify(eTextJustify Justify);

		/* Sets the length of the space(32) character. */
		void SetDefaultSpaceWidth(const std::optional<std::size_t> Width);
		/*
		* Manipulates Current Line Ascender and Previous Line Descender.
		* Default : 1.0f
		*/
		void SetLineHeightPercentage(const float Percentage);

		/* Makes the text dimension even. */ 
		void SetAscenderDescenderPowerOfTwo(const bool Value);
		void SetFontType(const eFontType& Type);
		void SetFontSize(const std::size_t& Size);

		/* A style used to format text. */
		void SetTextStylingEnabled(const bool Value);
		/* A style used to format text. */
		void AddTextStyle(const cbTextStyle& Style);
		void RemoveTextStyle(const std::size_t& Index);
		inline std::vector<cbTextStyle> GetTextStyles() const { return TextMetrics.TextStyles; }

		inline cbVertexColorStyle GetVertexColorStyle() const { return Style; }
		void SetVertexColorStyle(const cbVertexColorStyle& pStyle) { Style = pStyle; UpdateVertexColors(); }

		/* Returns all characters in the line and the bounds of the line. */
		bool GetLineData(std::size_t Index, cbBounds& outLineBounds, std::vector<cbCharacterData>& outCharacterData) const;
		/* Returns the bounds of the line. */
		std::optional<cbBounds> GetLineBounds(std::size_t Index) const;
		/* Returns the character data at the given index. */
		std::optional<cbCharacterData> GetCharacterData(std::size_t Index) const;

		/* Returns Text. If filtered is enabled, it ignores newline, space, and ignored character. */
		const std::u32string GetText(bool Filtered = false) const;
		/* Returns text character size. If filtered is enabled, it ignores newline, space, and ignored character. */
		const std::size_t GetTextSize(bool Filtered = false) const;

		inline constexpr cbTextDesc GetTextDesc() const { return TextDesc; }

		inline const std::size_t GetLineCount() const { return TextMetrics.LineCount(); }

		/* Updates Vertices, TextureCoordinates, VertexColors */
		void UpdateGeometry();
		/* Aligns the text. */
		void UpdateAlignments();
		void UpdateVerticalAlignment();
		void UpdateHorizontalAlignment();
		/* Updates Cached Text Bounds */
		void UpdateBounds();

	private:
		void RebuildText();

		void UpdateVertices();
		void UpdateVertexColors();
		void UpdateTextureCoordinates();

	private:
		cbText* Owner;
		cbFreeTypeFontFamily* FontFamily;
		cbVertexColorStyle Style;

		cbTextMetrics TextMetrics;

		cbTextDesc TextDesc;

		/*
		*  Specify space width otherwise "fonts default horiAdvance" is used.
		*/
		std::optional<std::size_t> DefaultSpaceWidth;
		/*
		* Manipulates Current Line Ascender and Previous Line Descender.
		* Default : 1.0f
		*/
		float LineHeightPercentage;

		bool bAutoWrapText;
		std::optional<float> CustomWrapSize;

		std::vector<cbVector4> Vertices;
		std::vector<cbColor> VertexColors;
		std::vector<cbVector> TextureCoordinates;
		cbBounds CachedBounds;
	};
}
