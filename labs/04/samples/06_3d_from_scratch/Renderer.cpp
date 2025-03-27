#include "stdafx.h"
#include "Renderer.h"

namespace
{

POINT Vec2ToPoint(glm::vec2 screenPos) noexcept
{
	return { static_cast<int>(std::round(screenPos.x)),
		static_cast<int>(std::round(screenPos.y)) };
}

COLORREF ColorToGDIColor(glm::vec4 color) noexcept
{
	glm::u8vec3 u8Color{ glm::round(glm::clamp(color, 0.0f, 1.0f) * 255.0f) };
	return RGB(u8Color.r, u8Color.g, u8Color.b);
}

} // namespace

void Renderer::SetViewPort(ViewPort viewPort) noexcept
{
	m_viewPort = viewPort;
}

void Renderer::SetModelViewMatrix(const glm::mat4& mat) noexcept
{
	m_modelViewMatrix = mat;
	m_modelViewProjMatrix = m_projMatrix * m_modelViewMatrix;
}

void Renderer::SetProjectionMatrix(const glm::mat4& mat) noexcept
{
	m_projMatrix = mat;
	m_modelViewProjMatrix = m_projMatrix * m_modelViewMatrix;
}

Renderer::TransformedVertex Renderer::TransformVertex(const Vertex& vertex) const noexcept
{
	glm::vec4 pos = m_modelViewProjMatrix * vertex.position;

	// Перспективное деление.
	pos *= 1.0f / pos.w;
	// pos - координаты внутри видового объёма находятся в диапазоне от - до 1

	auto pos2D = glm::vec2(pos) * 0.5f + 0.5f;
	pos2D.x = pos2D.x * m_viewPort.width + m_viewPort.left;
	pos2D.y = m_viewPort.GetBottom() - pos2D.y * m_viewPort.height;

	return { .screenPos = pos2D, .color = vertex.color };
}

void Renderer::BaseLineRenderer::EndDraw()
{
	if (m_oldPen)
	{
		m_dc.SelectPen(m_oldPen.Detach());
	}
}

void Renderer::BaseLineRenderer::SetColor(glm::vec4 color)
{
	if (auto newColor = ColorToGDIColor(color);
		m_color != newColor)
	{
		m_color = newColor;
		if (m_pen)
		{
			m_dc.SelectPen(m_oldPen);
			m_pen.DeleteObject();
		}
		m_pen.CreatePen(PS_SOLID, 1, newColor);
		m_oldPen = m_dc.SelectPen(m_pen);
	}
}

void Renderer::TrianglesRenderer::AddVertex(TransformedVertex vertex)
{
	if (m_vertexIndex != 2)
	{
		m_prevVertices[m_vertexIndex++] = vertex;
		return;
	}
	const auto& v0 = m_prevVertices.front();
	const auto& v1 = m_prevVertices.back();
	SetColor(v0.color);

	m_dc.MoveTo(Vec2ToPoint(v0.screenPos));
	m_dc.LineTo(Vec2ToPoint(v1.screenPos));
	m_dc.LineTo(Vec2ToPoint(vertex.screenPos));
	m_dc.LineTo(Vec2ToPoint(v0.screenPos));

	m_vertexIndex = 0;
}

void Renderer::LinesRenderer::AddVertex(TransformedVertex vertex)
{
	if (m_isFirstVertex)
	{
		m_prevVertex = vertex;
		m_isFirstVertex = false;
		return;
	}
	SetColor(m_prevVertex.color);
	m_dc.MoveTo(Vec2ToPoint(m_prevVertex.screenPos));
	m_dc.LineTo(Vec2ToPoint(vertex.screenPos));
	m_isFirstVertex = true;
}

void Renderer::PointRenderer::AddVertex(TransformedVertex vertex)
{
	m_dc.SetPixel(Vec2ToPoint(vertex.screenPos), ColorToGDIColor(vertex.color));
}

void Renderer::QuadsRenderer::AddVertex(TransformedVertex vertex)
{
	if (m_vertexIndex != 3)
	{
		m_prevVertices[m_vertexIndex++] = vertex;
		return;
	}
	SetColor(m_prevVertices[0].color);

	m_dc.MoveTo(Vec2ToPoint(m_prevVertices[0].screenPos));
	m_dc.LineTo(Vec2ToPoint(m_prevVertices[1].screenPos));
	m_dc.LineTo(Vec2ToPoint(m_prevVertices[2].screenPos));
	m_dc.LineTo(Vec2ToPoint(vertex.screenPos));
	m_dc.LineTo(Vec2ToPoint(m_prevVertices[0].screenPos));

	m_vertexIndex = 0;
}
