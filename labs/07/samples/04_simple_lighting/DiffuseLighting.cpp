#include "pch.h"
#include "DiffuseLighting.h"

DiffuseLighting::DiffuseLighting()
	: m_program{ R"(
#version 110

varying vec3 Normal;
varying vec3 LightDir;
varying vec4 DiffuseColor;

void main()
{
	gl_Position = ftransform();

	vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
	if (gl_LightSource[0].position.w == 0.0)
	{
		LightDir = gl_LightSource[0].position.xyz;
	}
	else
	{
		LightDir = gl_LightSource[0].position.xyz - vertexPos.xyz;
	}
	
	DiffuseColor = gl_Color;
	Normal = normalize(gl_NormalMatrix * gl_Normal);
}
)",
		R"(
#version 110

varying vec3 Normal;
varying vec3 LightDir;
varying vec4 DiffuseColor;

void main()
{
	vec3 n = normalize(Normal);
	vec3 l = normalize(LightDir);

	float diffFactor = max(dot(n, l), 0.0);
	vec4 lightDiffuse = gl_LightSource[0].diffuse;
	gl_FragColor = vec4(diffFactor * lightDiffuse.rgb * DiffuseColor.rgb, lightDiffuse.a * DiffuseColor.a);
}
)" }
{
}
