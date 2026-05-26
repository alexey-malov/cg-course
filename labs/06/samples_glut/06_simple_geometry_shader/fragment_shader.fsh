uniform sampler2D TextureMap;

void main()
{
	gl_FragColor = texture2D(TextureMap, gl_TexCoord[0].xy);
}