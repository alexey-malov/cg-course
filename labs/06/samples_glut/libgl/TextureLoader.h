#pragma once

class CTextureLoader
{
public:
	CTextureLoader(void);
	~CTextureLoader(void);

	// Выполняем загрузку двухмерной текстуры из файла
	// Если параметр textureName равен 0, то текстура будет загружена в 
	// новый текстурный объект. В противном случае - в существующий
	GLuint LoadTexture2D(std::wstring const& fileName, GLuint textureName = 0, GLint level = 0)const;

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
	bool m_buildMipmaps;
	GLenum m_wrapS;
	GLenum m_wrapT;
	GLenum m_minFilter;
	GLenum m_magFilter;
};
