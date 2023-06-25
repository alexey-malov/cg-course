#pragma once

#include "config.hpp"
#include <glm/vec2.hpp>

namespace core
{

namespace detail
{
template <typename T>
using Vec2Impl = glm::vec<2, T>;
}

template <typename T>
class Vector2
{
	using Impl = detail::Vec2Impl<T>;

public:
	using ValueType = T;

	constexpr Vector2() = default;

	constexpr Vector2(T x, T y) noexcept
		: m_impl(x, y)
	{
	}

private:
	Impl m_impl{};
};

using Vector2f = Vector2<Float>;
using Vector2i = Vector2<int>;

} // namespace core