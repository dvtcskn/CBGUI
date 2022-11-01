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
#include <array>
#include <stdexcept>
#include <optional>
#include <algorithm>
#include <cmath>
#include <random>
#include <limits>
#include "cbClassBody.h"
#include "cbStates.h"

namespace cbgui
{
	class cbVector4;

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	class cbTVector3;

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	class cbTVector
	{
	public:
		static constexpr cbTVector Zero()
		{
			return cbTVector(static_cast<T>(0.0f), static_cast<T>(0.0f));
		}

		static constexpr cbTVector One()
		{
			return cbTVector(static_cast<T>(1.0f), static_cast<T>(1.0f));
		}

		static constexpr cbTVector UnitX()
		{
			return cbTVector(static_cast<T>(1.0f), static_cast<T>(0.0f));
		}

		static constexpr cbTVector UnitY()
		{
			return cbTVector(static_cast<T>(0.0f), static_cast<T>(1.0f));
		}

	public:
		T X;
		T Y;

	public:
		cbFORCEINLINE constexpr cbTVector() noexcept
			: X(static_cast<T>(0.0f))
			, Y(static_cast<T>(0.0f))
		{}

		cbFORCEINLINE constexpr cbTVector(const cbTVector<T>& Other) noexcept
			: X(Other.X)
			, Y(Other.Y)
		{}

		cbFORCEINLINE constexpr cbTVector(T val) noexcept
			: X(val)
			, Y(val)
		{}

		cbFORCEINLINE constexpr cbTVector(T x, T y) noexcept
			: X(x)
			, Y(y)
		{}

		cbFORCEINLINE ~cbTVector() = default;

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ X: " + std::to_string(X) + " Y: " + std::to_string(Y) + " };");
		}

		cbFORCEINLINE constexpr bool Equals(const cbTVector& other) const
		{
			return (X == other.X && Y == other.Y);
		}

		cbFORCEINLINE constexpr void operator +=(const cbTVector<T>& value)
		{
			X += value.X;
			Y += value.Y;
		}

		cbFORCEINLINE constexpr void operator -=(const cbTVector<T>& value)
		{
			X -= value.X;
			Y -= value.Y;
		}

		cbFORCEINLINE constexpr void operator *=(const cbTVector<T>& value)
		{
			X *= value.X;
			Y *= value.Y;
		}

		cbFORCEINLINE constexpr void operator *=(const float& scaleFactor)
		{
			X *= scaleFactor;
			Y *= scaleFactor;
		}

		cbFORCEINLINE constexpr void operator /=(const cbTVector<T>& value)
		{
			X /= value.X;
			Y /= value.Y;
		}

		cbFORCEINLINE constexpr void operator /=(const float& divider)
		{
			X /= divider;
			Y /= divider;
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbTVector&) const = default;
#endif
	};

#if _MSVC_LANG < 202002L
	template<typename T>
	cbFORCEINLINE constexpr bool operator ==(const cbTVector<T>& value1, const cbTVector<T>& value2)
	{
		return (value1.X == value2.X && value1.Y == value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr bool operator !=(const cbTVector<T>& value1, const cbTVector<T>& value2)
	{
		return !((value1.X == value2.X) && (value1.Y == value2.Y));
	}
#endif

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator +(const cbTVector<T>& value1, const cbTVector<T>& value2)
	{
		return cbTVector<T>(value1.X + value2.X, value1.Y + value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator -(const cbTVector<T>& value1, const cbTVector<T>& value2)
	{
		return cbTVector<T>(value1.X - value2.X, value1.Y - value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator *(const cbTVector<T>& value1, const cbTVector<T>& value2)
	{
		return cbTVector<T>(value1.X * value2.X, value1.Y * value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator *(const cbTVector<T>& value, const float& scaleFactor)
	{
		return cbTVector<T>(value.X * scaleFactor, value.Y * scaleFactor);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator *(const float& scaleFactor, const cbTVector<T>& value)
	{
		return cbTVector<T>(value.X * scaleFactor, value.Y * scaleFactor);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator /(const cbTVector<T>& value1, const cbTVector<T>& value2)
	{
		return cbTVector<T>(value1.X / value2.X, value1.Y / value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector<T> operator /(const cbTVector<T>& value1, const float& divider)
	{
		return cbTVector<T>(value1.X * (1 / divider), value1.Y * (1 / divider));
	}

	typedef cbTVector<float> cbVector;
	typedef cbTVector<std::int32_t> cbIntVector;

	template<typename T, typename type>
	class cbTVector3
	{
	public:
		static constexpr cbTVector3 Zero()
		{
			return cbTVector3(static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
		}

		static constexpr cbTVector3 One()
		{
			return cbTVector3(static_cast<T>(1.0f), static_cast<T>(1.0f), static_cast<T>(1.0f));
		}

		static constexpr cbTVector3 UnitX()
		{
			return cbTVector3(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
		}

		static constexpr cbTVector3 UnitY()
		{
			return cbTVector3(static_cast<T>(0.0f), static_cast<T>(1.0f), static_cast<T>(0.0f));
		}

		static constexpr cbTVector3 UnitZ()
		{
			return cbTVector3(static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f));
		}

	public:
		T X;
		T Y;
		T Z;

	public:
		cbFORCEINLINE constexpr cbTVector3() noexcept
			: X(0.0f)
			, Y(0.0f)
			, Z(0.0f)
		{}

		cbFORCEINLINE constexpr cbTVector3(const cbTVector3& Other) noexcept
			: X(Other.X)
			, Y(Other.Y)
			, Z(Other.Z)
		{}

		cbFORCEINLINE constexpr cbTVector3(T value) noexcept
			: X(value)
			, Y(value)
			, Z(value)
		{}

		cbFORCEINLINE constexpr cbTVector3(const cbTVector<T>& V, T vZ = 0.0f) noexcept
			: X(V.X)
			, Y(V.Y)
			, Z(vZ)
		{}

		cbFORCEINLINE constexpr cbTVector3(T x, T y, T z) noexcept
			: X(x)
			, Y(y)
			, Z(z)
		{}

		cbFORCEINLINE ~cbTVector3() = default;

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ X: " + std::to_string(X) + " Y: " + std::to_string(Y) + " Z: " + std::to_string(Z) + " };");
		}

		cbFORCEINLINE constexpr bool Equals(const cbTVector3& other) const
		{
			return (X == other.X && Y == other.Y && Z == other.Z);
		}

		cbFORCEINLINE constexpr void operator +=(const cbTVector3& value)
		{
			X += value.X;
			Y += value.Y;
			Z += value.Z;
		}

		cbFORCEINLINE constexpr void operator -=(const cbTVector3& value)
		{
			X -= value.X;
			Y -= value.Y;
			Z -= value.Z;
		}

		cbFORCEINLINE constexpr void operator *=(const cbTVector3& value)
		{
			X *= value.X;
			Y *= value.Y;
			Z *= value.Z;
		}

		cbFORCEINLINE constexpr void operator *=(const float& scaleFactor)
		{
			X *= scaleFactor;
			Y *= scaleFactor;
			Z *= scaleFactor;
		}

		cbFORCEINLINE constexpr void operator /=(const cbTVector3& value)
		{
			X /= value.X;
			Y /= value.Y;
			Z /= value.Z;
		}

		cbFORCEINLINE constexpr void operator /=(const float& divider)
		{
			X /= divider;
			Y /= divider;
			Z /= divider;
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbTVector3&) const = default;
#endif
	};

#if _MSVC_LANG < 202002L
	template<typename T>
	cbFORCEINLINE constexpr bool operator ==(const cbTVector3<T>& value1, const cbTVector3<T>& value2)
	{
		return (value1.X == value2.X
			&& value1.Y == value2.Y
			&& value1.Z == value2.Z);
	}

	template<typename T>
	cbFORCEINLINE constexpr bool operator !=(const cbTVector3<T>& value1, const cbTVector3<T>& value2)
	{
		return !((value1.X == value2.X) && (value1.Y == value2.Y) && (value1.Z == value2.Z));
	}
#endif

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator +(const cbTVector3<T>& value1, const cbTVector3<T>& value2)
	{
		return cbTVector3<T>(value1.X + value2.X, value1.Y + value2.Y, value1.Z + value2.Z);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator -(const cbTVector3<T>& value1, const cbTVector3<T>& value2)
	{
		return cbTVector3<T>(value1.X - value2.X, value1.Y - value2.Y, value1.Z - value2.Z);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator *(const cbTVector3<T>& value1, const cbTVector3<T>& value2)
	{
		return cbTVector3<T>(value1.X * value2.X, value1.Y * value2.Y, value1.Z * value2.Z);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator *(const cbTVector3<T>& value, const float& scaleFactor)
	{
		return cbTVector3<T>(value.X * scaleFactor, value.Y * scaleFactor, value.Z * scaleFactor);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator *(const float& scaleFactor, const cbTVector3<T>& value)
	{
		return cbTVector3<T>(value.X * scaleFactor, value.Y * scaleFactor, value.Z * scaleFactor);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator /(const cbTVector3<T>& value1, const cbTVector3<T>& value2)
	{
		return cbTVector3<T>(value1.X / value2.X, value1.Y / value2.Y, value1.Z / value2.Z);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTVector3<T> operator /(const cbTVector3<T>& value1, const float& divider)
	{
		return cbTVector3<T>(value1.X * (1 / divider), value1.Y * (1 / divider), value1.Z * (1 / divider));
	}

	typedef cbTVector3<float> cbVector3;
	typedef cbTVector3<std::int32_t> cbIntVector3;

	__declspec(align(16)) class cbVector4
	{
	public:
		static constexpr cbVector4 Zero()
		{
			return cbVector4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		static constexpr cbVector4 One()
		{
			return cbVector4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		static constexpr cbVector4 UnitX()
		{
			return cbVector4(1.0f, 0.0f, 0.0f, 0.0f);
		}

		static constexpr cbVector4 UnitY()
		{
			return cbVector4(0.0f, 1.0f, 0.0f, 0.0f);
		}

		static constexpr cbVector4 UnitZ()
		{
			return cbVector4(0.0f, 0.0f, 1.0f, 0.0f);
		}

		static constexpr cbVector4 UnitW()
		{
			return cbVector4(0.0f, 0.0f, 0.0f, 1.0f);
		}

	public:
		float X;
		float Y;
		float Z;
		float W;

	public:
		cbFORCEINLINE constexpr cbVector4() noexcept
			: X(0.0f)
			, Y(0.0f)
			, Z(0.0f)
			, W(0.0f)
		{}

		cbFORCEINLINE constexpr cbVector4(const cbVector4& Other) noexcept
			: X(Other.X)
			, Y(Other.Y)
			, Z(Other.Z)
			, W(Other.W)
		{}

		cbFORCEINLINE constexpr cbVector4(float value) noexcept
			: X(value)
			, Y(value)
			, Z(value)
			, W(value)
		{}

		cbFORCEINLINE constexpr cbVector4(const cbVector& value, float z = 0.0f, float w = 1.0f) noexcept
			: X(value.X)
			, Y(value.Y)
			, Z(z)
			, W(w)
		{}

		cbFORCEINLINE constexpr cbVector4(const cbVector3& value, float w = 1.0f) noexcept
			: X(value.X)
			, Y(value.Y)
			, Z(value.Z)
			, W(w)
		{}

		explicit cbFORCEINLINE constexpr cbVector4(const cbVector& XY, const cbVector& ZW) noexcept
			: X(XY.X)
			, Y(XY.Y)
			, Z(ZW.X)
			, W(ZW.Y)
		{}

		explicit cbFORCEINLINE constexpr cbVector4(float x, float y, float z, float w = 1.0f) noexcept
			: X(x)
			, Y(y)
			, Z(z)
			, W(w)
		{}

		cbFORCEINLINE ~cbVector4() = default;

		cbFORCEINLINE constexpr float& operator[](const std::size_t& idx)
		{
			return (&X)[idx];
		}

		cbFORCEINLINE constexpr const float& operator[](const std::size_t& idx) const
		{
			return (&X)[idx];
		}

		cbFORCEINLINE constexpr cbVector4 GetInverse() const //requires std::signed_integral<T>
		{
			return cbVector4(-X, -Y, -Z, -W);
		}
				
		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ X: " + std::to_string(X) + " Y: " + std::to_string(Y) + " Z: " + std::to_string(Z) + " W: " + std::to_string(W) + " };");
		}

		cbFORCEINLINE constexpr bool Equals(const cbVector4& other) const
		{
			return (X == other.X && Y == other.Y && Z == other.Z && W == other.W);
		}


		cbFORCEINLINE constexpr void operator +=(const cbVector4& value)
		{
			X += value.X;
			Y += value.Y;
			Z += value.Z;
			W += value.W;
		}

		cbFORCEINLINE constexpr void operator -=(const cbVector4& value)
		{
			X -= value.X;
			Y -= value.Y;
			Z -= value.Z;
			W += value.W;
		}

		cbFORCEINLINE constexpr void operator *=(const cbVector4& value)
		{
			X *= value.X;
			Y *= value.Y;
			Z *= value.Z;
			W += value.W;
		}

		cbFORCEINLINE constexpr void operator *=(const float& scaleFactor)
		{
			X *= scaleFactor;
			Y *= scaleFactor;
			Z *= scaleFactor;
			W += scaleFactor;
		}

		cbFORCEINLINE constexpr void operator /=(const cbVector4& value)
		{
			X /= value.X;
			Y /= value.Y;
			Z /= value.Z;
			W /= value.W;
		}

		cbFORCEINLINE constexpr void operator /=(const float& divider)
		{
			X /= divider;
			Y /= divider;
			Z /= divider;
			W /= divider;
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbVector4&) const = default;
#endif
	};

#if _MSVC_LANG < 202002L
	cbFORCEINLINE constexpr bool operator ==(const cbVector4& value1, const cbVector4& value2)
	{
		return (value1.X == value2.X
			&& value1.Y == value2.Y
			&& value1.Z == value2.Z
			&& value1.W == value2.W);
	}

	cbFORCEINLINE constexpr bool operator !=(const cbVector4& value1, const cbVector4& value2)
	{
		return !((value1.X == value2.X) && (value1.Y == value2.Y) && (value1.Z == value2.Z) && (value1.W == value2.W));
	}
#endif

	cbFORCEINLINE constexpr cbVector4 operator +(const cbVector4& value1, const cbVector4& value2)
	{
		return cbVector4(value1.X + value2.X, value1.Y + value2.Y, value1.Z + value2.Z, value1.W + value2.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator +(const cbVector4& value1, const cbVector3& value2)
	{
		return cbVector4(value1.X + value2.X, value1.Y + value2.Y, value1.Z + value2.Z, value1.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator +(const cbVector4& value1, const cbVector& value2)
	{
		return cbVector4(value1.X + value2.X, value1.Y + value2.Y, value1.Z, value1.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator +(const cbVector& value1, const cbVector4& value2)
	{
		return cbVector4(value1.X + value2.X, value1.Y + value2.Y, value2.Z, value2.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator -(const cbVector4& value1, const cbVector4& value2)
	{
		return cbVector4(value1.X - value2.X, value1.Y - value2.Y, value1.Z - value2.Z, value1.W - value2.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator -(const cbVector4& value1, const cbVector& value2)
	{
		return cbVector4(value1.X - value2.X, value1.Y - value2.Y, value1.Z, value1.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator -(const cbVector4& value1, const cbVector3& value2)
	{
		return cbVector4(value1.X - value2.X, value1.Y - value2.Y, value1.Z - value2.Z, value1.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator -(const cbVector3& value1, const cbVector4& value2)
	{
		return cbVector4(value1.X - value2.X, value1.Y - value2.Y, value1.Z - value2.Z, value2.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator *(const cbVector4& value1, const cbVector4& value2)
	{
		return cbVector4(value1.X * value2.X, value1.Y * value2.Y, value1.Z * value2.Z, value1.W * value2.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator *(const cbVector4& value, const float& scaleFactor)
	{
		return cbVector4(value.X * scaleFactor, value.Y * scaleFactor, value.Z * scaleFactor, value.W * scaleFactor);
	}

	cbFORCEINLINE constexpr cbVector4 operator *(const float& scaleFactor, const cbVector4& value)
	{
		return cbVector4(value.X * scaleFactor, value.Y * scaleFactor, value.Z * scaleFactor, value.W * scaleFactor);
	}

	cbFORCEINLINE constexpr cbVector4 operator /(const cbVector4& value1, const cbVector4& value2)
	{
		return cbVector4(value1.X / value2.X, value1.Y / value2.Y, value1.Z / value2.Z, value1.W / value2.W);
	}

	cbFORCEINLINE constexpr cbVector4 operator /(const cbVector4& value, const float& divider)
	{
		return cbVector4(value.X * (1 / divider), value.Y * (1 / divider), value.Z * (1 / divider), value.W * (1 / divider));
	}

	__declspec(align(16)) class cbColor
	{
	public:
		static constexpr cbColor Transparent()
		{
			return cbColor::Zero();
		}

		static constexpr cbColor Zero()
		{
			return cbColor(0.0f, 0.0f, 0.0f, 0.0f);
		}

		static constexpr cbColor Black()
		{
			return cbColor(0.0f, 0.0f, 0.0f, 1.0f);
		}

		static constexpr cbColor White()
		{
			return cbColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		static constexpr cbColor Red()
		{
			return cbColor(1.0f, 0.0f, 0.0f, 1.0f);
		}

		static constexpr cbColor Green()
		{
			return cbColor(0.0f, 1.0f, 0.0f, 1.0f);
		}

		static constexpr cbColor Blue()
		{
			return cbColor(0.0f, 0.0f, 1.0f, 1.0f);
		}

		static constexpr cbColor Grey()
		{
			return cbColor(0.4f, 0.4f, 0.4f, 1.0f);
		}

		static cbColor Random()
		{
			std::random_device random;
			std::mt19937 generator(random());
			std::uniform_real_distribution<> distribution(0.0f, 1.0f);

			cbColor RGB;
			RGB.R = static_cast<float>(distribution(generator));
			RGB.G = static_cast<float>(distribution(generator));
			RGB.B = static_cast<float>(distribution(generator));

			return RGB;
		}

	public:
		float R;
		float G;
		float B;
		float A;

	public:
		cbFORCEINLINE constexpr cbColor() noexcept
			: R(1.0f)
			, G(1.0f)
			, B(1.0f)
			, A(1.0f)
		{}

		cbFORCEINLINE constexpr cbColor(const float r, const float g, const float b, const float a = 1.0f) noexcept
			: R(r)
			, G(g)
			, B(b)
			, A(a)
		{}

		cbFORCEINLINE constexpr cbColor(const std::int32_t r, const std::int32_t g, const std::int32_t b, const std::int32_t a = 255) noexcept
			: R(static_cast<float>(r) / 255.0f)
			, G(static_cast<float>(g) / 255.0f)
			, B(static_cast<float>(b) / 255.0f)
			, A(static_cast<float>(a) / 255.0f)
		{}

		cbFORCEINLINE constexpr cbColor(const cbColor& color) noexcept
			: R(color.R)
			, G(color.G)
			, B(color.B)
			, A(color.A)
		{}

		cbFORCEINLINE constexpr cbColor(const float value) noexcept
			: R(value)
			, G(value)
			, B(value)
			, A(1.0f)
		{}

		cbFORCEINLINE ~cbColor() = default;

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ R: " + std::to_string(R) + " G: " + std::to_string(G) + " B: " + std::to_string(B) + " Alpha: " + std::to_string(A) + " };");
		}

		inline static constexpr cbColor FromNonPremultiplied(const int& r, const int& g, const int& b, const int& a)
		{
			return cbColor(
				(static_cast<float>(r) * static_cast<float>(a) / 255.0f),
				(static_cast<float>(g) * static_cast<float>(a) / 255.0f),
				(static_cast<float>(b) * static_cast<float>(a) / 255.0f),
				static_cast<float>(a)
			);
		}

		inline static constexpr cbColor Lerp(const cbColor& value1, const cbColor& value2, const float amount)
		{
			auto lClamp = [](const float& value, const float& min, const float& max) -> float
			{
				float Value = value;
				Value = (Value > max) ? max : Value;

				Value = (Value < min) ? min : Value;

				return Value;
			};

			float _amount = lClamp(amount, 0.0f, 1.0f);
			return cbColor(
				lClamp(value1.R, value2.R, _amount),
				lClamp(value1.G, value2.G, _amount),
				lClamp(value1.B, value2.B, _amount),
				lClamp(value1.A, value2.A, _amount)
			);
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbColor&) const = default;
#endif

#if _MSVC_LANG < 202002L
		inline constexpr bool operator ==(const cbColor& b)
		{
			return (A == b.A &&
				R == b.R &&
				G == b.G &&
				B == b.B);
		}

		inline constexpr bool operator !=(const cbColor& b)
		{
			return !(A == b.A)
				&& (R == b.R)
				&& (G == b.G)
				&& (B == b.B);
		}
#endif

		inline constexpr cbColor operator *(const float& scale)
		{
			return cbColor(
				(R * scale),
				(G * scale),
				(B * scale),
				(A * scale)
			);
		}
	};

	cbFORCEINLINE constexpr cbColor operator *(const cbColor& value, const float& scaleFactor)
	{
		return cbColor(value.R * scaleFactor, value.G * scaleFactor, value.B * scaleFactor, value.A/* * scaleFactor*/);
	}

	cbFORCEINLINE constexpr cbColor operator *(const float& scaleFactor, const cbColor& value)
	{
		return cbColor(value.R * scaleFactor, value.G * scaleFactor, value.B * scaleFactor, value.A/* * scaleFactor*/);
	}

	struct cbMargin
	{
		std::int32_t Left;
		std::int32_t Top;
		std::int32_t Right;
		std::int32_t Bottom;

		cbFORCEINLINE constexpr cbMargin() noexcept
			: Left(0)
			, Top(0)
			, Right(0)
			, Bottom(0)
		{}
		cbFORCEINLINE constexpr cbMargin(const cbMargin& Other) noexcept
			: Left(Other.Left)
			, Top(Other.Top)
			, Right(Other.Right)
			, Bottom(Other.Bottom)
		{}
		cbFORCEINLINE constexpr cbMargin(const std::int32_t& Value) noexcept
			: Left(Value)
			, Top(Value)
			, Right(Value)
			, Bottom(Value)
		{}
		cbFORCEINLINE constexpr cbMargin(const std::int32_t& Horizontal, const std::int32_t& Vertical) noexcept
			: Left(Horizontal)
			, Top(Vertical)
			, Right(Horizontal)
			, Bottom(Vertical)
		{}
		cbFORCEINLINE constexpr cbMargin(const std::int32_t& InLeft, const std::int32_t& InTop, const std::int32_t& InRight, const std::int32_t& InBottom) noexcept
			: Left(InLeft)
			, Top(InTop)
			, Right(InRight)
			, Bottom(InBottom)
		{}

		cbFORCEINLINE ~cbMargin() = default;

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ Left: " + std::to_string(Left) + " Top: " + std::to_string(Top) + " Right: " + std::to_string(Right) + " Bottom: " + std::to_string(Bottom) + " };");
		}

		cbFORCEINLINE constexpr bool IsEqual(const cbMargin& Other) const
		{
			return Left == Other.Left && Top == Other.Top && Right == Other.Right && Bottom == Other.Bottom;
		}

		cbFORCEINLINE constexpr std::int32_t GetWidth() const
		{
			return Left + Right;
		}

		cbFORCEINLINE constexpr std::int32_t GetHeight() const
		{
			return Top + Bottom;
		}

		cbFORCEINLINE constexpr cbMargin operator +=(const cbMargin& value2)
		{
			Left = Left + value2.Left;
			Right = Right + value2.Right;
			Top = Top + value2.Top;
			Bottom = Bottom + value2.Bottom;
			return *this;
		}

		cbFORCEINLINE constexpr cbMargin operator -=(const cbMargin& value2)
		{
			Left = Left - value2.Left;
			Right = Right - value2.Right;
			Top = Top - value2.Top;
			Bottom = Bottom - value2.Bottom;
			return *this;
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbMargin&) const = default;
#endif
	};

	cbFORCEINLINE constexpr cbMargin operator +(const cbMargin& value1, const cbMargin& value2)
	{
		cbMargin Padding;
		Padding.Left = value1.Left + value2.Left;
		Padding.Right = value1.Right + value2.Right;
		Padding.Top = value1.Top + value2.Top;
		Padding.Bottom = value1.Bottom + value2.Bottom;
		return Padding;
	}

	cbFORCEINLINE constexpr cbMargin operator -(const cbMargin& value1, const cbMargin& value2)
	{
		cbMargin Padding;
		Padding.Left = value1.Left - value2.Left;
		Padding.Right = value1.Right - value2.Right;
		Padding.Top = value1.Top - value2.Top;
		Padding.Bottom = value1.Bottom - value2.Bottom;
		return Padding;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	struct cbTDimension
	{
		T Width;
		T Height;

		cbFORCEINLINE constexpr cbTDimension() noexcept
			: Width(0.0f)
			, Height(0.0f)
		{}
		cbFORCEINLINE constexpr cbTDimension(const T InWidth, const T InHeight) noexcept
			: Width(InWidth)
			, Height(InHeight)
		{}
		cbFORCEINLINE constexpr cbTDimension(const cbTVector<T> Dimension2D) noexcept
			: Width(Dimension2D.X)
			, Height(Dimension2D.Y)
		{}
		cbFORCEINLINE constexpr cbTDimension(const cbTDimension<T>& Other) noexcept
			: Width(Other.Width)
			, Height(Other.Height)
		{}

		cbFORCEINLINE ~cbTDimension() = default;

		inline constexpr void SetWidth(const T width) { Width = width; }
		inline constexpr void SetHeight(const T height) { Height = height; }

		inline constexpr T GetWidth() const { return Width; }
		inline constexpr T GetHeight() const { return Height; }

		inline constexpr bool IsEqual(const cbTDimension<T>& Other) const
		{
			return (GetWidth()) == (Other.GetWidth()) && (GetHeight()) == (Other.GetHeight());
		}

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ Width: " + std::to_string(Width) + " Height: " + std::to_string(Height) + " };");
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbTDimension&) const = default;
#endif
	};

	typedef cbTDimension<float> cbDimension;

	template<typename T>
	cbFORCEINLINE constexpr cbTDimension<T> operator +(const cbTDimension<T>& value1, const cbTVector<T>& value2)
	{
		return cbTDimension<T>(value1.Width + value2.X, value1.Height + value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTDimension<T> operator +(const cbTDimension<T>& value1, const cbMargin& value2)
	{
		return cbTDimension<T>(value1.GetWidth() + value2.GetWidth(), value1.GetHeight() + value2.GetHeight());
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTDimension<T> operator +(const cbTDimension<T>& value1, const cbTDimension<T>& value2)
	{
		return cbTDimension<T>(value1.Width + value2.Width, value1.Height + value2.Height);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTDimension<T> operator -(const cbTDimension<T>& value1, const cbTVector<T>& value2)
	{
		return cbTDimension<T>(value1.Width - value2.X, value1.Height - value2.Y);
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTDimension<T> operator -(const cbTDimension<T>& value1, const cbMargin& value2)
	{
		return cbTDimension<T>(value1.GetWidth() - value2.GetWidth(), value1.GetHeight() - value2.GetHeight());
	}

	template<typename T>
	cbFORCEINLINE constexpr cbTDimension<T> operator -(const cbTDimension<T>& value1, const cbTDimension<T>& value2)
	{
		return cbTDimension<T>(value1.Width - value2.Width, value1.Height - value2.Height);
	}

	class cbBounds
	{
	public:
		static constexpr cbBounds Zero()
		{
			return cbBounds(cbVector::Zero(), cbVector::Zero());
		}

	public:
		cbVector Min;
		cbVector Max;

	public:
		cbFORCEINLINE constexpr cbBounds() noexcept
			: Min(cbVector::Zero())
			, Max(cbVector::Zero())
		{}

		cbFORCEINLINE constexpr cbBounds(const cbBounds& Other) noexcept
			: Min(Other.Min)
			, Max(Other.Max)
		{}

		cbFORCEINLINE constexpr cbBounds(const cbVector& InMin, const cbVector& InMax) noexcept
			: Min(InMin)
			, Max(InMax)
		{}

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		cbFORCEINLINE constexpr cbBounds(const cbTDimension<T>& InDimension, const cbTVector<T>& Location = cbTVector<T>::Zero())
			: Min(cbVector(static_cast<float>(cbTVector<T>::Zero().X - (InDimension.GetWidth() / 2.0f)), static_cast<float>(cbTVector<T>::Zero().Y - (InDimension.GetHeight() / 2.0f))))
			, Max(cbVector(static_cast<float>(cbTVector<T>::Zero().X + (InDimension.GetWidth() / 2.0f)), static_cast<float>(cbTVector<T>::Zero().Y + (InDimension.GetHeight() / 2.0f))))
		{
			SetPosition(cbVector(static_cast<float>(Location.X), static_cast<float>(Location.Y)));
		}

		cbFORCEINLINE constexpr cbBounds(const cbDimension& InDimension, const cbVector& Location = cbVector::Zero())
			: Min(cbVector(cbVector::Zero().X - (InDimension.GetWidth() / 2.0f), cbVector::Zero().Y - (InDimension.GetHeight() / 2.0f)))
			, Max(cbVector(cbVector::Zero().X + (InDimension.GetWidth() / 2.0f), cbVector::Zero().Y + (InDimension.GetHeight() / 2.0f)))
		{
			SetPosition(Location);
		}

		cbFORCEINLINE ~cbBounds() = default;

		cbFORCEINLINE constexpr void SetPosition(const cbVector& Position)
		{
			const cbVector Offset = Position - GetCenter();
			Min = Min + Offset;
			Max = Max + Offset;
		}

		cbFORCEINLINE constexpr bool IsValid() const
		{
			return (Min.X < Max.X && Min.Y < Max.Y);
		}

		cbFORCEINLINE constexpr bool Equals(const cbBounds& other) const
		{
			return (Min.Equals(other.Min) && Max.Equals(other.Max));
		}

		cbFORCEINLINE constexpr cbVector GetCorner(const unsigned int& index) const
		{
			switch (index)
			{
			case 0: return Min;
			case 1: return cbVector(Max.X, Min.Y);
			case 2: return Max;
			case 3: return cbVector(Min.X, Max.Y);
			}
			return cbVector(0.f, 0.f);
		}

		cbFORCEINLINE constexpr float GetTop() const { return Min.Y; }
		cbFORCEINLINE constexpr float GetLeft() const { return Min.X; }
		cbFORCEINLINE constexpr float GetRight() const { return Max.X; }
		cbFORCEINLINE constexpr float GetBottom() const { return Max.Y; }

		cbFORCEINLINE constexpr void SetTop(const float Value) { Min.Y = Value; }
		cbFORCEINLINE constexpr void SetLeft(const float Value) { Min.X = Value; }
		cbFORCEINLINE constexpr void SetRight(const float Value) { Max.X = Value; }
		cbFORCEINLINE constexpr void SetBottom(const float Value) { Max.Y = Value; }

		cbFORCEINLINE constexpr cbVector GetCenter() const { return ((Min + Max) * 0.5f); }
		cbFORCEINLINE constexpr cbVector GetExtent() const { return (0.5f * (Max - Min)); }
		cbFORCEINLINE constexpr void SetExtents(const cbVector& InExtent) { cbVector Center = GetCenter(); Min = (Center - InExtent); Max = (Center + InExtent); }
		cbFORCEINLINE constexpr void SetExtents(const cbVector& InExtent, const cbVector& InOrigin) { cbVector Center = InOrigin; Min = (Center - InExtent); Max = (Center + InExtent); }
		cbFORCEINLINE constexpr void SetMinExtend(const cbVector& InExtent) { cbVector Center = GetCenter(); Min = (Center - InExtent); }
		cbFORCEINLINE constexpr void SetMinExtend(const cbVector& InExtent, const cbVector& InOrigin) { cbVector Center = InOrigin; Min = (Center - InExtent); }
		cbFORCEINLINE constexpr void SetMaxExtend(const cbVector& InExtent) { cbVector Center = GetCenter(); Max = (Center + InExtent); }
		cbFORCEINLINE constexpr void SetMaxExtend(const cbVector& InExtent, const cbVector& InOrigin) { cbVector Center = InOrigin; Max = (Center + InExtent); }
		cbFORCEINLINE constexpr float GetHeight() const { return Max.Y - Min.Y; }
		cbFORCEINLINE constexpr float GetWidth() const { return Max.X - Min.X; }
		cbFORCEINLINE constexpr cbDimension GetDimension() const { return cbDimension(GetWidth(), GetHeight()); }

		cbFORCEINLINE constexpr void SetHeight(const float InHeight)
		{
			cbVector Center = GetCenter();
			Min.Y = (Center.Y - (static_cast<float>(InHeight) / 2.0f));
			Max.Y = (Center.Y + (static_cast<float>(InHeight) / 2.0f));
		}

		cbFORCEINLINE constexpr void SetHeight(const float InHeight, const cbVector& InOrigin)
		{
			const cbVector Center = InOrigin;
			Min.Y = (Center.Y - (static_cast<float>(InHeight) / 2.0f));
			Max.Y = (Center.Y + (static_cast<float>(InHeight) / 2.0f));
		}

		cbFORCEINLINE constexpr void SetWidth(const float InWidth)
		{
			const cbVector Center = GetCenter();
			Min.X = (Center.X - (static_cast<float>(InWidth) / 2.0f));
			Max.X = (Center.X + (static_cast<float>(InWidth) / 2.0f));
		}

		cbFORCEINLINE constexpr void SetWidth(const float InWidth, const cbVector& InOrigin)
		{
			const cbVector Center = InOrigin;
			Min.X = (Center.X - (static_cast<float>(InWidth) / 2.0f));
			Max.X = (Center.X + (static_cast<float>(InWidth) / 2.0f));
		}

		cbFORCEINLINE constexpr void SetDimension(const cbDimension& InDimension)
		{
			const cbVector Center = GetCenter();
			Min.X = (Center.X - (static_cast<float>(InDimension.GetWidth()) / 2.0f));
			Min.Y = (Center.Y - (static_cast<float>(InDimension.GetHeight()) / 2.0f));
			Max.X = (Center.X + (static_cast<float>(InDimension.GetWidth()) / 2.0f));
			Max.Y = (Center.Y + (static_cast<float>(InDimension.GetHeight()) / 2.0f));
		}

		cbFORCEINLINE constexpr void Reset(const cbDimension& InDimension, const cbVector& InOrigin)
		{
			const cbVector Center = InOrigin;
			Min.X = (Center.X - (static_cast<float>(InDimension.GetWidth()) / 2.0f));
			Min.Y = (Center.Y - (static_cast<float>(InDimension.GetHeight()) / 2.0f));
			Max.X = (Center.X + (static_cast<float>(InDimension.GetWidth()) / 2.0f));
			Max.Y = (Center.Y + (static_cast<float>(InDimension.GetHeight()) / 2.0f));
		}

		cbFORCEINLINE constexpr void Expand(const cbBounds& Rect)
		{
			if (Rect.Min.X < Min.X)
			{
				Min.X = Rect.Min.X;
			}
			if (Rect.Min.Y < Min.Y)
			{
				Min.Y = Rect.Min.Y;
			}
			if (Rect.Max.X > Max.X)
			{
				Max.X = Rect.Max.X;
			}
			if (Rect.Max.Y > Max.Y)
			{
				Max.Y = Rect.Max.Y;
			}
		}

		cbFORCEINLINE constexpr cbBounds Crop(const cbBounds& FilterBound)
		{
			if (Min.X < FilterBound.Min.X)
			{
				Min.X = FilterBound.Min.X;
			}
			if (Min.Y < FilterBound.Min.Y)
			{
				Min.Y = FilterBound.Min.Y;
			}
			if (Max.X > FilterBound.Max.X)
			{
				Max.X = FilterBound.Max.X;
			}
			if (Max.Y > FilterBound.Max.Y)
			{
				Max.Y = FilterBound.Max.Y;
			}

			return *this;
		}

		cbFORCEINLINE constexpr bool IsInside(const cbVector& value) const
		{
			return ((value.X >= Min.X) && (value.X <= Max.X) && (value.Y >= Min.Y) && (value.Y <= Max.Y));
		}

		cbFORCEINLINE constexpr bool IsInside(const cbBounds& Other) const
		{
			return (IsInside(Other.Min) && IsInside(Other.Max));
		}

		cbFORCEINLINE constexpr bool Intersect(const cbVector& InLocation) const
		{
			if ((Min.X > InLocation.X) || (InLocation.X > Max.X))
			{
				return false;
			}

			if ((Min.Y > InLocation.Y) || (InLocation.Y > Max.Y))
			{
				return false;
			}

			return true;
		}

		cbFORCEINLINE constexpr bool Intersect(const cbBounds& Other) const
		{
			if ((Min.X > Other.Max.X) || (Other.Min.X > Max.X))
			{
				return false;
			}

			if ((Min.Y > Other.Max.Y) || (Other.Min.Y > Max.Y))
			{
				return false;
			}

			return true;
		}

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ Min: " + Min.ToString() + " Max: " + Max.ToString() + " };");
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbBounds&) const = default;
#endif
	};

	cbFORCEINLINE constexpr cbBounds operator +(const cbBounds& value1, const cbMargin& value2)
	{
		return cbBounds(cbVector(value1.Min.X - value2.Left, value1.Min.Y - value2.Top), cbVector(value1.Max.X + value2.Right, value1.Max.Y + value2.Bottom));
	}

	cbFORCEINLINE constexpr cbBounds operator -(const cbBounds& value1, const cbMargin& value2)
	{
		return cbBounds(cbVector(value1.Min.X + value2.Left, value1.Min.Y + value2.Top), cbVector(value1.Max.X - value2.Right, value1.Max.Y - value2.Bottom));
	}

	cbFORCEINLINE constexpr cbBounds operator +(const cbBounds& Rect, const cbVector& V)
	{
		return cbBounds(Rect.Min + V, Rect.Max + V);
	}

	cbFORCEINLINE constexpr cbBounds operator -(const cbBounds& Rect, const cbVector& V)
	{
		return cbBounds(Rect.Min - V, Rect.Max - V);
	}

	typedef cbTVector<std::int16_t> cbInt16Vec;

	class cbIntBounds
	{
	public:
		static constexpr cbIntBounds Zero()
		{
			return cbIntBounds(cbInt16Vec::Zero(), cbInt16Vec::Zero());
		}

	public:
		cbInt16Vec Min;
		cbInt16Vec Max;

	public:
		cbFORCEINLINE constexpr cbIntBounds() noexcept
			: Min(cbInt16Vec::Zero())
			, Max(cbInt16Vec::Zero())
		{}

		cbFORCEINLINE constexpr cbIntBounds(const cbIntBounds& Other) noexcept
			: Min(Other.Min)
			, Max(Other.Max)
		{}

		cbFORCEINLINE constexpr cbIntBounds(const cbBounds& Bounds) noexcept
			: Min(cbInt16Vec(static_cast<std::int16_t>(std::floor(Bounds.Min.X)), static_cast<std::int16_t>(std::floor(Bounds.Min.Y))))
			, Max(cbInt16Vec(static_cast<std::int16_t>(std::ceil(Bounds.Max.X)), static_cast<std::int16_t>(std::ceil(Bounds.Max.Y))))
		{}

		cbFORCEINLINE constexpr cbIntBounds(const cbInt16Vec& InMin, const cbInt16Vec& InMax) noexcept
			: Min(InMin)
			, Max(InMax)
		{}

		cbFORCEINLINE ~cbIntBounds() = default;

		cbFORCEINLINE constexpr bool IsValid() const
		{
			return (Min.X < Max.X && Min.Y < Max.Y);
		}

		cbFORCEINLINE constexpr bool Equals(const cbIntBounds& other) const
		{
			return (Min.Equals(other.Min) && Max.Equals(other.Max));
		}

		cbFORCEINLINE constexpr std::int16_t GetTop() const { return Min.Y; }
		cbFORCEINLINE constexpr std::int16_t GetLeft() const { return Min.X; }
		cbFORCEINLINE constexpr std::int16_t GetRight() const { return Max.X; }
		cbFORCEINLINE constexpr std::int16_t GetBottom() const { return Max.Y; }

		cbFORCEINLINE constexpr cbVector GetCenter() const { return cbVector((Min.X + Max.X) * 0.5f, (Min.Y + Max.Y) * 0.5f); }
		cbFORCEINLINE constexpr float GetHeight() const { return static_cast<float>(Max.Y) - static_cast<float>(Min.Y); }
		cbFORCEINLINE constexpr float GetWidth() const { return static_cast<float>(Max.X) - static_cast<float>(Min.X); }
		cbFORCEINLINE constexpr cbDimension GetDimension() const { return cbDimension(GetWidth(), GetHeight()); }

		cbFORCEINLINE constexpr const cbIntBounds& Crop(const cbIntBounds& FilterBound) noexcept
		{
			if (Min.X < FilterBound.Min.X)
			{
				Min.X = FilterBound.Min.X;
			}
			if (Min.Y < FilterBound.Min.Y)
			{
				Min.Y = FilterBound.Min.Y;
			}
			if (Max.X > FilterBound.Max.X)
			{
				Max.X = FilterBound.Max.X;
			}
			if (Max.Y > FilterBound.Max.Y)
			{
				Max.Y = FilterBound.Max.Y;
			}

			return *this;
		}

		cbCONSTEXPR20 std::string ToString() const
		{
			return std::string("{ Min: " + Min.ToString() + " Max: " + Max.ToString() + " };");
		}

#if _MSVC_LANG >= 202002L
		auto operator<=>(const cbIntBounds&) const = default;
#endif
	};

	cbFORCEINLINE constexpr cbIntBounds operator +(const cbIntBounds& value1, const cbMargin& value2)
	{
		return cbIntBounds(cbInt16Vec(value1.Min.X - value2.Left, value1.Min.Y - value2.Top), cbInt16Vec(value1.Max.X + value2.Right, value1.Max.Y + value2.Bottom));
	}

	cbFORCEINLINE constexpr cbIntBounds operator -(const cbIntBounds& value1, const cbMargin& value2)
	{
		return cbIntBounds(cbInt16Vec(value1.Min.X + value2.Left, value1.Min.Y + value2.Top), cbInt16Vec(value1.Max.X - value2.Right, value1.Max.Y - value2.Bottom));
	}

	cbFORCEINLINE constexpr cbTVector<float> GetInverse(const cbTVector<float>& V)
	{
		return cbTVector(-V.X, -V.Y);
	}

	cbFORCEINLINE constexpr cbTVector<std::int32_t> GetInverse(const cbTVector<std::int32_t>& V)
	{
		return cbTVector(-V.X, -V.Y);
	}

	cbFORCEINLINE constexpr cbTVector3<float> GetInverse(const cbTVector3<float>& V)
	{
		return cbTVector3(-V.X, -V.Y, -V.Z);
	}

	cbFORCEINLINE constexpr cbTVector3<std::int32_t> GetInverse(const cbTVector3<std::int32_t>& V)
	{
		return cbTVector3(-V.X, -V.Y, -V.Z);
	}

	cbFORCEINLINE constexpr cbVector GetAnchorPointsFromRect(const eCanvasAnchor& Anchor, const cbBounds& Rect)
	{
		switch (Anchor)
		{
		case eCanvasAnchor::NONE: return cbVector();
		case eCanvasAnchor::Center: return Rect.GetCenter();
		case eCanvasAnchor::Top: return cbVector(Rect.GetCenter().X, Rect.Min.Y);
		case eCanvasAnchor::Bottom: return cbVector(Rect.GetCenter().X, Rect.Max.Y);
		case eCanvasAnchor::Left: return cbVector(Rect.Min.X, Rect.GetCenter().Y);
		case eCanvasAnchor::Right: return cbVector(Rect.Max.X, Rect.GetCenter().Y);
		case eCanvasAnchor::LeftTop: return Rect.Min;
		case eCanvasAnchor::RightTop: return cbVector(Rect.Max.X, Rect.Min.Y);
		case eCanvasAnchor::RightBottom: return Rect.Max;
		case eCanvasAnchor::LeftBottom: return cbVector(Rect.Min.X, Rect.Max.Y);
		}
		return cbVector(0.f, 0.f);
	}

	cbFORCEINLINE constexpr float Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	cbFORCEINLINE constexpr bool IsInside(const cbBounds& Bounds, const cbVector& Point)
	{
		return Bounds.IsInside(Point);
	}

	cbFORCEINLINE constexpr bool Intersect(const cbBounds& Bounds, const cbBounds& Other)
	{
		return Bounds.Intersect(Other);
	}

	cbFORCEINLINE constexpr float Clamp(float x, float lowerlimit, float upperlimit)
	{
		if (x < lowerlimit)
			x = lowerlimit;
		if (x > upperlimit)
			x = upperlimit;
		return x;
	}

	cbFORCEINLINE constexpr float RadiansToDegrees(const float Radians)
	{
		return Radians * (180.f / 3.1415926535897932f);
	}

	cbFORCEINLINE constexpr float DegreesToRadians(const float Degrees)
	{
		return Degrees * (3.1415926535897932f / 180.f);
	}

	cbFORCEINLINE cbVector RotateVectorAroundPoint(const cbVector& V, const cbVector& P, const float Angle)
	{
		auto ClampAxis = [](const float pAngle) -> float
		{
			return 360.0f * ((pAngle / 360.0f) - std::floor(pAngle / 360.0f));
		};

		const float Cos(cos(DegreesToRadians(ClampAxis(Angle))));
		const float Sin(sin(DegreesToRadians(ClampAxis(Angle))));
		return cbVector((V.X - P.X) * Cos - (V.Y - P.Y) * Sin + P.X, (V.X - P.X) * Sin + (V.Y - P.Y) * Cos + P.Y);
	}

	cbFORCEINLINE cbVector RotateVectorAroundPoint(const cbVector4& V, const cbVector& P, const float Angle)
	{
		return RotateVectorAroundPoint(cbVector(V.X, V.Y), P, Angle);
	}

	cbFORCEINLINE constexpr bool Intersect(const cbBounds& Bounds, const cbBounds& Other, const float Rotation, const cbVector& Origin)
	{
		if (Rotation != 0.0f)
		{
			const cbBounds RotatedRect = cbBounds(Bounds.GetDimension(), RotateVectorAroundPoint(Bounds.GetCenter(), Origin, Rotation));
			const cbBounds RecalculatedBB = cbBounds(Other.GetDimension(), RotateVectorAroundPoint(Other.GetCenter(), RotatedRect.GetCenter(), Rotation * (-1.0f)));
			return RotatedRect.Intersect(RecalculatedBB);
		}
		return Bounds.Intersect(Other);
	}

	cbFORCEINLINE constexpr bool IsInside(const cbBounds& Bounds, const cbVector& Point, const float Rotation, const cbVector& Origin)
	{
		if (Rotation != 0.0f)
		{
			const cbBounds RotatedRect = cbBounds(Bounds.GetDimension(), RotateVectorAroundPoint(Bounds.GetCenter(), Origin, Rotation));
			return RotatedRect.IsInside(RotateVectorAroundPoint(Point, RotatedRect.GetCenter(), Rotation * (-1.0f)));
		}
		return Bounds.IsInside(Point);
	}

	cbFORCEINLINE constexpr cbBounds RecalculateBounds(const cbDimension& Dimension, const cbVector& Location, float Rotation, const cbVector& Origin)
	{
		auto Recalculate = [](const cbBounds& Bounds, std::optional<cbVector> Origin, const float Roll) -> cbBounds
		{
			if (Roll != 0.0f)
				return cbBounds(cbDimension(std::abs(((float)Bounds.GetHeight()) * std::sin(DegreesToRadians(Roll))) + std::abs(((float)Bounds.GetWidth()) * std::cos(DegreesToRadians(Roll))),
					std::abs(((float)Bounds.GetWidth()) * std::sin(DegreesToRadians(Roll))) + std::abs(((float)Bounds.GetHeight()) * std::cos(DegreesToRadians(Roll)))), Origin.has_value() ? Origin.value() : Bounds.GetCenter());
			return Bounds;
		};

		if (Rotation == 0.0f)
			return cbBounds(Dimension, Location);

		return Recalculate(Dimension, RotateVectorAroundPoint(Location, Origin, Rotation), Rotation);
	}

	cbFORCEINLINE constexpr cbBounds Crop(const cbBounds& InBound, const cbBounds& FilterBound)
	{
		cbBounds Bound = InBound;

		if (InBound.Min.X < FilterBound.Min.X)
		{
			Bound.Min.X = FilterBound.Min.X;
		}
		if (InBound.Min.Y < FilterBound.Min.Y)
		{
			Bound.Min.Y = FilterBound.Min.Y;
		}
		if (InBound.Max.X > FilterBound.Max.X)
		{
			Bound.Max.X = FilterBound.Max.X;
		}
		if (InBound.Max.Y > FilterBound.Max.Y)
		{
			Bound.Max.Y = FilterBound.Max.Y;
		}

		return Bound;
	}

	cbFORCEINLINE constexpr std::array<cbVector4, 4> GetViewportTransform(const int width, const int height)
	{
		const int Dx = 0;
		const int Dy = 0;
		const int x = width;
		const int y = height;

		const float L = static_cast<float>(Dx);
		const float R = static_cast<float>(Dx + x);
		const float T = static_cast<float>(Dy);
		const float B = static_cast<float>(Dy + y);

		std::array<cbVector4, 4> Matrix;
		Matrix[0][0] = 2.0f / (R - L);
		Matrix[0][1] = 0.0f;
		Matrix[0][2] = 0.0f;
		Matrix[0][3] = 0.0f;

		Matrix[1][0] = 0.0f;
		Matrix[1][1] = 2.0f / (T - B);
		Matrix[1][2] = 0.0f;
		Matrix[1][3] = 0.0f;

		Matrix[2][0] = 0.0f;
		Matrix[2][1] = 0.0f;
		Matrix[2][2] = 0.5f;
		Matrix[2][3] = 0.0f;

		Matrix[3][0] = (R + L) / (L - R);
		Matrix[3][1] = (T + B) / (B - T);
		Matrix[3][2] = 0.5f;
		Matrix[3][3] = 1.00000000f;

		return Matrix;
	}
}
