// fragment shader source

uniform sampler2D mainTexture;

void main(void)
{
	gl_FragColor = gl_Color + texture2D(mainTexture, gl_TexCoord[0].st);
}