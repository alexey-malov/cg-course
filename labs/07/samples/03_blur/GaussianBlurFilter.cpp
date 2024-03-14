#include "pch.h"
#include "GaussianBlurFilter.h"

GLuint CGaussianBlurFilter::Blur(
	GLuint texture,
	GLfloat sigma,
	int width, int height,
	bool generateMipmaps) const
{
	// Выполняем подготовку вспомогательного буфера кадра
	PrepareFrameBuffer(width, height, generateMipmaps);

	assert(m_program);
	assert(m_frameBuffer);

	// Выполняем привязку к вспомогательному буферу кадра
	m_frameBuffer.Bind();

	// Устанавливаем видовой порт, а также матрицы
	// проецирования и моделирования-вида
	SetupViewportAndMatrices();

	// Активируем шейдерную программу
	ActivateProgram(sigma);

	// Выполняем размытие по горизонтали
	BlurHorizontal(texture);
	// Затем выполняем размытие обработанного изображения по вертикали
	BlurVertical();

	// Отключаем использование вспомогательного буфера кадра
	CFrameBuffer::Bind(GL_FRAMEBUFFER_EXT, 0);
	// а также шейдерную программу.
	glUseProgram(0);

	// Восстанавливаем параметры видового порта и матриц трансформации
	RestoreViewportAndMatrices();

	// Генерируем mip-уровни для результирующей текстуры в
	// соответствии с требованиями пользователя пользователя
	if (generateMipmaps)
	{
		m_colorBuffer1.Bind();
		glGenerateMipmapEXT(GL_TEXTURE_2D);
	}
	return m_colorBuffer1;
}

void CGaussianBlurFilter::BlurHorizontal(GLuint texture) const
{
	// Выполняем привязку к переданному текстурному объекту
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Задаем шаг выборки значений из текстуры
	glUniform2f(m_stepLocation, 1.0f / m_width, 0);

	// Присоединяем 0 уровень деталей текстуры m_colorBuffer0
	// в качестве буфера цвета во вспомогательном буфере кадра
	CFrameBuffer::SetTexture2D(
		GL_FRAMEBUFFER_EXT,
		GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D,
		m_colorBuffer0,
		0);

	assert(
		CFrameBuffer::CheckStatus(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);

	DrawRectangle();
}

void CGaussianBlurFilter::BlurVertical() const
{
	m_colorBuffer0.Bind();
	glUniform2f(m_stepLocation, 0, 1.0f / m_height);

	// Присоединяем 0 уровень деталей текстуры m_colorBuffer1
	// в качестве буфера цвета во вспомогательном буфере кадра
	m_frameBuffer.SetTexture2D(
		GL_FRAMEBUFFER_EXT,
		GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D,
		m_colorBuffer1,
		0);

	assert(
		CFrameBuffer::CheckStatus(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);

	DrawRectangle();
}

void CGaussianBlurFilter::ActivateProgram(float sigma) const
{
	glUseProgram(m_program);
	glUniform1i(m_imageLocation, 0);
	std::vector<float> gauss = CalcGauss(m_filterRadius, sigma);
	glUniform1fv(m_coefficientsLocation, static_cast<GLsizei>(gauss.size()), &gauss[0]);
}

void CGaussianBlurFilter::SetupViewportAndMatrices() const
{
	// Сохраняем параметры видового порта в стеке атрибутов
	glPushAttrib(GL_VIEWPORT_BIT);
	// и задаем видовой порт размером со вспомогательный буфер кадра
	glViewport(0, 0, m_width, m_height);

	// Сохраняем матрицу моделирования-вида в стеке матриц
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	// и загружаем единичную матрицу моделирования вида
	glLoadIdentity();

	// Сохраняем матрицу проецирования в стеке матриц
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	// и задаем новую матрицу проецирования
	glLoadIdentity();
	glOrtho(0, m_width, m_height, 0, -1, 1);
}

void CGaussianBlurFilter::RestoreViewportAndMatrices() const
{
	// Восстанавливаем матрицу проецироваиня
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	// Восстанавливаем матрицу моделирования-вида
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// Восстанавливаем видовой порт
	glPopAttrib();
}

void CGaussianBlurFilter::DrawRectangle() const
{
	glBegin(GL_QUADS);
	{
		glTexCoord2i(0, 1);
		glVertex2i(0, 0);

		glTexCoord2i(0, 0);
		glVertex2i(0, m_height);

		glTexCoord2i(1, 0);
		glVertex2i(m_width, m_height);

		glTexCoord2i(1, 1);
		glVertex2i(m_width, 0);
	}
	glEnd();
}

void CGaussianBlurFilter::PrepareFrameBuffer(int width, int height, bool generateMipmaps) const
{
	// Определяем, какие объекты буфера кадра нуждаются
	// в создании или обновлении
	bool createFrameBuffer = !m_frameBuffer;

	bool createTexture = createFrameBuffer || !m_colorBuffer0 || !m_colorBuffer1;

	bool updateTextureImage = createTexture || (m_width != width) || (m_height != height);

	bool updateMipmaps = (m_generateMipmaps != generateMipmaps) || updateTextureImage || createTexture;

	m_generateMipmaps = generateMipmaps;

	// Prepare frame buffer
	if (createFrameBuffer)
	{
		m_frameBuffer.Create();
	}

	// Инициализируем буферы цвета
	if (createTexture)
	{
		m_colorBuffer0.Create();
		m_colorBuffer1.Create();
	}

	m_width = width;
	m_height = height;

	// Выбираем текстуру 0 буфера цвета
	if (updateTextureImage || updateMipmaps)
	{
		m_colorBuffer0.Bind();
	}

	if (updateTextureImage)
	{
		// Для первого буфера цвета фильтрация будет осуществляться с выбором
		// ближайшего пикселя
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Объявляем 0 уровень детализации текстурного изображения
		m_colorBuffer0.TexImage(
			0, GL_RGBA,
			width, height,
			0, GL_RGBA,
			GL_UNSIGNED_BYTE, NULL);
	}

	// Выбираем текстуру 1 буфера цвета
	if (updateTextureImage || updateMipmaps)
	{
		m_colorBuffer1.Bind();
	}

	if (updateTextureImage)
	{
		// Объявляем 0 уровень детализации текстурного изображения
		m_colorBuffer1.TexImage(
			0, GL_RGBA,
			m_width, m_height,
			0, GL_RGBA,
			GL_UNSIGNED_BYTE, NULL);
	}

	if (updateMipmaps)
	{
		// Для второго буфера буфера цвета фильтрация будет осуществляться
		// согласно параметру generateMipmaps
		if (generateMipmaps)
		{
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		else
		{
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_NEAREST);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MAG_FILTER,
				GL_NEAREST);
		}
	}
}

void CGaussianBlurFilter::SetProgram(
	GLuint program,
	GLint stepLocation,
	GLint imageLocation,
	GLint coefficientsLocation,
	size_t filterRadius)
{
	assert(program);
	assert(stepLocation >= 0);
	assert(imageLocation >= 0);
	assert(coefficientsLocation >= 0);
	assert(filterRadius > 0 && filterRadius < 20);

	m_program = program;
	m_stepLocation = stepLocation;
	m_imageLocation = imageLocation;
	m_coefficientsLocation = coefficientsLocation;
	m_filterRadius = filterRadius;
}

std::vector<float> CGaussianBlurFilter::CalcGauss(size_t size, float sigma)
{
	std::vector<float> gauss(size);

	// Вычисляем коэффициенты гауссова распределения
	const float k1 = 1.0f / sqrtf(2 * static_cast<float>(std::numbers::pi) * sigma * sigma);
	const float k2 = 1.0f / (2 * sigma * sigma);

	float sum = 0;

	for (size_t i = 0; i < size; ++i)
	{
		// Вычисляем плотность вероятности в данной точке
		float k = k1 * expf(-k2 * (i * i));
		gauss[i] = k;

		sum += k;

		// Учитываем симметричность функции распределения
		// относительно оси x=0
		// (сумма расположенных на одинаковом расстоянии коэффициентов
		// фильтра в шейдерной программе умножается на один коэффициент)
		if (i > 0)
		{
			sum += k;
		}
	}

	// Выполняем нормализацию коэффициентов фильтра
	sum = 1.0f / sum;
	for (size_t i = 0; i < size; ++i)
	{
		gauss[i] *= sum;
	}

	return gauss;
}
