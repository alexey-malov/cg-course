uniform sampler2D image;

uniform vec2 step;

uniform float coefficients[10];
 
void main()
{
	vec2 pos = gl_TexCoord[0].st;
	
	vec4 color = 
		texture2D(image, pos) * coefficients[0] + 
		(texture2D(image, pos - step) + 
			texture2D(image, pos + step)) * coefficients[1] +
		(texture2D(image, pos - 2.0 * step) + 
			texture2D(image, pos + 2.0 * step)) * coefficients[2] +
		(texture2D(image, pos - 3.0 * step) + 
			texture2D(image, pos + 3.0 * step)) * coefficients[3] +
		(texture2D(image, pos - 4.0 * step) + 
			texture2D(image, pos + 4.0 * step)) * coefficients[4] +
		(texture2D(image, pos - 5.0 * step) + 
			texture2D(image, pos + 5.0 * step)) * coefficients[5] +
		(texture2D(image, pos - 6.0 * step) + 
			texture2D(image, pos + 6.0 * step)) * coefficients[6] +
		(texture2D(image, pos - 7.0 * step) + 
			texture2D(image, pos + 7.0 * step)) * coefficients[7] +
		(texture2D(image, pos - 8.0 * step) + 
			texture2D(image, pos + 8.0 * step)) * coefficients[8] +
		(texture2D(image, pos - 9.0 * step) + 
			texture2D(image, pos + 9.0 * step)) * coefficients[9];
		
	gl_FragColor = color;
}
