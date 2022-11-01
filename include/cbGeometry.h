/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Coşkun.
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

#include "cbClassBody.h"
#include "cbVertexColorStyle.h"
#include "cbMath.h"

namespace cbgui
{
	/* Widget vertex data. */
	__declspec(align(16)) struct cbGeometryVertexData
	{
		cbVector4 position;
		cbVector texCoord;
		cbColor Color;

		cbFORCEINLINE constexpr cbGeometryVertexData()
			: position(cbVector4::Zero())
			, texCoord(cbVector::Zero())
			, Color(cbColor(1.0f, 1.0f, 1.0f, 1.0f))
		{}
	};

	/* Helper functions for creating geometry. */
	namespace cbGeometryFactory
	{
		std::vector<cbVector4> Create4DPlaneVerticesFromRect(const cbBounds& Rect);
		std::array<cbVector4, 4> CreatePlaneVerticesFromRect(const cbBounds& Rect);
		std::vector<cbVector4> Create4DPlaneVerticesFromDimension(const cbDimension& Dimension);
		std::array<cbVector4, 4> CreatePlaneVerticesFromDimension(const cbDimension& Dimension);
		std::array<cbVector4, 6> CreateTriangleVerticesFromRect(const cbBounds& Rect, unsigned int Corner = 0);
		std::vector<std::uint32_t> GeneratePlaneIndices(const std::uint32_t PrimitiveSize = 1);
		std::vector<std::uint32_t> GenerateTriangleIndices(const std::uint32_t PrimitiveSize = 1);
		std::vector<cbVector> GeneratePlaneTextureCoordinate(const float Angle = 0.0f);
		std::vector<cbVector> GenerateTriangleTextureCoordinate(const float Angle = 0.0f);
		std::vector<std::uint32_t> GenerateIndices(const std::size_t& Size);
		std::vector<cbVector4> GenerateBorderVertices(const cbBounds& Bounds, const cbMargin& Thickness);
		std::vector<cbVector4> GenerateBorderVertices(const cbDimension& Dimension, const cbMargin& Thickness);
		std::vector<cbVector> GenerateBorderTextureCoordinate();
		std::vector<std::uint32_t> GenerateBorderIndices();

		std::vector<cbGeometryVertexData> GetAlignedVertexData(const std::vector<cbVector4>& Vertices, const std::vector<cbVector>& TextureCoordinates,
			const cbColor& Color, const cbVector& Location, const float& Rotation, const cbVector& Origin);
	}

	struct cbGeometryDrawData
	{
		cbBaseClassBody(cbClassConstructor, cbGeometryDrawData);
	public:
		cbFORCEINLINE cbGeometryDrawData(const std::string inGeometryType, const unsigned short inStyleState,
										 const std::size_t inVertexCount, const std::size_t inIndexCount, const std::size_t inDrawCount)
			: GeometryType(inGeometryType)
			, StyleState(inStyleState)
			, VertexCount(inVertexCount)
			, IndexCount(inIndexCount)
			, DrawCount(inDrawCount)
		{}

		virtual ~cbGeometryDrawData() = default;

		std::string GeometryType;
		/* If it's a checkbox, button, or similar, it returns its state. */
		unsigned short StyleState;
		std::size_t VertexCount;
		std::size_t IndexCount;
		/* Controls the number of verticle drawn on the screen */
		std::size_t DrawCount;
	};
}
