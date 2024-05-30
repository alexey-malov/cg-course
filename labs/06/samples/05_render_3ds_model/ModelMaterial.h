#pragma once

#include "Material.h"
#include "TextureMap.h"

class CModelMaterial : private boost::noncopyable
{
public:
	CModelMaterial();

	// Добавить текстурную карту №1 к материалу
	CTextureMap & AddTextureMap1(GLuint texture);

	// Связана ли с материалом текстурная карта №1?
	bool HasTextureMap1()const;

	// Получить текстурную карту №1
	CTextureMap const& GetTextureMap1()const;
	CTextureMap & GetTextureMap1();

	// Получить материал OpenGL
	CMaterial & GetMaterial();
	CMaterial const& GetMaterial()const;
private:
	CTextureMap	m_textureMap1;
	CMaterial m_material;
};
