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
#include "cbText.h"
#include <locale>
#include "cbFont.h"
#include "cbCanvas.h"
#include "cbScrollBox.h"

namespace cbgui
{
	cbText::cbText(const std::u32string& inText, const cbTextDesc& TextDesc, cbIFontFamily* FontFamily)
		: cbWidget()
		, Transform(cbTransform(cbDimension(100.0f, 40.0f)))
		, LetterDrawCount(std::nullopt)
		, FontGeometryBuilder(cbFontGeometry::CreateUnique(this, inText, TextDesc, FontFamily))
		, bIsItWrapped(false)
	{
		SetFocusable(false);
	}

	cbText::cbText(const cbText& Other, cbSlot* NewOwner)
		: cbWidget(Other, NewOwner)
		, Transform(Other.Transform)
		, LetterDrawCount(Other.LetterDrawCount)
		, FontGeometryBuilder(Other.FontGeometryBuilder->CloneUnique(this))
		, bIsItWrapped(Other.bIsItWrapped)
	{
		SetFocusable(Other.IsFocusable());
	}

	cbText::~cbText()
	{
		FontGeometryBuilder = nullptr;
	}

	cbWidget::SharedPtr cbText::CloneWidget(cbSlot* NewOwner)
	{
		return cbText::Create(*this);
	}

	void cbText::BeginPlay()
	{
		OnBeginPlay();
	}

	void cbText::Tick(float DeltaTime)
	{
		if (!IsEnabled())
			return;

		OnTick(DeltaTime);
	}

	float cbText::GetRotation() const
	{
		return HasOwner() ? GetOwner()->GetRotation() + Transform.GetRotation() : Transform.GetRotation();
	}

	const cbIFontFamily* cbText::GetFontFamily() const
	{
		return FontGeometryBuilder->GetFontFamily();
	}

	void cbText::SetFontFamily(cbIFontFamily* FontFamily)
	{
		FontGeometryBuilder->SetFontFamily(FontFamily);
	}

	void cbText::SetXY(std::optional<float> X, std::optional<float> Y, bool Force)
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
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbText::SetWidthHeight(std::optional<float> Width, std::optional<float> Height)
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
					if (IsAutoWrapEnabled())
						AutoWrap();
					else
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
					if (IsAutoWrapEnabled())
						AutoWrap();
					else
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
					NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbText::SetPadding(const cbMargin& Padding)
	{
		if (Transform.SetPadding(Padding))
		{
			if (HasOwner())
				DimensionUpdated();
			else if (IsAlignedToCanvas())
				AlignToCanvas();
		}
	}

	void cbText::SetRotation(const float Rotation)
	{
		if (Transform.Rotate2D(Rotation))
			NotifyCanvas_WidgetUpdated();
	}

	const std::u32string cbText::GetText(bool Filtered) const
	{
		return FontGeometryBuilder->GetText(Filtered);
	}

	void cbText::SetText(const std::u32string& inText, const std::optional<cbTextDesc> Desc)
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->SetText(inText, Desc);
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();
	}

	void cbText::AppendText(const std::u32string& inText)
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->AppendText(inText);
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();
	}

	void cbText::SetAutoGrowVerticesSize(const bool value, const std::size_t Size)
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->SetAutoGrowVerticesSize(value, Size);
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();
	}

	void cbText::AddChar(const char32_t pChar, const std::optional<std::size_t> Index, bool Filtered)
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->AddChar(pChar, Index, Filtered);
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();
	}

	void cbText::AddChar(const char32_t* pChar, const std::optional<std::size_t> Index, bool Filtered)
	{
		AddChar(*pChar, Index, Filtered);
	}

	bool cbText::RemoveChar(std::size_t Index)
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->RemoveChar(Index);
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();

		return true;
	}

	bool cbText::RemoveChars(const std::vector<std::size_t>& Indices)
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->RemoveChars(Indices);
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();

		return true;
	}

	bool cbText::RemoveLastChar()
	{
		return GetTextSize() > 0 ? RemoveChar(GetTextSize() - 1) : false;
	}

	void cbText::SetFontType(const eFontType& FontType)
	{
		FontGeometryBuilder->SetFontType(FontType);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::SetFontSize(const std::size_t& Size)
	{
		FontGeometryBuilder->SetFontSize(Size <= 0 ? 1 : Size);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	std::vector<cbTextStyle> cbText::GetTextStyles() const
	{
		return FontGeometryBuilder->GetTextStyles();
	}

	void cbText::AddTextStyle(const cbTextStyle& Style)
	{
		FontGeometryBuilder->AddTextStyle(Style);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::RemoveTextStyle(const std::size_t& Index)
	{
		FontGeometryBuilder->RemoveTextStyle(Index);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::SetTextStylingEnabled(const bool Value)
	{
		FontGeometryBuilder->SetTextStylingEnabled(Value);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::SetLineHeightPercentage(const float Percentage)
	{
		FontGeometryBuilder->SetLineHeightPercentage(Percentage);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::SetAutoWrap(const bool Value)
	{
		if (Value)
			Transform.ResetWidthCompressed();

		FontGeometryBuilder->SetAutoWrapText(Value);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	bool cbText::IsAutoWrapEnabled() const
	{
		return FontGeometryBuilder->IsAutoWrapTextEnabled();
	}

	void cbText::SetCustomWrapWidth(const std::optional<float> Width)
	{
		if (!IsAutoWrapEnabled())
			return;

		FontGeometryBuilder->SetCustomWrapWidth(Width);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	float cbText::GetAutoWrapWidth() const
	{
		return FontGeometryBuilder->GetAutoWrapWidth();
	}

	void cbgui::cbText::AutoWrap()
	{
		if (!IsAutoWrapEnabled())
			return;
		if (GetTextSize(true) == 0)
			return;

		FontGeometryBuilder->UpdateVerticalAlignment();
		FontGeometryBuilder->UpdateBounds();
		FontGeometryBuilder->UpdateGeometry();

		if (IsItWrapped())
		{
			const auto GeometryBounds = FontGeometryBuilder->GetGeometryBounds();

			if (Transform.CompressHeight(GeometryBounds.GetHeight()))
			{
				UpdateVerticalAlignment(true);

				if (HasOwner())
					DimensionUpdated();
			}
		}
		else
		{
			UpdateVerticalAlignment(true);
		}
	}

	void cbText::SetAscenderDescenderPowerOfTwo(const bool Value)
	{
		FontGeometryBuilder->SetAscenderDescenderPowerOfTwo(Value);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::SetDefaultSpaceWidth(const std::optional<std::size_t> Width)
	{
		FontGeometryBuilder->SetDefaultSpaceWidth(Width);

		NotifyCanvas_WidgetUpdated();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();
	}

	void cbText::Clear()
	{
		const std::size_t OldVerticesSize = FontGeometryBuilder->GetVerticesSize();
		FontGeometryBuilder->Clear();
		const std::size_t NewVerticesSize = FontGeometryBuilder->GetVerticesSize();

		if (IsItWrapped())
			Wrap();
		else
			UpdateAlignments();

		if (OldVerticesSize != NewVerticesSize)
			GeometryVerticesSizeChanged(NewVerticesSize);
		else
			NotifyCanvas_WidgetUpdated();
	}

	bool cbText::GetLineData(std::size_t Index, cbBounds& outLineBounds, std::vector<cbCharacterData>& outCharacterData, bool useTextWidth) const
	{
		auto GetVertices = [](cbBounds Bounds) -> std::vector<cbVector4>
		{
			std::vector<cbVector4> VertexData;
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Min.X;
				Verticle.Y = Bounds.Min.Y;
				VertexData.push_back(Verticle);
			}
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Max.X;
				Verticle.Y = Bounds.Min.Y;
				VertexData.push_back(Verticle);
			}
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Max.X;
				Verticle.Y = Bounds.Max.Y;
				VertexData.push_back(Verticle);
			}
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Min.X;
				Verticle.Y = Bounds.Max.Y;
				VertexData.push_back(Verticle);
			}

			return VertexData;
		};

		if (!FontGeometryBuilder->GetLineData(Index, outLineBounds, outCharacterData))
			return false;

		if (useTextWidth)
			outLineBounds.SetWidth(GetWidth());
		outLineBounds.SetPosition(cbVector(Transform.GetLeft() + (outLineBounds.GetWidth() / 2.0f), Transform.GetTop() + (outLineBounds.GetCenter().Y)));

		const cbBounds& GeometryBounds = FontGeometryBuilder->GetGeometryBounds();
		const cbVector& GeometryCenter = GeometryBounds.GetCenter();
		const cbVector& Location = Transform.GetCenter();

		const bool bForceLeft = GeometryBounds.GetWidth() > Transform.GetWidth();

		const eTextJustify TextJustify = FontGeometryBuilder->GetTextJustify();

		const float Top = Transform.GetTop() + (GeometryBounds.GetHeight() / 2.0f);
		const float Left = TextJustify == eTextJustify::Left || bForceLeft ? Transform.GetLeft() + (GeometryBounds.GetWidth() / 2.0f) : 0.0f;
		const float Right = TextJustify == eTextJustify::Right || !bForceLeft ? Transform.GetRight() - (GeometryBounds.GetWidth() / 2.0f) : 0.0f;

		const float& Rotation(GetRotation());
		const cbVector& Origin = Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero();

		for (std::size_t i = 0; i < outCharacterData.size(); i++)
		{
			std::vector<cbVector4> vertices = GetVertices(outCharacterData.at(i).Bounds);
			for (auto& position : vertices)
			{
				if (bForceLeft)
				{
					position += Location - GeometryCenter;
					position.Y += Top - Location.Y;
					position.X += Left - Location.X;
				}
				else
				{
					switch (TextJustify)
					{
					case eTextJustify::Left:
						position += Location - GeometryCenter;
						position.Y += Top - Location.Y;
						position.X += Left - Location.X;
						break;
					case eTextJustify::Center:
						position += Location - GeometryCenter;
						position.Y += Top - Location.Y;
						break;
					case eTextJustify::Right:
						position += Location - GeometryCenter;
						position.Y += Top - Location.Y;
						position.X += Right - Location.X;
						break;
					}
				}

				if (Rotation != 0.0f)
					position = cbgui::RotateVectorAroundPoint(position, Origin, Rotation);
			}
			outCharacterData.at(i).Bounds = cbBounds(cbVector(vertices.at(0).X, vertices.at(0).Y), cbVector(vertices.at(2).X, vertices.at(2).Y));
		}

		return true;
	}

	std::optional<cbBounds> cbText::GetLineBounds(std::size_t Index, bool useTextWidth) const
	{
		if (Index > GetLineSize())
			return std::nullopt;

		std::optional<cbBounds> Result = FontGeometryBuilder->GetLineBounds(Index);
		if (!Result.has_value())
			return std::nullopt;

		cbBounds LineBounds = Result.value();
		if (useTextWidth)
			LineBounds.SetWidth(GetWidth());
		LineBounds.SetPosition(cbVector(Transform.GetLeft() + (LineBounds.GetWidth() / 2.0f), Transform.GetTop() + (LineBounds.GetCenter().Y)));

		return LineBounds;
	}

	std::optional<cbCharacterData> cbText::GetCharacterData(std::size_t Index) const
	{
		auto GetVertices = [](cbBounds Bounds) -> std::vector<cbVector4>
		{
			std::vector<cbVector4> VertexData;
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Min.X;
				Verticle.Y = Bounds.Min.Y;
				VertexData.push_back(Verticle);
			}
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Max.X;
				Verticle.Y = Bounds.Min.Y;
				VertexData.push_back(Verticle);
			}
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Max.X;
				Verticle.Y = Bounds.Max.Y;
				VertexData.push_back(Verticle);
			}
			{
				cbVector4 Verticle;
				Verticle.X = Bounds.Min.X;
				Verticle.Y = Bounds.Max.Y;
				VertexData.push_back(Verticle);
			}

			return VertexData;
		};

		std::optional<cbCharacterData> outCharacterData = FontGeometryBuilder->GetCharacterData(Index);
		if (!outCharacterData.has_value())
			return std::nullopt;

		const cbBounds& GeometryBounds = FontGeometryBuilder->GetGeometryBounds();
		const cbVector& GeometryCenter = GeometryBounds.GetCenter();
		const cbVector& Location = Transform.GetCenter();

		const bool bForceLeft = GeometryBounds.GetWidth() > Transform.GetWidth();

		const eTextJustify TextJustify = FontGeometryBuilder->GetTextJustify();

		const float Top = Transform.GetTop() + (GeometryBounds.GetHeight() / 2.0f);
		const float Left = TextJustify == eTextJustify::Left || bForceLeft ? Transform.GetLeft() + (GeometryBounds.GetWidth() / 2.0f) : 0.0f;
		const float Right = TextJustify == eTextJustify::Right || !bForceLeft ? Transform.GetRight() - (GeometryBounds.GetWidth() / 2.0f) : 0.0f;

		const float& Rotation(GetRotation());
		const cbVector& Origin = Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero();

		{
			std::vector<cbVector4> vertices = GetVertices(outCharacterData.value().Bounds);
			for (auto& position : vertices)
			{
				if (bForceLeft)
				{
					position += Location - GeometryCenter;
					position.Y += Top - Location.Y;
					position.X += Left - Location.X;
				}
				else
				{
					switch (TextJustify)
					{
					case eTextJustify::Left:
						position += Location - GeometryCenter;
						position.Y += Top - Location.Y;
						position.X += Left - Location.X;
						break;
					case eTextJustify::Center:
						position += Location - GeometryCenter;
						position.Y += Top - Location.Y;
						break;
					case eTextJustify::Right:
						position += Location - GeometryCenter;
						position.Y += Top - Location.Y;
						position.X += Right - Location.X;
						break;
					}
				}

				if (Rotation != 0.0f)
					position = cbgui::RotateVectorAroundPoint(position, Origin, Rotation);
			}
			outCharacterData.value().Bounds = cbBounds(cbVector(vertices.at(0).X, vertices.at(0).Y), cbVector(vertices.at(2).X, vertices.at(2).Y));
		}

		return outCharacterData;
	}

	eTextJustify cbText::GetTextJustify() const
	{
		return FontGeometryBuilder->GetTextJustify();
	}

	void cbText::SetTextJustify(eTextJustify Justify)
	{
		FontGeometryBuilder->SetTextJustify(Justify);
		NotifyCanvas_WidgetUpdated();
	}

	std::size_t cbText::GetCharacterCount(const char32_t ch) const
	{
		const auto& Text = GetText();
		return std::count(Text.begin(), Text.end(), ch);
	}

	std::size_t cbText::GetTextSize(bool Filtered) const
	{
		return FontGeometryBuilder->GetTextSize(Filtered);
	}

	std::size_t cbText::GetLineSize() const
	{
		return FontGeometryBuilder->GetLineCount();
	}

	std::size_t cbText::GetCharSize() const
	{
		return FontGeometryBuilder->GetDefaultCharSize();
	}

	void cbText::UpdateVerticalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerHeight = GetOwner()->GetHeight();
			const std::optional<const eVerticalAlignment> CustomAlignment = GetNonAlignedHeight() > OwnerHeight ? std::optional<eVerticalAlignment>(eVerticalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsHeightAligned())
				Transform.ResetHeightAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? CustomAlignment.value() : GetVerticalAlignment(), GetOwner()->GetBounds(), GetVerticalAnchor()) || ForceAlign)
			{
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
				NotifyCanvas_WidgetUpdated();
			}
			else if (GetVerticalAlignment() == eVerticalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetY(CanvasOffset.Y);
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsHeightAligned())
			{
				Transform.ResetHeightAlignment();
				NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbText::UpdateHorizontalAlignment(const bool ForceAlign)
	{
		if (HasOwner())
		{
			const float OwnerWidth = GetOwner()->GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? *CustomAlignment : GetHorizontalAlignment(), GetOwner()->GetBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
				if (CustomAlignment.has_value())
				{
					if (*CustomAlignment == eHorizontalAlignment::Align_Fill)
					{
						if (IsAutoWrapEnabled())
							AutoWrap();
						else
							NotifyCanvas_WidgetUpdated();
					}
				}
				else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_Fill)
				{
					if (IsAutoWrapEnabled())
						AutoWrap();
					else
						NotifyCanvas_WidgetUpdated();
				}
				else
				{
					NotifyCanvas_WidgetUpdated();
				}
			}
		}
		else if (IsAlignedToCanvas())
		{
			const auto Canvas = GetCanvas();
			const float OwnerWidth = Canvas->GetScreenDimension().GetWidth();
			const std::optional<const eHorizontalAlignment> CustomAlignment = GetNonAlignedWidth() > OwnerWidth ? std::optional<eHorizontalAlignment>(eHorizontalAlignment::Align_Fill) : std::nullopt;

			if (ForceAlign && Transform.IsWidthAligned())
				Transform.ResetWidthAlignment();

			if (Transform.Align(CustomAlignment.has_value() ? *CustomAlignment : GetHorizontalAlignment(), Canvas->GetScreenBounds(), GetHorizontalAnchor()) || ForceAlign)
			{
				if (CustomAlignment.has_value())
				{
					if (*CustomAlignment == eHorizontalAlignment::Align_Fill)
					{
						if (IsAutoWrapEnabled())
							AutoWrap();
						else
							NotifyCanvas_WidgetUpdated();
					}
				}
				else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_Fill)
				{
					if (IsAutoWrapEnabled())
						AutoWrap();
					else
						NotifyCanvas_WidgetUpdated();
				}
				else
				{
					NotifyCanvas_WidgetUpdated();
				}
			}
			else if (GetHorizontalAlignment() == eHorizontalAlignment::Align_NONE || ForceAlign)
			{
				const auto CanvasOffset = cbgui::GetAnchorPointsFromRect(GetCanvasAnchor(), Canvas->GetScreenBounds());
				Transform.SetPositionOffsetX(CanvasOffset.X);
				NotifyCanvas_WidgetUpdated();
			}
		}
		else
		{
			if (Transform.IsWidthAligned())
			{
				Transform.ResetWidthAlignment();
				if (IsAutoWrapEnabled())
					AutoWrap();
				else
					NotifyCanvas_WidgetUpdated();
			}
		}
	}

	void cbText::UpdateRotation()
	{
		NotifyCanvas_WidgetUpdated();
	}

	void cbText::UpdateStatus()
	{
		SetFocus(false);
		NotifyCanvas_WidgetUpdated();
	}

	bool cbText::IsItWrapped() const
	{
		return HasOwner() || bIsItWrapped;
	}

	void cbText::Wrap()
	{
		bIsItWrapped = true;

		bool VerticalWrap = false;
		bool HorizontalWrap = false;

		const auto GeometryBounds = FontGeometryBuilder->GetGeometryBounds();

		if (FontGeometryBuilder->IsAutoWrapTextEnabled())
			Transform.ResetWidthCompressed();
		else
			HorizontalWrap = Transform.CompressWidth(GeometryBounds.GetWidth());
		VerticalWrap = Transform.CompressHeight(GeometryBounds.GetHeight());

		if (VerticalWrap || HorizontalWrap)
		{
			if (VerticalWrap)
				UpdateVerticalAlignment(true);

			if (HorizontalWrap)
				UpdateHorizontalAlignment(true);

			if (HasOwner())
				DimensionUpdated();
		}
	}

	bool cbText::WrapVertical()
	{
		bIsItWrapped = true;

		const auto GeometryBounds = FontGeometryBuilder->GetGeometryBounds();
		if (Transform.CompressHeight(GeometryBounds.GetHeight()))
		{
			NotifyCanvas_WidgetUpdated();
			return true;
		}
		return false;
	}

	bool cbText::WrapHorizontal()
	{
		if (FontGeometryBuilder->IsAutoWrapTextEnabled() && !HasOwner())
			return false;

		bIsItWrapped = true;

		const auto GeometryBounds = FontGeometryBuilder->GetGeometryBounds();
		if (Transform.CompressWidth(GeometryBounds.GetWidth()))
		{
			NotifyCanvas_WidgetUpdated();
			return true;
		}
		return false;
	}

	void cbText::UnWrap()
	{
		if (HasOwner() && !IsItWrapped())
			return;

		bIsItWrapped = false;

		Transform.ResetHeightCompressed();
		Transform.ResetWidthCompressed();
		NotifyCanvas_WidgetUpdated();

		if (HasOwner())
			DimensionUpdated();
		else
			UpdateAlignments();
	}

	void cbText::OnAttach()
	{
		WrapVertical();
		WrapHorizontal();
		UpdateAlignments();
	}

	void cbText::OnRemoveFromParent()
	{
		if (Transform.IsWidthAligned() || Transform.IsHeightAligned())
		{
			Transform.ResetWidthAlignment();
			Transform.ResetHeightAlignment();
			NotifyCanvas_WidgetUpdated();
		}

		if (IsItWrapped())
			UnWrap();
	}

	std::vector<cbGeometryVertexData> cbText::GetVertexData(const bool LineGeometry) const
	{
		float Rotation = GetRotation();
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
				IsEnabled() ? FontGeometryBuilder->GetVertexColorStyle().GetColor() 
							: FontGeometryBuilder->GetVertexColorStyle().GetDisabledColor(),
				GetLocation(), Rotation, Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero());
		}

		const auto& Vertices = FontGeometryBuilder->GetVertices();
		if (Vertices.size() == 0)
			return std::vector<cbGeometryVertexData>();

		const std::size_t VerticesSize = Vertices.size();

		const cbBounds& GeometryBounds = FontGeometryBuilder->GetGeometryBounds();
		const cbVector& GeometryCenter = GeometryBounds.GetCenter();
		const cbVector& Location = Transform.GetCenter();

		const bool bForceLeft = GeometryBounds.GetWidth() > Transform.GetWidth();

		const eTextJustify TextJustify = FontGeometryBuilder->GetTextJustify();

		const float Top = Transform.GetTop() + (GeometryBounds.GetHeight() / 2.0f);
		const float Left = TextJustify == eTextJustify::Left || bForceLeft ? Transform.GetLeft() + (GeometryBounds.GetWidth() / 2.0f) : 0.0f;
		const float Right = TextJustify == eTextJustify::Right || !bForceLeft ? Transform.GetRight() - (GeometryBounds.GetWidth() / 2.0f) : 0.0f;

		const cbVector& Origin = Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero();

		const auto& TextureCoordinates = FontGeometryBuilder->GetTextureCoordinate();
		const auto& Colors = FontGeometryBuilder->GetVertexColors();
		const auto& DisabledVertexColorStyle = FontGeometryBuilder->GetVertexColorStyle().GetDisabledVertexColorStyle();

		const bool bIsEnabled = IsEnabled();

		std::vector<cbGeometryVertexData> Container;
		for (std::size_t i = 0; i < VerticesSize; i++)
		{
			cbGeometryVertexData GeometryData;
			GeometryData.position = Vertices[i];
			{
				if (bForceLeft)
				{
					GeometryData.position += Location - GeometryCenter;
					GeometryData.position.Y += Top - Location.Y;
					GeometryData.position.X += Left - Location.X;
				}
				else
				{
					switch (TextJustify)
					{
					case eTextJustify::Left:
						GeometryData.position += Location - GeometryCenter;
						GeometryData.position.Y += Top - Location.Y;
						GeometryData.position.X += Left - Location.X;
						break;
					case eTextJustify::Center:
						GeometryData.position += Location - GeometryCenter;
						GeometryData.position.Y += Top - Location.Y;
						break;
					case eTextJustify::Right:
						GeometryData.position += Location - GeometryCenter;
						GeometryData.position.Y += Top - Location.Y;
						GeometryData.position.X += Right - Location.X;
						break;
					}
				}

				if (Rotation != 0.0f)
					GeometryData.position = cbgui::RotateVectorAroundPoint(GeometryData.position, Origin, Rotation);
			}
			GeometryData.texCoord = TextureCoordinates[i];
			if (Colors.size() == 0)
				GeometryData.Color = bIsEnabled ? cbColor::White() : DisabledVertexColorStyle(cbColor::White());
			else if (Colors.size() > i)
				GeometryData.Color = bIsEnabled ? Colors[i] : DisabledVertexColorStyle(Colors[i]);
			else
				GeometryData.Color = bIsEnabled ? Colors[Colors.size() - 1] : DisabledVertexColorStyle(Colors[Colors.size() - 1]);
			Container.push_back(GeometryData);
		}

		return Container;
	}

	std::vector<std::uint32_t> cbText::GetIndexData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return std::vector<std::uint32_t>{ 0, 1, 1, 2, 2, 3, 3, 0 };
		return FontGeometryBuilder->GenerateIndices();
	}

	cbGeometryDrawData cbText::GetGeometryDrawData(const bool LineGeometry) const
	{
		if (LineGeometry)
			return cbGeometryDrawData("Line", 0, 4, 8, 8);
		return cbGeometryDrawData("Text", 0, FontGeometryBuilder->GetVerticesSize(), FontGeometryBuilder->GetIndexCount(),
									LetterDrawCount.has_value() ? 
										*LetterDrawCount >= FontGeometryBuilder->GetTextSize(true) ? 
											(FontGeometryBuilder->GetTextSize(true) * 6) : (*LetterDrawCount * 6) 
									: (FontGeometryBuilder->GetTextSize(true) * 6));
	};

	void cbText::SetVertexColorStyle(const cbVertexColorStyle& style)
	{
		if (HasGeometry())
		{
			FontGeometryBuilder->SetVertexColorStyle(style);
			NotifyCanvas_WidgetUpdated();
		}
	}

	void cbgui::cbText::GeometryVerticesSizeChanged(std::size_t Size)
	{
		if (!bShouldNotifyCanvas)
			return;

		if (cbICanvas* Canvas = GetCanvas())
		{
			Canvas->VerticesSizeChanged(this, Size);
			Canvas->WidgetUpdated(this);
		}
	}

	bool cbText::OnMouseEnter(const cbMouseInput& Mouse)
	{
		if (!IsFocusable() || !IsEnabled())
			return false;

		if (!IsInside(Mouse.MouseLocation))
			return false;

		if (IsItCulled())
			return false;

		if (!IsFocused())
			SetFocus(true);

		return true;
	}

	bool cbText::OnMouseLeave(const cbMouseInput& Mouse)
	{
		if (!IsFocused())
			return false;

		if (IsFocused())
			SetFocus(false);

		return true;
	}

	void cbText::ResetInput()
	{
		SetFocus(false);
	}
}
