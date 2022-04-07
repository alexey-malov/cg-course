#pragma once

enum class CubeSide
{
	NEGATIVE_X,
	POSITIVE_X,
	NEGATIVE_Y,
	POSITIVE_Y,
	NEGATIVE_Z,
	POSITIVE_Z,
};

class Cube
{
public:
	explicit Cube(float size = 1);
	void Draw() const;

	void SetSideColor(CubeSide side, const glm::vec4& color);
	void SetSpecularColor(glm::vec4 color);
	void SetShininess(float shininess);

private:
	float m_size;
	glm::vec4 m_sideColors[6];
	glm::vec4 m_specularColor = { 0, 0, 0, 1 };
	float m_shininess = 1;
};
