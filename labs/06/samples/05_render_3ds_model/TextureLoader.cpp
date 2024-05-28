#include "StdAfx.h"
#include "TextureLoader.h"
#include "Texture.h"

CTextureLoader::CTextureLoader(void)
:m_buildMipmaps(true)
,m_minFilter(GL_LINEAR_MIPMAP_LINEAR)
,m_magFilter(GL_LINEAR)
,m_wrapS(GL_REPEAT)
,m_wrapT(GL_REPEAT)
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

GLuint CTextureLoader::LoadTexture2D(std::wstring const& fileName, GLuint textureName, GLint level)const
{
	// ��������� ����������� ��� ������ GDI+
	Gdiplus::Bitmap bmp(fileName.c_str());

	if (bmp.GetLastStatus() != Gdiplus::Ok)
	{
		throw std::runtime_error("Error loading texture file " + std::string(fileName.begin(), fileName.end()));
	}

	// ���������� ������ �������� ��� ������������� 
	// � ������� Bitmap::Lock � �������� gluBuild2DMipmaps
	Gdiplus::PixelFormat pixelFormat = 0;
	GLint textureFormat = 0;
	GLint colorComponents = 0;
	GLint internalFormat = 0;

	// ���� �� � ����������� �����-�����?
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

	// �������� ������ ������ ��� ������ � ������ ���������� �����������
	Gdiplus::BitmapData bitmapData;
	bitmapData.Scan0 = NULL;
	bmp.LockBits(&lockRect, Gdiplus::ImageLockModeRead, pixelFormat, &bitmapData);

	// ������� ���������� ������ (��� �������������)
	CTexture2DHandle texture(textureName);
	if (textureName == 0)
	{
		texture.Create();
	}
	// ������ �������� ���������� ������ � ������ ���������������
	// (� ��� ��� ���� �� ������� ������� �����������)
	texture.Bind();

	if (m_buildMipmaps)
	{
		// ������ ��������� ���-������� ��� ������������ �����������
		// � ����������� �� � ���������� ����������� �������
		gluBuild2DMipmaps(
			GL_TEXTURE_2D, 
			colorComponents, 
			bitmapData.Width, bitmapData.Height, 
			textureFormat, GL_UNSIGNED_BYTE, 
			bitmapData.Scan0);
	}
	else
	{
		// ������ ����������� ��� ��������� ������ �����������
		texture.TexImage(
			level,	// ������� �����������
			internalFormat, 
			bitmapData.Width, bitmapData.Height, 
			0, // ������ �����
			textureFormat, 
			GL_UNSIGNED_BYTE, 
			bitmapData.Scan0);
	}

	// ������ ��������� ���������� ��������,
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
	// � ����� ��������� "�������������" ���������� ���������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);

	// ��������� ������ � ������� ������
	bmp.UnlockBits(&bitmapData);

	// ���������� ������������� ���������� �����������
	return texture;
}

