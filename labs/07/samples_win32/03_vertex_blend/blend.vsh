attribute vec3 vertex2;
uniform float phase;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * mix(gl_Vertex, vec4(vertex2, 1.0), phase);
	gl_FrontColor = gl_Color;
}
