// простой фрагментный шейдер, рассчитывающий цвет фрагмента путем выборки цвета из lookup-таблицы содержащей спектр
// (роль таблицы играет одномерная текстура)
// картинка анимируется путем модификации uniform-переменной phase из кода основной программы


uniform sampler1D spectrum;
uniform float phase;

float PI = 3.14159265;

void main()
{
	float distance = 0.5 * sin( (length(gl_TexCoord[0].xy) + phase) * PI / 0.5) + 0.5;
	
	gl_FragColor = texture1D(spectrum, distance);//vec4(distance);
}
