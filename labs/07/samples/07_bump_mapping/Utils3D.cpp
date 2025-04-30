#include "StdAfx.h"
#include "Utils3D.h"

glm::vec3 CalculateTangent(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
	glm::vec2 t1, glm::vec2 t2, glm::vec2 t3)
{
	float denominator = (t2.x - t1.x) * (t3.y - t1.y) - (t3.x - t1.x) * (t2.y - t1.y);
	glm::vec3 numerator = (p2 - p1) * (t3.y - t2.y) - (p3 - p1) * (t2.y - t1.y);
	glm::vec3 tangent = numerator * (1.0f / denominator);
	return glm::normalize(tangent);
}
