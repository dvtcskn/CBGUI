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
	/* The checkbox widget allows you to switch between unchecked, checked and Undetermined states. */
	class cbCheckBox : public cbWidget
	{
		cbClassBody(cbClassConstructor, cbCheckBox, cbWidget)
	private:
		cbCheckBoxVertexColorStyle VertexColorStyle;
		cbTransform Transform;

		std::function<void(eCheckBoxState)> fOnCheckStateChanged;
		eCheckBoxState CheckBoxState;

		bool bIsPressed;
		bool bIsHovered;

	public:
		cbCheckBox();
		cbCheckBox(const cbCheckBox& Other, cbSlot* NewOwner = nullptr);
		virtual ~cbCheckBox();

		virtual cbWidget::SharedPtr CloneWidget(cbSlot* NewOwner = nullptr) override;

	private:
		virtual void BeginPlay() override final;
		virtual void Tick(float DeltaTime) override final;

		virtual void OnBeginPlay() {}
		virtual void OnTick(float DeltaTime) {}

	public:
		virtual cbVector GetLocation() const override final { return Transform.GetCenter(); }
		virtual float GetRotation() const override final { return Transform.GetRotation(); }
		virtual cbBounds GetBounds() const override final { return Transform.GetBounds(); }
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

		eCheckBoxState GetState() const;

		void SetStateToChecked();
		void SetStateToUnchecked();
		void SetStateToUndetermined();
		/* 
		* Checks if unchecked.
		* Unchecks if checked or undetermined.
		*/
		void ToggleCheckBoxState();

		bool IsChecked() const;
		bool IsUndetermined() const;

		/* Called when clicked or check state changed. */
		inline void BindFunctionTo_OnCheckStateChanged(std::function<void(eCheckBoxState)> Function) { fOnCheckStateChanged = Function; }
		inline void UnBindFunctionTo_OnCheckStateChanged() { fOnCheckStateChanged = nullptr; }

		bool IsPressed() const { return bIsPressed; }
		bool IsHovered() const { return bIsHovered; }

		bool Clicked();
		bool Pressed();
		bool Released();
		bool Hovered();
		bool Unhovered();

	private:
		void SetCheckBoxState(const eCheckBoxState& CheckState);

		/* Called when check state changed. */
		virtual void OnCheckBoxStateChanged(const eCheckBoxState CheckState) {}

	public:
		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;

		cbCheckBoxVertexColorStyle GetVertexColorStyle() const { return VertexColorStyle; }
		void SetVertexColorStyle(const cbCheckBoxVertexColorStyle& style);
		void SetVertexColorStyle(const eCheckBoxState State, const cbColor& Color);

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
