#include "StdAfx.h"
#include "ModelMaterial.h"

CModelMaterial::CModelMaterial()
:m_twoSided(false)
{
}

CMaterial & CModelMaterial::GetMaterial()
{
	return m_material;
}

CMaterial const& CModelMaterial::GetMaterial()const
{
	return m_material;
}

CTextureMap & CModelMaterial::AddTextureMap1(GLuint texture)
{
	m_textureMap1.AttachTexture(texture);
	return m_textureMap1;
}

bool CModelMaterial::HasTextureMap1()const
{
	return m_textureMap1.IsLoaded();
}

CTextureMap const& CModelMaterial::GetTextureMap1()const
{
	return m_textureMap1;
}

CTextureMap & CModelMaterial::GetTextureMap1()
{
	return m_textureMap1;
}

bool CModelMaterial::IsTwoSided()const
{
	return m_twoSided;
}

void CModelMaterial::SetTwoSided(bool value)
{
	m_twoSided = value;
}
