// vertex end phase position
attribute vec3 endPosition;

// current animation phase
uniform float phase;

void main()
{
	// Calculate vertex position by interpolating 
	// startPosition (gl_Vertex) and endPosition
	vec4 vertex = mix(gl_Vertex, vec4(endPosition, 1.0), phase);
	
	// Transform calculated position
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
	
	// Specify front color
	gl_FrontColor = gl_Color;
}
