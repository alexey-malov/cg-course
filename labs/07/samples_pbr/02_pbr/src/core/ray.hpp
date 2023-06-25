#pragma once
#include "point3.hpp"
#include "vector3.hpp"

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

    bool HasNaNs() const noexcept
    {
        return origin.HasNaNs() || direction.HasNaNs() || IsNaN(time);
    }

	Point3f origin;
	Vector3f direction;
	Float time{};
};

} // namespace core
