uniform float TWIST;

void main()
{

	// Calculate rotation angle
	float angle = gl_Vertex.y * TWIST;

	// calculate sin(angle) and cos(angle)
	float sa = sin(angle);
	float ca = cos(angle);

	/*
		Rotate vertex around Y axis:
		x' = x * cos(angle) - z * sin(angle)
		y' = y;
		z' = x * sin(angle) + z * cos(angle);
		w' = w;
	*/
	vec4 twistedCoord = vec4(
		gl_Vertex.x * ca - gl_Vertex.z * sa,
		gl_Vertex.y,
		gl_Vertex.x * sa + gl_Vertex.z * ca,
		gl_Vertex.w
	);

	gl_FrontColor = gl_Color;

	// Transform twisted coordinate
	gl_Position = gl_ModelViewProjectionMatrix * twistedCoord;
}
