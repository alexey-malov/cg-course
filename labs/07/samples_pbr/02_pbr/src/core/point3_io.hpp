#pragma once

#include "point3.hpp"
#include <ostream>

namespace core
{

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Point3<T>& p)
{
	return out << '{' << p.X() << ", " << p.Y() << ", " << p.Z() << '}';
}

} // namespace core
