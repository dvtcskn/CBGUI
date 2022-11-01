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

namespace cbgui
{
	/* Base class for Components. */
	struct cbComponent : public cbWidgetObj
	{
		cbClassBody(cbClassNoDefaults, cbComponent, cbWidgetObj)
	private:
		cbWidget* Owner;
		cbComponent* ComponentOwner;
		std::optional<std::string> Name;

	public:
		cbComponent(cbWidget* pOwner)
			: Owner(pOwner)
			, ComponentOwner(nullptr)
			, Name(std::nullopt)
		{}
		cbComponent(cbComponent* pComponentOwner)
			: Owner(pComponentOwner->Owner)
			, ComponentOwner(pComponentOwner)
			, Name(std::nullopt)
		{}

	public:
		virtual ~cbComponent()
		{
			Owner = nullptr;
			ComponentOwner = nullptr;
		}

		virtual void BeginPlay() {}
		virtual void Tick(float DeltaTime) {}

		virtual std::string GetName() const override final
		{
			if (Name.has_value())
				return Name.value();

			if (IsItAttachedToComponent())
				return ComponentOwner->GetName() + "::" + GetClassID();
			return Owner->GetName() + "::" + GetClassID();
		}
		inline void SetName(const std::string& inName) { Name = inName; }

		bool IsItAttachedToComponent() const { return ComponentOwner != nullptr; }
		template<class T>
		T* GetComponentOwner(const bool Dynamic = false) const
		{
			if (Dynamic)
				return dynamic_cast<T*>(GetComponentOwner());
			return static_cast<T*>(GetComponentOwner());
		}
		cbComponent* GetComponentOwner() const { return ComponentOwner; }

		virtual bool HasOwner() const override final { return Owner != nullptr; }
		template<class T>
		constexpr T* GetOwner(const bool Dynamic = false) const
		{
			if (Dynamic)
				return dynamic_cast<T*>(GetOwner());
			return static_cast<T*>(GetOwner());
		}

		virtual cbWidgetObj* GetOwner() const override final { return Owner; }

		virtual bool HasAnyChildren() const override { return false; }
		virtual std::vector<cbWidgetObj*> GetAllChildren() const override { return std::vector<cbWidgetObj*>(); }

		virtual bool HasAnyComponents() const { return false; }
		virtual std::vector<cbComponent*> GetAllComponents() const { return std::vector<cbComponent*>(); }

		virtual bool HasGeometry() const override { return false; }
		virtual std::vector<cbGeometryVertexData> GetVertexData(const bool LineGeometry = false) const override { return std::vector<cbGeometryVertexData>(); };
		virtual std::vector<std::uint32_t> GetIndexData(const bool LineGeometry = false) const override { return std::vector<std::uint32_t>(); };
		virtual cbGeometryDrawData GetGeometryDrawData(const bool LineGeometry = false) const override { return cbGeometryDrawData("NONE", 0, 0, 0, 0); };

		virtual std::int32_t GetZOrder() const override { return IsItAttachedToComponent() ? ComponentOwner->GetZOrder() : Owner->GetZOrder(); }
		virtual eZOrderMode GetZOrderMode() const override { return IsItAttachedToComponent() ? ComponentOwner->GetZOrderMode() : Owner->GetZOrderMode(); }

		bool HasCanvas() const { return Owner->HasCanvas(); }
		cbICanvas* GetCanvas() const { return Owner->GetCanvas(); }

		bool NotifyCanvas_WidgetUpdated()
		{
			if (HasCanvas())
			{
				GetCanvas()->WidgetUpdated(this);
				return true;
			}
			return false;
		}

		void UpdateAlignments() { UpdateVerticalAlignment(); UpdateHorizontalAlignment(); }
		virtual void UpdateVerticalAlignment() = 0;
		virtual void UpdateHorizontalAlignment() = 0;
		virtual void UpdateRotation() = 0;
		virtual void UpdateStatus() = 0;

		virtual cbBounds GetBounds() const override { return IsItAttachedToComponent() ? ComponentOwner->GetBounds() : Owner->GetBounds(); }
		virtual cbVector GetLocation() const { return GetBounds().GetCenter(); }
		virtual cbDimension GetDimension() const override { return GetBounds().GetDimension(); }
		virtual float GetWidth() const override { return GetDimension().GetWidth(); }
		virtual float GetHeight() const override { return GetDimension().GetHeight(); }
		
		virtual cbVector GetOrigin() const override { return Owner->GetOrigin(); }
		virtual float GetRotation() const override { return IsItAttachedToComponent() ? ComponentOwner->GetRotation() : Owner->GetRotation(); }

		/* Resets the focus and input state. */
		virtual void ResetInput() = 0;

		virtual bool IsInside(const cbVector& Location) const
		{
			if (GetRotation() != 0.0f)
				return cbgui::IsInside(GetBounds(), Location, GetRotation(), GetOrigin());
			return cbgui::IsInside(GetBounds(), Location);
		}
		virtual bool Intersect(const cbBounds& Other) const
		{
			if (GetRotation() != 0.0f)
				return cbgui::Intersect(GetBounds(), Other, GetRotation(), GetOrigin());
			return cbgui::Intersect(GetBounds(), Other);
		}

		virtual bool IsItCulled() const override { return IsItAttachedToComponent() ? ComponentOwner->IsItCulled() : Owner->IsItCulled(); }
		virtual cbBounds GetCulledBounds() const override { return IsItAttachedToComponent() ? ComponentOwner->GetCulledBounds() : Owner->GetCulledBounds(); }

		virtual bool IsEnabled() const override { return IsItAttachedToComponent() ? ComponentOwner->IsEnabled() : Owner->IsEnabled(); }

		virtual bool IsVisible() const override { return IsItAttachedToComponent() ? ComponentOwner->IsVisible() : Owner->IsVisible(); }
		virtual bool IsInvisible() const override { return IsItAttachedToComponent() ? ComponentOwner->IsInvisible() : Owner->IsInvisible(); }
		virtual bool IsHidden() const override { return IsItAttachedToComponent() ? ComponentOwner->IsHidden() : Owner->IsHidden(); }
	};
}
