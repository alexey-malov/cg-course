#include "SolarSystem.h"

#include <algorithm>
#include <cmath>

namespace
{

constexpr glm::vec3 OrbitColor{ 0.35f, 0.35f, 0.40f };
constexpr float OrbitPointSize = 1.5f;

} // namespace

SolarSystem::SolarSystem()
{
	m_planets.reserve(10);
	m_planets.emplace_back("Меркурий", Orbit{ 0.387, 0.206, 1 / 0.241 },
		glm::vec3{ 0.357f, 0.443f, 1.0f }, 1.0f);
	m_planets.emplace_back("Венера", Orbit{ 0.723, 0.007, 1 / 0.635 },
		glm::vec3{ 1.0f, 0.859f, 0.349f }, 3.0f);
	m_planets.emplace_back("Земля", Orbit{ 1.000, 0.017, 1 / 1.000 },
		glm::vec3{ 0.086f, 0.557f, 1.0f }, 3.0f);
	m_planets.emplace_back("Марс", Orbit{ 1.524, 0.093, 1 / 1.881 },
		glm::vec3{ 1.0f, 0.478f, 0.408f }, 2.0f);
	m_planets.emplace_back("Юпитер", Orbit{ 5.203, 0.048, 1 / 11.862 },
		glm::vec3{ 0.537f, 0.478f, 1.0f }, 6.0f);
	m_planets.emplace_back("Сатурн", Orbit{ 6.539, 0.056, 1 / 20.658 },
		glm::vec3{ 0.278f, 1.0f, 0.506f }, 5.0f);
	m_planets.emplace_back("Уран", Orbit{ 19.190, 0.048, 1 / 84.800 },
		glm::vec3{ 1.0f, 0.957f, 0.388f }, 5.0f);
	m_planets.emplace_back("Нептун", Orbit{ 30.081, 0.009, 1 / 154.232 },
		glm::vec3{ 0.439f, 1.0f, 1.0f }, 5.0f);
	m_planets.emplace_back("Плутон", Orbit{ 38.525, 0.249, 1 / 247.305 },
		glm::vec3{ 0.612f, 0.388f, 1.0f }, 1.0f);
	m_planets.emplace_back("Комета Галлея", Orbit{ 17.800, 0.967, 1 / 75.300 },
		glm::vec3{ 1.0f, 1.0f, 1.0f }, 0.5f);
}

void SolarSystem::Advance(float deltaTime)
{
	m_time += static_cast<double>(deltaTime) * static_cast<double>(m_timeSpeed);
}

int SolarSystem::Year() const
{
	return EpochYear + static_cast<int>(std::floor(m_time));
}

void SolarSystem::ZoomBy(float factor)
{
	m_zoom = std::clamp(m_zoom * factor, 5.0f, 500.0f);
}

std::vector<PointVertex> SolarSystem::OrbitPoints() const
{
	std::vector<PointVertex> points;
	points.reserve(m_planets.size() * (static_cast<std::size_t>(OrbitSegments) + 1));

	for (const Planet& planet : m_planets)
	{
		for (const glm::vec2& point : planet.GetOrbit().Outline(OrbitSegments))
			points.push_back(
				PointVertex{ .pos = point, .color = OrbitColor, .size = OrbitPointSize });
	}

	return points;
}

std::vector<PointVertex> SolarSystem::BodyPoints() const
{
	std::vector<PointVertex> points;
	points.reserve(m_planets.size() + 1);

	points.push_back(PointVertex{
		.pos = { 0.0f, 0.0f }, .color = { 1.0f, 1.0f, 0.0f }, .size = 2.0f * std::sqrt(m_zoom) });

	for (const Planet& planet : m_planets)
		points.push_back(planet.PointAt(m_time, m_zoom));

	return points;
}
