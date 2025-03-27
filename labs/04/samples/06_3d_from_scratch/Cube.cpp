#include "stdafx.h"
#include "Cube.h"
#include "Renderer.h"

namespace
{

Vertex MakeVertex(glm::vec3 pos, float size, glm::vec4 color)
{
	return Vertex{ .position = glm::vec4{ pos * size, 1.0f }, .color = color };
}

} // namespace

Cube::Cube(float size, glm::vec4 color)
	: m_size(size)
	, m_color(color)
	, m_vertices{
		MakeVertex({-1, -1, -1}, size, color),
		MakeVertex({+1, -1, -1}, size, color),
		MakeVertex({+1, +1, -1}, size, color),
		MakeVertex({-1, +1, -1}, size, color),
		MakeVertex({-1, -1, +1}, size, color),
		MakeVertex({+1, -1, +1}, size, color),
		MakeVertex({+1, +1, +1}, size, color),
		MakeVertex({-1, +1, +1}, size, color),
	}
{
}

void Cube::Draw(RendererContext& renderer)
{
	/*
   Y
   |
   |
   |
   +---X
  /
 /
Z
   3----2
  /    /|
 /    / |
7----6  |
|  0 |  1
|    | /
|    |/
4----5
*/

	constexpr static std::array<uint8_t, 24> indices{
		4, 7, 3, 0, // грань x<0
		5, 1, 2, 6, // грань x>0
		4, 0, 1, 5, // грань y<0
		7, 6, 2, 3, // грань y>0
		0, 3, 2, 1, // грань z<0
		4, 5, 6, 7, // грань z>0
	};

	renderer.DrawElements(PrimitiveType::Quads, m_vertices, indices);
}
