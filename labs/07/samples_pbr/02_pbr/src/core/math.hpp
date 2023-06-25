#pragma once
#include <cmath>

namespace core
{

template <typename T>
inline constexpr bool IsNaN(T x) noexcept
{
	return std::isnan(x);
}

template <>
inline constexpr bool IsNaN(int) noexcept
{
	return false;
}

} // namespace core
