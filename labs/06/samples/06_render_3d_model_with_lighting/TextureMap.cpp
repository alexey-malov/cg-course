#include "StdAfx.h"
#include "TextureMap.h"

CTextureMap::CTextureMap(GLuint textureId)
:m_sx(1)
,m_sy(1)
,m_dx(0)
,m_dy(0)
,m_rotation(0)
,m_texture(textureId)
{
}

CTextureMap::~CTextureMap(void)
{
}

CTexture2DHandle const& CTextureMap::GetTexture()const
{
	return m_texture;
}

bool CTextureMap::IsLoaded()const
{
	return m_texture != 0;
}

void CTextureMap::AttachTexture(GLuint textureId)
{
	m_texture = textureId;
}

void CTextureMap::SetScale(float sx, float sy)
{
	m_sx = sx;
	m_sy = sy;
}

void CTextureMap::SetOffset(float dx, float dy)
{
	m_dx = dx;
	m_dy = dy;
}

void CTextureMap::SetRotation(float rotation)
{
	m_rotation = rotation;
}


