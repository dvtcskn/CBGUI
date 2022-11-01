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

#include <vector>
#include <map>

#include "cbClassBody.h"
#include "cbMath.h"
#include "cbStates.h"

namespace cbgui
{
	/* Automatically creates disabled Vertex Color. */
	struct cbDisabledVertexColorStyle
	{
		cbBaseClassBody(cbClassConstructor, cbDisabledVertexColorStyle)
	public:
		cbFORCEINLINE constexpr cbDisabledVertexColorStyle(float InValue = 0.20f, float InDisabledBlackValue = 0.05f)
			: Multiplier(InValue)
			, DisabledBlackValue(InDisabledBlackValue)
		{}

		virtual ~cbDisabledVertexColorStyle() = default;

		float Multiplier;
		float DisabledBlackValue;

		cbFORCEINLINE constexpr cbColor GetColor(cbColor Color) const
		{
			if (Color == cbColor::Black())
				return cbColor(DisabledBlackValue);

			Color.R *= Multiplier;
			Color.G *= Multiplier;
			Color.B *= Multiplier;

			return Color;
		}

		cbFORCEINLINE constexpr cbColor operator()(cbColor Color) const
		{
			return GetColor(Color);
		}
	};

	/* Interface class for Vertex Color Style. */
	class cbIVertexColorStyle
	{
		cbBaseClassBody(cbClassDefaultProtectedConstructor, cbIVertexColorStyle)
	};

	/* Default Vertex Color Style. */
	class cbVertexColorStyle : public cbIVertexColorStyle
	{
		cbClassBody(cbClassNoDefaults, cbVertexColorStyle, cbIVertexColorStyle)
	private:
		/* Automatically creates disabled Vertex Color. */
		bool bUseDisabledVertexColorStyle;
		cbDisabledVertexColorStyle DisabledVertexColorStyle;

	public:
		cbFORCEINLINE cbVertexColorStyle(const cbColor& DefaultColor = cbColor::White(), const bool UseDisabledVertexColorStyle = true,
										 const cbDisabledVertexColorStyle inDisabledVertexColorStyle = cbDisabledVertexColorStyle())
			: Color(DefaultColor)
			, bUseDisabledVertexColorStyle(UseDisabledVertexColorStyle)
			, DisabledVertexColorStyle(inDisabledVertexColorStyle)
		{}

		virtual ~cbVertexColorStyle() = default;

	public:
		cbColor Color;

		cbDisabledVertexColorStyle GetDisabledVertexColorStyle() const { return DisabledVertexColorStyle; };

		inline void SetColor(const cbColor& DefaultColor)
		{
			Color = DefaultColor;
		}

		inline cbColor GetColor() const
		{
			return Color;
		}

		inline cbColor GetDisabledColor() const
		{
			return bUseDisabledVertexColorStyle ? DisabledVertexColorStyle(Color) : Color;
		}
	};

	/* Default Button Vertex Color Style. */
	class cbButtonVertexColorStyle : public cbIVertexColorStyle
	{
		cbClassBody(cbClassNoDefaults, cbButtonVertexColorStyle, cbIVertexColorStyle)
	public:
		typedef eButtonState MaterialState;
	private:
		/* Automatically creates disabled Vertex Color. */
		bool bUseDisabledVertexColorStyle;
		cbDisabledVertexColorStyle DisabledVertexColorStyle;

	public:
		cbFORCEINLINE cbButtonVertexColorStyle(const cbColor& DefaultColor = cbColor::White(), const bool UseDisabledVertexColorStyle = false,
											   const cbDisabledVertexColorStyle inDisabledVertexColorStyle = cbDisabledVertexColorStyle())
			: bUseDisabledVertexColorStyle(UseDisabledVertexColorStyle)
			, DisabledVertexColorStyle(inDisabledVertexColorStyle)
		{
			PolygonColors.insert({ eButtonState::Default, DefaultColor });
			PolygonColors.insert({ eButtonState::Pressed, DefaultColor });
			PolygonColors.insert({ eButtonState::Hovered, DefaultColor });
			PolygonColors.insert({ eButtonState::Disabled, DefaultColor });
		}
		cbFORCEINLINE cbButtonVertexColorStyle(const cbColor& DefaultColor, const cbColor& PressedColor, const cbColor& HoveredColor, const cbColor& DisabledColor, 
						const bool UseDisabledVertexColorStyle = false, const cbDisabledVertexColorStyle inDisabledVertexColorStyle = cbDisabledVertexColorStyle())
			: bUseDisabledVertexColorStyle(UseDisabledVertexColorStyle)
			, DisabledVertexColorStyle(inDisabledVertexColorStyle)
		{
			PolygonColors.insert({ eButtonState::Default, DefaultColor });
			PolygonColors.insert({ eButtonState::Pressed, PressedColor });
			PolygonColors.insert({ eButtonState::Hovered, HoveredColor });
			PolygonColors.insert({ eButtonState::Disabled, DisabledColor });
		}

		virtual ~cbButtonVertexColorStyle()
		{
			PolygonColors.clear();
		}

	public:
		std::map<eButtonState, cbColor> PolygonColors;

		cbDisabledVertexColorStyle GetDisabledVertexColorStyle() const { return DisabledVertexColorStyle; };

		inline cbColor GetColor(const eButtonState& Status) const
		{
			return PolygonColors.at(Status);
		}

		inline void SetColor(const eButtonState State, const cbColor& Color)
		{
			PolygonColors[State] = Color;
		}

		inline void SetDefaultColor(const cbColor& Color)
		{
			PolygonColors[eButtonState::Default] = Color;
		}

		inline void SetDisabledColor(const cbColor& Color)
		{
			PolygonColors[eButtonState::Disabled] = Color;
		}

		inline void SetHoveredColor(const cbColor& Color)
		{
			PolygonColors[eButtonState::Hovered] = Color;
		}

		inline void SetPressedColor(const cbColor& Color)
		{
			PolygonColors[eButtonState::Pressed] = Color;
		}

		inline cbColor GetDisabledColor() const
		{
			return bUseDisabledVertexColorStyle ? DisabledVertexColorStyle(PolygonColors.at(eButtonState::Default)) : PolygonColors.at(eButtonState::Disabled);
		}
	};

	/* Default Check Box Vertex Color Style. */
	class cbCheckBoxVertexColorStyle : public cbIVertexColorStyle
	{
		cbClassBody(cbClassNoDefaults, cbCheckBoxVertexColorStyle, cbIVertexColorStyle)
	public:
		typedef eCheckBoxState MaterialState;
	private:
		/* Automatically creates disabled Vertex Color. */
		bool bUseDisabledVertexColorStyle;
		cbDisabledVertexColorStyle DisabledVertexColorStyle;

	public:
		cbFORCEINLINE cbCheckBoxVertexColorStyle(const cbColor& DefaultColor = cbColor::White(), const bool UseDisabledVertexColorStyle = false,
												 const cbDisabledVertexColorStyle inDisabledVertexColorStyle = cbDisabledVertexColorStyle())
			: bUseDisabledVertexColorStyle(UseDisabledVertexColorStyle)
			, DisabledVertexColorStyle(inDisabledVertexColorStyle)
		{
			PolygonColors.insert({ eCheckBoxState::Unchecked, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::UncheckedHovered, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::UncheckedPressed, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::Checked, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::CheckedHovered, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::CheckedPressed, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::Undetermined, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::UndeterminedHovered, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::UndeterminedPressed, DefaultColor });

			PolygonColors.insert({ eCheckBoxState::UncheckedDisabled, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::CheckedDisabled, DefaultColor });
			PolygonColors.insert({ eCheckBoxState::UndeterminedDisabled, DefaultColor });
		}

		virtual ~cbCheckBoxVertexColorStyle()
		{
			PolygonColors.clear();
		}

	public:
		std::map<eCheckBoxState, cbColor> PolygonColors;

		cbDisabledVertexColorStyle GetDisabledVertexColorStyle() const { return DisabledVertexColorStyle; };

		inline cbColor GetColor(const eCheckBoxState& Status) const
		{
			return PolygonColors.at(Status);
		}
		inline cbColor GetDisabledColor(const eCheckBoxState& Status) const
		{
			return bUseDisabledVertexColorStyle ? DisabledVertexColorStyle(PolygonColors.at((Status == eCheckBoxState::Checked || Status == eCheckBoxState::CheckedPressed || Status == eCheckBoxState::CheckedHovered || 
				Status == eCheckBoxState::CheckedDisabled) ? eCheckBoxState::Checked : (Status == eCheckBoxState::Undetermined || Status == eCheckBoxState::UndeterminedPressed || Status == eCheckBoxState::UndeterminedHovered || 
				Status == eCheckBoxState::UndeterminedDisabled) ? eCheckBoxState::Undetermined : eCheckBoxState::Unchecked)) : PolygonColors.at(Status);
		}

		inline void SetColor(const eCheckBoxState State, const cbColor& Color)
		{
			PolygonColors[State] = Color;
		}

		inline void SetUncheckedColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::Unchecked] = (Color);
		}
		inline void SetUncheckedHoveredColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UncheckedHovered] = (Color);
		}
		inline void SetUncheckedPressedColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UncheckedPressed] = (Color);
		}

		inline void SetCheckedColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::Checked] = (Color);
		}
		inline void SetCheckedHoveredColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::CheckedHovered] = (Color);
		}
		inline void SetCheckedPressedColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::CheckedPressed] = (Color);
		}

		inline void SetUndeterminedColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::Undetermined] = (Color);
		}
		inline void SetUndeterminedHoveredColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UndeterminedHovered] = (Color);
		}
		inline void SetUndeterminedPressedColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UndeterminedPressed] = (Color);
		}

		inline void SetUncheckedDisabledColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UncheckedDisabled] = (Color);
		}
		inline void SetCheckedDisabledColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::CheckedDisabled] = (Color);
		}
		inline void SetUndeterminedDisabledColor(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UndeterminedDisabled] = (Color);
		}

		inline void SetDefaultHoveredColors(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UncheckedHovered] = (Color);
			PolygonColors[eCheckBoxState::CheckedHovered] = (Color);
			PolygonColors[eCheckBoxState::UndeterminedHovered] = (Color);
		}
		inline void SetDefaultDisabledColors(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UncheckedDisabled] = (Color);
			PolygonColors[eCheckBoxState::CheckedDisabled] = (Color);
			PolygonColors[eCheckBoxState::UndeterminedDisabled] = (Color);
		}
		inline void SetDefaultPressedColors(const cbColor& Color)
		{
			PolygonColors[eCheckBoxState::UncheckedPressed] = (Color);
			PolygonColors[eCheckBoxState::CheckedPressed] = (Color);
			PolygonColors[eCheckBoxState::UndeterminedPressed] = (Color);
		}
	};
}
