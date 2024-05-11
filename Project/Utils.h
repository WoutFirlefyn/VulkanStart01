#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Vertex.h"
#include <tiny_obj_loader.h>
#include <unordered_map>

#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
static void ParseOBJ(const std::string& filename, std::vector<Vertex3D>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, &err, filename.c_str()))
		throw std::runtime_error(err);

	std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) 
	{
		for (const auto& index : shape.mesh.indices) 
		{
			Vertex3D vertex{};
			
			vertex.pos = 
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = 
			{
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			vertex.normal =
			{
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			if (uniqueVertices.count(vertex) == 0) 
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}
#pragma warning(pop)