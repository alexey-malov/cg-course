#include "pch.h"
#include "DiffuseAndSpecular.h"

 DiffuseAndSpecular::DiffuseAndSpecular()
	: m_program{ R"(
#version 110

varying vec3 Normal;
varying vec3 LightDir;
varying vec4 DiffuseColor;
varying vec3 EyeDir;

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
	EyeDir = -vertexPos;
}
)",
		R"(
#version 110

varying vec3 Normal;
varying vec3 LightDir;
varying vec4 DiffuseColor;
varying vec3 EyeDir;

void main()
{
	vec3 n = normalize(Normal);
	vec3 l = normalize(LightDir);

	float diffFactor = max(dot(n, l), 0.0);
	vec4 lightDiffuse = gl_LightSource[0].diffuse;
	vec4 diffuseColor = vec4(diffFactor * lightDiffuse.rgb * DiffuseColor.rgb, lightDiffuse.a * DiffuseColor.a);

	vec3 eye = normalize(EyeDir);
	vec3 r = reflect(-l, n);
	float specularFactor = max(dot(r, eye), 0.0);
	float specularIntensity = pow(specularFactor, gl_FrontMaterial.shininess);
	vec4 specularColor = vec4(specularIntensity * gl_LightSource[0].specular.rgb * gl_FrontMaterial.specular.rgb, 0.0);

	gl_FragColor = diffuseColor + specularColor;
}
)" }
{
}
