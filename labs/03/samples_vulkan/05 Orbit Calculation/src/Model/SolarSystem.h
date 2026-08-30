#pragma once

#include "Planet.h"

#include <vector>

class SolarSystem
{
public:
	static constexpr int OrbitSegments = 256;
	static constexpr int EpochYear = 2012;

	SolarSystem();

	void Advance(float deltaTime);

	double Time() const noexcept { return m_time; }
	int Year() const;

	float TimeSpeed() const noexcept { return m_timeSpeed; }
	void SetTimeSpeed(float speed) noexcept { m_timeSpeed = speed; }

	float Zoom() const noexcept { return m_zoom; }
	void ZoomBy(float factor);

	const std::vector<Planet>& Planets() const noexcept { return m_planets; }

	std::vector<PointVertex> OrbitPoints() const;
	std::vector<PointVertex> BodyPoints() const;

private:
	std::vector<Planet> m_planets;
	double m_time = 0.0; // годы от эпохи
	float m_timeSpeed = 0.1f; // лет за секунду реального времени
	float m_zoom = 100.0f;
};
