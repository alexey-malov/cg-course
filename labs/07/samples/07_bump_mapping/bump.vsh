attribute vec3 tangent;

varying vec3 lightDir;
varying vec3 eyeDir;

void main()
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec3 normal = gl_NormalMatrix * gl_Normal;
	vec3 tangent = gl_NormalMatrix * tangent;
	vec3 binormal = cross(normal, tangent);
	
	vec3 vertex = (gl_ModelViewMatrix * gl_Vertex).xyz;
	
	vec3 l = gl_LightSource[0].position.xyz - vertex;
	lightDir = vec3(
		dot(tangent, l),
		dot(binormal, l),
		dot(normal, l));

	vec3 eye = -vertex;
	eyeDir = vec3(
		dot(tangent, eye),
		dot(binormal, eye),
		dot(normal, eye)
		);
		
}

