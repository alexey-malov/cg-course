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
class Point3;

template <typename T>
class Vector3
{
	using Impl = detail::Vec3Impl<T>;

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

	constexpr Vector3& operator*=(T s) noexcept
	{
		m_impl *= s;
		return *this;
	}

	constexpr Vector3& operator-=(const Vector3& rhs)
	{
		m_impl -= rhs.m_impl;
		return *this;
	}

private:
	constexpr Vector3(const Impl& impl)
		: m_impl{ impl }
	{
	}

	Impl m_impl{};
};

template <typename T>
Vector3<T> operator*(T s, Vector3<T> vec) noexcept
{
	return vec *= s;
}

template <typename T>
Vector3<T> operator*(Vector3<T> vec, T scale) noexcept
{
	return vec *= scale;
}
using Vector3f = Vector3<Float>;
using Vector3i = Vector3<int>;

} // namespace core