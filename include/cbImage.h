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

#include "cbWidget.h"
#include "cbGeometry.h"

namespace cbgui
{
	/* The image widget allows you to display color, image, texture or material. */
	class cbImage : public cbWidget
	{
		cbClassBody(cbClassConstructor, cbImage, cbWidget);
	public:
		cbImage();
		cbImage(const cbImage& Other);

		virtual ~cbImage();

		virtual cbWidget::SharedPtr CloneWidget() override;

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

		virtual bool OnMouseEnter(const cbMouseInput& Mouse) override;
		virtual bool OnMouseLeave(const cbMouseInput& Mouse) override;

		/* Resets the focus and input state. */
		virtual void ResetInput() override final;

		virtual bool HasAnyChildren() const override final { return false; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override final { return std::vector<cbWidgetObj*>(); }

		virtual bool HasAnyComponents() const override final { return false; }
		virtual std::vector<cbComponent*> GetAllComponents() const override final { return std::vector<cbComponent*>(); }

	public:
		virtual bool HasGeometry() const override final { return true; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override final;
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override final;
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override final;
		cbVertexColorStyle GetVertexColorStyle() const { return VertexColorStyle; }
		void SetVertexColorStyle(const cbVertexColorStyle& style);

	private:
		virtual void OnAttach() override final;
		virtual void OnRemoveFromParent() override final;

	private:
		cbTransform Transform;
		cbVertexColorStyle VertexColorStyle;
	};
}
