#pragma once

class CTextureLoader
{
public:
	CTextureLoader(void);
	~CTextureLoader(void);

	// ¬ыполн€ем загрузку двухмерной текстуры из файла
	// ≈сли параметр textureName равен 0, то текстура будет загружена в 
	// новый текстурный объект. ¬ противном случае - в существующий
	GLuint LoadTexture2D(
		std::wstring const& fileName, 
		GLuint textureName = 0, GLint level = 0, 
		unsigned * pWidth = NULL, unsigned * pHeight = NULL)const;

	// «адаем параметры фильтрации при увеличении и уменьшении текстуры, 
	// задаваемые сразу после загрузки
	void SetMinFilter(GLenum minFilter);
	void SetMagFilter(GLenum magFilter);

	// определ€ет, должно ли выполн€тьс€ автоматическое построение 
	// дополнительных уровней детализации дл€ текстурного объекта
	void BuildMipmaps(bool buildMipmaps);

	// ”станавливаем режим "заворачивани€" текстурных координат
	void SetWrapMode(GLenum wrapS, GLenum wrapT);
private:
	bool m_buildMipmaps;
	GLenum m_wrapS;
	GLenum m_wrapT;
	GLenum m_minFilter;
	GLenum m_magFilter;
};
