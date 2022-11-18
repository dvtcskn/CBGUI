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

#include <algorithm>
#include <map>
#include <cbStates.h>
#include <cbClassBody.h>
#include <cbMath.h>

using namespace cbgui;

struct IUIMaterial
{
	cbBaseClassBody(cbClassDefaultProtectedConstructor, IUIMaterial)
};

class IUIMaterialStyle
{
	cbBaseClassBody(cbClassDefaultProtectedConstructor, IUIMaterialStyle)
public:
	virtual void SetName(const std::string& StyleName) = 0;
	virtual std::string GetName() const = 0;
	virtual IUIMaterial* GetMaterial(unsigned short StyleState = 0) const = 0;
};

class UIMaterialStyle : public IUIMaterialStyle, public std::enable_shared_from_this<UIMaterialStyle>
{
	cbClassBody(cbClassConstructor, UIMaterialStyle, IUIMaterialStyle)
public:
	UIMaterialStyle(const std::string& StyleName, const IUIMaterial::SharedPtr& DefaultMaterial = nullptr)
		: Name(StyleName)
		, Material(DefaultMaterial)
	{}

	virtual ~UIMaterialStyle()
	{
		Material = nullptr;
	}

	template <typename T>
	inline T* GetMaterialAs(unsigned short StyleState = 0) const
	{
		return static_cast<T*>(GetMaterial(StyleState));
	}

	virtual void SetMaterial(const IUIMaterial::SharedPtr& pMaterial)
	{
		Material = pMaterial;
	}

	virtual IUIMaterial* GetMaterial(unsigned short StyleState = 0) const
	{
		return Material.get();
	}

	virtual void RemoveMaterial()
	{
		Material = nullptr;
	}

	virtual void SetDefaultMaterial(const IUIMaterial::SharedPtr& Color)
	{
		Material = Color;
	}

	virtual void SetName(const std::string& StyleName) override { Name = StyleName; }
	virtual std::string GetName() const override { return Name; }

protected:
	IUIMaterial::SharedPtr Material;
	std::string Name;
};

class IUIFontMaterialStyle : public IUIMaterialStyle, public std::enable_shared_from_this<IUIFontMaterialStyle>
{
	cbClassBody(cbClassDefaultProtectedConstructor, IUIFontMaterialStyle, IUIMaterialStyle)
public:
};

class UIFontMaterialStyle : public IUIFontMaterialStyle
{
	cbClassBody(cbClassConstructor, UIFontMaterialStyle, IUIFontMaterialStyle)
public:
	UIFontMaterialStyle(const std::string& StyleName)
		: Name(StyleName)
	{}
	UIFontMaterialStyle(const std::string& StyleName, const std::string& FontName, const IUIMaterial::SharedPtr& Mat)
		: Material(Mat)
		, FontFamilyName(FontName)
		, Name(StyleName)
	{}
	
	virtual ~UIFontMaterialStyle()
	{
		Material = nullptr;
	}

	template <typename T>
	inline T GetMaterialAs() const
	{
		return static_cast<T>(GetMaterial());
	}

	virtual IUIMaterial* GetMaterial(unsigned short StyleState = 0) const override { return Material.get(); }
	inline void SetMaterial(const IUIMaterial::SharedPtr& Mat) { Material = Mat; }

	inline std::string GetFontFamilyName() const { return FontFamilyName; }

	virtual void SetName(const std::string& StyleName) override { Name = StyleName; }
	virtual std::string GetName() const override { return Name; }

protected:
	IUIMaterial::SharedPtr Material;
	std::string FontFamilyName;
	std::string Name;
};

class IUIButtonMaterialStyle : public IUIMaterialStyle, public std::enable_shared_from_this<IUIButtonMaterialStyle>
{
	cbClassBody(cbClassDefaultProtectedConstructor, IUIButtonMaterialStyle, IUIMaterialStyle)
public:
	typedef eButtonState MaterialState;
public:
};

class UIButtonMaterialStyle : public IUIButtonMaterialStyle
{
	cbClassBody(cbClassConstructor, UIButtonMaterialStyle, IUIButtonMaterialStyle)
public:
	typedef eButtonState MaterialState;

public:
	UIButtonMaterialStyle(const std::string& StyleName)
		: Name(StyleName)
		, Default(nullptr)
		, OnPressed(nullptr)
		, OnHover(nullptr)
		, Disabled(nullptr)
	{}
	
	virtual ~UIButtonMaterialStyle()
	{
		Default = nullptr;
		OnPressed = nullptr;
		OnHover = nullptr;
		Disabled = nullptr;
	}

	virtual std::vector<IUIMaterial*> GetAllMaterials()
	{
		std::vector<IUIMaterial*> Mats;
		Mats.push_back(Default.get());
		Mats.push_back(OnPressed.get());
		Mats.push_back(OnHover.get());
		Mats.push_back(Disabled.get());
		return Mats;
	}

	virtual IUIMaterial* GetMaterial(unsigned short StyleState = 0) const
	{
		const eButtonState& State = static_cast<eButtonState>(StyleState);
		switch (State)
		{
		case eButtonState::Default:
			return Default.get();
			break;
		case eButtonState::Disabled:
			return Disabled.get();
			break;
		case eButtonState::Hovered:
			return OnHover.get();
			break;
		case eButtonState::Pressed:
			return OnPressed.get();
			break;
		}
		return nullptr;
	}

	virtual void SetDefaultMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		Default = pMat;
	}
	virtual void SetOnPressedMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		OnPressed = pMat;
	}
	virtual void SetOnHoverMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		OnHover = pMat;
	}
	virtual void SetOnDisabledMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		Disabled = pMat;
	}

	inline IUIMaterial* GetDefault() { return Default.get(); }
	inline IUIMaterial* GetOnPressed() { return OnPressed.get(); }
	inline IUIMaterial* GetOnHover() { return OnHover.get(); }
	inline IUIMaterial* GetDisabled() { return Disabled.get(); }

	virtual void SetName(const std::string& StyleName) override { Name = StyleName; }
	virtual std::string GetName() const override { return Name; }

protected:
	IUIMaterial::SharedPtr Default;
	IUIMaterial::SharedPtr OnPressed;
	IUIMaterial::SharedPtr OnHover;
	IUIMaterial::SharedPtr Disabled;

	std::string Name;
};

class IUICheckBoxMaterialStyle : public IUIMaterialStyle, public std::enable_shared_from_this<IUICheckBoxMaterialStyle>
{
	cbClassBody(cbClassDefaultProtectedConstructor, IUICheckBoxMaterialStyle, IUIMaterialStyle)
public:
	typedef eCheckBoxState MaterialState;
public:
};

class UICheckBoxMaterialStyle : public IUICheckBoxMaterialStyle
{
	cbClassBody(cbClassConstructor, UICheckBoxMaterialStyle, IUICheckBoxMaterialStyle)
public:
	UICheckBoxMaterialStyle(const std::string& StyleName)
		: Name(StyleName)
		, Unchecked(nullptr)
		, UncheckedHovered(nullptr)
		, UncheckedPressed(nullptr)
		, Checked(nullptr)
		, CheckedHovered(nullptr)
		, CheckedPressed(nullptr)
		, Undetermined(nullptr)
		, UndeterminedHovered(nullptr)
		, UndeterminedPressed(nullptr)
		, CheckedDisabled(nullptr)
		, UncheckedDisabled(nullptr)
		, UndeterminedDisabled(nullptr)
	{}

	virtual ~UICheckBoxMaterialStyle()
	{
		Unchecked = nullptr;
		UncheckedHovered = nullptr;
		UncheckedPressed = nullptr;
		Checked = nullptr;
		CheckedHovered = nullptr;
		CheckedPressed = nullptr;
		Undetermined = nullptr;
		UndeterminedHovered = nullptr;
		UndeterminedPressed = nullptr;
		CheckedDisabled = nullptr;
		UncheckedDisabled = nullptr;
		UndeterminedDisabled = nullptr;
	}

	virtual std::vector<IUIMaterial*> GetAllMaterials()
	{
		std::vector<IUIMaterial*> Mats;
		Mats.push_back(Unchecked.get());
		Mats.push_back(UncheckedHovered.get());
		Mats.push_back(UncheckedPressed.get());
		Mats.push_back(Checked.get());
		Mats.push_back(CheckedHovered.get());
		Mats.push_back(CheckedPressed.get());
		Mats.push_back(Undetermined.get());
		Mats.push_back(UndeterminedHovered.get());
		Mats.push_back(UndeterminedPressed.get());
		if (CheckedDisabled)
			Mats.push_back(CheckedDisabled.get());
		if (UncheckedDisabled)
			Mats.push_back(UncheckedDisabled.get());
		if (UndeterminedDisabled)
			Mats.push_back(UndeterminedDisabled.get());
		return Mats;
	}

	virtual IUIMaterial* GetMaterial(unsigned short StyleState = 0) const
	{
		const eCheckBoxState& State = static_cast<eCheckBoxState>(StyleState);
		switch (State)
		{
		case eCheckBoxState::UncheckedPressed:
			return GetUncheckedPressed();
			break;
		case eCheckBoxState::UncheckedHovered:
			return GetUncheckedHovered();
			break;
		case eCheckBoxState::Unchecked:
			return GetUnchecked();
			break;
		case eCheckBoxState::CheckedPressed:
			return GetCheckedPressed();
			break;
		case eCheckBoxState::CheckedHovered:
			return GetCheckedHovered();
			break;
		case eCheckBoxState::Checked:
			return GetChecked();
			break;
		case eCheckBoxState::UndeterminedPressed:
			return GetUndeterminedPressed();
			break;
		case eCheckBoxState::UndeterminedHovered:
			return GetUndeterminedHovered();
			break;
		case eCheckBoxState::Undetermined:
			return GetUndetermined();
			break;
		case eCheckBoxState::UncheckedDisabled:
		{
			const auto& Mat = GetUncheckedDisabled();
			if (Mat)
				return GetUncheckedDisabled();
			else
				return GetUnchecked();
		}
		break;
		case eCheckBoxState::CheckedDisabled:
		{
			const auto& Mat = GetCheckedDisabled();
			if (Mat)
				return GetCheckedDisabled();
			else
				return GetChecked();
		}
		break;
		case eCheckBoxState::UndeterminedDisabled:
		{
			const auto& Mat = GetUndeterminedDisabled();
			if (Mat)
				return GetUndeterminedDisabled();
			else
				return GetUndetermined();
		}
		break;
		}
		return nullptr;
	}

	virtual void SetUncheckedMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		Unchecked = pMat;
	}
	virtual void SetUncheckedMaterialHovered(const IUIMaterial::SharedPtr& pMat)
	{
		UncheckedHovered = pMat;
	}
	virtual void SetUncheckedMaterialPressed(const IUIMaterial::SharedPtr& pMat)
	{
		UncheckedPressed = pMat;
	}
	virtual void SetCheckedMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		Checked = pMat;
	}
	virtual void SetCheckedMaterialHovered(const IUIMaterial::SharedPtr& pMat)
	{
		CheckedHovered = pMat;
	}
	virtual void SetCheckedMaterialPressed(const IUIMaterial::SharedPtr& pMat)
	{
		CheckedPressed = pMat;
	}
	virtual void SetUndeterminedMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		Undetermined = pMat;
	}
	virtual void SetUndeterminedMaterialHovered(const IUIMaterial::SharedPtr& pMat)
	{
		UndeterminedHovered = pMat;
	}
	virtual void SetUndeterminedMaterialPressed(const IUIMaterial::SharedPtr& pMat)
	{
		UndeterminedPressed = pMat;
	}
	/*
	* Optional
	*/
	virtual void SetCheckedDisabledMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		CheckedDisabled = pMat;
	}
	/*
	* Optional
	*/
	virtual void SetUncheckedDisabledMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		UncheckedDisabled = pMat;
	}
	/*
	* Optional
	*/
	virtual void SetUndeterminedDisabledMaterial(const IUIMaterial::SharedPtr& pMat)
	{
		UndeterminedDisabled = pMat;
	}

	inline IUIMaterial* GetUnchecked() const { return Unchecked.get(); }
	inline IUIMaterial* GetUncheckedHovered() const { return UncheckedHovered.get(); }
	inline IUIMaterial* GetUncheckedPressed() const { return UncheckedPressed.get(); }
	inline IUIMaterial* GetChecked() const { return Checked.get(); }
	inline IUIMaterial* GetCheckedHovered() const { return CheckedHovered.get(); }
	inline IUIMaterial* GetCheckedPressed() const { return CheckedPressed.get(); }
	inline IUIMaterial* GetUndetermined() const { return Undetermined.get(); }
	inline IUIMaterial* GetUndeterminedHovered() const { return UndeterminedHovered.get(); }
	inline IUIMaterial* GetUndeterminedPressed()const { return UndeterminedPressed.get(); }
	inline IUIMaterial* GetCheckedDisabled() const { return CheckedDisabled.get(); }
	inline IUIMaterial* GetUncheckedDisabled() const { return UncheckedDisabled.get(); }
	inline IUIMaterial* GetUndeterminedDisabled() const { return UndeterminedDisabled.get(); }

	virtual void SetName(const std::string& StyleName) override { Name = StyleName; }
	virtual std::string GetName() const override { return Name; }

protected:
	IUIMaterial::SharedPtr Unchecked;
	IUIMaterial::SharedPtr UncheckedHovered;
	IUIMaterial::SharedPtr UncheckedPressed;
	IUIMaterial::SharedPtr Checked;
	IUIMaterial::SharedPtr CheckedHovered;
	IUIMaterial::SharedPtr CheckedPressed;
	IUIMaterial::SharedPtr Undetermined;
	IUIMaterial::SharedPtr UndeterminedHovered;
	IUIMaterial::SharedPtr UndeterminedPressed;
	IUIMaterial::SharedPtr CheckedDisabled;
	IUIMaterial::SharedPtr UncheckedDisabled;
	IUIMaterial::SharedPtr UndeterminedDisabled;

	std::string Name;
};
