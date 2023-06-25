#pragma once

#include "vector3.hpp"
#include <ostream>

namespace core
{

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Vector3<T>& v)
{
	return out << '{' << v.X() << ", " << v.Y() << ", " << v.Z() << '}';
}

} // namespace core
