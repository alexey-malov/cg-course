#include "StdAfx.h"
#include "TextureLoader.h"
#include "Texture.h"

CTextureLoader::CTextureLoader(void)
	: m_buildMipmaps(true)
	, m_minFilter(GL_LINEAR_MIPMAP_LINEAR)
	, m_magFilter(GL_LINEAR)
	, m_wrapS(GL_REPEAT)
	, m_wrapT(GL_REPEAT)
{
}

CTextureLoader::~CTextureLoader(void)
{
}

void CTextureLoader::SetMinFilter(GLenum minFilter)
{
	m_minFilter = m_minFilter;
}

void CTextureLoader::SetMagFilter(GLenum magFilter)
{
	m_magFilter = m_magFilter;
}

void CTextureLoader::BuildMipmaps(bool buildMipmaps)
{
	m_buildMipmaps = buildMipmaps;
}

void CTextureLoader::SetWrapMode(GLenum wrapS, GLenum wrapT)
{
	m_wrapS = wrapS;
	m_wrapT = wrapT;
}

GLuint CTextureLoader::LoadTexture2D(std::wstring const& fileName, GLuint textureName, GLint level) const
{
	// Загружаем изображение при помощи GDI+
	Gdiplus::Bitmap bmp(fileName.c_str());

	if (bmp.GetLastStatus() != Gdiplus::Ok)
	{
		std::string name;
		size_t size;
		name.resize(fileName.length());
		wcstombs_s(&size, &name[0], name.size() + 1, fileName.c_str(), fileName.size());
		throw std::runtime_error("Error loading texture file " + name);
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
		textureFormat = GL_BGR_EXT;
		pixelFormat = PixelFormat24bppRGB;
		internalFormat = GL_RGB;
	}

	Gdiplus::Rect lockRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	// Получаем прямой доступ для чтения к данным растрового изображения
	Gdiplus::BitmapData bitmapData{};
	bitmapData.Scan0 = NULL;
	bmp.LockBits(&lockRect, Gdiplus::ImageLockModeRead, pixelFormat, &bitmapData);

	// Создаем текстурный объект (при необходимости)
	CTexture2DHandle texture(textureName);
	if (textureName == 0)
	{
		texture.Create();
	}
	// делаем активным текстурный объект с данным идентификатором
	// (с ним еще пока не связано никакое изображение)
	texture.Bind();

	if (m_buildMipmaps)
	{
		// Строим семейство мип-уровней для загруженного изображения
		// и присоединям их к выбранному текстурному объекту
		gluBuild2DMipmaps(
			GL_TEXTURE_2D,
			colorComponents,
			bitmapData.Width, bitmapData.Height,
			textureFormat, GL_UNSIGNED_BYTE,
			bitmapData.Scan0);
	}
	else
	{
		// Задаем изображение для заданного уровня детализации
		texture.TexImage(
			level, // Уровень детализации
			internalFormat,
			bitmapData.Width, bitmapData.Height,
			0, // ширина рамки
			textureFormat,
			GL_UNSIGNED_BYTE,
			bitmapData.Scan0);
	}

	// Задаем параметры фильтрации текстуры,
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
	// а также параметры "заворачивания" текстурных координат
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);

	// Завершаем работу с данными растра
	bmp.UnlockBits(&bitmapData);

	// Возвращаем идентификатор созданного изображения
	return texture;
}
