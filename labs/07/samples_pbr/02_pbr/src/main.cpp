#include "core/point3.hpp"
#include "core/point3_io.hpp"
#include "core/ray_io.hpp"
#include "core/vector2.hpp"
#include <iostream>

int main()
{
	[[maybe_unused]] int a = 42;
	core::Vector3f direction;
	core::Point3f start;
	core::Ray r{ start, direction };
	core::RayDifferential rd{start, direction};
	rd.ScaleDifferentials(3.5f);
	std::cout << r << std::endl;
}
