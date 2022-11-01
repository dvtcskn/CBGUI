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
#include "cbTransform.h"

namespace cbgui
{
	bool cbTransform::SetLocation(const cbVector& InLocation)
	{
		if (Location.X != InLocation.X || Location.Y != InLocation.Y)
		{
			Location.X = InLocation.X;
			Location.Y = InLocation.Y;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPositionX(const float value)
	{
		if (Location.X != value)
		{
			Location.X = value;
			LocationOffset.X = 0.0f;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPositionY(const float value)
	{
		if (Location.Y != value)
		{
			Location.Y = value;
			LocationOffset.Y = 0.0f;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPositionOffsetX(const float value)
	{
		if (LocationOffset.X != value)
		{
			LocationOffset.X = value;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPositionOffsetY(const float value)
	{
		if (LocationOffset.Y != value)
		{
			LocationOffset.Y = value;
			return true;
		}
		return false;
	}

	bool cbTransform::SetDimension(const cbDimension InDimension)
	{
		if (InDimension.Width != Dimension.Width || InDimension.Height != Dimension.Height)
		{
			SetWidth(InDimension.GetWidth());
			SetHeight(InDimension.GetHeight());
			return true;
		}
		return false;
	}

	bool cbTransform::SetWidth(const float Width)
	{
		if (Width != Dimension.Width)
		{
			Dimension.Width = Width;
			return true;
		}
		return false;
	}

	bool cbTransform::SetHeight(const float Height)
	{
		if (Height != Dimension.Height)
		{
			Dimension.Height = Height;
			return true;
		}
		return false;
	}

	bool cbTransform::AlignWidth(float Alignment)
	{
		if (WidthAlignment.has_value())
		{
			if (WidthAlignment.value() != Alignment)
			{
				WidthAlignment = Alignment;
				return true;
			}
			return false;
		}
		WidthAlignment = Alignment;
		return true;
	}

	bool cbTransform::AlignHeight(float Alignment)
	{
		if (HeightAlignment.has_value())
		{
			if (HeightAlignment.value() != Alignment)
			{
				HeightAlignment = Alignment;
				return true;
			}
			return false;
		}
		HeightAlignment = Alignment;
		return true;
	}

	void cbTransform::ResetWidthAlignment()
	{
		WidthAlignment = std::nullopt;
	}

	void cbTransform::ResetHeightAlignment()
	{
		HeightAlignment = std::nullopt;
	}

	bool cbTransform::CompressWidth(const float Compression)
	{
		if (WidthCompression.has_value())
		{
			if (WidthCompression.value() != Compression)
			{
				WidthCompression = Compression;
				return true;
			}
			return false;
		}
		WidthCompression = Compression;
		return true;
	}

	bool cbTransform::CompressHeight(const float Compression)
	{
		if (HeightCompression.has_value())
		{
			if (HeightCompression.value() != Compression)
			{
				HeightCompression = Compression;
				return true;
			}
			return false;
		}
		HeightCompression = Compression;
		return true;
	}

	void cbTransform::ResetWidthCompressed()
	{
		WidthCompression = std::nullopt;
	}

	void cbTransform::ResetHeightCompressed()
	{
		HeightCompression = std::nullopt;
	}

	void cbTransform::ResetRotation()
	{
		Rotation = 0.0f;
		RotationOffset = 0.0f;
	}

	bool cbTransform::Rotate2D(const float Roll)
	{
		if (Rotation != Roll)
		{
			Rotation = Roll;
			return true;
		}
		return true;
	}

	bool cbTransform::SetRollOffset(const float value)
	{
		if (RotationOffset != value)
		{
			RotationOffset = value;
			return true;
		}
		return false;
	}

	bool cbTransform::Align(const eHorizontalAlignment HorizontalAlignment, const cbBounds BoundsToAlign, const eAnchors Anchor)
	{
		bool Result = false;
		if (IsWidthAligned() && HorizontalAlignment != eHorizontalAlignment::Align_Fill)
		{
			ResetWidthAlignment();
			Result = true;
		}

		switch (HorizontalAlignment)
		{
		case eHorizontalAlignment::Align_Left:
			switch (Anchor)
			{
			case eAnchors::Inside:
				return SetPositionX(BoundsToAlign.GetLeft() + (GetWidth() / 2.0f) + Padding.Left) || Result;
				break;
			case eAnchors::Mid:
				return SetPositionX(BoundsToAlign.GetLeft() + Padding.Left) || Result;
				break;
			case eAnchors::Outside:
				return SetPositionX(BoundsToAlign.GetLeft() - (GetWidth() / 2.0f) + Padding.Left) || Result;
				break;
			}
			break;
		case eHorizontalAlignment::Align_Center:
			switch (Anchor)
			{
			case eAnchors::Inside:
				return SetPositionX(BoundsToAlign.GetCenter().X + (Padding.Left - Padding.Right)) || Result;
				break;
			case eAnchors::Mid:
				return SetPositionX(BoundsToAlign.GetCenter().X + (Padding.Left - Padding.Right)) || Result;
				break;
			case eAnchors::Outside:
				return SetPositionX(BoundsToAlign.GetCenter().X + (Padding.Left - Padding.Right)) || Result;
				break;
			}
			break;
		case eHorizontalAlignment::Align_Right:
			switch (Anchor)
			{
			case eAnchors::Inside:
				return SetPositionX(BoundsToAlign.GetRight() - (GetWidth() / 2.0f) - Padding.Right) || Result;
				break;
			case eAnchors::Mid:
				return SetPositionX(BoundsToAlign.GetRight() + Padding.Right) || Result;
				break;
			case eAnchors::Outside:
				return SetPositionX(BoundsToAlign.GetRight() + (GetWidth() / 2.0f) - Padding.Right) || Result;
				break;
			}
			break;
		case eHorizontalAlignment::Align_Fill:
		{
			const bool bIsWidthAligned = AlignWidth(BoundsToAlign.GetWidth() - (Padding.Left + Padding.Right));
			const bool bIsPositionAligned = SetPositionX(BoundsToAlign.GetCenter().X + ((Padding.Left - Padding.Right) / 2.0f));

			return bIsWidthAligned || bIsPositionAligned || Result;
		}
		break;
		}

		return Result;
	}

	bool cbTransform::Align(const eVerticalAlignment VerticalAlignment, const cbBounds BoundsToAlign, const eAnchors Anchor)
	{
		bool Result = false;
		if (IsHeightAligned() && VerticalAlignment != eVerticalAlignment::Align_Fill)
		{
			ResetHeightAlignment();
			Result = true;
		}

		switch (VerticalAlignment)
		{
		case eVerticalAlignment::Align_Top:
			switch (Anchor)
			{
			case eAnchors::Inside:
				return SetPositionY(BoundsToAlign.GetTop() + (GetHeight() / 2.0f) + Padding.Top) || Result;
				break;
			case eAnchors::Mid:
				return SetPositionY(BoundsToAlign.GetTop() + Padding.Top) || Result;
				break;
			case eAnchors::Outside:
				return SetPositionY(BoundsToAlign.GetTop() - (GetHeight() / 2.0f) + Padding.Top) || Result;
				break;
			}
			break;
		case eVerticalAlignment::Align_Center:
			switch (Anchor)
			{
			case eAnchors::Inside:
				return SetPositionY(BoundsToAlign.GetCenter().Y + (Padding.Top - Padding.Bottom)) || Result;
				break;
			case eAnchors::Mid:
				return SetPositionY(BoundsToAlign.GetCenter().Y + (Padding.Top - Padding.Bottom)) || Result;
				break;
			case eAnchors::Outside:
				return SetPositionY(BoundsToAlign.GetCenter().Y + (Padding.Top - Padding.Bottom)) || Result;
				break;
			}
			break;
		case eVerticalAlignment::Align_Bottom:
			switch (Anchor)
			{
			case eAnchors::Inside:
				return SetPositionY(BoundsToAlign.GetBottom() - (GetHeight() / 2.0f) - Padding.Bottom) || Result;
				break;
			case eAnchors::Mid:
				return SetPositionY(BoundsToAlign.GetBottom() + Padding.Bottom) || Result;
				break;
			case eAnchors::Outside:
				return SetPositionY(BoundsToAlign.GetBottom() + (GetHeight() / 2.0f) - Padding.Bottom) || Result;
				break;
			}
			break;
		case eVerticalAlignment::Align_Fill:
		{
			const bool bIsHeightAligned = AlignHeight((BoundsToAlign.GetHeight()) - (Padding.Bottom + Padding.Top));
			const bool bIsPositionAligned = SetPositionY(BoundsToAlign.GetCenter().Y + ((Padding.Top - Padding.Bottom) / 2.0f));

			return bIsHeightAligned || bIsPositionAligned || Result;
		}
		break;
		}

		return Result;
	}

	bool cbTransform::SetPadding(const cbMargin& padding)
	{
		if (!Padding.IsEqual(padding))
		{
			Padding = padding;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPaddingToLeft(const std::int32_t& value)
	{
		if (Padding.Left != value)
		{
			Padding.Left = value;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPaddingToTop(const std::int32_t& value)
	{
		if (Padding.Top != value)
		{
			Padding.Top = value;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPaddingToRight(const std::int32_t& value)
	{
		if (Padding.Right != value)
		{
			Padding.Right = value;
			return true;
		}
		return false;
	}

	bool cbTransform::SetPaddingToBottom(const std::int32_t& value)
	{
		if (Padding.Bottom != value)
		{
			Padding.Bottom = value;
			return true;
		}
		return false;
	}
}
