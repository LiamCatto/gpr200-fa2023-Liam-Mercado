#include "procGen.h"
#include <stdlib.h>

namespace lm {
	ew::MeshData createPlane(float width, float height, int subdivisions)
	{
		if (subdivisions < 1)
		{
			subdivisions = 1;
		}

		int columns = subdivisions + 1;
		ew::MeshData mesh;
		mesh.vertices.reserve(4);	// 4 x 1
		mesh.indices.reserve(6);	// 6 x 1

		ew::Vec3 normal = ew::Normalize(ew::Vec3(0.0f, 0.0f, 1.0f));
		ew::Vec3 u = ew::Vec3(normal.z, normal.x, normal.y);
		ew::Vec3 v = ew::Cross(normal, u);

		// Vertices
		for (int row = 0; row <= subdivisions; row++)
		{
			for (int col = 0; col <= subdivisions; col++)
			{
				ew::Vertex vertex;
				vertex.pos -= (u + v) * (1 / columns);
				vertex.pos += (u * col + v * row);
				vertex.pos.x *= width;
				vertex.pos.y *= height;
				vertex.normal = normal;
				//vertex.uv = ew::Vec2(col * (width / (subdivisions - width)), row * (height / (subdivisions - height)));
				vertex.uv = ew::Vec2(col * 2 * (width / subdivisions), row * 2 * (height / subdivisions));
				mesh.vertices.push_back(vertex);
			}
		}

		// Indices
		for (int row = 0; row < subdivisions; row++)
		{
			for (int col = 0; col < subdivisions; col++)
			{
				unsigned int startVertex = row * columns + col;

				mesh.indices.push_back(startVertex);
				mesh.indices.push_back(startVertex + 1);
				mesh.indices.push_back(startVertex + columns + 1);
				mesh.indices.push_back(startVertex + columns + 1);
				mesh.indices.push_back(startVertex + columns);
				mesh.indices.push_back(startVertex);
			}
		}
		
		return mesh;
	}

	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		if (numSegments < 3)
		{
			numSegments = 3;
		}

		ew::MeshData mesh;
		mesh.vertices.reserve(4);	// 4 x 1
		mesh.indices.reserve(6);	// 6 x 1

		ew::Vec3 normal = ew::Normalize(ew::Vec3(0.0f, 0.0f, 1.0f));
		ew::Vec3 u = ew::Vec3(normal.z, normal.x, normal.y);
		ew::Vec3 v = ew::Cross(normal, u);

		float thetaStep = 2 * ew::PI / numSegments;
		float bottomY = -height / 2;
		float topY = -bottomY;

		// Vertices

		// Top Center
		ew::Vertex topV;
		topV.pos = ew::Vec3(0, topY, 0);
		topV.normal = normal;
		topV.uv = ew::Vec2(0.5, 0.5);
		mesh.vertices.push_back(topV);

		
		// Top Ring -- Up
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex topRingVertex;
			topRingVertex.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
			topRingVertex.normal = normal;
			topRingVertex.uv = ew::Vec2(cos(theta) / 2 + 0.5, sin(theta) / 2 + 0.5);
			mesh.vertices.push_back(topRingVertex);
		}

		// Top Ring -- Out
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex topRingVertex;
			topRingVertex.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
			topRingVertex.normal = ew::Normalize(topRingVertex.pos);
			mesh.vertices.push_back(topRingVertex);
		}

		// Bottom Ring - Out
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex bottomRingVertex;
			bottomRingVertex.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
			bottomRingVertex.normal = ew::Normalize(bottomRingVertex.pos);
			mesh.vertices.push_back(bottomRingVertex);
		}
		
		
		// Bottom Ring - Down
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex bottomRingVertex;
			bottomRingVertex.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
			bottomRingVertex.normal = -normal;
			bottomRingVertex.uv = ew::Vec2(cos(theta) / 2 + 0.5, sin(theta) / 2 + 0.5);
			mesh.vertices.push_back(bottomRingVertex);
		}

		// Bottom Center
		ew::Vertex bottomV;
		bottomV.pos = ew::Vec3(0, bottomY, 0);
		bottomV.normal = -normal;
		bottomV.uv = ew::Vec2(0.5, 0.5);
		mesh.vertices.push_back(bottomV); 

		// Indices
		int columns = numSegments + 1;

		// Bottom Cap
		int start = 1;
		int center = 0;

		for (int i = 0; i < numSegments * 2; i++)
		{
			mesh.indices.push_back(start + i);
			mesh.indices.push_back(center);
			mesh.indices.push_back(start + i + 1);
		}

		// Top Cap
		start = numSegments * 4 + 6;
		center = numSegments * 4 + 5;

		for (int i = 0; i <= numSegments * 2 + 1; i++)
		{
			mesh.indices.push_back(start - i);
			mesh.indices.push_back(center);
			mesh.indices.push_back(start - i - 1);
		}

		// Side
		int sideStart = numSegments + 1;
		for (int i = 0; i < numSegments * 2; i++)
		{
			start = sideStart + i;

			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns);
			mesh.indices.push_back(start);
		}

		return mesh;
	}

	ew::MeshData createSphere(float radius, int numSegments)
	{
		if (numSegments < 3)
		{
			numSegments = 3;
		}

		ew::MeshData mesh;
		mesh.vertices.reserve(4);	// 4 x 1
		mesh.indices.reserve(6);	// 6 x 1

		ew::Vec3 normal = ew::Vec3(0.0f, 0.0f, 1.0f);
		ew::Vec3 u = ew::Vec3(normal.z, normal.x, normal.y);
		ew::Vec3 v = ew::Cross(normal, u);

		float thetaStep = 2 * ew::PI / numSegments;
		float phiStep = ew::PI / numSegments;
		float phi = 0;
		float theta = 0;

		// Vertices
		for (int row = 0; row <= numSegments; row++)
		{
			//First and last row converge at poles
			phi = row * phiStep;

			//Duplicate column for each row
			for (int col = 0; col <= numSegments; col++)
			{
				theta = col * thetaStep;
				ew::Vertex ringVertex;
				ringVertex.pos = ew::Vec3(radius * cos(theta) * sin(phi), radius * cos(phi), radius * sin(theta) * sin(phi));
				mesh.vertices.push_back(ringVertex);
			}
		}

		// Indices

		// Top Cap
		int poleStart = 0;
		int sideStart = numSegments + 1;

		for (int i = 0; i < numSegments; i++)
		{
			mesh.indices.push_back(sideStart + i);
			mesh.indices.push_back(poleStart + i);	//Pole
			mesh.indices.push_back(sideStart + i + 1);
		}

		// Bottom Cap
		poleStart = numSegments * numSegments + numSegments + numSegments;
		sideStart = numSegments * numSegments + numSegments;

		for (int i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(sideStart - i);
			mesh.indices.push_back(poleStart - i);	//Pole
			mesh.indices.push_back(sideStart - i - 1);
		}

		// Side
		int columns = numSegments + 1;

		for (int row = 1; row < numSegments - 1; row++)
		{
			for (int col = 0; col < numSegments; col++)
			{
				int start = row * columns + col;

				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + columns);
				mesh.indices.push_back(start);
			}
		}

		return mesh;
	}
}