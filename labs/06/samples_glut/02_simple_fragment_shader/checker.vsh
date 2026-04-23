void main()
{
	// Transform the vertex:
	// gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex
	gl_Position = ftransform();

	// Copy texture coordinates from gl_MultiTexCoord0 vertex attribute
	// to gl_TexCoord[0] varying variable
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
