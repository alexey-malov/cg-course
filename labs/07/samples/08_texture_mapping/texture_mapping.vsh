
void main()
{
	// Calculate vertex position
	gl_Position = ftransform();

	// copy multi texture coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
