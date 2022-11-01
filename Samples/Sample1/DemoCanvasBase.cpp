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
#include "DemoCanvasBase.h"

#define CanvasConsole(x) std::cout << x << std::endl;

DemoCanvasBase::DemoCanvasBase(WindowsPlatform* WPlatformOwner)
	: Super(WPlatformOwner)
	, Transform(cbTransform(cbDimension((float)WPlatformOwner->GetWindowWidth(), (float)WPlatformOwner->GetWindowHeight())))
	, PrevVertexOffset(0)
	, PrevIndexOffset(0)
	, Animation(cbAnimation::Create())
{
	Transform.SetLocation(cbVector((float)WPlatformOwner->GetWindowWidth() / 2.0f, (float)WPlatformOwner->GetWindowHeight() / 2.0f));

	{
		sBufferDesc Desc;
		Desc.Size = 3000000;// (std::uint32_t)(sizeof(cbGeometryVertexData));
		Desc.Stride = sizeof(cbGeometryVertexData);
		VertexBuffer = std::make_unique<AVertexBuffer>(Desc, nullptr);
	}
	{
		sBufferDesc Desc;
		Desc.Size = 300000;// (std::uint32_t)(sizeof(int));
		Desc.Stride = 0;
		IndexBuffer = std::make_unique<AIndexBuffer>(Desc, nullptr);
	}
}

DemoCanvasBase::DemoCanvasBase(WindowsPlatform* WPlatformOwner, const cbDimension& pScreenDimension)
	: Super(WPlatformOwner)
	, Transform(cbTransform(pScreenDimension))
	, PrevVertexOffset(0)
	, PrevIndexOffset(0)
	, Animation(cbAnimation::Create())
{
	Transform.SetLocation(cbVector(WPlatformOwner->GetWindowWidth() / 2.0f, WPlatformOwner->GetWindowHeight() / 2.0f));

	{
		sBufferDesc Desc;
		Desc.Size = 3000000;// (std::uint32_t)(sizeof(cbGeometryVertexData));
		Desc.Stride = sizeof(cbGeometryVertexData);
		VertexBuffer = std::make_unique<AVertexBuffer>(Desc, nullptr);
	}
	{
		sBufferDesc Desc;
		Desc.Size = 300000;// (std::uint32_t)(sizeof(int));
		Desc.Stride = 0;
		IndexBuffer = std::make_unique<AIndexBuffer>(Desc, nullptr);
	}
}

DemoCanvasBase::~DemoCanvasBase()
{
	for (auto& UI : Widgets)
	{
		UI = nullptr;
	}
	Widgets.clear();

	for (auto& Widget : Hierarchy)
	{
		delete Widget;
		Widget = nullptr;
	}
	Hierarchy.clear();

	WidgetsList.clear();

	VertexBuffer = nullptr;
	IndexBuffer = nullptr;

	Animation = nullptr;
}

void DemoCanvasBase::BeginPlay()
{
	for (auto& Obj : Widgets)
		Obj->BeginPlay();

	Animation->BeginPlay();
}

void DemoCanvasBase::Tick(const float DeltaTime)
{
	for (auto& Obj : Widgets)
		Obj->Tick(DeltaTime);

	Animation->Tick(DeltaTime);
}

void DemoCanvasBase::ResizeWindow(std::size_t Width, std::size_t Height)
{
	Transform = cbTransform(cbDimension((float)Width, (float)Height));
	Transform.SetLocation(cbVector(Width / 2.0f, Height / 2.0f));

	for (const auto& Widget : Widgets)
		Widget->UpdateAlignments(true);
}

void DemoCanvasBase::InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	auto GetMouseLocation = [&]() -> cbVector
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);

		return cbVector(static_cast<float>(point.x), static_cast<float>(point.y));
	};

	cbMouseInput MouseInput;

	std::int32_t WheelDelta = 0;

	//std::cout << GetMouseLocation().ToString() << std::endl;

	if (uMsg)
	{
		switch (uMsg)
		{
		case WM_SYSKEYUP:
			break;
		case WM_SYSKEYDOWN:
			break;
		case WM_KEYDOWN:
		{

		}
		break;
		case WM_CHAR:
		{
			if (!KeyboardBTNEvent.bIsPressed)
				return;

			int iKeyPressed = static_cast<int>(wParam);

			if (iKeyPressed < 32)
			{
				KeyboardBTNEvent.KeyCode = iKeyPressed;
				for (auto& Obj : Widgets)
					Obj->OnKeyDown(KeyboardBTNEvent.KeyCode);
				return;
			}

			if ((GetAsyncKeyState(VK_CAPITAL) & 0x0001) == 0)
			{
				iKeyPressed = towlower(iKeyPressed);

				if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
					iKeyPressed = towupper(iKeyPressed);
			}
			else if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
				iKeyPressed = towlower(iKeyPressed);

			KeyboardBTNEvent.KeyCode = iKeyPressed;

			for (auto& Obj : Widgets)
				Obj->OnKeyDown(KeyboardBTNEvent.KeyCode);
		}
		break;
		case WM_KEYUP:
			KeyboardBTNEvent.bIsPressed = false;

			for (auto& Obj : Widgets)
				Obj->OnKeyUp(KeyboardBTNEvent.KeyCode);

			break;
		case WM_LBUTTONDOWN:
			MouseInput.Buttons["Left"] = cbMouseButtonState::Pressed;
			MouseInput.MouseLocation = GetMouseLocation();
			if (FocusedObj)
			{
				if (!FocusedObj->OnMouseButtonDown(MouseInput))
					FocusedObj = nullptr;
			}
			else
			{
				return;
			}
			break;
		case WM_LBUTTONUP:
			MouseInput.Buttons["Left"] = cbMouseButtonState::Released;
			MouseInput.MouseLocation = GetMouseLocation();
			if (FocusedObj)
			{
				if (FocusedObj->OnMouseButtonUp(MouseInput))
				{
					if (FocusedObj->IsFocused())
					{
						if (!FocusedObj->IsInside(MouseInput.MouseLocation))
						{
							if (FocusedObj->OnMouseLeave(MouseInput))
								FocusedObj = nullptr;
						}
						else
						{
							return;
						}
					}
					else
					{
						FocusedObj = nullptr;
					}
				}
			}
			break;
		case WM_RBUTTONDOWN:
			MouseInput.Buttons["Right"] = cbMouseButtonState::Pressed;
			MouseInput.MouseLocation = GetMouseLocation();
			break;
		case WM_LBUTTONDBLCLK:
			break;
		case WM_RBUTTONUP:
			MouseInput.Buttons["Right"] = cbMouseButtonState::Released;
			MouseInput.MouseLocation = GetMouseLocation();
			break;
		case WM_MBUTTONDOWN:
			MouseInput.Buttons["Middle"] = cbMouseButtonState::Pressed;
			MouseInput.MouseLocation = GetMouseLocation();
			break;
		case WM_MBUTTONUP:
			MouseInput.Buttons["Middle"] = cbMouseButtonState::Released;
			MouseInput.MouseLocation = GetMouseLocation();
			break;
		case WM_XBUTTONDOWN:
			switch (GET_XBUTTON_WPARAM(wParam))
			{
			case XBUTTON1:
				MouseInput.Buttons["XB1"] = cbMouseButtonState::Pressed;
				break;

			case XBUTTON2:
				MouseInput.Buttons["XB2"] = cbMouseButtonState::Pressed;
				break;
			}
			break;

		case WM_XBUTTONUP:
			switch (GET_XBUTTON_WPARAM(wParam))
			{
			case XBUTTON1:
				MouseInput.Buttons["XB1"] = cbMouseButtonState::Released;
				break;

			case XBUTTON2:
				MouseInput.Buttons["XB2"] = cbMouseButtonState::Released;
				break;
			}
			break;
		case WM_MOUSEWHEEL:
		{
			if (FocusedObj)
			{
				MouseInput.MouseLocation = GetMouseLocation();

				if (FocusedObj->IsInside(MouseInput.MouseLocation))
					FocusedObj->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), MouseInput);
				return;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			MouseInput.State = cbMouseState::Moving;
			MouseInput.MouseLocation = GetMouseLocation();

			/*{
				cbBounds Bounds = cbBounds(cbDimension(5.0f, 5.0f), MouseInput.MouseLocation);
				auto OverlappingObjects = GetRawOverlappingObjects(Bounds);

				for (auto& Obj : OverlappingObjects)
				{
					CanvasConsole(Obj->GetClassID());
				}
			}*/

			if (FocusedObj)
			{
				if (FocusedObj->IsInside(MouseInput.MouseLocation))
				{
					FocusedObj->OnMouseMove(MouseInput);
					//CanvasConsole("OnMouseMove");
					return;
				}
				else
				{
					if (FocusedObj->OnMouseLeave(MouseInput))
					{
						//CanvasConsole("OnMouseMove/OnMouseLeave");
						FocusedObj = nullptr;
					}
					else
					{
						FocusedObj->OnMouseMove(MouseInput);
						//CanvasConsole("OnMouseMove");
						return;
					}
				}
			}

			cbWidget* Focus = nullptr;

			for (auto& Obj : Widgets)
			{
				if (Obj->IsInside(MouseInput.MouseLocation))
				{
					if (Obj->IsFocusable())
						Focus = Obj.get();
				}
				else if (Obj->IsFocused())
				{
					Obj->OnMouseLeave(MouseInput);
				}
			}

			if (Focus)
			{
				if (FocusedObj && FocusedObj != Focus)
				{
					FocusedObj->OnMouseLeave(MouseInput);
					FocusedObj = nullptr;
				}

				if (!Focus->OnMouseEnter(MouseInput))
					Focus = nullptr;

				FocusedObj = Focus;
			}
		}
		break;
		}
	}
}

void DemoCanvasBase::ReuploadGeometry()
{
	std::function<void(WidgetHierarchy*)> it;
	it = [&](WidgetHierarchy* Widget)
	{
		if (Widget->Widget->HasGeometry())
		{
			Widget->bVertexDirty = true;
			Widget->bIndexDirty = true;
		}

		for (const auto& Widget : Widget->Nodes)
			it(Widget);
	};

	for (const auto& Widget : Hierarchy)
		it(Widget);
}

void DemoCanvasBase::SetScreenDimension(const cbDimension& pScreenDimension)
{
	Transform.SetDimension(pScreenDimension);

	for (const auto& Obj : Widgets)
	{
		if (Obj->IsAlignedToCanvas())
			Obj->UpdateAlignments();
	}
}

void DemoCanvasBase::ZOrderChanged(cbWidgetObj* Object, const std::int32_t ZOrder)
{
	if (Object->HasOwner())
	{
		std::function<WidgetHierarchy* (WidgetHierarchy*, const cbWidgetObj*)> GetParentHierarchy;
		GetParentHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pParent) -> WidgetHierarchy*
		{
			for (auto& WidgetHierarchy : pWidgetHierarchy->Nodes)
			{
				if (WidgetHierarchy->Widget == pParent)
				{
					return WidgetHierarchy;
				}
				else if (auto WP = GetParentHierarchy(WidgetHierarchy, pParent))
				{
					return WP;
				}
			}
			return nullptr;
		};

		WidgetHierarchy* pWH = nullptr;
		for (auto& pWidgetHierarchy : Hierarchy)
		{
			if (pWidgetHierarchy->Widget == Object->GetOwner()->GetOwner())
			{
				pWH = pWidgetHierarchy;
				break;
			}
			else if (auto WP = GetParentHierarchy(pWidgetHierarchy, Object->GetOwner()->GetOwner()))
			{
				pWH = WP;
				break;
			}
		}

		if (pWH)
		{
			std::sort(pWH->Nodes.begin(), pWH->Nodes.end(), [](const WidgetHierarchy* Widged1, const WidgetHierarchy* Widged2)
				{
					return (Widged1->Widget->GetZOrder() < Widged2->Widget->GetZOrder());
				});
		}
	}
	else
	{
		SortWidgetsByZOrder();
	}
}

/*
* Set material by class ID, geometry component name, or geometry type.
*/
void DemoCanvasBase::SetMaterial(WidgetHierarchy* pWH)
{
	if (pWH->Widget->HasGeometry())
	{
		auto DrawData = pWH->Widget->GetGeometryDrawData();
		if (DrawData.GeometryType == "Button")
			pWH->MaterialName = "Button";
		else if (DrawData.GeometryType == "CheckBox")
			pWH->MaterialName = "CheckBox";
		else if (DrawData.GeometryType == "Plane")
			pWH->MaterialName = "Image";
		else if (DrawData.GeometryType == "Border")
			pWH->MaterialName = "BorderStyle";
		else if (DrawData.GeometryType == "Text")
			pWH->MaterialName = "Text";
		else if (DrawData.GeometryType == "Line")
			pWH->MaterialName = "Line";
		else
			DebugBreak();
	}
}

void DemoCanvasBase::SortWidgetsByZOrder()
{
	std::sort(Widgets.begin(), Widgets.end(), [](const cbWidget::SharedPtr& Widged1, const cbWidget::SharedPtr& Widged2)
		{
			return (Widged1->GetZOrder() < Widged2->GetZOrder());
		});
	std::sort(Hierarchy.begin(), Hierarchy.end(), [](WidgetHierarchy* Widged1, WidgetHierarchy* Widged2)
		{
			return (Widged1->Widget->GetZOrder() < Widged2->Widget->GetZOrder());
		});
}

void DemoCanvasBase::Add(cbWidget* Widget)
{
}

void DemoCanvasBase::Add(const std::shared_ptr<cbWidget>& Widget)
{
	if (IsWidgetExist(Widget.get()))
		return;

	Widgets.push_back(Widget);

	std::function<void(WidgetHierarchy*, cbWidgetObj*)> SetHierarchy;
	SetHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, cbWidgetObj* pWidget)
	{
		if (pWidget->HasAnyChildren())
		{
			const auto& Children = pWidget->GetAllChildren();
			for (const auto& Child : Children)
			{
				if (!Child)
					continue;

				WidgetHierarchy* pHierarchy = new WidgetHierarchy;
				pHierarchy->Widget = Child;
				SetVertexOffset(pHierarchy);
				SetIndexOffset(pHierarchy);

				SetMaterial(pHierarchy);

				WidgetsList.insert({ Child, pHierarchy });
				SetHierarchy(pHierarchy, Child);
				pWidgetHierarchy->Nodes.push_back(pHierarchy);
			}
		}
	};

	WidgetHierarchy* mWidgetHierarchy = new WidgetHierarchy;
	mWidgetHierarchy->Widget = Widget.get();
	SetVertexOffset(mWidgetHierarchy);
	SetIndexOffset(mWidgetHierarchy);

	SetMaterial(mWidgetHierarchy);

	WidgetsList.insert({ Widget.get(), mWidgetHierarchy });
	SetHierarchy(mWidgetHierarchy, Widget.get());

	Hierarchy.push_back(mWidgetHierarchy);

	SortWidgetsByZOrder();
}

bool DemoCanvasBase::IsWidgetExist(cbWidget* Widget) const
{
	for (const auto& pWidget : Widgets)
	{
		if (pWidget.get() == Widget)
			return true;
	}
	return false;
}

void DemoCanvasBase::WidgetUpdated(cbWidgetObj* Object)
{
	const auto& It = WidgetsList.find(Object);
	if (It != WidgetsList.end())
		It->second->bVertexDirty = true;
}

void DemoCanvasBase::RemoveFromCanvas(cbWidget* Object)
{
	cbWidget::SharedPtr Widget = nullptr;
	std::vector<cbWidget::SharedPtr>::iterator it = Widgets.begin();
	while (it != Widgets.end())
	{
		if ((*it).get() == Object)
		{
			Widget = (*it);
			it = Widgets.erase(it);
			break;
		}
		else {
			it++;
		}
	}

	if (!Widget)
		return;

	{
		WidgetHierarchy* pHierarchy = nullptr;
		std::vector<WidgetHierarchy*>::iterator it = Hierarchy.begin();
		while (it != Hierarchy.end())
		{
			if ((*it)->Widget == Widget.get())
			{
				pHierarchy = (*it);
				it = Hierarchy.erase(it);
				break;
			}
			else {
				it++;
			}
		}

		if (pHierarchy)
		{
			delete pHierarchy;
			pHierarchy = nullptr;
		}
	}

	{
		std::function<void(const cbWidgetObj*)> Remove;
		Remove = [&](const cbWidgetObj* pWidget)
		{
			if (pWidget->HasAnyChildren())
			{
				const auto& Children = pWidget->GetAllChildren();
				for (const auto& Child : Children)
				{
					if (!Child)
						continue;

					WidgetsList.erase(Child);
					Remove(Child);
				}
			}
		};
		Remove(Widget.get());
		WidgetsList.erase(Widget.get());
	}

	Widget = nullptr;

	ReorderWidgets();
}

void DemoCanvasBase::NewSlotAdded(cbSlottedBox* Parent, cbSlot* Slot)
{
	std::function<void(WidgetHierarchy*, const cbWidgetObj*)> SetHierarchy;
	SetHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pWidget)
	{
		if (pWidget->HasAnyChildren())
		{
			const auto& Children = pWidget->GetAllChildren();
			for (const auto& Child : Children)
			{
				if (!Child)
					continue;

				WidgetHierarchy* pHierarchy = new WidgetHierarchy;
				pHierarchy->Widget = Child;
				SetVertexOffset(pHierarchy);
				SetIndexOffset(pHierarchy);

				SetMaterial(pHierarchy);

				SetHierarchy(pHierarchy, Child);
				WidgetsList.insert({ Child, pHierarchy });
				pWidgetHierarchy->Nodes.push_back(pHierarchy);
			}
		}
	};

	std::function<WidgetHierarchy* (WidgetHierarchy*, const cbWidgetObj*)> GetParentHierarchy;
	GetParentHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pParent) -> WidgetHierarchy*
	{
		for (auto& WidgetHierarchy : pWidgetHierarchy->Nodes)
		{
			if (WidgetHierarchy->Widget == pParent)
			{
				return WidgetHierarchy;
			}
			else if (auto WP = GetParentHierarchy(WidgetHierarchy, pParent))
			{
				return WP;
			}
		}
		return nullptr;
	};

	WidgetHierarchy* pWH = nullptr;
	for (auto& pWidgetHierarchy : Hierarchy)
	{
		if (pWidgetHierarchy->Widget == Parent)
		{
			pWH = pWidgetHierarchy;
			break;
		}
		else if (auto WP = GetParentHierarchy(pWidgetHierarchy, Parent))
		{
			pWH = WP;
			break;
		}
	}

	if (pWH)
	{
		WidgetHierarchy* mWidgetHierarchy = new WidgetHierarchy;
		mWidgetHierarchy->Widget = Slot;
		SetVertexOffset(mWidgetHierarchy);
		SetIndexOffset(mWidgetHierarchy);

		SetMaterial(mWidgetHierarchy);

		SetHierarchy(mWidgetHierarchy, Slot);

		pWH->Nodes.push_back(mWidgetHierarchy);
		WidgetsList.insert({ Slot, mWidgetHierarchy });
	}
}

void DemoCanvasBase::NewSlotContentAdded(cbSlot* Parent, cbWidget* Content)
{
	std::function<void(WidgetHierarchy*, const cbWidgetObj*)> SetHierarchy;
	SetHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pWidget)
	{
		if (pWidget->HasAnyChildren())
		{
			const auto& Children = pWidget->GetAllChildren();
			for (const auto& Child : Children)
			{
				if (!Child)
					continue;

				WidgetHierarchy* pHierarchy = new WidgetHierarchy;
				pHierarchy->Widget = Child;
				SetVertexOffset(pHierarchy);
				SetIndexOffset(pHierarchy);

				SetMaterial(pHierarchy);

				SetHierarchy(pHierarchy, Child);
				WidgetsList.insert({ Child, pHierarchy });
				pWidgetHierarchy->Nodes.push_back(pHierarchy);
			}
		}
	};

	std::function<WidgetHierarchy* (WidgetHierarchy*, const cbWidgetObj*)> GetParentHierarchy;
	GetParentHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pParent) -> WidgetHierarchy*
	{
		for (auto& WidgetHierarchy : pWidgetHierarchy->Nodes)
		{
			if (WidgetHierarchy->Widget == pParent)
			{
				return WidgetHierarchy;
			}
			else if (auto WP = GetParentHierarchy(WidgetHierarchy, pParent))
			{
				return WP;
			}
		}
		return nullptr;
	};

	WidgetHierarchy* pWH = nullptr;
	for (auto& pWidgetHierarchy : Hierarchy)
	{
		if (pWidgetHierarchy->Widget == Parent)
		{
			pWH = pWidgetHierarchy;
			break;
		}
		else if (auto WP = GetParentHierarchy(pWidgetHierarchy, Parent))
		{
			pWH = WP;
			break;
		}
	}

	if (pWH)
	{
		WidgetHierarchy* mWidgetHierarchy = new WidgetHierarchy;
		mWidgetHierarchy->Widget = Content;
		SetVertexOffset(mWidgetHierarchy);
		SetIndexOffset(mWidgetHierarchy);

		SetMaterial(mWidgetHierarchy);

		SetHierarchy(mWidgetHierarchy, Content);

		pWH->Nodes.push_back(mWidgetHierarchy);
		WidgetsList.insert({ Content, mWidgetHierarchy });
	}
}

void DemoCanvasBase::SlotRemoved(cbSlottedBox* Parent, cbSlot* Slot)
{
	{
		std::function<WidgetHierarchy* (WidgetHierarchy*, const cbWidgetObj*)> GetParentHierarchy;
		GetParentHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pParent) -> WidgetHierarchy*
		{
			for (auto& WidgetHierarchy : pWidgetHierarchy->Nodes)
			{
				if (WidgetHierarchy->Widget == pParent)
				{
					return WidgetHierarchy;
				}
				else if (auto WP = GetParentHierarchy(WidgetHierarchy, pParent))
				{
					return WP;
				}
			}
			return nullptr;
		};

		WidgetHierarchy* pWH = nullptr;
		for (auto& pWidgetHierarchy : Hierarchy)
		{
			if (pWidgetHierarchy->Widget == Parent)
			{
				pWH = pWidgetHierarchy;
				break;
			}
			else if (auto WP = GetParentHierarchy(pWidgetHierarchy, Parent))
			{
				pWH = WP;
				break;
			}
		}

		if (pWH)
		{
			WidgetHierarchy* pHierarchy = nullptr;
			std::vector<WidgetHierarchy*>::iterator it = pWH->Nodes.begin();
			while (it != pWH->Nodes.end())
			{
				if ((*it)->Widget == Slot)
				{
					pHierarchy = (*it);
					it = pWH->Nodes.erase(it);
					break;
				}
				else {
					it++;
				}
			}

			{
				std::function<void(const cbWidgetObj*)> Remove;
				Remove = [&](const cbWidgetObj* pWidget)
				{
					if (pWidget->HasAnyChildren())
					{
						const auto& Children = pWidget->GetAllChildren();
						for (const auto& Child : Children)
						{
							if (!Child)
								continue;

							WidgetsList.erase(Child);
							Remove(Child);
						}
					}
				};
				Remove(pHierarchy->Widget);
				WidgetsList.erase(pHierarchy->Widget);
			}
			if (pHierarchy)
			{
				delete pHierarchy;
				pHierarchy = nullptr;
			}
		}
	}

	ReorderWidgets();
}

void DemoCanvasBase::SlotContentReplaced(cbSlot* Parent, cbWidget* Old, cbWidget* New)
{
	{
		std::function<WidgetHierarchy* (WidgetHierarchy*, const cbWidgetObj*)> GetParentHierarchy;
		GetParentHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pParent) -> WidgetHierarchy*
		{
			for (auto& WidgetHierarchy : pWidgetHierarchy->Nodes)
			{
				if (WidgetHierarchy->Widget == pParent)
				{
					return WidgetHierarchy;
				}
				else if (auto WP = GetParentHierarchy(WidgetHierarchy, pParent))
				{
					return WP;
				}
			}
			return nullptr;
		};

		WidgetHierarchy* pWH = nullptr;
		for (auto& pWidgetHierarchy : Hierarchy)
		{
			if (pWidgetHierarchy->Widget == Parent)
			{
				pWH = pWidgetHierarchy;
				break;
			}
			else if (auto WP = GetParentHierarchy(pWidgetHierarchy, Parent))
			{
				pWH = WP;
				break;
			}
		}

		if (pWH)
		{
			std::function<void(WidgetHierarchy*, const cbWidgetObj*)> SetHierarchy;
			SetHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, const cbWidgetObj* pWidget)
			{
				if (pWidget->HasAnyChildren())
				{
					const auto& Children = pWidget->GetAllChildren();
					for (const auto& Child : Children)
					{
						if (!Child)
							continue;

						WidgetHierarchy* pHierarchy = new WidgetHierarchy;
						pHierarchy->Widget = Child;
						SetVertexOffset(pHierarchy);
						SetIndexOffset(pHierarchy);

						SetMaterial(pHierarchy);

						SetHierarchy(pHierarchy, Child);
						pWidgetHierarchy->Nodes.push_back(pHierarchy);
						WidgetsList.insert({ Child, pHierarchy });
					}
				}
			};

			WidgetHierarchy* pHierarchy = nullptr;
			std::vector<WidgetHierarchy*>::iterator it = pWH->Nodes.begin();
			while (it != pWH->Nodes.end())
			{
				if ((*it)->Widget == Old)
				{
					pHierarchy = (*it);
					it = pWH->Nodes.erase(it);
					break;
				}
				else {
					it++;
				}
			}

			if (pHierarchy->Widget)
				WidgetsList.erase(pHierarchy->Widget);

			if (pHierarchy)
			{
				delete pHierarchy;
				pHierarchy = nullptr;
			}

			WidgetHierarchy* mWidgetHierarchy = new WidgetHierarchy;
			mWidgetHierarchy->Widget = New;
			SetVertexOffset(mWidgetHierarchy);
			SetIndexOffset(mWidgetHierarchy);

			SetMaterial(mWidgetHierarchy);

			SetHierarchy(mWidgetHierarchy, New);

			pWH->Nodes.push_back(mWidgetHierarchy);
			WidgetsList.insert({ New, mWidgetHierarchy });
		}
	}

	ReorderWidgets();
}

void DemoCanvasBase::VerticesSizeChanged(cbWidgetObj* Object, const std::size_t NewSize)
{
	ReorderWidgets();
}

std::vector<cbWidget*> DemoCanvasBase::GetOverlappingWidgets(const cbBounds& Bounds) const
{
	std::vector<cbWidget*> UIObjects;
	for (const auto& Obj : Widgets)
	{
		if (Obj->Intersect(Bounds))
		{
			UIObjects.push_back(Obj.get());
		}
	}

	return UIObjects;
}

void DemoCanvasBase::ReorderWidgets()
{
	std::size_t VertexOrder = 0;
	std::size_t IndexOrder = 0;

	std::function<void(WidgetHierarchy*, std::size_t&, std::size_t&)> SetHierarchy;
	SetHierarchy = [&](WidgetHierarchy* pWidgetHierarchy, std::size_t& VertexOrder, std::size_t& IndexOrder)
	{
		for (auto& Child : pWidgetHierarchy->Nodes)
		{
			Child->DrawParams.VertexOffset = VertexOrder;
			Child->DrawParams.IndexOffset = IndexOrder;

			Child->bVertexDirty = true;
			Child->bIndexDirty = true;

			if (Child->Widget->HasGeometry())
			{
				auto DrawData = Child->Widget->GetGeometryDrawData();
				VertexOrder += DrawData.VertexCount;
			}
			else
				VertexOrder += 4;
			if (Child->Widget->HasGeometry())
			{
				auto DrawData = Child->Widget->GetGeometryDrawData();
				IndexOrder += DrawData.IndexCount;
			}
			else
				IndexOrder += 8;

			SetHierarchy(Child, VertexOrder, IndexOrder);
		}
	};

	for (auto& pHierarchy : Hierarchy)
	{
		pHierarchy->DrawParams.VertexOffset = VertexOrder;
		pHierarchy->DrawParams.IndexOffset = IndexOrder;
		pHierarchy->bVertexDirty = true;
		pHierarchy->bIndexDirty = true;
		if (pHierarchy->Widget->HasGeometry())
		{
			auto DrawData = pHierarchy->Widget->GetGeometryDrawData();
			VertexOrder += DrawData.VertexCount;
		}
		else
			VertexOrder += 4;
		if (pHierarchy->Widget->HasGeometry())
		{
			auto DrawData = pHierarchy->Widget->GetGeometryDrawData();
			IndexOrder += DrawData.IndexCount;
		}
		else
			IndexOrder += 8;
		SetHierarchy(pHierarchy, VertexOrder, IndexOrder);
	}

	PrevVertexOffset = VertexOrder;
	PrevIndexOffset = IndexOrder;
}

void DemoCanvasBase::SetVertexOffset(WidgetHierarchy* pWH)
{
	std::size_t PrevVertexSize = 0;
	std::size_t CurrentVertexOffset = PrevVertexOffset;

	if (pWH->Widget->HasGeometry())
	{
		PrevVertexSize = pWH->Widget->GetGeometryDrawData().VertexCount;
		pWH->DrawParams.VertexSize = PrevVertexSize;
		pWH->DrawParams.VertexOffset = CurrentVertexOffset;
	}
	else
	{
		PrevVertexSize = 4;
		pWH->DrawParams.VertexSize = PrevVertexSize;
		pWH->DrawParams.VertexOffset = CurrentVertexOffset;
	}
	PrevVertexOffset = CurrentVertexOffset + PrevVertexSize;
}

void DemoCanvasBase::SetIndexOffset(WidgetHierarchy* pWH)
{
	std::size_t PrevIndexSize = 0;
	std::size_t CurrentIndexOffset = PrevIndexOffset;

	if (pWH->Widget->HasGeometry())
	{
		PrevIndexSize = pWH->Widget->GetGeometryDrawData().IndexCount;
		pWH->DrawParams.IndexSize = PrevIndexSize;
		pWH->DrawParams.IndexOffset = CurrentIndexOffset;
	}
	else
	{
		PrevIndexSize = 8;
		pWH->DrawParams.IndexSize = PrevIndexSize;
		pWH->DrawParams.IndexOffset = CurrentIndexOffset;
	}
	PrevIndexOffset = CurrentIndexOffset + PrevIndexSize;
}
