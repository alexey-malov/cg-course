#pragma once

#include "Texture.h"

class CTextureMap : private boost::noncopyable
{
public:
	CTextureMap(GLuint textureId = 0);
	~CTextureMap(void);

	// ��������� ���������� ����� � ���������� �������� OpenGL
	void AttachTexture(GLuint textureId);

	// �������� ���������� ������, ��������� � ���������� ������
	CTexture2DHandle const& GetTexture()const;

	// ���������� ������������� ��������������� ���������� ���������
	void SetScale(float sx, float sy);

	// ���������� �������� ���������� ���������
	void SetOffset(float dx, float dy);

	// ���������� ���� �������� ���������� ���������
	void SetRotation(float rotation);

	// ������ �� ���������� ������ � ������ ���������� ������
	bool IsLoaded()const;
private:
	CTexture2DHandle m_texture;
	float m_sx;
	float m_sy;
	float m_dx;
	float m_dy;
	float m_rotation;
};
