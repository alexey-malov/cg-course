#pragma once

#include <glm/glm.hpp>

#include <vector>

/// Эллиптическая орбита в плоскости.
class Orbit
{
public:
	Orbit(double semiMajorAxis, double eccentricity, double revolutionsPerYear);

	double SemiMajorAxis() const noexcept { return m_semiMajorAxis; }
	double Eccentricity() const noexcept { return m_eccentricity; }

	glm::dvec2 PositionAt(double time) const;
	std::vector<glm::vec2> Outline(int segments) const;

private:
	double MeanAnomaly(double time) const;
	double EccentricAnomaly(double time) const;

	double m_semiMajorAxis;
	double m_eccentricity;
	double m_revolutionsPerYear;
};
