#pragma once
#include "point3.hpp"
#include "vector3.hpp"
#include <optional>

namespace core
{

struct Ray
{
	Ray() = default;

	Ray(Point3f origin, Vector3f direction) noexcept
		: origin(origin)
		, direction(direction)
	{
		assert(!HasNaNs());
	}

	constexpr bool HasNaNs() const noexcept
	{
		return origin.HasNaNs() || direction.HasNaNs() || IsNaN(time);
	}

	Point3f origin;
	Vector3f direction;
	Float time{};
};

struct RayDifferential : Ray
{
	struct OffsetRays
	{
		Point3f rxOrigin;
		Vector3f rxDirection;

		Point3f ryOrigin;
		Vector3f ryDirection;
	};

	using Ray::Ray;

	constexpr void ScaleDifferentials(Float s) noexcept
	{
		if (offsetRays.has_value())
		{
			offsetRays->rxOrigin = origin + (offsetRays->rxOrigin - origin) * s;
		}
	}

	std::optional<OffsetRays> offsetRays;
};

} // namespace core
