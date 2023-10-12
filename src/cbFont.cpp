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
#include "cbFont.h"

#include <sstream>
#include <limits>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#include "cbText.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H
#include FT_IMAGE_H
#include <freetype/ftmodapi.h>
#pragma comment(lib, "freetype.lib")

namespace cbgui
{
	#define FONTSIZE(x) FT_F26Dot6(x * 64)

	struct SizedGlyphData
	{
	protected:
		std::map<std::size_t, cbGlyph*> Glyphs;
	public:
		cbFORCEINLINE SizedGlyphData() = default;
		cbFORCEINLINE SizedGlyphData(const std::size_t& Size, cbGlyph* Glyph)
		{
			Insert(Size, Glyph);
		}
		cbFORCEINLINE ~SizedGlyphData()
		{
			for (auto& Glyph : Glyphs)
			{
				delete Glyph.second;
				Glyph.second = nullptr;
			}
			Glyphs.clear();
		}

		cbFORCEINLINE void Insert(const std::size_t& Size, cbGlyph* Glyph)
		{
			if (Glyphs.find(Size) != Glyphs.end())
			{
				if (Glyphs[Size])
				{
					delete Glyphs[Size];
					Glyphs[Size] = nullptr;
				}
				Glyphs[Size] = Glyph;
			}
			else
			{
				Glyphs.insert({ Size,Glyph });
			}
		}

		cbFORCEINLINE cbGlyph* Get(const std::size_t& Size) const
		{
			if (Glyphs.find(Size) != Glyphs.end())
				return Glyphs.at(Size);
			else
				return nullptr;
		}

		cbFORCEINLINE const std::optional<cbBounds> GenerateTextureCoordinate(const std::size_t& Size, const cbVector& PenLocation) const
		{
			if (Glyphs.find(Size) == Glyphs.end())
				return std::nullopt;

			cbBounds Rect(cbVector(0.0f + PenLocation.X, 0.0f + PenLocation.Y), cbVector(Glyphs.at(Size)->Width + PenLocation.X, Glyphs.at(Size)->Height + PenLocation.Y));

			if (Glyphs.at(Size)->FontTextureGlyphData->UseVerticalAligment)
			{
				Rect.Min.Y -= Glyphs.at(Size)->TopSideBitmapBearing;
				Rect.Max.Y -= Glyphs.at(Size)->TopSideBitmapBearing;
			}
			return Rect;
		}
	};

	struct cbFontTextureMetaData
	{
	protected:
		struct cbSizedTextureData
		{
		protected:
			std::map<std::size_t, cbFontTextureGlyphData*> FontTextureGlyphData;

		public:
			cbFORCEINLINE cbSizedTextureData() = default;
			cbFORCEINLINE cbSizedTextureData(const std::size_t& Size, cbFontTextureGlyphData* Glyph)
			{
				Insert(Size, Glyph);
			}
			cbFORCEINLINE ~cbSizedTextureData()
			{
				for (auto& Glyph : FontTextureGlyphData)
				{
					delete Glyph.second;
					Glyph.second = nullptr;
				}
				FontTextureGlyphData.clear();
			}

			cbFORCEINLINE constexpr const std::map<std::size_t, cbFontTextureGlyphData*>& GetTextureGlyphData() const
			{
				return FontTextureGlyphData;
			}

			cbFORCEINLINE cbFontTextureGlyphData* Get(const std::size_t& Size) const
			{
				if (FontTextureGlyphData.find(Size) != FontTextureGlyphData.end())
				{
					return FontTextureGlyphData.at(Size);
				}
				return nullptr;
			}

			cbFORCEINLINE void Insert(const std::size_t& Size, cbFontTextureGlyphData* pTextureData)
			{
				if (FontTextureGlyphData.find(Size) != FontTextureGlyphData.end())
				{
					if (FontTextureGlyphData[Size])
					{
						delete FontTextureGlyphData[Size];
						FontTextureGlyphData[Size] = nullptr;
					}
					FontTextureGlyphData[Size] = pTextureData;
				}
				else
				{
					FontTextureGlyphData.insert({ Size, pTextureData });
				}
			}

		};
		struct cbTextureData
		{
		protected:
			std::map<char32_t, cbSizedTextureData*> TextureData;

		public:
			cbFORCEINLINE cbTextureData() = default;
			cbFORCEINLINE cbTextureData(const char32_t& Ch, const std::size_t& Size, cbFontTextureGlyphData* Glyph)
			{
				Insert(Ch, Size, Glyph);
			}
			cbFORCEINLINE ~cbTextureData()
			{
				for (auto& Glyph : TextureData)
				{
					delete Glyph.second;
					Glyph.second = nullptr;
				}
				TextureData.clear();
			}

			cbFORCEINLINE constexpr const std::map<char32_t, cbSizedTextureData*>& GetSizedTextureGlypheData() const
			{
				return TextureData;
			}

			cbFORCEINLINE cbFontTextureGlyphData* Get(const char32_t& Ch, const std::size_t& Size) const
			{
				if (TextureData.find(Ch) != TextureData.end())
				{
					return TextureData.at(Ch)->Get(Size);
				}
				return nullptr;
			}

			cbFORCEINLINE void Insert(const char32_t& Ch, const std::size_t& Size, cbFontTextureGlyphData* pTextureData)
			{
				if (TextureData.find(Ch) != TextureData.end())
				{
					if (!TextureData.at(Ch))
						TextureData.insert({ Ch, new cbSizedTextureData(Size, pTextureData) });
					else
						TextureData[Ch]->Insert(Size, pTextureData);
				}
				else
				{
					TextureData.insert({ Ch, new cbSizedTextureData(Size, pTextureData) });
				}
			}
		};

	protected:
		std::map<eFontType, cbTextureData*> TextureMetaData;

	public:
		cbFORCEINLINE cbFontTextureMetaData(const cbFontDesc& FontDesc)
			: LastPenPosition(FontDesc.StartPosition)
			, Dimension(cbDimension((float)FontDesc.AtlasWidth, (float)FontDesc.AtlasHeight))
			, SpaceBetweenChars(FontDesc.SpaceBetweenChars)
			, LastlineMaximumHeight(0.0f)
		{}
		cbFORCEINLINE ~cbFontTextureMetaData()
		{
			for (auto& MetaData : TextureMetaData)
			{
				delete MetaData.second;
				MetaData.second = nullptr;
			}
			TextureMetaData.clear();
		}

		cbVector LastPenPosition;
		cbVector SpaceBetweenChars;
		cbDimension Dimension;
		float LastlineMaximumHeight;

		cbFORCEINLINE constexpr const std::map<eFontType, cbTextureData*>& GetTextureMetaData() const
		{
			return TextureMetaData;
		}

		cbFORCEINLINE const cbFontTextureGlyphData* Get(const eFontType& FontType, const char32_t& ch, const std::size_t& Size) const
		{
			if (TextureMetaData.find(FontType) != TextureMetaData.end())
			{
				return TextureMetaData.at(FontType)->Get(ch, Size);
			}
			return nullptr;
		}

		cbFORCEINLINE cbFontTextureGlyphData* UpdateMetaData(const eFontType& FontType, const char32_t& ch, const std::size_t& FontSize, const FT_GlyphSlot& glyph, bool UseVerticalAligment)
		{
			const FT_Bitmap& bmp = glyph->bitmap;

			if (((glyph->metrics.height >> 6) + LastPenPosition.Y) > LastlineMaximumHeight)
			{
				LastlineMaximumHeight = (glyph->metrics.height >> 6) + LastPenPosition.Y;
			}

			if (LastlineMaximumHeight >= Dimension.GetHeight() || (LastlineMaximumHeight + (glyph->metrics.height >> 6)) >= Dimension.GetWidth())
			{
				return nullptr;
			}

			if (LastPenPosition.X + (bmp.width) >= Dimension.GetWidth())
			{
				LastPenPosition.X = 0;
				LastPenPosition.Y = LastlineMaximumHeight + (std::size_t)SpaceBetweenChars.Y;
			}

			cbFontTextureGlyphData* pTextureData = new cbFontTextureGlyphData(bmp.width, bmp.rows, bmp.pitch, LastPenPosition, false, 0);

			for (std::size_t row = 0; row < bmp.rows; ++row)
			{
				for (std::size_t col = 0; col < bmp.width; ++col)
				{
					std::size_t x = (std::size_t)LastPenPosition.X + col;
					std::size_t y = (std::size_t)LastPenPosition.Y + row;
					if (UseVerticalAligment && y > (glyph->bitmap_top))
					{
						y -= (glyph->bitmap_top);
					}
					pTextureData->Pixels.push_back(bmp.buffer[row * bmp.pitch + col]);
				}
			}

			if (UseVerticalAligment)
			{
				pTextureData->UseVerticalAligment = UseVerticalAligment;
				pTextureData->VerticalAligment = glyph->bitmap_top;
				if (pTextureData->VerticalAligment < 0)
					pTextureData->VerticalAligment *= (-1);
			}

			if (TextureMetaData.find(FontType) != TextureMetaData.end())
			{
				if (!TextureMetaData.at(FontType))
					TextureMetaData.insert({ FontType, new cbTextureData(ch, FontSize, pTextureData) });
				else
					TextureMetaData[FontType]->Insert(ch, FontSize, pTextureData);
			}
			else
			{
				TextureMetaData.insert({ FontType, new cbTextureData(ch, FontSize, pTextureData) });
			}

			LastPenPosition.X += (bmp.width) + (std::size_t)SpaceBetweenChars.X;

			return pTextureData;
		}
	};

	class cbFreeTypeFontFamily::FreeTypeFont final
	{
	public:
		struct FreeTypeFontFace
		{
		public:
			FreeTypeFontFace(FreeTypeFont* pOwner, const FT_Library& FreeTypeLibrary, const std::string& Path, const eFontType& pType)
				: Type(pType)
				, Owner(pOwner)
			{
				FT_New_Face(FreeTypeLibrary, Path.c_str(), 0, &FontFace);
				FT_Select_Charmap(FontFace, FT_ENCODING_UNICODE);
			}
			FreeTypeFontFace(FreeTypeFont* pOwner, const FT_Library& FreeTypeLibrary, unsigned char* pFont, FT_Long file_size, FT_Long face_index, const eFontType& pType)
				: Type(pType)
				, Owner(pOwner)
			{
				auto Error = FT_New_Memory_Face(FreeTypeLibrary, pFont, file_size, face_index, &FontFace);
				FT_Select_Charmap(FontFace, FT_ENCODING_UNICODE);
			}
			~FreeTypeFontFace()
			{
				Owner = nullptr;
				FT_Done_Face(FontFace);
				for (auto& Glyph : Glyphs)
				{
					delete Glyph.second;
					Glyph.second = nullptr;
				}
				Glyphs.clear();
			}

			cbFORCEINLINE void LoadGlyphs(cbFontTextureMetaData* pTextureMetaData)
			{
				const cbFontDesc& Desc = Owner->GetDesc();

				FT_Set_Char_Size(
					FontFace,                  // handle to face object
					0,                         // char_width in 1/64th of points
					FONTSIZE(Desc.FontSize),   // char_height in 1/64th of points
					(FT_UInt)Desc.DPI,         // horizontal device resolution
					(FT_UInt)Desc.DPI);        // vertical device resolution

				FT_Int32 flags = NULL;
				if (FT_HAS_COLOR(FontFace))
					flags |= FT_LOAD_COLOR;
				else
					flags |= FT_LOAD_DEFAULT;

				for (int i = 32; i < Desc.Numchars + 32; ++i)
				{
					FT_Load_Char(FontFace, i, flags | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
					const FT_GlyphSlot& glyph = FontFace->glyph;

					if (Desc.SDF)
						FT_Render_Glyph(glyph, FT_RENDER_MODE_SDF);
					else
						FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

					if (const auto& MetaData = pTextureMetaData->UpdateMetaData(Type, i, Desc.FontSize, glyph, Desc.UseVerticalAligment))
					{
						cbGlyph* Glyph = new cbGlyph(glyph->metrics.width >> 6, glyph->metrics.height >> 6, glyph->metrics.horiBearingX >> 6,
							glyph->metrics.horiBearingY >> 6, glyph->metrics.horiAdvance >> 6, glyph->bitmap_left, glyph->bitmap_top, MetaData);

						AddGlyph(i, Desc.FontSize, Glyph);
					}
				}
			}

			cbFORCEINLINE const cbGlyph* CreateChar(const std::size_t& FontSize, const char32_t& character, cbFontTextureMetaData* pTextureMetaData)
			{
				if (IsCharExist(FontSize, character) && character != '\n')
					return nullptr;

				const cbFontDesc& Desc = Owner->GetDesc();

				FT_Set_Char_Size(
					FontFace,               // handle to face object
					0,                      // char_width in 1/64th of points
					FONTSIZE(FontSize),     // char_height in 1/64th of points
					(FT_UInt)Desc.DPI,      // horizontal device resolution
					(FT_UInt)Desc.DPI);     // vertical device resolution

				FT_Int32 flags = NULL;
				if (FT_HAS_COLOR(FontFace))
					flags |= FT_LOAD_COLOR;
				else
					flags |= FT_LOAD_DEFAULT;

				auto Error = FT_Load_Char(FontFace, character, flags | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
				if (Error)
					return nullptr;

				const FT_GlyphSlot& glyph = FontFace->glyph;

				if (Desc.SDF)
					Error = FT_Render_Glyph(glyph, FT_RENDER_MODE_SDF);
				else
					Error = FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
				if (Error)
					return nullptr;

				if (const auto& MetaData = pTextureMetaData->UpdateMetaData(Type, character, FontSize, glyph, Desc.UseVerticalAligment))
				{
					cbGlyph* Glyph = new cbGlyph(glyph->metrics.width >> 6, glyph->metrics.height >> 6, glyph->metrics.horiBearingX >> 6,
						glyph->metrics.horiBearingY >> 6, glyph->metrics.horiAdvance >> 6, glyph->bitmap_left, glyph->bitmap_top, MetaData);

					AddGlyph(character, FontSize, Glyph);
					return Glyph;
				}

				return nullptr;
			}

			inline bool IsKerningSupported() const { return FT_HAS_KERNING(FontFace); }
			inline int GetAscender() const { return FontFace->ascender >> 6; }
			inline int GetDescender() const { return FontFace->descender >> 6; }

			inline std::int32_t GetKerning(const int& ch1, const int& ch2, const std::size_t& Size) const
			{
				const cbFontDesc& Desc = Owner->GetDesc();

				FT_Set_Char_Size(
					FontFace,                  // handle to face object
					0,                         // char_width in 1/64th of points
					FONTSIZE(Size),            // char_height in 1/64th of points
					(FT_UInt)Desc.DPI,                  // horizontal device resolution
					(FT_UInt)Desc.DPI);                 // vertical device resolution

				FT_Vector delta;
				FT_Get_Kerning(FontFace, FT_Get_Char_Index(FontFace, ch1), FT_Get_Char_Index(FontFace, ch2), FT_Kerning_Mode_::FT_KERNING_DEFAULT, &delta);
				return std::int32_t(delta.x >> 6);
			}

			std::size_t GetLineDistance(const std::size_t& Size)
			{
				const cbFontDesc& Desc = Owner->GetDesc();

				FT_Set_Char_Size(
					FontFace,                  // handle to face object
					0,                         // char_width in 1/64th of points
					FONTSIZE(Size),            // char_height in 1/64th of points
					(FT_UInt)Desc.DPI,                  // horizontal device resolution
					(FT_UInt)Desc.DPI);

				return FontFace->size->metrics.height >> 6;
			}

			bool IsCharExist(const std::size_t& Size, const char32_t& Character) const
			{
				if (Glyphs.find(Character) != Glyphs.end())
				{
					if (Glyphs.at(Character)->Get(Size))
						return true;
				}

				return false;
			}

			inline void AddGlyph(const char32_t& ch, const std::size_t& Size, cbGlyph* Glyph)
			{
				if (Glyphs.find(ch) != Glyphs.end())
				{
					if (!Glyphs.at(ch))
						Glyphs.insert({ ch, new SizedGlyphData(Size, Glyph) });
					else
						Glyphs.at(ch)->Insert(Size, Glyph);
				}
				else
				{
					Glyphs.insert({ ch, new SizedGlyphData(Size, Glyph) });
				}
			}
			inline const cbGlyph* GetGlyph(const char32_t& ch, const std::size_t& Size) const
			{
				if (Glyphs.find(ch) != Glyphs.end())
				{
					return Glyphs.at(ch)->Get(Size);
				}
				return nullptr;
			}

			const std::optional<cbBounds> GenerateTextureCoordinate(const char32_t& Ch, const std::size_t& Size, const cbVector& PenLocation) const
			{
				if (Glyphs.find(Ch) != Glyphs.end())
				{
					return Glyphs.at(Ch)->GenerateTextureCoordinate(Size, PenLocation);
				}
				return std::nullopt;
			}

			const eFontType& GetType() const { return Type; }
			const FT_Face& Get() const { return FontFace; }
			const std::map<char32_t, SizedGlyphData*>& GetGlyphs() const { return Glyphs; }

		protected:
			FT_Face FontFace;
			std::map<char32_t, SizedGlyphData*> Glyphs;
			eFontType Type;
			FreeTypeFont* Owner;
		};

	public:
		FreeTypeFont(cbFreeTypeFontFamily* pOwner, const cbFontDesc& FontDesc)
			: mTextureMetaData(new cbFontTextureMetaData(FontDesc))
			, Owner(pOwner)
		{
			FT_Init_FreeType(&FreeTypeLibrary);

			FT_Property_Set(FreeTypeLibrary, "bsdf", "spread", &FontDesc.Spread);
			FT_Property_Set(FreeTypeLibrary, "sdf", "spread", &FontDesc.Spread);
			FT_Property_Set(FreeTypeLibrary, "sdf", "overlaps", &FontDesc.Overlaps);
		}

		~FreeTypeFont()
		{
			Owner = nullptr;
			for (auto& FontFace : FontFaces)
			{
				delete FontFace.second;
				FontFace.second = nullptr;
			}
			FontFaces.clear();

			delete mTextureMetaData;
			mTextureMetaData = nullptr;

			FT_Done_FreeType(FreeTypeLibrary);
		}

		inline const cbFontDesc& GetDesc() const { return Owner->GetDesc(); }

		bool LoadFont(const eFontType& FontType, const std::string& Path)
		{
			if (FontFaces.find(FontType) == FontFaces.end())
			{
				FontFaces.insert({ FontType, new FreeTypeFontFace(this, FreeTypeLibrary, Path, FontType) });
				return true;
			}
			return false;
		}

		bool LoadFont(const eFontType& FontType, unsigned char* pFont, FT_Long file_size, FT_Long face_index)
		{
			if (FontFaces.find(FontType) == FontFaces.end())
			{
				FontFaces.insert({ FontType, new FreeTypeFontFace(this, FreeTypeLibrary, pFont, file_size, face_index, FontType) });
				return true;
			}
			return false;
		}

		void LoadGlyphs(const eFontType& FontType)
		{
			if (IsFontTypeExist(FontType))
				FontFaces.at(FontType)->LoadGlyphs(mTextureMetaData);
		}

		inline void AddGlyph(const eFontType& FontType, const char32_t& ch, const std::size_t& Size, cbGlyph* Glyph)
		{
			if (IsFontTypeExist(FontType))
				FontFaces.at(FontType)->AddGlyph(ch, Size, Glyph);
		}

		std::int32_t GetKerning(const int& ch1, const int& ch2, const std::size_t& Size, const eFontType& FontType) const
		{
			if (IsFontTypeExist(FontType))
				return FontFaces.at(FontType)->GetKerning(ch1, ch2, Size);
			return 0;
		}

		std::size_t GetLineDistance(const std::size_t& Size, const eFontType& FontType)
		{
			if (IsFontTypeExist(FontType))
				return FontFaces.at(FontType)->GetLineDistance(Size);
			return 0;
		}

		bool IsCharExist(const eFontType& FontType, const std::size_t& Size, const char32_t& Character)
		{
			if (IsFontTypeExist(FontType))
				return FontFaces.at(FontType)->IsCharExist(Size, Character);
			return false;
		}

		inline const cbGlyph* GetGlyph(const eFontType& FontType, const char32_t& ch, const std::size_t& Size) const
		{
			if (IsFontTypeExist(FontType))
				return FontFaces.at(FontType)->GetGlyph(ch, Size);
			return nullptr;
		}

		std::optional<cbBounds> GetTextureCoordinate(const eFontType& FontType, const std::size_t& Size, const char32_t& Character)
		{
			if (!IsFontTypeExist(FontType))
				return std::nullopt;

			if (const auto& pTC = mTextureMetaData->Get(FontType, Character, Size))
				return pTC->GenerateTextureCoordinate();

			return std::nullopt;
		}


		const cbGlyph* CreateChar(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character)
		{
			if (IsFontTypeExist(FontType))
			{
				return FontFaces.at(FontType)->CreateChar(FontSize, character, mTextureMetaData);
			}
			return nullptr;
		}

		const cbGlyph* FindOrAdd(const eFontType& FontType, const std::size_t& Size, const char32_t& character)
		{
			if (!IsFontTypeExist(FontType))
				return nullptr;

			if (const cbGlyph* Glyph = GetGlyph(FontType, character, Size))
				return Glyph;

			return CreateChar(FontType, Size, character);
		}

		const bool IsFontTypeExist(const eFontType& FontType) const { return FontFaces.find(FontType) != FontFaces.end(); }
		const FT_Face& GetFace(const eFontType& FontType) const { return FontFaces.at(FontType)->Get(); }
		const FreeTypeFontFace* GetFontFace(const eFontType& FontType) const { return FontFaces.at(FontType); }
		const std::map<eFontType, FreeTypeFontFace*>& GetFontFaces() const { return FontFaces; }

		const std::vector<unsigned char> GenerateTexture(const std::size_t& AtlasWidth, const std::size_t& AtlasHeight, const cbVector& SpaceBetweenChars, const cbMargin& CharPadding)
		{
			std::vector<unsigned char> Image;
			Image.resize(AtlasWidth * AtlasHeight);

			for (const auto& pTextureMetaData : mTextureMetaData->GetTextureMetaData())
			{
				for (const auto& pTextureData : pTextureMetaData.second->GetSizedTextureGlypheData())
				{
					for (const auto& mTextureData : pTextureData.second->GetTextureGlyphData())
					{
						if (!mTextureData.second->IsValid())
							continue;

						for (std::size_t row = 0; row < mTextureData.second->Height; ++row)
						{
							for (std::size_t col = 0; col < mTextureData.second->Width; ++col)
							{
								std::size_t x = (std::size_t)mTextureData.second->PenLocation.X + col;
								std::size_t y = (std::size_t)mTextureData.second->PenLocation.Y + row;
								if (mTextureData.second->UseVerticalAligment && y > (mTextureData.second->VerticalAligment))
								{
									y -= (mTextureData.second->VerticalAligment);
								}
								Image[y * AtlasWidth + x] = mTextureData.second->Pixels[row * mTextureData.second->Pitch + col];
							}
						}
					}
				}
			}

			return Image;
		}

	private:
		FT_Library FreeTypeLibrary;
		std::map<eFontType, FreeTypeFontFace*> FontFaces;
		cbFontTextureMetaData* mTextureMetaData;
		cbFreeTypeFontFamily* Owner;
	};

	cbFreeTypeFontFamily::cbFreeTypeFontFamily(const cbFontDesc& FontDesc)
		: Desc(FontDesc)
		, FreeTypeFontManager(new FreeTypeFont(this, FontDesc))
	{
		for (const auto& Font : Desc.Fonts)
		{
			if (Font.second.pFont)
			{
				FreeTypeFontManager->LoadFont(Font.first, Font.second.pFont, (FT_Long)Font.second.pFontSize, 0);
				FreeTypeFontManager->LoadGlyphs(Font.first);
			}
			else
			{
				FreeTypeFontManager->LoadFont(Font.first, Desc.DefaultFontLocation + Font.second.FontPath);
				FreeTypeFontManager->LoadGlyphs(Font.first);
			}
		}
	}

	inline std::size_t cbFreeTypeFontFamily::GetLineDistance(const std::size_t& Size, const eFontType& FontType)
	{
		return FreeTypeFontManager->GetLineDistance(Size, FontType);
	}

	cbFreeTypeFontFamily::~cbFreeTypeFontFamily()
	{
		delete FreeTypeFontManager;
		FreeTypeFontManager = nullptr;
	}

	const std::vector<unsigned char> cbFreeTypeFontFamily::GetTexture() const
	{
		return FreeTypeFontManager->GenerateTexture(Desc.AtlasWidth, Desc.AtlasHeight, Desc.SpaceBetweenChars, Desc.CharPadding);
	}

	bool cbFreeTypeFontFamily::CreateChar(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character)
	{
		auto Result = FreeTypeFontManager->CreateChar(FontType, FontSize, character);
		if (!Result)
			return false;

		const cbFontTextureGlyphData* FontTextureGlyphData = Result->FontTextureGlyphData;

		if (FontTextureGlyphData)
		{
			if (character == 32)
				return true;
		}

		if (FontTextureGlyphData)
		{
			if (!FontTextureGlyphData->IsValid())
				return false;

			if (Desc.fFontTextureUpdate_Callback)
			{
				const cbBounds Rect = FontTextureGlyphData->GenerateTextureCoordinate();
				Desc.fFontTextureUpdate_Callback(&FontTextureGlyphData->Pixels[0], FontTextureGlyphData->Pitch, (std::size_t)Rect.Min.X, (std::size_t)Rect.Min.Y, (std::size_t)Rect.Max.X, (std::size_t)Rect.Max.Y);
				return true;
			}
		}
		return false;
	}

	bool cbFreeTypeFontFamily::IsCharExist(const eFontType& FontType, const std::size_t& Size, const char32_t& Character)
	{
		return FreeTypeFontManager->IsCharExist(FontType, Size, Character);
	}

	std::optional<cbBounds> cbFreeTypeFontFamily::GetTextureCoordinate(const eFontType& FontType, const std::size_t& Size, const char32_t& Character)
	{
		return FreeTypeFontManager->GetTextureCoordinate(FontType, Size, Character);
	}

	int cbFreeTypeFontFamily::GetAscender(const std::size_t& Size, const eFontType& Type) const
	{
		if (!FreeTypeFontManager->IsFontTypeExist(Type))
			return 0;

		if (Desc.DefaultAscender.size() > 0)
		{
			if (Desc.DefaultAscender.find(Size) != Desc.DefaultAscender.end())
				return Desc.DefaultAscender.at(Size);
		}

		const FT_Face& Face = FreeTypeFontManager->GetFace(Type);

		FT_Set_Char_Size(
			Face,                      // handle to face object
			0,                         // char_width in 1/64th of points
			FONTSIZE(Size),             // char_height in 1/64th of points
			(FT_UInt)Desc.DPI,                       // horizontal device resolution
			(FT_UInt)Desc.DPI);

		return Face->size->metrics.ascender >> 6;
	}

	int cbFreeTypeFontFamily::GetDescender(const std::size_t& Size, const eFontType& Type) const
	{
		if (!FreeTypeFontManager->IsFontTypeExist(Type))
			return 0;

		if (Desc.DefaultDescender.size() > 0)
		{
			if (Desc.DefaultDescender.find(Size) != Desc.DefaultDescender.end())
				return Desc.DefaultDescender.at(Size);
		}

		const FT_Face& Face = FreeTypeFontManager->GetFace(Type);

		FT_Set_Char_Size(
			Face,                      // handle to face object
			0,                         // char_width in 1/64th of points
			FONTSIZE(Size),             // char_height in 1/64th of points
			(FT_UInt)Desc.DPI,                       // horizontal device resolution
			(FT_UInt)Desc.DPI);

		return Face->size->metrics.descender >> 6;
	}

	const cbGlyph* cbFreeTypeFontFamily::FindOrAdd(const eFontType& FontType, const std::size_t& FontSize, const char32_t& character)
	{
		return FreeTypeFontManager->FindOrAdd(FontType, FontSize, character);
	}

	const cbGlyph* cbFreeTypeFontFamily::GetGlyph(const eFontType& Type, const std::size_t& Size, const char32_t& ch) const
	{
		return FreeTypeFontManager->GetGlyph(Type, ch, Size);
	}

	std::int32_t cbFreeTypeFontFamily::GetKerning(const int& ch1, const int& ch2, const std::size_t& Size, const eFontType& Type) const
	{
		return FreeTypeFontManager->GetKerning(ch1, ch2, Size, Type);
	}

	cbFreeTypeFontFamily* cbgui::CreateFreeTypeFont(const cbFontDesc& FontDesc)
	{
		return new cbFreeTypeFontFamily(FontDesc);
	}

	cbFontGeometry::cbFontGeometry(cbText* pOwner, cbIFontFamily* FontFamily)
		: Owner(pOwner)
		, bAutoWrapText(false)
		, CustomWrapSize(std::nullopt)
		, FontFamily(static_cast<cbFreeTypeFontFamily*>(FontFamily))
		, LineHeightPercentage(1.0f)
		, TextDesc(cbTextDesc())
	{
		TextMetrics.FontName = FontFamily->GetFontFamilyName();
	}

	cbFontGeometry::cbFontGeometry(cbText* pOwner, const std::u32string& inText, const cbTextDesc& InTextDesc, cbIFontFamily* FontFamily)
		: Owner(pOwner)
		, bAutoWrapText(false)
		, CustomWrapSize(std::nullopt)
		, FontFamily(static_cast<cbFreeTypeFontFamily*>(FontFamily))
		, LineHeightPercentage(1.0f)
		, TextDesc(InTextDesc)
	{
		TextMetrics.FontName = FontFamily->GetFontFamilyName();
		SetText(inText, TextDesc);
	}

	cbFontGeometry::cbFontGeometry(const cbFontGeometry& Other, cbText* pOwner)
		: Owner(pOwner)
		, bAutoWrapText(Other.bAutoWrapText)
		, CustomWrapSize(Other.CustomWrapSize)
		, FontFamily(Other.FontFamily)
		, LineHeightPercentage(Other.LineHeightPercentage)
		, TextDesc(Other.TextDesc)
		, Style(Other.Style)
		, TextMetrics(Other.TextMetrics)
		, DefaultSpaceWidth(Other.DefaultSpaceWidth)
		, Vertices(Other.Vertices)
		, VertexColors(Other.VertexColors)
		, TextureCoordinates(Other.TextureCoordinates)
		, CachedBounds(Other.CachedBounds)
	{}

	cbFontGeometry::~cbFontGeometry()
	{
		Owner = nullptr;
		FontFamily = nullptr;
	}

	void cbFontGeometry::SetText(const std::u32string& Text, const std::optional<cbTextDesc> Desc)
	{
		if (Desc.has_value())
		{
			TextDesc = Desc.value();

			TextMetrics.FontSize = TextDesc.CharSize;
			TextMetrics.FontType = TextDesc.FontType;
		}

		ParseCharacters(Text);
		UpdateAlignments();
		UpdateGeometry();
	}

	void cbFontGeometry::AppendText(const std::u32string& Text)
	{
		std::size_t i = 0;
		for (const auto& Char : Text)
		{
			cbTextMetrics::cbCharacter Character(Char, TextMetrics.FontSize, TextMetrics.FontType, i, !((Char != '\n') && Char >= 32));
			BuildCharacter(&Character);
			TextMetrics.Characters.push_back(Character);
			i++;
		}

		ParseCharacters(TextMetrics.GetText());
		UpdateAlignments();
		UpdateGeometry();
	}

	void cbFontGeometry::Clear()
	{
		TextMetrics.Characters.clear();
		TextMetrics.LineMetaData.clear();
		TextMetrics.WordMetaData.clear();
		UpdateAlignments();
		UpdateGeometry();
	}

	void cbFontGeometry::RebuildText()
	{
		ParseCharacters(TextMetrics.GetText());
		UpdateAlignments();
		UpdateGeometry();
	}

	void cbFontGeometry::UpdateAlignments()
	{
		UpdateHorizontalAlignment();
		UpdateVerticalAlignment();
		UpdateBounds();
	}

	void cbFontGeometry::UpdateHorizontalAlignment()
	{
		TextMetrics.WordMetaData.clear();

		std::optional<cbTextMetrics::cbCharacter> PastChar;
		float X = 0.0f;
		std::size_t WordCounter = 0;
		float TotalWidth = 0.0f;
		float WordWidth = 0.0f;

		for (std::size_t i = 0; i < TextMetrics.Characters.size(); i++)
		{
			auto& Character = TextMetrics.Characters.at(i);

			if (Character.ignore && !Character.IsNewLine())
			{
				if (i == (TextMetrics.Characters.size() - 1))
				{
					TextMetrics.WordMetaData.push_back(cbTextMetrics::cbWordMetaData(WordCounter, TotalWidth - WordWidth, WordWidth + ((TotalWidth - WordWidth) / 2.0f)));

					WordWidth = TotalWidth;
					WordCounter++;
				}
				continue;
			}

			if (PastChar.has_value())
				X += FontFamily->GetKerning(PastChar->Character, Character.Character, Character.CharacterSize, Character.Type);

			Character.WordOrder = WordCounter;
			Character.Kerning = X;
			Character.AlignedKerning = Character.Kerning;

			TotalWidth = Character.Bounds.Max.X + Character.AlignedKerning;

			X += Character.xadvance;

			PastChar = Character;

			if (Character.IsSpace() || i == (TextMetrics.Characters.size() - 1))
			{
				TextMetrics.WordMetaData.push_back(cbTextMetrics::cbWordMetaData(WordCounter, TotalWidth - WordWidth, WordWidth + ((TotalWidth - WordWidth) / 2.0f)));

				WordWidth = TotalWidth;
				WordCounter++;
			}
		}
	}

	void cbFontGeometry::UpdateVerticalAlignment()
	{
		TextMetrics.LineMetaData.clear();

		int Ascender = 0;
		int Descender = 0;
		float PrevLineDescender = 0.0f;
		float Baseline = 0.0f;
		std::size_t Linecounter = 0;
		float TotalKerning = 0.0f;
		float KerningOffset = 0.0f;
		float TotalWidth = 0.0f;
		float LineWidth = 0.0f;
		std::size_t CurrentWordIndex = 0;
		cbTextMetrics::cbCharacter PastChar;

		const float WrapWith = bAutoWrapText ? GetAutoWrapWidth() : 0.0f;

		for (std::size_t i = 0; i < TextMetrics.Characters.size(); i++)
		{
			auto& Character = TextMetrics.Characters.at(i);

			if (Character.ignore && !Character.IsNewLine())
			{
				Character.LineOrder = Linecounter;
				if (i == (TextMetrics.Characters.size() - 1))
				{
					Baseline += Linecounter != 0 ? (Ascender * LineHeightPercentage) - PrevLineDescender : Ascender - PrevLineDescender;
					TextMetrics.LineMetaData.push_back(cbTextMetrics::cbLineMetaData(Linecounter, TotalWidth - LineWidth, Ascender, Descender, Baseline, 0.0f));

					CurrentWordIndex = Character.WordOrder;
					PrevLineDescender = Descender * LineHeightPercentage;
					Ascender = 0;
					Descender = 0;

					LineWidth = TotalWidth;
					KerningOffset = TotalKerning;

					Linecounter++;
				}
				continue;
			}

			Character.AlignedKerning = Character.Kerning;
			TotalWidth = Character.Bounds.Max.X + Character.AlignedKerning;

			TotalKerning = Character.AlignedKerning;
			Character.AlignedKerning -= KerningOffset;

			Character.LineOrder = Linecounter;

			PastChar = Character;

			if (!Character.IsNewLine() || (Character.IsNewLine() && (Ascender == 0 && Descender == 0)))
			{
				int Asc = FontFamily->GetAscender(Character.CharacterSize, Character.Type);
				int Desc = FontFamily->GetDescender(Character.CharacterSize, Character.Type);
				if (TextMetrics.bIsAscenderDescenderPowerOfTwo && Asc % 2)
					Asc += 1;
				if (TextMetrics.bIsAscenderDescenderPowerOfTwo && Desc % 2)
					Desc -= 1;

				Ascender = Ascender < Asc ? Asc : Ascender;
				Descender = Descender > Desc ? Desc : Descender;
			}

			if (Character.IsNewLine() || i == (TextMetrics.Characters.size() - 1))
			{
				Baseline += Linecounter != 0 ? (Ascender * LineHeightPercentage) - PrevLineDescender : Ascender - PrevLineDescender;
				TextMetrics.LineMetaData.push_back(cbTextMetrics::cbLineMetaData(Linecounter, TotalWidth - LineWidth, Ascender, Descender, Baseline, 0.0f));

				CurrentWordIndex = Character.WordOrder;
				PrevLineDescender = Descender * LineHeightPercentage;
				Ascender = 0;
				Descender = 0;

				LineWidth = TotalWidth;
				KerningOffset = TotalKerning;

				Linecounter++;
			}
			else if ((bAutoWrapText &&
				Character.WordOrder != CurrentWordIndex && Character.WordOrder != 0 && (TextMetrics.WordMetaData.at(Character.WordOrder).GetLeft() - KerningOffset) > WrapWith))
				//((Character.Bounds.Min.X + Character.AlignedKerning) > TextMetrics.AutoWrapWidth)))
			{
				Character.AlignedKerning = 0;

				Baseline += Linecounter != 0 ? (Ascender * LineHeightPercentage) - PrevLineDescender : Ascender - PrevLineDescender;
				TextMetrics.LineMetaData.push_back(cbTextMetrics::cbLineMetaData(Linecounter, TotalWidth - LineWidth, Ascender, Descender, Baseline, 0.0f));

				CurrentWordIndex = Character.WordOrder;
				PrevLineDescender = Descender * LineHeightPercentage;
				Ascender = 0;
				Descender = 0;

				LineWidth = TotalWidth;
				KerningOffset = TotalKerning;

				Linecounter++;
				Character.LineOrder = Linecounter;
			}
		}

		if (PastChar.IsNewLine())
		{
			int Asc = FontFamily->GetAscender(TextMetrics.FontSize, TextMetrics.FontType);
			int Desc = FontFamily->GetDescender(TextMetrics.FontSize, TextMetrics.FontType);
			if (TextMetrics.bIsAscenderDescenderPowerOfTwo && Asc % 2)
				Asc += 1;
			if (TextMetrics.bIsAscenderDescenderPowerOfTwo && Desc % 2)
				Desc -= 1;

			Baseline += Linecounter != 0 ? (Asc * LineHeightPercentage) - PrevLineDescender : Asc - PrevLineDescender;
			TextMetrics.LineMetaData.push_back(cbTextMetrics::cbLineMetaData(Linecounter, TotalWidth - LineWidth, Asc, Desc, Baseline, 0.0f));
		}
	}

	void cbFontGeometry::ParseCharacters(const std::u32string& pText)
	{
		std::vector<std::size_t> CharsToSkip;
		std::map<std::size_t, std::size_t> CharStyles;

		if (TextMetrics.bIsTextStylingEnabled)
		{
			std::size_t StyleTagStart = 0;
			std::size_t StyleTagEnd = 0;
			std::size_t StyleIndex = 0;

			while (true)
			{
				if (TextMetrics.TextStyles.size() == 0)
					break;

				StyleTagStart = pText.find(TextMetrics.TextStyles[StyleIndex].GetStyleStartTag(), StyleTagStart);
				StyleTagEnd = pText.find(TextMetrics.TextStyles[StyleIndex].GetStyleEndTag(), StyleTagStart);

				if ((StyleTagStart == std::wstring::npos) || (StyleTagEnd == std::wstring::npos))
				{
					StyleTagStart = 0;
					StyleTagEnd = 0;

					StyleIndex++;
					if (StyleIndex >= TextMetrics.TextStyles.size())
						break;

					continue;
				}

				StyleTagStart += TextMetrics.TextStyles[StyleIndex].GetStyleStartTag().size();

				std::u32string substring(pText.substr(StyleTagStart, StyleTagEnd - StyleTagStart));

				for (std::size_t i = StyleTagStart - TextMetrics.TextStyles[StyleIndex].GetStyleStartTag().size(); i < StyleTagStart; i++)
				{
					CharsToSkip.push_back(i);
				}

				for (std::size_t i = StyleTagEnd; i < StyleTagEnd + TextMetrics.TextStyles[StyleIndex].GetStyleEndTag().size(); i++)
				{
					CharsToSkip.push_back(i);
				}

				for (std::size_t i = StyleTagStart; i < StyleTagEnd; i++)
				{
					CharStyles.insert({ i, StyleIndex });
				}
			}
		}

		TextMetrics.Characters.clear();
		std::size_t i = 0;
		for (const auto& Char : pText)
		{
			cbTextMetrics::cbCharacter Character(Char, TextMetrics.FontSize, TextMetrics.FontType, i, !((Char != '\n') && Char >= 32));
			if (CharStyles.find(i) != CharStyles.end())
			{
				const auto& TextStyle = TextMetrics.GetTextStyle(CharStyles[i]);
				Character.CharacterSize = TextStyle->FontSize;
				Character.Type = TextStyle->FontType;
				if (TextStyle->Color.has_value())
					Character.Color = TextStyle->Color.value();
			}
			if (std::find(CharsToSkip.begin(), CharsToSkip.end(), i) != CharsToSkip.end())
				Character.ignore = true;

			BuildCharacter(&Character);
			TextMetrics.Characters.push_back(Character);
			i++;
		}
	}

	void cbFontGeometry::BuildText()
	{
		for (auto& Character : TextMetrics.Characters)
		{
			BuildCharacter(&Character);
		}
	}

	const cbGlyph* cbFontGeometry::GetGlyph(const eFontType& Type, const std::size_t& Size, const char32_t& ch) const
	{
		return FontFamily->GetGlyph(Type, Size, ch);
	}

	bool cbFontGeometry::BuildCharacter(cbTextMetrics::cbCharacter* Character)
	{
		if (!Character->IsValid())
			return false;

		if (!FontFamily->IsCharExist(Character->Type, Character->CharacterSize, Character->Character))
			if (!FontFamily->CreateChar(Character->Type, Character->CharacterSize, Character->Character))
				return false;

		if (const cbGlyph* Glyph = GetGlyph(Character->Type, Character->CharacterSize, Character->Character))
		{
			const float X = 0.0f;
			const float Y = 0.0f;

			cbBounds CharRect(cbVector(X + Glyph->BearingX, Y - Glyph->BearingY), cbVector(X + Glyph->BearingX + Glyph->Width, Y + Glyph->Height - Glyph->BearingY));

			const int xadvance = DefaultSpaceWidth.has_value() && (Character->Character == 32) ? (int)DefaultSpaceWidth.value() : Glyph->xadvance;

			if (CharRect.Min.X == CharRect.Max.X)
				CharRect.Max.X += xadvance;

			if (CharRect.Min.Y == CharRect.Max.Y)
				CharRect.Min.Y -= 1.0f;

			CharRect.Min.Y -= FontFamily->GetDesc().CharPadding.Top;
			CharRect.Min.X -= FontFamily->GetDesc().CharPadding.Left;
			CharRect.Max.Y += FontFamily->GetDesc().CharPadding.Bottom;
			CharRect.Max.X += FontFamily->GetDesc().CharPadding.Right;

			Character->Bounds = CharRect;

			Character->Ascender = FontFamily->GetAscender(Character->CharacterSize, Character->Type);
			Character->Descender = FontFamily->GetDescender(Character->CharacterSize, Character->Type);

			Character->xadvance = xadvance;
			Character->BearingX = Glyph->BearingX;

			{
				const cbBounds& TextureCoordinate = Glyph->FontTextureGlyphData->GenerateTextureCoordinate();

				const cbMargin& Padding = FontFamily->GetDesc().CharPadding;

				const unsigned int mip = 0;
				int32_t mipWidth = std::max(1U, (unsigned int)(FontFamily->GetDesc().AtlasWidth >> mip));
				uint32_t mipHeight = std::max(1U, (unsigned int)(FontFamily->GetDesc().AtlasHeight >> mip));

				const float invWidth = 1.0f / mipWidth;
				const float invHeight = 1.0f / mipHeight;

				Character->TextureCoordinates = cbBounds(cbVector((TextureCoordinate.Min.X - Padding.Left) * invWidth, (TextureCoordinate.Min.Y - Padding.Top) * invHeight),
														 cbVector((TextureCoordinate.Max.X - Padding.Right) * invWidth, (TextureCoordinate.Max.Y - Padding.Bottom) * invHeight));
			}
		}

		return true;
	}

	void cbFontGeometry::UpdateVertexColors()
	{
		std::size_t i = 0;
		for (const auto& Char : TextMetrics.Characters)
		{
			if (!Char.IsValid() || Char.IsSpace())
				continue;

			if (Char.LineOrder >= TextMetrics.LineMetaData.size())
				continue;

			if (Char.Color.has_value())
			{
				const cbColor& Color = Char.Color.value();
				{
					if (VertexColors.size() > i)
					{
						VertexColors[i] = Color;
						VertexColors[i + 1] = Color;
						VertexColors[i + 2] = Color;
						VertexColors[i + 3] = Color;
						i += 4;
					}
					else
					{
						VertexColors.push_back(Color);
						VertexColors.push_back(Color);
						VertexColors.push_back(Color);
						VertexColors.push_back(Color);
						i += 4;
					}
				}
			}
			else
			{
				if (VertexColors.size() > i)
				{
					VertexColors[i] = Style.GetColor();
					VertexColors[i + 1] = Style.GetColor();
					VertexColors[i + 2] = Style.GetColor();
					VertexColors[i + 3] = Style.GetColor();
					i += 4;
				}
				else
				{
					VertexColors.push_back(Style.GetColor());
					VertexColors.push_back(Style.GetColor());
					VertexColors.push_back(Style.GetColor());
					VertexColors.push_back(Style.GetColor());
					i += 4;
				}
			}
		}
	}

	void cbFontGeometry::UpdateTextureCoordinates()
	{
		TextureCoordinates.clear();
		for (const auto& Character : TextMetrics.Characters)
		{
			if (Character.IsValid() && !Character.IsSpace())
			{
				if (Character.LineOrder >= TextMetrics.LineMetaData.size())
					continue;

				const cbBounds& Char = Character.TextureCoordinates;
				TextureCoordinates.push_back(cbVector(Char.Min.X, Char.Min.Y));
				TextureCoordinates.push_back(cbVector(Char.Max.X, Char.Min.Y));
				TextureCoordinates.push_back(cbVector(Char.Max.X, Char.Max.Y));
				TextureCoordinates.push_back(cbVector(Char.Min.X, Char.Max.Y));
			}
		}
	}

	void cbFontGeometry::UpdateVertices()
	{
		Vertices.clear();
		for (const auto& Character : TextMetrics.Characters)
		{
			if (!Character.IsValid() || Character.IsSpace())
				continue;

			if (Character.LineOrder >= TextMetrics.LineMetaData.size())
				continue;

			const auto& Baseline = TextMetrics.LineMetaData.at(Character.LineOrder).Baseline;
			const auto& Bounds = Character.Bounds;
			Vertices.push_back(cbVector4(Bounds.Min.X + Character.AlignedKerning, Bounds.Min.Y + Baseline, 0.0f));
			Vertices.push_back(cbVector4(Bounds.Max.X + Character.AlignedKerning, Bounds.Min.Y + Baseline, 0.0f));
			Vertices.push_back(cbVector4(Bounds.Max.X + Character.AlignedKerning, Bounds.Max.Y + Baseline, 0.0f));
			Vertices.push_back(cbVector4(Bounds.Min.X + Character.AlignedKerning, Bounds.Max.Y + Baseline, 0.0f));
		}
	}

	std::vector<std::uint32_t> cbFontGeometry::GenerateIndices() const
	{
		return cbGeometryFactory::GeneratePlaneIndices(TextDesc.bEnableAutoGrowVerticesSize ? std::uint32_t((GetTextSize(true)) + TextDesc.AutoGrowVerticesSize) : std::uint32_t(GetTextSize(true)));
	}

	void cbFontGeometry::UpdateGeometry()
	{
		UpdateVertices();
		UpdateTextureCoordinates();
		UpdateVertexColors();

		GrowTextGeometryData();
	}

	void cbFontGeometry::UpdateBounds()
	{
		cbBounds bbox;
		for (const auto& Line : TextMetrics.LineMetaData)
		{
			const cbBounds Bounds = Line.GetBounds();
			{
				if (Bounds.Min.X < bbox.Min.X)
					bbox.Min.X = Bounds.Min.X;

				if (Bounds.Max.X > bbox.Max.X)
					bbox.Max.X = Bounds.Max.X;
			}

			// Ascender - Descender is maximum Height
			bbox.Max.Y += Line.GetHeight();
		}

		CachedBounds = bbox;

		if (CachedBounds.GetHeight() == 0.0f)
			CachedBounds.SetHeight((float)FontFamily->GetAscender(TextMetrics.FontSize, TextMetrics.FontType) - FontFamily->GetDescender(TextMetrics.FontSize, TextMetrics.FontType));

		if (CachedBounds.GetWidth() == 0.0f)
			CachedBounds.SetWidth(2.0f);
	}

	std::vector<cbBounds> cbFontGeometry::GetLineBounds() const
	{
		std::vector<cbBounds> Bounds;
		for (const auto& Line : TextMetrics.LineMetaData)
		{
			Bounds.push_back(Line.GetBounds());
		}
		return Bounds;
	}

	void cbFontGeometry::SetAutoGrowVerticesSize(const bool value, const std::size_t Size)
	{
		const bool oldValue = TextDesc.bEnableAutoGrowVerticesSize;
		TextDesc.bEnableAutoGrowVerticesSize = value;
		TextDesc.AutoGrowVerticesSize = Size;

		if (oldValue && !TextDesc.bEnableAutoGrowVerticesSize)
		{
			std::size_t TextSize = GetTextSize();
			Vertices.erase(Vertices.begin() + (TextSize * 4), Vertices.end());
			TextureCoordinates.erase(TextureCoordinates.begin() + (TextSize * 4), TextureCoordinates.end());
		}
		else
		{
			GrowTextGeometryData();
		}
	}

	void cbFontGeometry::GrowTextGeometryData()
	{
		if (TextDesc.bEnableAutoGrowVerticesSize)
		{
			if (Vertices.size() < (TextDesc.AutoGrowVerticesSize * 4))
			{
				for (std::size_t i = Vertices.size(); i < (TextDesc.AutoGrowVerticesSize * 4); i++)
					Vertices.push_back(cbVector4());
				for (std::size_t i = TextureCoordinates.size(); i < (TextDesc.AutoGrowVerticesSize * 4); i++)
					TextureCoordinates.push_back(cbVector());
			}
			else if (Vertices.size() > (TextDesc.AutoGrowVerticesSize * 4))
			{
				TextDesc.AutoGrowVerticesSize = Vertices.size() + (TextDesc.AutoGrowVerticesSize * 4);
				GrowTextGeometryData();
			}
		}

		if (TextDesc.bEnableAutoGrowVerticesSize)
		{
			std::size_t TextSize = GetTextSize();
			if (Vertices.size() > ((TextSize * 4) + (TextDesc.AutoGrowVerticesSize * 4)))
				Vertices.erase(Vertices.begin() + ((TextSize * 4) + (TextDesc.AutoGrowVerticesSize * 4)), Vertices.end());

			if (TextureCoordinates.size() > ((TextSize * 4) + (TextDesc.AutoGrowVerticesSize * 4)))
				TextureCoordinates.erase(TextureCoordinates.begin() + ((TextSize * 4) + (TextDesc.AutoGrowVerticesSize * 4)), TextureCoordinates.end());
		}
		else
		{
			std::size_t TextSize = GetTextSize();
			if (Vertices.size() > (TextSize * 4))
				Vertices.erase(Vertices.begin() + (TextSize * 4), Vertices.end());

			if (TextureCoordinates.size() > (TextSize * 4))
				TextureCoordinates.erase(TextureCoordinates.begin() + (TextSize * 4), TextureCoordinates.end());
		}
	}

	bool cbFontGeometry::IsAutoWrapTextEnabled() const
	{
		return bAutoWrapText;
	}

	float cbFontGeometry::GetAutoWrapWidth() const
	{
		return CustomWrapSize.has_value() ? CustomWrapSize.value() : Owner->GetWidth();
	}

	void cbFontGeometry::SetTextJustify(eTextJustify Justify)
	{
		TextDesc.TextJustify = Justify;
	}

	void cbFontGeometry::SetDefaultSpaceWidth(const std::optional<std::size_t> Width)
	{
		if (!Width.has_value())
		{
			DefaultSpaceWidth = std::nullopt; 
			RebuildText();
		}
		else if (DefaultSpaceWidth.has_value())
		{
			if (DefaultSpaceWidth.value() != Width.value())
			{
				DefaultSpaceWidth = Width.value();
				RebuildText();
			}
		}
		else
		{
			DefaultSpaceWidth = Width.value();
			RebuildText();
		}
	}

	void cbFontGeometry::SetLineHeightPercentage(const float Percentage)
	{
		if (LineHeightPercentage != Percentage)
		{
			LineHeightPercentage = Percentage;
			UpdateAlignments();
			UpdateGeometry();
		}		
	}

	void cbFontGeometry::SetAutoWrapText(const bool Value)
	{
		if (bAutoWrapText != Value)
		{
			bAutoWrapText = Value;
			UpdateAlignments();
			UpdateGeometry();
		}
	}

	void cbFontGeometry::SetCustomWrapWidth(const std::optional<float> Width)
	{
		if (!Width.has_value())
		{
			CustomWrapSize = std::nullopt;
			if (IsAutoWrapTextEnabled())
			{
				UpdateAlignments();
				UpdateGeometry();
			}
		}
		else if (CustomWrapSize.has_value())
		{
			if (CustomWrapSize.value() != Width.value())
			{
				CustomWrapSize = Width.value();
				if (IsAutoWrapTextEnabled())
				{
					UpdateAlignments();
					UpdateGeometry();
				}
			}
		}
		else
		{
			CustomWrapSize = Width.value();
			if (IsAutoWrapTextEnabled())
			{
				UpdateAlignments();
				UpdateGeometry();
			}
		}
	}

	void cbFontGeometry::SetAscenderDescenderPowerOfTwo(const bool Value)
	{
		if (TextMetrics.bIsAscenderDescenderPowerOfTwo != Value)
		{
			TextMetrics.bIsAscenderDescenderPowerOfTwo = Value;
			UpdateAlignments();
			UpdateGeometry();
		}
	}

	void cbFontGeometry::SetFontType(const eFontType& Type)
	{
		if (TextMetrics.FontType != Type)
		{
			TextMetrics.FontType = Type;
			RebuildText();
		}
	}

	void cbFontGeometry::SetFontSize(const std::size_t& Size)
	{
		if (TextMetrics.FontSize != Size)
		{
			TextMetrics.FontSize = Size;
			RebuildText();
		}
	}

	void cbFontGeometry::SetTextStylingEnabled(const bool Value)
	{
		if (TextMetrics.bIsTextStylingEnabled != Value)
		{
			TextMetrics.bIsTextStylingEnabled = Value;
			RebuildText();
		}
	}

	void cbFontGeometry::AddTextStyle(const cbTextStyle& Style)
	{
		TextMetrics.TextStyles.push_back(Style);
		if (TextMetrics.bIsTextStylingEnabled)
			RebuildText();
	}

	void cbFontGeometry::RemoveTextStyle(const std::size_t& Index)
	{
		TextMetrics.TextStyles.erase(TextMetrics.TextStyles.begin() + Index);
		if (TextMetrics.bIsTextStylingEnabled)
			RebuildText();
	}

	void cbFontGeometry::SetFontFamily(cbIFontFamily* inFontFamily)
	{
		FontFamily = static_cast<cbFreeTypeFontFamily*>(inFontFamily);

		RebuildText();
	}

	//void cbFontGeometry::SetFontVertexSize(const float& pFontSize)
	//{
	//    //std::size_t FontSize = 24.0f;
	//
	//    //for (auto& Char : Characters)
	//    //{
	//    //    Char.Bounds.Min = Char.Bounds.Min * (pFontSize / FontSize);
	//    //    Char.Bounds.Max = Char.Bounds.Max * (pFontSize / FontSize);
	//    //    //Char.CharacterSize = pFontSize;
	//    //}
	//
	//    //for (auto& Char : Characters)
	//    //{
	//    //    Char.Bounds.Min = Char.Bounds.Min * (FontSize / pFontSize);
	//    //    Char.Bounds.Max = Char.Bounds.Max * (FontSize / pFontSize);
	//    //    //Char.CharacterSize = pFontSize;
	//    //}
	//
	//    //UpdateVertices();
	//}

	bool cbFontGeometry::GetLineData(std::size_t Index, cbBounds& outLineBounds, std::vector<cbCharacterData>& outCharacterData) const
	{
		if (Index >= TextMetrics.LineMetaData.size())
			return false;

		const cbTextMetrics::cbLineMetaData& Line = TextMetrics.LineMetaData.at(Index);
		outLineBounds = Line.GetBounds();

		for (const auto& Character : TextMetrics.Characters)
		{
			if (Character.LineOrder != Index)
				continue;

			outCharacterData.push_back(cbCharacterData(Character, Line.Baseline));
		}

		return true;
	}

	std::optional<cbBounds> cbFontGeometry::GetLineBounds(std::size_t Index) const
	{
		if (Index >= TextMetrics.LineMetaData.size())
			return std::nullopt;

		const cbTextMetrics::cbLineMetaData& Line = TextMetrics.LineMetaData.at(Index);

		return Line.GetBounds();
	}

	std::optional<cbCharacterData> cbFontGeometry::GetCharacterData(std::size_t Index) const
	{
		if (Index > TextMetrics.Characters.size() || TextMetrics.Characters.size() == 0)
			return std::nullopt;

		const cbTextMetrics::cbCharacter& Character = Index == TextMetrics.Characters.size() ? TextMetrics.Characters.back() : TextMetrics.Characters.at(Index);
		if (Character.LineOrder >= TextMetrics.LineMetaData.size())
			return std::nullopt;
		const cbTextMetrics::cbLineMetaData& Line = TextMetrics.LineMetaData.at(Character.LineOrder);

		return cbCharacterData(Character, Line.Baseline);
	}

	void cbFontGeometry::AddChar(const char32_t pChar, const std::optional<std::size_t> inIndex, bool Filtered)
	{
		cbTextMetrics::cbCharacter Character(pChar, TextMetrics.FontSize, TextMetrics.FontType, 0, !((pChar != '\n') && pChar >= 32));

		if (!TextMetrics.bIsTextStylingEnabled)
			BuildCharacter(&Character);

		if (inIndex.has_value())
			TextMetrics.Characters.insert(TextMetrics.Characters.begin() + inIndex.value(), Character);
		else
			TextMetrics.Characters.push_back(Character);

		if (TextMetrics.bIsTextStylingEnabled)
		{
			RebuildText();
		}
		else
		{
			std::size_t i = 0;
			for (auto& Char : TextMetrics.Characters)
			{
				Char.CharacterOrder = i;
				i++;
			}

			UpdateAlignments();
			UpdateGeometry();
		}
	}

	bool cbFontGeometry::RemoveChar(const std::size_t& Index)
	{
		if (Index >= TextMetrics.Characters.size())
			return false;

		TextMetrics.Characters.erase(TextMetrics.Characters.begin() + Index);

		if (TextMetrics.bIsTextStylingEnabled)
		{
			RebuildText();
		}
		else
		{
			std::size_t i = 0;
			for (auto& Char : TextMetrics.Characters)
			{
				Char.CharacterOrder = i;
				i++;
			}

			UpdateAlignments();
			UpdateGeometry();
		}

		return true;
	}

	bool cbFontGeometry::RemoveChars(const std::vector<std::size_t>& inIndices)
	{
		std::vector<std::size_t> Indices = inIndices;

		std::sort(Indices.begin(), Indices.end(), [](std::size_t& Slot1, std::size_t& Slot2)
			{
				return (Slot1 > Slot2);
			});

		for (const auto& Index : Indices)
		{
			TextMetrics.Characters.erase(TextMetrics.Characters.begin() + Index);
		}

		if (TextMetrics.bIsTextStylingEnabled)
		{
			RebuildText();
		}
		else
		{
			std::size_t i = 0;
			for (auto& Char : TextMetrics.Characters)
			{
				Char.CharacterOrder = i;
				i++;
			}

			UpdateAlignments();
			UpdateGeometry();
		}

		return true;
	}

	const std::u32string cbFontGeometry::GetText(bool Filtered) const
	{
		return Filtered ? TextMetrics.GetFilteredText() : TextMetrics.GetText();
	}

	const std::size_t cbFontGeometry::GetTextSize(bool Filtered) const
	{
		if (Filtered)
		{
			return TextMetrics.GetFilteredSize();
		}

		return TextMetrics.Characters.size();
	}
}
