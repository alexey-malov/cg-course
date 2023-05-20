#pragma once

#include "../libgl/Texture.h"

class CTexturedCube
{
public:
	void Draw(GLfloat size = 1) const;
	void SetTextures(
		GLuint leftTexture, GLuint rightTexture,
		GLuint bottomTexture, GLuint topTexture,
		GLuint backTexture, GLuint frontTexture);

private:
	CTexture2D m_backTexture;
	CTexture2D m_frontTexture;
	CTexture2D m_leftTexture;
	CTexture2D m_rightTexture;
	CTexture2D m_bottomTexture;
	CTexture2D m_topTexture;
};
