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
#include "cbGeometry.h"

namespace cbgui
{
	/* A button is a clickable widget to enable basic interaction. */
	class cbButton : public cbWidget
	{
		cbClassBody(cbClassConstructor, cbButton, cbWidget)
	private:
		cbButtonVertexColorStyle VertexColorStyle;
		cbTransform Transform;

		std::function<void()> fOnClicked;
		std::function<void()> fOnPressed;
		std::function<void()> fOnReleased;
		std::function<void()> fOnHovered;
		std::function<void()> fOnUnhovered;

		eButtonState ButtonState;

	public:
		cbButton();
		cbButton(const cbButton& Other, cbSlot* NewOwner = nullptr);
		virtual ~cbButton();

		virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override;

	private:
		virtual void BeginPlay() override final;
		virtual void Tick(float DeltaTime) override final;

		virtual void OnBeginPlay() {}
		virtual void OnTick(float DeltaTime) {}

	public:
		virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
		virtual cbBounds GetBounds() const override final { return Transform.GetBounds(); }
		virtual float GetRotation() const override final;
		virtual cbMargin GetPadding() const override final { return Transform.GetPadding(); }
		virtual cbDimension GetDimension() const override final { return Transform.GetDimension(); }
		virtual float GetWidth() const override final { return Transform.GetWidth(); }
		virtual float GetHeight() const override final { return Transform.GetHeight(); }

		virtual float GetNonAlignedWidth() const override final { return Transform.GetNonAlignedWidth(); }
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

		/* Called when clicked. */
		inline void BindFunctionTo_OnClicked(std::function<void()> Function) { fOnClicked = Function; }
		inline void UnBindFunctionTo_OnClicked() { fOnClicked = nullptr; }
		/* Called when Pressed. */
		inline void BindFunctionTo_OnPressed(std::function<void()> Function) { fOnPressed = Function; }
		inline void UnBindFunctionTo_OnPressed() { fOnPressed = nullptr; }
		/* Called when Released. */
		inline void BindFunctionTo_OnReleased(std::function<void()> Function) { fOnReleased = Function; }
		inline void UnBindFunctionTo_OnReleased() { fOnReleased = nullptr; }
		/* Called when Hovered. */
		inline void BindFunctionTo_OnHovered(std::function<void()> Function) { fOnHovered = Function; }
		inline void UnBindFunctionTo_OnHovered() { fOnHovered = nullptr; }
		/* Called when Unhovered. */
		inline void BindFunctionTo_OnUnhovered(std::function<void()> Function) { fOnUnhovered = Function; }
		inline void UnBindFunctionTo_OnUnhovered() { fOnUnhovered = nullptr; }

		inline bool IsPressed() const { return ButtonState == eButtonState::Pressed; }
		inline bool IsHovered() const { return ButtonState == eButtonState::Hovered; }

		eButtonState GetState() const;

		bool Clicked();
		bool Pressed();
		bool Released();
		bool Hovered();
		bool Unhovered();
		/* Resets to the default state. */
		void Reset();

	private:
		void SetButtonStatus(const eButtonState& ButtonStatus);

		virtual void OnClicked() {}
		virtual void OnPressed() {}
		virtual void OnReleased() {}
		virtual void OnHovered() {}
		virtual void OnUnhovered() {}

	public:
		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

		cbButtonVertexColorStyle GetVertexColorStyle() const { return VertexColorStyle; }
		void SetVertexColorStyle(const cbButtonVertexColorStyle& style);
		void SetVertexColorStyle(const eButtonState State, const cbColor& Color);

		virtual bool HasAnyChildren() const override final { return false; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>(); }

		virtual bool HasAnyComponents() const override final { return false; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>(); }

	public:
		virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;
		virtual bool OnMouseMove(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDoubleClick(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonUp(const cbMouseInput& Mouse) override;
		virtual bool OnMouseButtonDown(const cbMouseInput& Mouse) override;

		/* Resets the focus and input state. */
		virtual void ResetInput() override final;

	private:
		virtual void OnAttach() override final;
		virtual void OnRemoveFromParent() override final;
	};
}
