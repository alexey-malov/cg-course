
#include <glm/gtx/string_cast.hpp>
#include <glm/vec4.hpp>
#include <iostream>

int main()
{
	glm::vec4 a{ 1, 2, 3, 0 };
	glm::vec4 b{ 2, 3, 4, 0 };
	auto c = a + b;

	std::cout << "c=" << glm::to_string(c) << std::endl;
}