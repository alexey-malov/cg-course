#pragma once
#include "Renderer.h"

class Cube
{
public:
	Cube(float size, glm::vec4 color);

	void Draw(RendererContext& renderer);

private:
	float m_size;
	glm::vec4 m_color;
	std::array<Vertex, 8> m_vertices;
};
