#pragma once

struct Vertex
{
	glm::vec3 position;
	glm::u8vec4 color;
};

class Mesh
{
public:
	using Vertices = std::vector<Vertex>;
	using Indices = std::vector<unsigned>;

	enum class PrimitiveType
	{
		Triangles,
		Quads,
	};

	Mesh(Vertices vertices, Indices indices, PrimitiveType primitiveType)
		: m_vertices(std::move(vertices))
		, m_indices(std::move(indices))
		, m_primitiveType(primitiveType)
	{
	}

	void Draw() const;

	void DrawWithArrays() const;

private:
	Vertices m_vertices;
	Indices m_indices;
	PrimitiveType m_primitiveType;
};
