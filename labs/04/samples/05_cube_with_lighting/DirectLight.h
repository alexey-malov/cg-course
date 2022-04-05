#pragma once

class DirectLight
{
public:
	explicit DirectLight(glm::vec3 direction);

	void SetDirection(glm::vec3 direction) { m_direction = { direction, 1.0f }; }
	void SetDiffuseIntensity(glm::vec4 color) { m_diffuseColor = color; }
	void SetAmbientIntensity(glm::vec4 color) { m_ambientColor = color; }
	void SetSpecularIntensity(glm::vec4 color) { m_specularColor = color; }

	void Apply(GLenum light)const;
private:
	glm::vec4 m_direction;
	glm::vec4 m_diffuseColor{ 0.8f, 0.8f, 0.8f, 1.0f };
	glm::vec4 m_ambientColor{ 0.2f, 0.2f, 0.2f, 1.0f };
	glm::vec4 m_specularColor{ 0.5f, 0.5f, 0.5f, 1.0f };
};
