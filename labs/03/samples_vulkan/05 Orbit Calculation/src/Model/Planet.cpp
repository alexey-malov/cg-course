#include "Planet.h"

#include <cmath>
#include <utility>

Planet::Planet(std::string name, Orbit orbit, glm::vec3 color, float size)
	: m_name(std::move(name))
	, m_orbit(orbit)
	, m_color(color)
	, m_size(size)
{
}

PointVertex Planet::PointAt(double time, float zoom) const
{
	const glm::dvec2 position = m_orbit.PositionAt(time);

	return PointVertex{
		.pos = { static_cast<float>(position.x), static_cast<float>(position.y) },
		.color = m_color,
		.size = m_size * std::sqrt(zoom) / 4.0f,
	};
}
