#pragma once
#include "glapplication.h"
#include "BufferObject.h"
#include "RotationController.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnMotion(int x, int y);
	virtual void OnMouse(int button, int state, int x, int y);
private:
	void DrawCube()const;
	// Выполняем создание и инициализацию буферов вершин и индексов
	void InitBuffers();
	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;

	// Расстояние до ближней и дальней плоскостей отсечения
	static const double ZNEAR;
	static const double ZFAR;
	CRotationController m_rotationController;

	// Буфер вершин
	CArrayBuffer m_vertices;
	// Буфер индексов
	CElementArrayBuffer m_indices;

	// Адреса массивов вершин, цвета и индексов
	GLvoid const * m_pVertexArray;
	GLvoid const * m_pColorArray;
	GLvoid const * m_pIndexArray;
};
