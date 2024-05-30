uniform sampler2D mainTexture;
uniform sampler2D cloudsTexture;

void main()
{
	// Fetch Earth color
	vec4 earthColor = texture2D(mainTexture, gl_TexCoord[0].st);
	
	// Fetch data from clouds texture
	vec4 clouds = texture2D(cloudsTexture, gl_TexCoord[0].st);
	
	// Calculate clouds color
	vec4 cloudsColor = vec4(clouds.r);
	
	// Add Earth and clouds color to determine the resulting color
	gl_FragColor = earthColor + cloudsColor;
}


