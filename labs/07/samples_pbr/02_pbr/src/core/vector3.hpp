#pragma once

#include "config.hpp"
#include "math.hpp"

#include <cassert>
#include <cmath>
#include <glm/vec3.hpp>

namespace core
{

namespace detail
{
template <typename T>
using Vec3Impl = glm::vec<3, T>;
}

template <typename T>
class Vector3
{
public:
	constexpr Vector3() = default;
	constexpr Vector3(T x, T y, T z) noexcept
		: m_impl(x, y, z)
	{
		assert(!HasNaNs());
	}

	constexpr bool HasNaNs() const noexcept
	{
		return IsNaN(m_impl.x) || IsNaN(m_impl.y) || IsNaN(m_impl.z);
	}

	constexpr T X() const noexcept { return m_impl.x; }
	constexpr T Y() const noexcept { return m_impl.y; }
	constexpr T Z() const noexcept { return m_impl.z; }

private:
	detail::Vec3Impl<T> m_impl{};
};

using Vector3f = Vector3<Float>;
using Vector3i = Vector3<int>;

} // namespace core