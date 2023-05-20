#include "MyApplication.h"

// Расстояние до ближней плоскости отсечения отображаемого объема
const double CMyApplication::ZNEAR = 0.5;
// Расстояние до дальей плоскости отсечения отображаемого объема
const double CMyApplication::ZFAR = 10;
// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

CMyApplication::CMyApplication(const char* title, int width, int height)
	: CGLApplication(title, width, height)
	, m_carTexture(0)
{
}

CMyApplication::~CMyApplication()
{
	// Удаляем текстурный объект
	if (m_carTexture)
	{
		glDeleteTextures(1, &m_carTexture);
	}
}

void CMyApplication::OnInit()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 1);

	// Загружаем текстуру из файла
	m_carTexture = LoadTexture(L"car.jpg");
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0, -1, 2, 0, 0, 0, 0, 1, 0);

	// Включаем режим наложения текстуры
	glEnable(GL_TEXTURE_2D);
	// И делаем активным текстурный объект
	glBindTexture(GL_TEXTURE_2D, m_carTexture);
	/*
	Рисуем прямоугольник, указывая для каждой из его вершин текстурные координаты
		 Y
		 |
	 0---|---3
	 |   |   |
	 |   |   |
	-----+------X
	 |   |   |
	 |   |   |
	 1---|---2
		 |
	*/
	glBegin(GL_QUADS);
	{
		// верхний левый угол
		glTexCoord2f(0, 0);
		glVertex3f(-1, 1, 0);

		// нижний левый угол
		glTexCoord2f(0, 1);
		glVertex3f(-1, -1, 0);

		// нижний правый угол
		glTexCoord2f(1, 1);
		glVertex3f(1, -1, 0);

		// верхний правый угол
		glTexCoord2f(1, 0);
		glVertex3f(1, 1, 0);
	}
	glEnd();
}

GLuint CMyApplication::LoadTexture(std::wstring const& fileName)
{
	// Загружаем изображение при помощи GDI+
	Gdiplus::Bitmap bmp(fileName.c_str());

	if (bmp.GetLastStatus() != Gdiplus::Ok)
	{
		throw std::runtime_error("Error loading texture file");
	}

	// Определяем формат пикселей для использования
	// с методом Bitmap::Lock и функцией gluBuild2DMipmaps
	Gdiplus::PixelFormat pixelFormat = 0;
	GLint textureFormat = 0;
	GLint colorComponents = 0;
	GLint internalFormat = 0;

	// Есть ли в изображении альфа-канал?
	if ((bmp.GetPixelFormat() & PixelFormatAlpha) != 0)
	{
		colorComponents = 4;
		pixelFormat = PixelFormat32bppARGB;
		textureFormat = GL_BGRA_EXT;
		internalFormat = GL_RGBA;
	}
	else
	{
		colorComponents = 3;
		pixelFormat = PixelFormat24bppRGB;
		textureFormat = GL_BGR_EXT;
		internalFormat = GL_RGB;
	}

	// Размеры и положение прямоугольной области изображения,
	// доступ к данным которой мы хотим получить
	Gdiplus::Rect lockRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	// Получаем прямой доступ для чтения к данным растрового изображения
	Gdiplus::BitmapData bitmapData;
	bmp.LockBits(&lockRect, Gdiplus::ImageLockModeRead,
		pixelFormat, &bitmapData);

	GLuint textureName = 0;
	// генерируем один уникальный идентификатор текстурного объекта
	glGenTextures(1, &textureName);

	// делаем активным текстурный объект с данным идентификатором
	// (с ним еще пока не связано никакое изображение)
	glBindTexture(GL_TEXTURE_2D, textureName);

	// Строим семейство мип-уровней для загруженного изображения
	// и присоединям их к выбранному текстурному объекту
	// Задаем текстурное изображение для 0 уровня детализации
	glTexImage2D(
		GL_TEXTURE_2D,
		0, // уровень детализации
		internalFormat,
		bitmapData.Width,
		bitmapData.Height,
		0, // ширина рамки текстуры (0 - нет рамки, 1 - рамка в 1 пиксель)
		textureFormat,
		GL_UNSIGNED_BYTE,
		bitmapData.Scan0);
	// Задаем параметры фильтрации текстуры при уменьшении и при увеличении
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Завершаем работу с данными растра
	bmp.UnlockBits(&bitmapData);

	// Возвращаем идентификатор созданного изображения
	return textureName;
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, ZNEAR, ZFAR);
	glMatrixMode(GL_MODELVIEW);
}
