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
#include "cbWidget.h"
#include "cbComponent.h"
#include "cbGeometry.h"

namespace cbgui
{
	/* A slider with a handle that lets you control the value between the minimum and maximum value. */
	class cbSlider : public cbWidget
	{
		cbClassBody(cbClassConstructor, cbSlider, cbWidget)
	protected:
		/* Default Slider Handle Component */
		class cbSliderHandleComponent : public cbComponent
		{
			cbClassBody(cbClassConstructor, cbSliderHandleComponent, cbComponent);
		public:
			cbSliderHandleComponent(cbSlider* pOwner);
			cbSliderHandleComponent(const cbSliderHandleComponent& eComponent, cbSlider* NewOwner);
		public:
			virtual ~cbSliderHandleComponent();

			virtual cbBounds GetBounds() const override final { return cbBounds(GetDimension(), GetLocation()); }
			virtual cbVector GetLocation() const override final;
			bool SetOffset(const float value);
			virtual cbDimension GetDimension() const override final;
			virtual float GetWidth() const override final;
			virtual float GetHeight() const override final;
			bool SetThickness(const float value);
			bool SetLength(const float value);

			float GetThickness() const;
			float GetLength() const { return Length; }

			void SetPadding(const std::int32_t value) { Padding = value; UpdateAlignments(); }
			cbMargin GetPadding() const { return Padding; }

			/* Aligns and checks Length */
			virtual void UpdateVerticalAlignment() override final;
			/* Aligns and checks Length */
			virtual void UpdateHorizontalAlignment() override final;

			virtual void UpdateRotation() override final;
			virtual void UpdateStatus() override final;

			/* Slide by percentage. */
			void Slide(const float Percent);

			inline bool IsPressed() const { return ButtonState == eButtonState::Pressed; }
			inline bool IsHovered() const { return ButtonState == eButtonState::Hovered; }
			eButtonState GetState() const { return ButtonState; }

			/* The handle fills the slider bar instead of moving. */
			void SetFillMode(bool FillMode);
			bool IsFillModeEnabled() const { return bFillMode; }
			/* According to the orientation, the handle thickens vertically or horizontally according to the thickness of the Slider. */
			void SetFillThickness(const bool value);
			bool IsFillThicknessEnabled() const { return bFillThickness; }

			bool OnMouseEnter(const cbMouseInput& Mouse);
			bool OnMouseLeave(const cbMouseInput& Mouse);
			bool OnMouseMove(const cbMouseInput& Mouse);
			bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse);
			bool OnMouseButtonUp(const cbMouseInput& Mouse);
			bool OnMouseButtonDown(const cbMouseInput& Mouse);

			/* Resets the focus and input state. */
			virtual void ResetInput() override final;

			/* Determines whether it is an active handle. */
			void SetFocus(const bool value) { bIsItFocused = value; }
			/* Indicates whether there is an active handle. */
			bool IsFocused() const { if (!GetOwner<cbSlider>()->IsFocusable()) return false; return bIsItFocused; }

		public:
			virtual bool HasGeometry() const override final { return true; }
			virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
			virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
			virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

			cbButtonVertexColorStyle GetVertexColorStyle() const { return VertexColorStyle; }
			void SetVertexColorStyle(const cbButtonVertexColorStyle& style);

		private:
			void OnPressed();
			void OnReleased();
			void OnHovered();
			void OnUnhovered();

			void SetButtonStatus(const eButtonState& ButtonStatus);

		private:
			cbButtonVertexColorStyle VertexColorStyle;
			eButtonState ButtonState;
			float Length;
			float Thickness;
			bool bIsItFocused;
			bool bFillMode;
			bool bFillThickness;
			float Offset;
			cbMargin Padding;
		};

	public:
		cbSlider(eOrientation orientation = eOrientation::Horizontal);
		cbSlider(const cbSlider& Other, cbSlot* NewOwner = nullptr);

		virtual ~cbSlider();

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

		virtual void UpdateVerticalAlignment(const bool ForceAlign = false) override final;
		virtual void UpdateHorizontalAlignment(const bool ForceAlign = false) override final;
		virtual void UpdateRotation() override final;
		virtual void UpdateStatus() override final;

		/* Set the Screen Position of the Widget.*/
		virtual void SetXY(std::optional<float> X, std::optional<float> Y, bool Force = false) override final;
		/* Set NonAligned/NonWrapped Dimension of the Widget */
		virtual void SetWidthHeight(std::optional<float> Width, std::optional<float> Height) override final;
		/* 2D Rotation. Requires Stencil Clipping during rendering, */
		virtual void SetRotation(const float Rotation) override final;
		/* Padding is used to align with offset. The padding only works if attached. */
		virtual void SetPadding(const cbMargin& Padding) override final;

		void ChangeOrientation(const eOrientation& orientation);
		inline eOrientation GetOrientation() const { return Orientation; }

		float GetBarLength() const;
		float GetBarThickness() const;

		/* Sets Slider/Bar default NonAligned width or NonAligned height based on orientation. */
		void SetBarLength(const float value);
		/* Sets bar default width or height based on orientation. */
		void SetBarThickness(const float value);

		void SetHandleThickness(const float value);
		void SetHandleLength(const float value);
		inline float GetHandleLength() const { return Handle->GetLength(); }
		inline float GetHandleThickness() const { return Handle->GetThickness(); }

		/* According to the orientation, the handle thickens vertically or horizontally according to the thickness of the Slider. */
		void SetHandleFillThickness(const bool value);
		/* Bar thickens verticallyaccording to the thickness of the Slider. */
		void SetBarVerticalFill(const bool VerticalFill);
		inline float IsBarVerticalFillEnabled() const { return bVerticalFill; }
		/* Bar thickens horizontally according to the thickness of the Slider. */
		void SetBarHorizontalFill(const bool HorizontalFill);
		inline float IsBarHorizontalFillEnabled() const { return bHorizontalFill; }
		/* The handle fills the slider bar instead of moving. */
		void SetHandleFillMode(const bool FillMode);
		inline float IsHandleFillModeEnabled() const { return Handle->IsFillModeEnabled(); }
		void SetHandlePadding(const std::int32_t Padding);
		inline cbMargin GetHandlePadding() const { return Handle->GetPadding(); }

		/* Sets the minimum value that the slider controls. */
		void SetMinimumValue(const float value);
		/* Sets the maximum value that the slider controls. */
		void SetMaximumValue(const float value);

		/* Returns current percentage. */
		inline float GetPercent() const { return Percent; }
		/* Returns current Value. */
		inline float GetValue() const { return Value; }

		/* Slides step by step. */
		inline bool UseStepOnly() const { return bUseStepOnly; }
		inline void SetStepOnly(bool value) { bUseStepOnly = value; }
		/* Step size. */
		inline float GetStepSize() const { return StepSize; }
		inline void SetStepSize(float size) { StepSize = size; }

		/* It only steps on the Integers. */
		inline void SetIntegerOnly(bool value) { bIntegerOnly = value; }
		inline bool IsIntegerOnly() const { return bIntegerOnly; }

		/* Slide by value. */
		void SetSliderValue(float value);
		/* Slide by percent. */
		void SetSliderValueByPercent(float Percentage);
		/* Slide by WheelDelta. */
		void Slide(float WheelDelta);
		/* Slide by position. */
		void Slide(const cbVector& MouseLocation);
	private:
		void ValueChanged(const float value);
		virtual void OnValueChanged(const float value) {};

	public:
		virtual bool HasAnyChildren() const override final { return Handle != nullptr; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>{ Handle.get() }; }

		virtual bool HasAnyComponents() const override final { return Handle != nullptr; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>{ Handle.get() }; }

		inline void BindFunctionTo_OnValueChanged(std::function<void(float)> Function) { fOnValueChanged = Function; }
		inline void UnBindFunctionTo_OnValueChanged() { fOnValueChanged = nullptr; }

		virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;
		virtual bool OnMouseMove(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) override;
		virtual bool OnMouseWheel(float WheelDelta, const cbMouseInput& Mouse) override;

		/* Resets the focus and input state. */
		virtual void ResetInput() override final;

	public:
		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

		cbVertexColorStyle GetBarVertexColorStyle() const { return VertexColorStyle; }
		cbButtonVertexColorStyle GetHandleVertexColorStyle() const { return Handle->GetVertexColorStyle(); }
		void SetBarVertexColorStyle(const cbVertexColorStyle& style);
		void SetHandleVertexColorStyle(const cbButtonVertexColorStyle& style);

		void SetHandleName(const std::string Name) const { Handle->SetName(Name); }

	private:
		virtual void OnAttach() override final;
		virtual void OnRemoveFromParent() override final;

	private:
		cbVertexColorStyle VertexColorStyle;
		cbTransform Transform;
		float Thickness;
		bool bVerticalFill;
		bool bHorizontalFill;
		bool bIntegerOnly;
		bool bUseStepOnly;

		float Value;
		float MinValue;
		float MaxValue;
		float StepSize;
		float Percent;

		eOrientation Orientation;
		cbSliderHandleComponent::UniquePtr Handle;

		std::function<void(float)> fOnValueChanged;
	};
}
