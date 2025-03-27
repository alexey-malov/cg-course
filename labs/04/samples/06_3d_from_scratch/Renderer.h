#pragma once

struct Vertex
{
	glm::vec4 position;
	glm::vec4 color;
};

enum class PrimitiveType
{
	Points,
	Lines,
	Triangles,
	Quads,
};

// Координаты и размеры видового порта относительно верхнего левого угла
struct ViewPort
{
	int left = 0;
	int top = 0;
	int width = 0;
	int height = 0;
	int GetBottom() const noexcept { return top + height; }
};

class Renderer
{
public:
	void SetViewPort(ViewPort viewPort) noexcept;

	void SetModelViewMatrix(const glm::mat4& mat) noexcept;

	void SetProjectionMatrix(const glm::mat4& mat) noexcept;

	template <std::ranges::random_access_range Vertices, std::ranges::input_range Indices>
		requires(std::is_same_v<std::ranges::range_value_t<Vertices>, Vertex>
			&& std::is_integral_v<std::ranges::range_value_t<Indices>>)
	void DrawElements(WTL::CDCHandle dc, PrimitiveType type, const Vertices& vertices, const Indices& indices) const
	{
		DrawPrimitives(dc, type, indices | std::views::transform([&vertices](auto index) -> const Vertex& {
			return vertices[index];
		}));
	}

	template <std::ranges::input_range Vertices>
		requires std::is_same_v<std::ranges::range_value_t<Vertices>, Vertex>
	void DrawPrimitives(WTL::CDCHandle dc, PrimitiveType type, const Vertices& vertices) const
	{
		switch (type)
		{
		case PrimitiveType::Points: {
			PointRenderer renderer(dc, *this);
			DrawPrimitivesImpl(renderer, vertices);
		}
		break;
		case PrimitiveType::Lines: {
			LinesRenderer renderer(dc, *this);
			DrawPrimitivesImpl(renderer, vertices);
		}
		break;
		case PrimitiveType::Triangles: {
			TrianglesRenderer renderer(dc, *this);
			DrawPrimitivesImpl(renderer, vertices);
		}
		break;
		case PrimitiveType::Quads: {
			QuadsRenderer renderer(dc, *this);
			DrawPrimitivesImpl(renderer, vertices);
		}
		break;
		}
	}

private:
	struct TransformedVertex
	{
		glm::vec2 screenPos;
		glm::vec4 color;
	};

	class BaseRenderer
	{
	public:
		BaseRenderer(WTL::CDCHandle dc, const Renderer& renderer)
			: m_dc(dc)
			, m_renderer(renderer)
		{
		}

	protected:
		WTL::CDCHandle m_dc;
		const Renderer& m_renderer;
	};

	class PointRenderer : private BaseRenderer
	{
	public:
		using BaseRenderer::BaseRenderer;

		void AddVertex(TransformedVertex vertex);
		void EndDraw()
		{
		}
	};

	class BaseLineRenderer : public BaseRenderer
	{
	public:
		void EndDraw();

	protected:
		using BaseRenderer::BaseRenderer;

		void SetColor(glm::vec4 color);

		WTL::CPen m_pen;
		WTL::CPenHandle m_oldPen;
		std::optional<COLORREF> m_color;
	};

	class LinesRenderer : public BaseLineRenderer
	{
	public:
		using BaseLineRenderer::BaseLineRenderer;

		void AddVertex(TransformedVertex vertex);

	private:
		TransformedVertex m_prevVertex;
		bool m_isFirstVertex = true;
	};

	class TrianglesRenderer : public BaseLineRenderer
	{
	public:
		using BaseLineRenderer::BaseLineRenderer;

		void AddVertex(TransformedVertex vertex);

	private:
		std::array<TransformedVertex, 2> m_prevVertices;
		int m_vertexIndex = 0;
	};

	class QuadsRenderer : public BaseLineRenderer
	{
	public:
		using BaseLineRenderer::BaseLineRenderer;

		void AddVertex(TransformedVertex vertex);

	private:
		std::array<TransformedVertex, 3> m_prevVertices;
		int m_vertexIndex = 0;
	};

	template <std::ranges::input_range Vertices, typename PrimitiveRenderer>
		requires std::is_same_v<std::ranges::range_value_t<Vertices>, Vertex>
	void DrawPrimitivesImpl(PrimitiveRenderer& primitiveRenderer, const Vertices& vertices) const
	{
		for (const Vertex& vertex : vertices)
		{
			primitiveRenderer.AddVertex(TransformVertex(vertex));
		}
		primitiveRenderer.EndDraw();
	}

	TransformedVertex TransformVertex(const Vertex& vertex) const noexcept;

	ViewPort m_viewPort;
	glm::mat4 m_projMatrix = glm::mat4(1.0f);
	glm::mat4 m_modelViewMatrix = glm::mat4(1.0f);
	glm::mat4 m_modelViewProjMatrix = m_projMatrix * m_modelViewMatrix;
};

class RendererContext
{
public:
	RendererContext(WTL::CDCHandle dc, Renderer& renderer)
		: m_dc(dc)
		, m_renderer(renderer)
	{
	}

	template <std::ranges::input_range Vertices>
		requires std::is_same_v<std::ranges::range_value_t<Vertices>, Vertex>
	void DrawPrimitives(PrimitiveType type, const Vertices& vertices) const
	{
		m_renderer.DrawPrimitives(m_dc, type, vertices);
	}

	template <std::ranges::random_access_range Vertices, std::ranges::input_range Indices>
		requires(std::is_same_v<std::ranges::range_value_t<Vertices>, Vertex>
			&& std::is_integral_v<std::ranges::range_value_t<Indices>>)
	void DrawElements(PrimitiveType type, const Vertices& vertices, const Indices& indices) const
	{
		m_renderer.DrawElements(m_dc, type, vertices, indices);
	}

private:
	WTL::CDCHandle m_dc;
	Renderer& m_renderer;
};