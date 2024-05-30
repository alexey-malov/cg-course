// #version 120
// #extension GL_EXT_geometry_shader4 : enable

// Rectangle size
uniform vec2 Size;

void main()
{
	// Half size of the rectangle
	vec2 halfSize = Size * 0.5;

	// Rectangle center is a position of the original primitive
	vec4 center = gl_PositionIn[0];

	/*
	 *  Emit four vertices of the output triangle strip to form a quad
	 *     of size w*h
	 *
	 *  2----w----3
	 *  |         |
	 *  |    x    h
	 *  |         |
	 *  0---------1
	 */

	// Bottom left
	gl_Position = center + vec4(-halfSize.x, -halfSize.y, 0.0, 0.0);
	gl_TexCoord[0].xy = vec2(0.0, 1.0);
	EmitVertex();

	// Bottom right
	gl_Position = center + vec4(halfSize.x, -halfSize.y, 0.0, 0.0);
	gl_TexCoord[0].xy = vec2(1.0, 1.0);
	EmitVertex();

	// Top left
	gl_Position = center + vec4(-halfSize.x, +halfSize.x, 0.0, 0.0);
	gl_TexCoord[0].xy = vec2(0.0, 0.0);
	EmitVertex();

	// Top right
	gl_Position = center + vec4(+halfSize.x, +halfSize.y, 0.0, 0.0);
	gl_TexCoord[0].xy = vec2(1.0, 0.0);
	EmitVertex();

	// Emit a triangle strip
	EndPrimitive();
}
