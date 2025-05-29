#include "pch.h"
#include "DrawShadowMapAsGrayscale.h"

DrawShadowMapAsGrayscale::DrawShadowMapAsGrayscale()
	: m_program(
		R"VS(
#version 110 core

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
)VS",
		  R"FS(
#version 110 core

uniform sampler2D uShadowMap;

void main()
{
	float depth = texture2D(uShadowMap, gl_TexCoord[0].st).r;
	gl_FragColor = vec4(depth, depth, depth, 1.0);
}
)FS")
{
	m_depthTextureLocation = m_program.GetProgram().GetUniformLocation("uShadowMap");
}

void DrawShadowMapAsGrayscale::Activate() const
{
	glUseProgram(m_program.GetProgram());
	glUniform1i(m_depthTextureLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureId);
}
