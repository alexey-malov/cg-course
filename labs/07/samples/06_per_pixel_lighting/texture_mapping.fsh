uniform sampler2D mainTexture;
uniform sampler2D cloudsTexture;

varying vec3 normal;

varying vec3 eyeDir;	// eye direction

varying vec3 lightDir;

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
	// Fetch Earth color
	vec4 earthColor = texture2D(mainTexture, gl_TexCoord[0].st);
	
	// Fetch data from clouds texture
	vec4 clouds = texture2D(cloudsTexture, gl_TexCoord[0].st);
	
	// Calculate clouds color (water has specular color)
	vec4 cloudsColor = vec4(clouds.r);

	// Add Earth and clouds color to determine the resulting color
	vec4 diffuseMaterial = (earthColor + cloudsColor) * gl_FrontMaterial.diffuse;
	
	vec4 specularMaterial = vec4(clouds.g) * gl_FrontMaterial.specular;
	
	
	// calculate diffuse lighting
	vec3 n = normalize(normal);
	
	vec3 l = normalize(lightDir);
	
	vec4 diffuseColor = CalculateDiffuseColor(
		n, 
		l, 
		gl_LightSource[0].diffuse, 
		diffuseMaterial, 1.0);	
	
	// normalized eye direction
	vec3 eye = normalize(eyeDir);
	
	vec3 reflectedLight = reflect(-l, n);
	
	vec4 specularColor = CalculateSpecularColor(
		reflectedLight, 
		eye, 
		gl_LightSource[0].specular, 
		specularMaterial, 
		gl_FrontMaterial.shininess, 
		1.0);
	
	vec4 ambientColor = gl_LightSource[0].ambient * diffuseMaterial;
	
	gl_FragColor = diffuseColor + ambientColor + specularColor;
}
