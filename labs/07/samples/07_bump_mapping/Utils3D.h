#pragma once

class CUtils3D
{
public:
	static glm::vec3 CalculateTangent(
		glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3,
		glm::vec2 const& t1, glm::vec2 const& t2, glm::vec2 const& t3
		);
};
