#pragma once

#include <VulkanCore/Vertex.h>

#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <numbers>

class RotatingTriangle
{
public:
	static constexpr std::array<vkc::ColoredVertex2D, 3> Vertices()
	{
		return { {
			{ { -0.8f, -0.8f }, { 1.0f, 0.0f, 0.0f } },
			{ { 0.8f, -0.8f }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, 0.8f }, { 0.0f, 0.0f, 1.0f } },
		} };
	}

	void Advance(float deltaTime)
	{
		m_phase += deltaTime / m_period;
		m_phase -= std::floor(m_phase); // держим в пределах одного оборота
	}

	void SetAspect(float aspect) { m_aspect = aspect > 0.0f ? aspect : 1.0f; }

	float Period() const noexcept { return m_period; }
	float Phase() const noexcept { return m_phase; }

	/// Поворот вместе с проекцией, которая не даёт треугольнику
	/// растягиваться при изменении формы окна.
	glm::mat4 Transform() const
	{
		const float angle = m_phase * 2.0f * std::numbers::pi_v<float>;

		float halfWidth = 1.0f;
		float halfHeight = 1.0f;
		if (m_aspect > 1.0f)
			halfWidth = m_aspect;
		else
			halfHeight = 1.0f / m_aspect;

		const glm::mat4 projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

		return projection * glm::rotate(glm::mat4{ 1.0f }, angle, glm::vec3{ 0.0f, 0.0f, 1.0f });
	}

private:
	float m_period = 8.0f; // секунд на полный оборот
	float m_phase = 0.0f; // доля оборота, от 0 до 1
	float m_aspect = 1.0f;
};
