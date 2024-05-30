#pragma once
#include "Shaders.h"
#include "Texture.h"
#include "glapplication.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char* title, int width, int height);
	~CMyApplication(void);

protected:
	virtual void OnDisplay();
	virtual void OnInit();
	virtual void OnReshape(int width, int height);

private:
	void InitShaders();
	void InitTextures();

private:
	// Шейдерные объекты
	CProgram m_program;
	CShader m_vertexShader;
	CShader m_fragmentShader;
	CShader m_geometryShader;
	// Расположение переменной Size в программном объекте
	GLint m_sizeLocation;
	// Расположение переменной TextureMap в программном объекте
	GLint m_textureMapLocation;

	// Текстра
	CTexture2D m_texture;
};
