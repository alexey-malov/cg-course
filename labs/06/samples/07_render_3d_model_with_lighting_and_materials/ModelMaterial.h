#pragma once

#include "Material.h"
#include "TextureMap.h"

class CModelMaterial : private boost::noncopyable
{
public:
	CModelMaterial();

	// �������� ���������� ����� �1 � ���������
	CTextureMap & AddTextureMap1(GLuint texture);

	// ������� �� � ���������� ���������� ����� �1?
	bool HasTextureMap1()const;

	// �������� ���������� ����� �1
	CTextureMap const& GetTextureMap1()const;
	CTextureMap & GetTextureMap1();

	// �������� �������� OpenGL
	CMaterial & GetMaterial();
	CMaterial const& GetMaterial()const;

	// �������� ����� � ������� � ��������� �����?
	bool IsTwoSided()const;
	void SetTwoSided(bool value);
private:
	CTextureMap	m_textureMap1;
	CMaterial m_material;
	bool m_twoSided;
};
