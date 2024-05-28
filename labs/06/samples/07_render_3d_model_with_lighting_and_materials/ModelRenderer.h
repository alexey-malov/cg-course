#pragma once

class CModel;
class CMesh;

class CModelRenderer : private boost::noncopyable
{
public:
	CModelRenderer(void);
	void RenderModel(CModel const& model)const;
private:
	// Визуализируем подсетку, грани которой используют материал
	// materialIndex
	// Возвращает true, если при вызове метода был активирован материал
	bool RenderMaterialSubMesh(
		CModel const& model,	// модель
		unsigned meshIndex,		// индекс сетки
		int materialIndex,		// индекс материала
		bool activateMaterial	// нужно ли активировать материал?
		)const;

	// Инициализируем указатели OpenGL адресами массивов вершин, нормалей
	// и текстурных координат полигональной сетки
	void SetupMeshVertexPointers(
		CMesh const& mesh,					// Сетка
		GLubyte const * pVertexBufferData	// Адрес буфера 
		)const;

	// Визуализация граней подсетки
	// Предполагается, что адреса массивов вершин, нормалей и текстурных
	// координат уже настроены на текущую сетки
	void RenderSubMeshFaces(
		CMesh const & mesh,				// сетка
		unsigned subMeshIndex,			// индекс рисуемой подсетки
		GLubyte const * pMeshIndices	// адрес массива индексов сетки
		)const;

	// Был ли включен массив текстурных координат?
	mutable bool m_texCoordsEnabled;

	// Был ли включен режим отбраковки граней?
	mutable bool m_cullFace;
};
