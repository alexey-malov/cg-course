#include "pch.h"
#include "Mesh.h"

GLenum GetGLPrimitiveType(Mesh::PrimitiveType type)
{
	switch (type)
	{
	case Mesh::PrimitiveType::Triangles:
		return GL_TRIANGLES;
	case Mesh::PrimitiveType::Quads:
		return GL_QUADS;
	}
	return GL_POINTS;
}

void Mesh::Draw() const
{
	glBegin(GetGLPrimitiveType(m_primitiveType));
	for (auto index : m_indices)
	{
		glColor4ubv(glm::value_ptr(m_vertices[index].color));
		glVertex3fv(glm::value_ptr(m_vertices[index].position));
	}
	glEnd();
}

void Mesh::DrawWithArrays() const
{
	if (m_vertices.empty() || m_indices.empty())
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex),
		glm::value_ptr(m_vertices.data()->position));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex),
		glm::value_ptr(m_vertices.data()->color));

	glDrawElements(GetGLPrimitiveType(m_primitiveType),
		static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, m_indices.data());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
