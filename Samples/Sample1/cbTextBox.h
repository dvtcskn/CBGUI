/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Co�kun.
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

#include "cbSlottedBox.h"
#include "cbString.h"
#include "cbImage.h"
#include "cbComponent.h"
#include "UIAnimation.h"
#include "cbCanvas.h"

using namespace cbgui;

class cbTextBox : public cbgui::cbSlottedBox
{
	cbClassBody(cbClassConstructor, cbTextBox, cbSlottedBox);
protected:
	class cbTextSlot : public cbSlot
	{
		cbClassBody(cbClassConstructor, cbTextSlot, cbSlot)
		friend cbTextBox;
	private:
		bool bIsInserted;
		std::shared_ptr<cbString> Content;
		cbVector Location;

		void Inserted() { bIsInserted = true; OnInserted(); }
		virtual void OnInserted() { }

	public:
		cbTextSlot(cbTextBox* pOwner, const std::u32string& Text = U"", const cbTextDesc& TextDesc = cbTextDesc())
			: Super(pOwner)
			, bIsInserted(false)
			, Content(cbString::Create(Text, TextDesc))
			, Location(cbVector::Zero())
		{
			Content->AttachToSlot(this);
			Content->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Content->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Content->SetTextJustify(eTextJustify::Left);
			Content->SetVertexColorStyle(cbColor::White());
		}

		cbTextSlot(const cbTextSlot& Other, cbSlottedBox* NewOwner)
			: Super(Other, NewOwner)
			, bIsInserted(false)
			, Content(Other.Content->Clone<cbString>())
			, Location(Other.Location)
		{
			Content->AttachToSlot(this);
			Content->SetVerticalAlignment(eVerticalAlignment::Align_Fill);
			Content->SetHorizontalAlignment(eHorizontalAlignment::Align_Fill);
			Content->SetTextJustify(eTextJustify::Left);
			Content->SetVertexColorStyle(cbColor::White());
		}

		virtual ~cbTextSlot()
		{
			Content = nullptr;
		}

		virtual cbSlot::SharedPtr CloneSlot(cbSlottedBox* NewOwner) override
		{
			return cbTextSlot::Create(*this, NewOwner);
		}

	public:
		std::shared_ptr<cbString> GetText() const { return Content; }

		virtual bool IsInserted() const override final { return HasOwner() && bIsInserted; }
		void ReplaceContent(const cbWidget::SharedPtr& pContent);

		virtual bool HasContent() const override { return Content != nullptr; }
		virtual cbWidget::SharedPtr GetSharedContent() const override { return Content; }
		virtual cbWidget* GetContent() const override { return Content.get(); }

		virtual bool HasAnyChildren() const { return Content != nullptr; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const { return std::vector<cbWidgetObj*>{ Content.get() }; }

		virtual void Hidden(bool value) { Content->SetVisibilityState(value ? eVisibility::Hidden : eVisibility::Visible); }

		virtual cbVector GetLocation() const override { return Location; }
		virtual cbDimension GetDimension() const override { return GetOwner()->GetDimension(); }
		virtual float GetWidth() const override { return GetDimension().GetWidth(); }
		virtual float GetHeight() const override { return GetDimension().GetHeight(); }
		virtual cbBounds GetBounds() const override { return cbBounds(GetDimension(), GetLocation()); }

		virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const { return Content->IsInteractableWithKey(KeyCode); }
		virtual bool OnKeyUp(std::uint64_t KeyCode) { return Content->OnKeyUp(KeyCode); }
		virtual bool OnKeyDown(std::uint64_t KeyCode) { return Content->OnKeyDown(KeyCode); }
		virtual bool IsInteractableWithMouse() const { return Content->IsInteractableWithMouse(); }
		virtual bool OnMouseEnter(const cbMouseInput& Mouse) { return Content->OnMouseEnter(Mouse); }
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) { return Content->OnMouseLeave(Mouse); }
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) { return Content->OnMouseWheel(WheelDelta, Mouse); }
		virtual bool OnMouseMove(const cbMouseInput& Mouse) { return Content->OnMouseMove(Mouse); }
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) { return Content->OnMouseButtonDoubleClick(Mouse); }
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) { return Content->OnMouseButtonUp(Mouse); }
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) { return Content->OnMouseButtonDown(Mouse); }

		inline void UpdateVerticalAlignment()
		{
			if (!HasContent() || !HasOwner())
				return;

			Location.Y = GetOwner<cbTextBox>()->GetLocation().Y;

			Content->UpdateVerticalAlignment();
		}

		inline void UpdateHorizontalAlignment()
		{
			if (!HasContent() || !HasOwner())
				return;

			Location.X = GetOwner<cbTextBox>()->GetLocation().X;

			Content->UpdateHorizontalAlignment();
		}

		virtual void UpdateRotation() { if (HasContent()) Content->UpdateRotation(); }
	};

	enum class eCursorPosition
	{
		Left,
		Right,
	};

	class cbCursor final : public cbComponent
	{
		cbClassBody(cbClassConstructor, cbCursor, cbComponent);
	public:
		float ElapsedTime;
		bool bIsEnabled;
		bool bIsHidden;
		float Height;
		float Thickness;
		cbVector CursorLocation;
		eCursorPosition CursorPosition;

	public:
		cbCursor(cbTextBox* Owner, float inHeight = 24.0f, float inWidth = 2.0f)
			: cbComponent(Owner)
			, ElapsedTime(0.0f)
			, bIsHidden(false)
			, bIsEnabled(false)
			, Height(inHeight)
			, Thickness(inWidth)
			, CursorLocation(cbVector::Zero())
			, CursorPosition(eCursorPosition::Left)
		{}

		cbCursor(const cbCursor& Other, cbTextBox* Owner)
			: cbComponent(Owner)
			, ElapsedTime(Other.ElapsedTime)
			, bIsHidden(Other.bIsHidden)
			, bIsEnabled(Other.bIsEnabled)
			, Height(Other.Height)
			, Thickness(Other.Thickness)
			, CursorLocation(Other.CursorLocation)
			, CursorPosition(Other.CursorPosition)
		{
			SetName(Other.GetName());
		}

		virtual ~cbCursor()
		{
		}

		virtual void Tick(float DT)
		{
			if (!IsEnabled())
				return;
			ElapsedTime += DT;
			if (ElapsedTime >= 0.5f)
			{
				Hidden(true);
			}
			else
			{
				Hidden(false);
			}

			if (ElapsedTime >= 1.0f)
			{
				ElapsedTime = 0.0f;
			}
		}

		virtual void ResetInput() override
		{
			Disable();
			Hidden(true);
		}

		virtual bool IsVisible() const override final { if (GetOwner()->IsHidden()) return false; return !bIsHidden; }
		virtual bool IsHidden() const override final { if (GetOwner()->IsHidden()) return true; return bIsHidden; }
		inline void Hidden(bool value)
		{
			if (bIsHidden != value)
			{
				bIsHidden = value;
				if (HasCanvas())
					GetCanvas()->VisibilityChanged(this);
			}
		}
		virtual bool IsEnabled() const override final { return bIsEnabled; }
		void Enable() { bIsEnabled = true; Hidden(false); }
		void Disable() { bIsEnabled = false; Hidden(true); }

		virtual cbBounds GetBounds() const { return cbBounds(GetDimension(), GetLocation()); }
		virtual cbVector GetLocation() const { return CursorLocation; }
		virtual cbDimension GetDimension() const { return cbDimension(Thickness, Height); }
		virtual float GetWidth() const { return Thickness; }
		virtual float GetHeight() const { return Height; }

		void SetCursorPosition(eCursorPosition Pos) { CursorPosition = Pos; UpdateHorizontalAlignment(); }

		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry) const override final
		{
			float Rotation = GetRotation();
			cbColor Color = cbColor::Black();
			auto Alpha = GetVertexColorAlpha();
			if (Alpha.has_value())
				Color.A = *Alpha;

			return cbGeometryFactory::GetAlignedVertexData(cbGeometryFactory::Create4DPlaneVerticesFromRect(GetDimension()),
				cbGeometryFactory::GeneratePlaneTextureCoordinate(),
				cbColor::Black(),
				GetLocation(), Rotation, Rotation != 0.0f ? GetRotatorOrigin() : cbVector::Zero());
		}

		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry) const override final
		{
			return cbGeometryFactory::GeneratePlaneIndices();
		}

		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry) const override final
		{
			return cbGeometryDrawData("Plane", 0, 4, 6, 6);
		};

		void NotifyCanvas_WidgetUpdated()
		{
			if (HasCanvas())
				GetCanvas()->WidgetUpdated(this);
		}

		void SetPositionX(const float value)
		{
			if (CursorLocation.X != value)
			{
				CursorLocation.X = value;
				NotifyCanvas_WidgetUpdated();
			}
		}

		void SetPositionY(const float value)
		{
			if (CursorLocation.Y != value)
			{
				CursorLocation.Y = value;
				NotifyCanvas_WidgetUpdated();
			}
		}

		void SetThickness(float inThickness)
		{
			if (Thickness != inThickness)
			{
				Thickness = inThickness;
				NotifyCanvas_WidgetUpdated();
			}
		}
		void SetHeight(float inHeight)
		{
			if (Height != inHeight)
			{
				Height = inHeight;
				NotifyCanvas_WidgetUpdated();
			}
		}

		virtual void UpdateVerticalAlignment() override
		{
			std::optional<cbBounds> SelectedLine = GetOwner<cbTextBox>()->GetCurrentSelectedLineBounds();
			if (SelectedLine.has_value())
			{
				SetPositionY(SelectedLine.value().GetCenter().Y);
				SetHeight(SelectedLine.value().GetHeight());
			}
		}

		virtual void UpdateHorizontalAlignment() override
		{
			std::optional<cbBounds> SelectedChar = GetOwner<cbTextBox>()->GetCursorSelectedCharBounds();
			if (SelectedChar.has_value())
			{
				if (CursorPosition == eCursorPosition::Left)
					SetPositionX(SelectedChar.value().Min.X);
				else if (CursorPosition == eCursorPosition::Right)
					SetPositionX(SelectedChar.value().Max.X);
			}
		}

		virtual void UpdateRotation() { NotifyCanvas_WidgetUpdated(); }
		virtual void UpdateStatus() { NotifyCanvas_WidgetUpdated(); }
	};


	class TextHighlight final : public cbComponent
	{
		cbClassBody(cbClassConstructor, TextHighlight, cbComponent);
	public:
		bool bIsEnabled;
		bool bIsHidden;
		std::vector<cbBounds> Highlights;
		std::optional<std::size_t> HighlightStartIndex;
		std::optional<std::size_t> HighlightEndIndex;

	public:
		TextHighlight(cbTextBox* Owner)
			: cbComponent(Owner)
			, bIsHidden(false)
			, bIsEnabled(true)
			, HighlightStartIndex(std::nullopt)
			, HighlightEndIndex(std::nullopt)
		{}

		TextHighlight(const TextHighlight& Other, cbTextBox* Owner)
			: cbComponent(Owner)
			, bIsHidden(Other.bIsHidden)
			, bIsEnabled(Other.bIsEnabled)
			, HighlightStartIndex(Other.HighlightStartIndex)
			, HighlightEndIndex(Other.HighlightEndIndex)
		{
			SetName(Other.GetName());
		}

		virtual ~TextHighlight()
		{
			Highlights.clear();
		}

		virtual void ResetInput() override
		{
			Disable();
			Hidden(true);
		}

		virtual bool IsVisible() const override final { if (GetOwner()->IsHidden()) return false; return !bIsHidden; }
		virtual bool IsHidden() const override final { if (GetOwner()->IsHidden()) return true; return bIsHidden; }
		inline void Hidden(bool value)
		{
			if (bIsHidden != value)
			{
				bIsHidden = value;
				if (HasCanvas())
					GetCanvas()->VisibilityChanged(this);
			}
		}
		virtual bool IsEnabled() const override final { return bIsEnabled; }
		void Enable() { bIsEnabled = true; Hidden(false); }
		void Disable() { bIsEnabled = false; Hidden(true); }

		virtual cbBounds GetBounds() const { return cbBounds(GetDimension(), GetLocation()); }
		virtual cbVector GetLocation() const { return GetOwner<cbTextBox>()->GetLocation(); }
		virtual cbDimension GetDimension() const { return GetOwner()->GetDimension(); }
		virtual float GetWidth() const { return GetOwner()->GetWidth(); }
		virtual float GetHeight() const { return GetOwner()->GetHeight(); }

		bool IsHighlighted() const { return Highlights.size() > 0 && HighlightStartIndex.has_value() && HighlightEndIndex.has_value(); }
		void ClearHighlights()
		{
			HighlightStartIndex = std::nullopt;
			HighlightEndIndex = std::nullopt;
			Highlights.clear();

			if (cbICanvas* Canvas = GetCanvas())
			{
				Canvas->VerticesSizeChanged(this, Highlights.size() * 6);
				Canvas->WidgetUpdated(this);
			}
		}

		void SetHighlight(float StartChar, float EndChar, float LineTop, float LineBottom)
		{
			Highlights.push_back(cbBounds(cbVector(StartChar, LineTop), cbVector(EndChar, LineBottom)));

			Enable();
			Hidden(false);

			if (cbICanvas* Canvas = GetCanvas())
			{
				Canvas->VerticesSizeChanged(this, Highlights.size() * 6);
				Canvas->WidgetUpdated(this);
			}
		}

		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry) const override final
		{
			std::vector<cbGeometryVertexData> Container;

			auto Vertices = GenerateVertices();
			if (Vertices.size() == 0)
				return Container;

			auto TextureCoordinates = GenerateTextureCoordinate();
			std::size_t VerticesSize = Vertices.size();

			auto Rotation = GetRotation();
			auto Origin = GetRotation() != 0.0f ? GetRotatorOrigin() : cbVector::Zero();

			for (std::size_t i = 0; i < VerticesSize; i++)
			{
				cbGeometryVertexData GeometryData;
				GeometryData.position = Vertices[i];
				GeometryData.texCoord = TextureCoordinates[i];
				GeometryData.Color = cbColor(149, 179, 211, 255 / 2);

				if (Rotation != 0.0f)
					GeometryData.position = cbgui::RotateVectorAroundPoint(GeometryData.position, Origin, Rotation);

				Container.push_back(GeometryData);
			}

			return Container;
		}

		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry) const override final
		{
			return GenerateIndices();
		}

		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry) const override final
		{
			return cbGeometryDrawData("Plane", 0, 4 * Highlights.size(), 6 * Highlights.size(), 6 * Highlights.size());
		};

		std::vector<cbVector4> GenerateVertices() const;
		std::vector<cbVector> GenerateTextureCoordinate() const;
		std::vector<std::uint32_t> GenerateIndices() const;

		virtual void UpdateVerticalAlignment() override
		{
			if (IsHidden() || !IsEnabled())
				return;
			NotifyCanvas_WidgetUpdated();
		}

		virtual void UpdateHorizontalAlignment() override
		{
			if (IsHidden() || !IsEnabled())
				return;
			NotifyCanvas_WidgetUpdated();
		}

		virtual void UpdateRotation() { NotifyCanvas_WidgetUpdated(); }
		virtual void UpdateStatus() { NotifyCanvas_WidgetUpdated(); }
	};

public:
	enum class eTextBoxType
	{
		All,
		Letter,
		Integer,
		Float,
	};

public:
	cbTextBox();
	cbTextBox(const std::u32string& Text, const cbTextDesc& TextDesc = cbTextDesc());
	cbTextBox(const cbTextBox& TextBox);

public:
	virtual ~cbTextBox();

	virtual cbWidget::SharedPtr CloneWidget() override;

public:
	virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
	virtual float GetRotation() const override final;
	virtual cbBounds GetBounds() const override final { return Transform.GetBounds(); }
	virtual cbMargin GetPadding() const override final { return Transform.GetPadding(); }
	virtual cbDimension GetDimension() const override final { return Transform.GetDimension(); }
	virtual float GetWidth() const override final { return Transform.GetWidth(); }
	virtual float GetHeight() const override final { return Transform.GetHeight(); }

	virtual float GetNonAlignedWidth() const override final { return Transform.GetNonAlignedWidth(); }
	virtual float GetNonAlignedHeight() const override final { return Transform.GetNonAlignedHeight(); }

	virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false);
	virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height);
	virtual void SetPadding(const cbMargin& Padding) override final;
	virtual void SetRotation(const float Rotation) override final;

	virtual cbSlot* GetSlot(const std::size_t Index = 0) const override final { return Index == 0 ? TextSlot.get() : nullptr; }
	virtual std::vector<cbSlot*> GetSlots() const override { return std::vector<cbSlot*>{ TextSlot.get() }; }
	virtual std::size_t GetSlotSize(const bool ExcludeHidden = false) const override final { if (TextSlot && (ExcludeHidden && !TextSlot->IsHidden())) return 1; return 0; }

	virtual std::vector<cbWidgetObj*> GetAllChildren() const override;
	virtual bool HasAnyChildren() const override { return TextSlot != nullptr; }

	virtual bool HasAnyComponents() const override final { return true; }
	virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>{ Cursor.get(), Highlight.get() }; }

	virtual bool HasGeometry() const override final { return false; }
	virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final { return std::vector<cbGeometryVertexData>(); };
	virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final { return std::vector<std::uint32_t>(); };
	virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final { return cbGeometryDrawData("NONE", 0, 0, 0, 0); };

	cbSlot* GetOverlappingSlot(const cbBounds& Bounds) const;

	virtual bool IsInteractableWithKey(std::optional<std::uint64_t> KeyCode = std::nullopt) const { return true; }
	virtual bool OnKeyDown(std::uint64_t KeyCode);
	virtual bool OnMouseEnter(const cbMouseInput& Mouse);
	virtual bool OnMouseLeave(const cbMouseInput& Mouse);
	virtual bool OnMouseMove(const cbMouseInput& Mouse);
	virtual bool OnMouseButtonUp(const cbMouseInput& Mouse);
	virtual bool OnMouseButtonDown(const cbMouseInput& Mouse);

	virtual void UpdateVerticalAlignment(const bool ForceAlign = false) override final;
	virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) override final;
	virtual void UpdateRotation() override final;
	virtual void UpdateStatus() override final;

	const std::u32string GetText() const;
	void SetText(const std::u32string& Text, const std::optional<cbTextDesc> Desc = std::nullopt);
	void SetText(const std::string& Text, const std::optional<cbTextDesc> Desc = std::nullopt);
	void SetText(const cbString::SharedPtr& Text);
	void SetVertexColorStyle(const cbVertexColorStyle& style);
	void AppendText(const std::u32string& Text);
	void AddChar(const char32_t pChar, const std::optional<std::size_t> Index = std::nullopt, bool Filtered = true);
	void AddChar(const char32_t* pChar, const std::optional<std::size_t> Index = std::nullopt, bool Filtered = true);
	void SetFontType(const eFontType& FontType);
	void SetFontSize(const std::size_t& Size);
	void SetTextJustify(eTextJustify Justify);
	std::size_t GetTextSize(bool Filtered = false) const;
	bool RemoveChar(std::size_t InIndex);
	bool RemoveChars(const std::vector<std::size_t>& Indices);
	void ClearText();

	std::vector<cbTextStyle> GetTextStyles() const;
	void AddTextStyle(const cbTextStyle& Style);
	void RemoveTextStyle(const std::size_t& Index);
	void SetTextStylingEnabled(const bool& Value);
	void SetAscenderDescenderPowerOfTwo(const bool& Value);

	void SetAutoWrap(const bool& Value);
	bool IsAutoWrapEnabled() const;
	void SetCustomWrapWidth(const std::optional<float> value);
	float GetAutoWrapWidth() const;

	void SetTextBoxType(eTextBoxType Type);
	void CharacterLimit(std::size_t Limit);
	std::optional<std::size_t> GetCharacterLimit() const noexcept { return Limit; }
	void ClearCharacterLimit() { Limit = std::nullopt; }
	void EnableEditMode();
	void DisableEditMode();

	void BlockChar(char32_t Char);
	void UnblockChar(char32_t Char);
	void UnblockChars() { BlockChars.clear(); }
	bool IsCharacterBlocked(char32_t Char) const;

protected:
	void WrapOrUpdateAlignments();

protected:
	virtual void UpdateSlotVerticalAlignment() override final;
	virtual void UpdateSlotHorizontalAlignment() override final;

	std::optional<cbBounds> GetCursorSelectedCharBounds() const;
	std::optional<cbBounds> GetCurrentSelectedLineBounds() const;

	void UpdateHighlight();

private:
	virtual std::size_t GetSlotIndex(const cbSlot* Slot) const { return 0; }

	virtual void OnAttach() override;
	virtual void OnRemoveFromParent() override;

	virtual bool WrapVertical() override final;
	virtual bool WrapHorizontal() override final;
	virtual bool UnWrapVertical() override final;
	virtual bool UnWrapHorizontal() override final;

	void SlotContentReplaced(cbTextSlot* Slot);

	virtual void OnSlotVisibilityChanged(cbSlot* Slot);
	virtual void OnSlotDimensionUpdated(cbSlot* Slot);

	virtual bool OnRemoveSlot(cbSlot* Slot) override;

	void InputCallback();

public:
	std::function<void(std::u32string)> fStringInputCallback;
	std::function<void(double)> fInputCallback;

	std::int32_t GetInteger() const;
	float GetFloat() const;

private:
	cbTransform Transform;
	cbTextSlot::SharedPtr TextSlot;
	std::optional<std::uint32_t> SelectedIndex;
	std::optional<std::size_t> PastSelectedIndex;
	bool Pressed;

	cbCursor::SharedPtr Cursor;
	TextHighlight::SharedPtr Highlight;

	bool bEditMode;
	eTextBoxType TextBoxType;
	std::optional<std::size_t> Limit;
	std::vector<char32_t> BlockChars;
};
