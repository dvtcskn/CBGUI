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
#include "cbGeometry.h"
#include "cbFont.h"

/*
* cbGeometryFactory impl
*/

namespace cbgui
{
	namespace cbGeometryFactory
	{
		std::array<cbVector4, 4> CreatePlaneVertices()
		{
			std::array<cbVector4, 4> Plane;
			Plane[0] = cbVector4(-1.0000f, -1.0000f, 0.0f, 1.0f);
			Plane[1] = cbVector4(1.0000f, -1.0000f, 0.0f, 1.0f);
			Plane[2] = cbVector4(1.0000f, 1.0000f, 0.0f, 1.0f);
			Plane[3] = cbVector4(-1.0000f, 1.0000f, 0.0f, 1.0f);
			return Plane;
		}

		std::vector<cbVector4> Create4DPlaneVerticesFromDimension(const cbDimension& Dimension)
		{
			return Create4DPlaneVerticesFromRect(cbBounds(Dimension, cbVector::Zero()));
		}

		std::vector<cbVector4> Create4DPlaneVerticesFromRect(const cbBounds& Rect)
		{
			std::vector<cbVector4> Plane;
			Plane.resize(4);
			Plane[0] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
			Plane[1] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
			Plane[2] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
			Plane[3] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
			return Plane;
		}

		std::array<cbVector4, 4> CreatePlaneVerticesFromDimension(const cbDimension& Dimension)
		{
			return CreatePlaneVerticesFromRect(cbBounds(Dimension, cbVector::Zero()));
		}

		std::array<cbVector4, 4> CreatePlaneVerticesFromRect(const cbBounds& Rect)
		{
			std::array<cbVector4, 4> Plane;
			Plane[0] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
			Plane[1] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
			Plane[2] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
			Plane[3] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
			return Plane;
		}

		std::array<cbVector4, 6> CreateTriangleVerticesFromRect(const cbBounds& Rect, unsigned int Corner)
		{
			std::array<cbVector4, 6> Triangles;
			switch (Corner)
			{
			case 0:
			{
				Triangles[0] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[1] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[2] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
			}
			{
				Triangles[3] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[4] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[5] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
			}
			break;
			case 1:
			{
				Triangles[0] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[1] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[2] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
			}
			{
				Triangles[3] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[4] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[5] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
			}
			break;
			case 2:
			{
				Triangles[0] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[1] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[2] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
			}
			{
				Triangles[3] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[4] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[5] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
			}
			break;
			case 3:
			{
				Triangles[0] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[1] = cbVector4(Rect.Min.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[2] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
			}
			{
				Triangles[3] = cbVector4(Rect.Min.X, Rect.Max.Y, 0.0f, 1.0f);
				Triangles[4] = cbVector4(Rect.Max.X, Rect.Min.Y, 0.0f, 1.0f);
				Triangles[5] = cbVector4(Rect.Max.X, Rect.Max.Y, 0.0f, 1.0f);
			}
			break;
			}

			return Triangles;
		}

		std::vector<std::uint32_t> GeneratePlaneIndices(const std::uint32_t PrimitiveSize)
		{
			std::vector<std::uint32_t> Indices;
			for (std::uint32_t i = 0; i < PrimitiveSize; i++)
			{
				Indices.push_back(3 + (4 * i));
				Indices.push_back(1 + (4 * i));
				Indices.push_back(0 + (4 * i));

				Indices.push_back(3 + (4 * i));
				Indices.push_back(2 + (4 * i));
				Indices.push_back(1 + (4 * i));
			}
			return Indices;
		}

		std::vector<std::uint32_t> GenerateTriangleIndices(const std::uint32_t PrimitiveSize)
		{
			std::vector<std::uint32_t> Indices;
			for (std::uint32_t i = 0; i < PrimitiveSize; i++)
			{
				Indices.push_back(2 + (3 * i));
				Indices.push_back(1 + (3 * i));
				Indices.push_back(0 + (3 * i));
			}
			return Indices;
		}

		std::vector<cbVector> GeneratePlaneTextureCoordinate(const float Angle)
		{
			auto RotateCornerPoint = [](cbVector& Edge, float angle)
			{
				cbVector Center = cbVector(0.5f, 0.5f);
				double theta = (angle * (3.14159265359 / 180));

				float cs = static_cast<float>(cos(theta));
				float sn = static_cast<float>(sin(theta));

				float x = Edge.X - Center.X;
				float y = Edge.Y - Center.Y;

				Edge.X = x * cs - y * sn + Center.X;
				Edge.Y = (x * sn + y * cs + Center.Y)/* * (-1)*/;
			};
			cbVector P1 = cbVector(static_cast<float>(0.0), static_cast<float>(0.0));
			cbVector P2 = cbVector(static_cast<float>(1.0), static_cast<float>(0.0));
			cbVector P3 = cbVector(static_cast<float>(1.0), static_cast<float>(1.0));
			cbVector P4 = cbVector(static_cast<float>(0.0), static_cast<float>(1.0));
			if (Angle != 0.0f)
			{
				RotateCornerPoint(P1, Angle);
				RotateCornerPoint(P2, Angle);
				RotateCornerPoint(P3, Angle);
				RotateCornerPoint(P4, Angle);
			}
			return std::vector<cbVector>{ P1, P2, P3, P4 };
		}

		std::vector<cbVector> GenerateTriangleTextureCoordinate(const float Angle)
		{
			auto RotateCornerPoint = [](const cbVector& Center, cbVector& Edge, const float& angle)
			{
				double theta = (angle * (3.14159265359 / 180));

				float cs = static_cast<float>(cos(theta));
				float sn = static_cast<float>(sin(theta));

				float x = Edge.X - Center.X;
				float y = Edge.Y - Center.Y;

				Edge.X = x * cs - y * sn + Center.X;
				Edge.Y = (x * sn + y * cs + Center.Y)/* * (-1)*/;
			};
			cbVector P1 = cbVector(static_cast<float>(1.0), static_cast<float>(1.0));
			cbVector P2 = cbVector(static_cast<float>(1.0), static_cast<float>(0.0));
			cbVector P3 = cbVector(static_cast<float>(0.0), static_cast<float>(1.0));
			cbVector Center = cbVector((P1.X + P2.X + P3.X) / 3, (P1.Y + P2.Y + P3.Y) / 3);
			if (Angle != 0.0f)
			{
				RotateCornerPoint(Center, P1, Angle);
				RotateCornerPoint(Center, P2, Angle);
				RotateCornerPoint(Center, P3, Angle);
			}

			return std::vector<cbVector>{ P1, P2, P3 };
		}

		std::vector<std::uint32_t> GenerateIndices(const std::size_t& Size)
		{
			std::vector<std::uint32_t> Indices;
			for (int i = 0; i < Size; i++)
			{
				std::uint32_t Offset = i == 0 ? 0 : (4 * i);
				{
					Indices.push_back(3 + Offset);
					Indices.push_back(1 + Offset);
					Indices.push_back(0 + Offset);

					Indices.push_back(3 + Offset);
					Indices.push_back(2 + Offset);
					Indices.push_back(1 + Offset);
				}
			}
			return Indices;
		}

		std::vector<cbVector4> GenerateBorderVertices(const cbDimension& Dimension, const cbMargin& Thickness)
		{
			return GenerateBorderVertices(cbBounds(Dimension, cbVector::Zero()), Thickness);
		}

		std::vector<cbVector4> GenerateBorderVertices(const cbBounds& Bounds, const cbMargin& Thickness)
		{			
			const std::array<cbBounds, 8> FrameBounds = {cbBounds(cbDimension((float)Thickness.Left, (float)Thickness.Top), 
															 Bounds.Min + cbVector((float)Thickness.Left / 2.0f, (float)Thickness.Top / 2.0f)),
														 cbBounds(cbDimension((float)(Bounds.GetWidth() - (Thickness.Left + Thickness.Right)), (float)Thickness.Top),
															 cbVector(Bounds.GetCenter().X, Bounds.Min.Y) + cbVector(static_cast<float>(Thickness.Left - Thickness.Right) / 2.0f, (float)Thickness.Top / 2.0f)),
														 cbBounds(cbDimension((float)Thickness.Right, (float)Thickness.Top),
															 cbVector(Bounds.Max.X, Bounds.Min.Y) + cbVector(((float)Thickness.Right * (-1.0f)) / 2.0f, (float)Thickness.Top / 2.0f)),
														 cbBounds(cbDimension((float)Thickness.Right, (float)(Bounds.GetHeight() - (Thickness.Top + Thickness.Bottom))),
															 cbVector(Bounds.Max.X, Bounds.GetCenter().Y) + cbVector(((float)Thickness.Right * (-1.0f)) / 2.0f, static_cast<float>(Thickness.Top - Thickness.Bottom) / 2.0f)),
														 cbBounds(cbDimension((float)Thickness.Right, (float)Thickness.Bottom),
															 Bounds.Max + cbVector(((float)Thickness.Right * (-1.0f)) / 2.0f, ((float)Thickness.Bottom * (-1.0f)) / 2.0f)),
														 cbBounds(cbDimension((float)(Bounds.GetWidth() - (Thickness.Left + Thickness.Right)), (float)Thickness.Bottom),
															 cbVector(Bounds.GetCenter().X, Bounds.Max.Y) + cbVector(static_cast<float>(Thickness.Left - Thickness.Right) / 2.0f, ((float)Thickness.Bottom * (-1.0f)) / 2.0f)),
														 cbBounds(cbDimension((float)Thickness.Left, (float)Thickness.Bottom),
															 cbVector(Bounds.Min.X, Bounds.Max.Y) + cbVector((float)Thickness.Left / 2.0f, ((float)Thickness.Bottom * (-1.0f)) / 2.0f)),
														 cbBounds(cbDimension((float)Thickness.Left, (float)(Bounds.GetHeight() - (Thickness.Top + Thickness.Bottom))),
															 cbVector(Bounds.Min.X, Bounds.GetCenter().Y) + cbVector((float)Thickness.Left / 2.0f, static_cast<float>(Thickness.Top - Thickness.Bottom) / 2.0f)) };

			std::vector<cbVector4> VPlanes;
			std::vector<cbVector4> VTriangels;
			const std::array<unsigned short, 4> Edges = { 0, 1, 2, 3 };

			for (unsigned short i = 0; i < 8; i++)
			{
				if (i % 2)
				{
					auto Verts = CreatePlaneVerticesFromRect(FrameBounds[i]);
					VPlanes.insert(VPlanes.end(), Verts.begin(), Verts.end());
				}
				else
				{
					auto Triangles = CreateTriangleVerticesFromRect(FrameBounds[i], Edges[i / 2]);
					VTriangels.insert(VTriangels.end(), Triangles.begin(), Triangles.end());
				}
			}
			std::vector<cbVector4> VertexPositions;
			VertexPositions.insert(VertexPositions.end(), VPlanes.begin(), VPlanes.end());
			VertexPositions.insert(VertexPositions.end(), VTriangels.begin(), VTriangels.end());

			return VertexPositions;
		}

		std::vector<cbVector> GenerateBorderTextureCoordinate()
		{
			const float TexCoordModifier = 0.125075f;
			return std::vector<cbVector> {  cbVector(0.0f + TexCoordModifier, 0.0f),
											cbVector(1.0f - TexCoordModifier, 0.0f),
											cbVector(1.0f - TexCoordModifier, 0.0f + TexCoordModifier),
											cbVector(0.0f + TexCoordModifier, 0.0f + TexCoordModifier),			
											cbVector(1.0f - TexCoordModifier, 0.0f + TexCoordModifier),
											cbVector(1.0f, 0.0f + TexCoordModifier),
											cbVector(1.0f, 1.0f - TexCoordModifier),
											cbVector(1.0f - TexCoordModifier, 1.0f - TexCoordModifier),			
											cbVector(0.0f + TexCoordModifier, 1.0f - TexCoordModifier),
											cbVector(1.0f - TexCoordModifier, 1.0f - TexCoordModifier),
											cbVector(1.0f - TexCoordModifier, 1.0f),
											cbVector(0.0f + TexCoordModifier, 1.0f),			
											cbVector(0.0f, 0.0f + TexCoordModifier),
											cbVector(0.0f + TexCoordModifier, 0.0f + TexCoordModifier),
											cbVector(0.0f + TexCoordModifier, 1.0f - TexCoordModifier),
											cbVector(0.0f, 1.0f - TexCoordModifier),			
											cbVector(0.0f, 0.0f),
											cbVector(0.0f + TexCoordModifier, 0.0f + TexCoordModifier),
											cbVector(0.0f, 0.0f + TexCoordModifier),
											cbVector(0.0f, 0.0f),
											cbVector(0.0f + TexCoordModifier, 0.0f),
											cbVector(0.0f + TexCoordModifier, 0.0f + TexCoordModifier),			
											cbVector(1.0f, 0.0f),
											cbVector(1.0f - TexCoordModifier, 0.0f + TexCoordModifier),
											cbVector(1.0f - TexCoordModifier, 0.0f),
											cbVector(1.0f, 0.0f),
											cbVector(1.0f, 0.0f + TexCoordModifier),
											cbVector(1.0f - TexCoordModifier, 0.0f + TexCoordModifier),			
											cbVector(1.0f, 1.0f),
											cbVector(1.0f - TexCoordModifier, 1.0f - TexCoordModifier),
											cbVector(1.0f, 1.0f - TexCoordModifier),
											cbVector(1.0f, 1.0f),
											cbVector(1.0f - TexCoordModifier, 1.0f),
											cbVector(1.0f - TexCoordModifier, 1.0f - TexCoordModifier),			
											cbVector(0.0f, 1.0f),
											cbVector(0.0f, 1.0f - TexCoordModifier),
											cbVector(0.0f + TexCoordModifier, 1.0f - TexCoordModifier),
											cbVector(0.0f, 1.0f),
											cbVector(0.0f + TexCoordModifier, 1.0f - TexCoordModifier),
											cbVector(0.0f + TexCoordModifier, 1.0f)};
			
			/*std::vector<cbVector> PlaneTC;
			std::vector<cbVector> TriangelsTC;

			for (int i = 0; i < 8; i++)
			{
				if (i % 2)
				{
					auto TC = GeneratePlaneTextureCoordinate(i == 1 ? 90.0f : i == 3 ? 0.0f : i == 5 ? 270.0f : i == 7 ? 180.0f : 0.0f);
					PlaneTC.insert(PlaneTC.end(), TC.begin(), TC.end());
				}
				else
				{
					{
						auto TC = GenerateTriangleTextureCoordinate(i == 6 ? 0.0f : 270.0f);
						TriangelsTC.insert(TriangelsTC.end(), TC.begin(), TC.end());
					}
					{
						auto TC = GenerateTriangleTextureCoordinate(i == 6 ? 270.0f : 0.0f);
						TriangelsTC.insert(TriangelsTC.end(), TC.begin(), TC.end());
					}
				}
			}

			std::vector<cbVector> TextureCoordinates;
			TextureCoordinates.insert(TextureCoordinates.end(), PlaneTC.begin(), PlaneTC.end());
			TextureCoordinates.insert(TextureCoordinates.end(), TriangelsTC.begin(), TriangelsTC.end());

			return TextureCoordinates;*/
		}

		std::vector<std::uint32_t> GenerateBorderIndices()
		{
			std::vector<std::uint32_t> Indices;
			for (int i = 0; i < 4; i++)
			{
				std::uint32_t Offset = i == 0 ? 0 : (4 * i);
				{
					Indices.push_back(3 + Offset);
					Indices.push_back(1 + Offset);
					Indices.push_back(0 + Offset);

					Indices.push_back(3 + Offset);
					Indices.push_back(2 + Offset);
					Indices.push_back(1 + Offset);
				}
			}
			std::array<std::uint32_t, 3> Offsets = { Indices[Indices.size() - 3], Indices[Indices.size() - 2], Indices[Indices.size() - 1] };
			for (int i = 0; i < 8; i++)
			{
				std::uint32_t Offset = i == 0 ? 0 : (3 * i);
				{
					Indices.push_back((2 + Offset) + 1 + Offsets[0]);
					Indices.push_back((1 + Offset) + 2 + Offsets[1]);
					Indices.push_back((0 + Offset) + 3 + Offsets[2]);
				}
			}
			return Indices;
		}

		std::vector<cbGeometryVertexData> GetAlignedVertexData(const std::vector<cbVector4>& Vertices, const std::vector<cbVector>& TextureCoordinates,
			const cbColor& Color, const cbVector& Location, const float& Rotation, const cbVector& Origin)
		{
			std::vector<cbGeometryVertexData> Container;

			if (Vertices.size() == 0)
				return Container;

			std::size_t VerticesSize = Vertices.size();

			for (std::size_t i = 0; i < VerticesSize; i++)
			{
				cbGeometryVertexData GeometryData;
				GeometryData.position = Vertices[i] + Location;
				GeometryData.texCoord = TextureCoordinates[i];
				GeometryData.Color = Color;

				if (Rotation != 0.0f)
					GeometryData.position = cbgui::RotateVectorAroundPoint(GeometryData.position, Origin, Rotation);

				Container.push_back(GeometryData);
			}

			return Container;
		}
	}
}
