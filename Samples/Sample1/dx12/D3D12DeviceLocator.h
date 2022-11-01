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

#include <string>
#include "WindowsD3D12Device.h"

class D3D12DeviceLocator final
{
private:
	D3D12DeviceLocator() = default;
	D3D12DeviceLocator(const D3D12DeviceLocator& Other) = delete;
	D3D12DeviceLocator& operator=(const D3D12DeviceLocator&) = delete;

private:
	WindowsD3D12Device* Interface;

public:
	~D3D12DeviceLocator()
	{
		Interface = nullptr;
	}

	static D3D12DeviceLocator& Get()
	{
		static D3D12DeviceLocator instance;
		return instance;
	}

	void SetInterface(WindowsD3D12Device* InOwner) { Interface = InOwner; }
	WindowsD3D12Device* GetInterface() { return Interface; }
};
