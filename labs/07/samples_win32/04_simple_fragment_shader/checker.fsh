// this simple shader calculates fragment color using texture coordinates of the fragment
// the result is checker pattern
void main()
{
	vec2 colorXY = step(vec2(0.5, 0.5), fract(gl_TexCoord[0].xy));

	float color = (colorXY.x != colorXY.y) ? 1.0 : 0.0;
	
	gl_FragColor = vec4(color);
}
