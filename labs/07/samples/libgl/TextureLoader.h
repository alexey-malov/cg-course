#pragma once

class CTextureLoader
{
public:
	// Выполняем загрузку двухмерной текстуры из файла
	// Если параметр textureName равен 0, то текстура будет загружена в
	// новый текстурный объект. В противном случае - в существующий
	GLuint LoadTexture2D(
		std::wstring const& fileName,
		GLuint textureName = 0, GLint level = 0,
		unsigned* pWidth = NULL, unsigned* pHeight = NULL) const;

	// Задаем параметры фильтрации при увеличении и уменьшении текстуры,
	// задаваемые сразу после загрузки
	void SetMinFilter(GLenum minFilter);
	void SetMagFilter(GLenum magFilter);

	// определяет, должно ли выполняться автоматическое построение
	// дополнительных уровней детализации для текстурного объекта
	void BuildMipmaps(bool buildMipmaps);

	// Устанавливаем режим "заворачивания" текстурных координат
	void SetWrapMode(GLenum wrapS, GLenum wrapT);

private:
	bool m_buildMipmaps = true;
	GLenum m_wrapS = GL_REPEAT;
	GLenum m_wrapT = GL_REPEAT;
	GLenum m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
	GLenum m_magFilter = GL_LINEAR;
};
