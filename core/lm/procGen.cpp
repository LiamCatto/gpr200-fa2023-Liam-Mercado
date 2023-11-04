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

		ew::Vec3 normal = ew::Vec3(0.0f, 0.0f, 1.0f);
		ew::Vec3 u = ew::Vec3(normal.z, normal.x, normal.y);
		ew::Vec3 v = ew::Cross(normal, u);

		// Vertices
		for (int row = 0; row <= subdivisions; row++)
		{
			for (int col = 0; col <= subdivisions; col++)
			{
				ew::Vertex vertex;
				vertex.pos -= (u + v) * 0.5f;
				vertex.pos += (u * col + v * row);
				vertex.pos.x *= width;
				vertex.pos.y *= height;
				vertex.normal = normal;
				vertex.uv = ew::Vec2(width * (col / subdivisions), -height * (row / subdivisions));
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
}