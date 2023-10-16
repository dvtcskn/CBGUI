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

#include <memory>
#include <iostream>
#include <optional>
#include "cbClassBody.h"
#include "cbStates.h"
#include "cbMath.h"

namespace cbgui
{
	/* Determines and aligns the position and size of the widget on the screen. */
	struct cbTransform
	{
		cbBaseClassBody(cbClassNoDefaults, cbTransform)
	private:
		cbDimension Dimension;
		cbVector Location;
		cbVector LocationOffset;
		float Rotation;
		cbMargin Padding;

		std::optional<float> WidthAlignment;
		std::optional<float> HeightAlignment;
		std::optional<float> WidthCompression;
		std::optional<float> HeightCompression;

	public:
		cbTransform(const cbDimension dimension = cbDimension(256.0f, 256.0f))
			: Dimension(dimension)
			, Location(cbVector::Zero())
			, LocationOffset(cbVector::Zero())
			, Rotation(0.0f)
			, Padding(cbMargin())
			, WidthAlignment(std::nullopt)
			, HeightAlignment(std::nullopt)
			, WidthCompression(std::nullopt)
			, HeightCompression(std::nullopt)
		{}

	public:
		virtual ~cbTransform() = default;

		cbFORCEINLINE constexpr cbBounds GetBounds() const { return cbBounds(GetDimension(), GetCenter()); }

		inline constexpr cbVector GetCenter() const { return Location + LocationOffset; }
		bool SetLocation(const cbVector& Location);
		bool SetPositionX(const float value);
		bool SetPositionY(const float value);
		bool SetPositionOffsetX(const float value);
		bool SetPositionOffsetY(const float value);

		inline constexpr cbDimension GetDimension() const { return cbDimension(GetWidth(), GetHeight()); }
		inline constexpr float GetWidth() const
		{
			if (WidthAlignment.has_value())
				return WidthAlignment.value();
			if (WidthCompression.has_value())
				return WidthCompression.value();
			return Dimension.GetWidth();
		}
		inline constexpr float GetHeight() const 
		{
			if (HeightAlignment.has_value())
				return HeightAlignment.value();
			if (HeightCompression.has_value())
				return HeightCompression.value();
			return Dimension.GetHeight();
		}

		inline constexpr float GetNonAlignedWidth() const
		{
			if (WidthCompression.has_value())
				return WidthCompression.value();
			return Dimension.GetWidth();
		}
		inline constexpr float GetNonAlignedHeight() const
		{
			if (HeightCompression.has_value())
				return HeightCompression.value();
			return Dimension.GetHeight();
		}

		bool SetDimension(const cbDimension InDimension);
		bool SetWidth(const float Width);
		bool SetHeight(const float Height);

		/* Aligns vertically. */
		bool Align(const eHorizontalAlignment Align, const cbBounds BoundsToAlign, const eAnchors Anchor = eAnchors::Inside);
		/* Aligns Horizontally. */
		bool Align(const eVerticalAlignment Align, const cbBounds BoundsToAlign, const eAnchors Anchor = eAnchors::Inside);

		inline constexpr bool IsWidthAligned() const { return WidthAlignment.has_value(); }
		inline constexpr bool IsHeightAligned() const { return HeightAlignment.has_value(); }
		bool AlignWidth(float Alignment);
		bool AlignHeight(float Alignment);
		void ResetWidthAlignment();
		void ResetHeightAlignment();

		inline constexpr bool IsWidthCompressed() const { return WidthCompression != 0.0f; }
		inline constexpr bool IsHeightCompressed() const { return HeightCompression != 0.0f; }
		bool CompressWidth(const float Compression);
		bool CompressHeight(const float Compression);
		void ResetWidthCompressed();
		void ResetHeightCompressed();

		inline constexpr float GetRotation() const { return Rotation; }
		bool Rotate2D(const float Roll);
		void ResetRotation();

		inline constexpr cbMargin GetPadding() const { return Padding; }
		bool SetPadding(const cbMargin& padding);
		bool SetPaddingToLeft(const std::int32_t& value);
		bool SetPaddingToTop(const std::int32_t& value);
		bool SetPaddingToRight(const std::int32_t& value);
		bool SetPaddingToBottom(const std::int32_t& value);

	public:
		inline constexpr float GetTop() const
		{
			return Location.Y - (GetHeight() / 2.0f);
		}
		inline constexpr float GetBottom() const
		{
			return Location.Y + (GetHeight() / 2.0f);
		}
		inline constexpr float GetLeft() const
		{
			return Location.X - (GetWidth() / 2.0f);
		}
		inline constexpr float GetRight() const
		{
			return Location.X + (GetWidth() / 2.0f);
		}

		inline constexpr cbVector GetTopLeft() const
		{
			return cbVector(Location.X - (GetWidth() / 2.0f), Location.Y - (GetHeight() / 2.0f));
		}
		inline constexpr cbVector GetRightTop() const
		{
			return cbVector(Location.X + (GetWidth() / 2.0f), Location.Y - (GetHeight() / 2.0f));
		}
		inline constexpr cbVector GetBottomRight() const
		{
			return cbVector(Location.X + (GetWidth() / 2.0f), Location.Y + (GetHeight() / 2.0f));
		}
		inline constexpr cbVector GetLeftBottom() const
		{
			return cbVector(Location.X - (GetWidth() / 2.0f), Location.Y + (GetHeight() / 2.0f));
		}

	public:
		cbCONSTEXPR20 std::string ToString() const
		{
			return GetClassID() + ": " + std::string("{ Dimension: " + GetDimension().ToString() + " Location: " + GetCenter().ToString()
				+ " Padding: " + Padding.ToString() + " Rotation: " + std::to_string(GetRotation()) + " };");
		}
	};
}
