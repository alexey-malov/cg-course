uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

varying vec3 lightDir;
varying vec3 eyeDir;

vec4 CalculateDiffuseColor(
	vec3 normal,	// must be normalized 
	vec3 light,		// must be normalized
	vec4 diffuseLight,
	vec4 diffuseMaterial,
	float attenuation
	)
{
	float diffuseFactor = max(dot(normal, light), 0.0) * attenuation;
	
	return diffuseMaterial * diffuseFactor * diffuseLight;
}

vec4 CalculateSpecularColor(
	vec3 reflectedLight,
	vec3 eye,
	vec4 specularLight,
	vec4 specularMaterial,
	float shininess,
	float attenuation
	)
{
	float specularFactor = max(dot(reflectedLight, eye), 0.0);	
	float specularIntensity = pow(specularFactor, shininess) * attenuation;
	
	return specularLight * specularIntensity * specularMaterial;
}

void main()
{
	vec2 tex = gl_TexCoord[0].xy;	
	vec3 normal = normalize((texture2D(normalMap, tex).xyz - 0.5));
	vec3 eyeDir = normalize(eyeDir);
	float lightDistance = length(lightDir);
	float attenuation = 1.0 / (0.005 * lightDistance * lightDistance + 1.0);
	vec3 lightDir = normalize(lightDir);
	
	vec4 diffuseMaterial = texture2D(diffuseMap, tex) * gl_FrontMaterial.diffuse;
	
	vec4 diffuseColor = CalculateDiffuseColor(
		normal, 
		lightDir, 
		gl_LightSource[0].diffuse, 
		diffuseMaterial, attenuation);
	
	vec3 reflectedLight = reflect(-lightDir, normal);
	vec4 specularColor = CalculateSpecularColor(
		reflectedLight, 
		eyeDir, 
		gl_LightSource[0].specular, 
		gl_FrontMaterial.specular, 
		gl_FrontMaterial.shininess, 
		attenuation);
		
	gl_FragColor = diffuseColor + specularColor;
	
}
