#include "Orbit.h"

#include <cmath>
#include <numbers>

namespace
{

double SolveKepler(double meanAnomaly, double eccentricity)
{
	constexpr double tolerance = 1e-12;
	constexpr int maxIterations = 100;

	double angle = meanAnomaly + eccentricity * std::sin(meanAnomaly);

	for (int i = 0; i < maxIterations; ++i)
	{
		const double residual = angle - eccentricity * std::sin(angle) - meanAnomaly;
		const double derivative = 1.0 - eccentricity * std::cos(angle);
		const double step = residual / derivative;

		angle -= step;

		if (std::abs(step) < tolerance)
			break;
	}

	return angle;
}

} // namespace

Orbit::Orbit(double semiMajorAxis, double eccentricity, double revolutionsPerYear)
	: m_semiMajorAxis(semiMajorAxis)
	, m_eccentricity(eccentricity)
	, m_revolutionsPerYear(revolutionsPerYear)
{
}

double Orbit::MeanAnomaly(double time) const
{
	constexpr double fullTurn = 2.0 * std::numbers::pi;
	return std::fmod(fullTurn * m_revolutionsPerYear * time, fullTurn);
}

double Orbit::EccentricAnomaly(double time) const
{
	return SolveKepler(MeanAnomaly(time), m_eccentricity);
}

glm::dvec2 Orbit::PositionAt(double time) const
{
	// В системе координат, связанной с орбитой, эллипс задаётся
	// эксцентрической аномалией напрямую — истинную вычислять не нужно.
	const double angle = EccentricAnomaly(time);
	const double semiMinor = m_semiMajorAxis * std::sqrt(1.0 - m_eccentricity * m_eccentricity);
	const double focusShift = m_semiMajorAxis * m_eccentricity;

	return { m_semiMajorAxis * std::cos(angle) - focusShift, semiMinor * std::sin(angle) };
}

std::vector<glm::vec2> Orbit::Outline(int segments) const
{
	const double semiMinor = m_semiMajorAxis * std::sqrt(1.0 - m_eccentricity * m_eccentricity);
	const double focusShift = m_semiMajorAxis * m_eccentricity;

	std::vector<glm::vec2> points;
	points.reserve(static_cast<std::size_t>(segments) + 1);

	for (int i = 0; i <= segments; ++i)
	{
		const double angle = 2.0 * std::numbers::pi * i / segments;
		points.emplace_back(static_cast<float>(m_semiMajorAxis * std::cos(angle) - focusShift),
			static_cast<float>(semiMinor * std::sin(angle)));
	}

	return points;
}
