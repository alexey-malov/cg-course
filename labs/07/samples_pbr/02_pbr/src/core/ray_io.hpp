#pragma once

#include "point3_io.hpp"
#include "ray.hpp"
#include "vector3_io.hpp"
#include <ostream>

namespace core
{

inline std::ostream& operator<<(std::ostream& out, const Ray& r)
{
	return out << "{origin: " << r.origin << ", direction: " << r.direction << ", time: " << r.time << '}';
}

} // namespace core
