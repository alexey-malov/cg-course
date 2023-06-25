#pragma once

#include "config.hpp"
#include "math.hpp"
#include <cassert>
#include <glm/vec3.hpp>

namespace core
{

namespace detail
{
template <typename T>
using Point3Impl = glm::vec<3, T>;
}

template <typename T>
class Point3
{
	using Impl = detail::Point3Impl<T>;

public:
	constexpr Point3() = default;
	constexpr Point3(T x, T y, T z)
		: m_impl(x, y, z)
	{
		assert(!HasNaNs());
	}

	bool HasNaNs() const noexcept
	{
		return IsNaN(m_impl.x) || IsNaN(m_impl.y) || IsNaN(m_impl.z);
	}

	constexpr T X() const noexcept { return m_impl.x; }
	constexpr T Y() const noexcept { return m_impl.y; }
	constexpr T Z() const noexcept { return m_impl.z; }

private:
	Impl m_impl{};
};

using Point3f = Point3<Float>;
using Point3i = Point3<int>;

} // namespace core