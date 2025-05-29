#pragma once
#include "Program.h"

class DrawShadowMapAsGrayscale
{
public:
	DrawShadowMapAsGrayscale();


	void SetDepthTexture(GLuint textureId)
	{
		m_depthTextureId = textureId;
	}

	void Activate() const;

private:
	Program m_program;
	int m_depthTextureLocation = -1;
	GLuint m_depthTextureId = 0;
};

