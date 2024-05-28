#include "StdAfx.h"
#include "ModelRenderer.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelMaterial.h"


CModelRenderer::CModelRenderer(void)
:m_texCoordsEnabled(false)
,m_cullFace(false)
{
}

void CModelRenderer::RenderModel(CModel const& model)const
{
	const size_t meshCount = model.GetMeshCount();

	// Если нет полигональных сеток, то нечего рисовать
	if (meshCount == 0)
	{
		return;
	}

	// Осуществляем привязку к вершинным и индексным буферам 
	// рисуемой модели
	model.GetVertexBuffer().Bind();
	model.GetIndexBuffer().Bind();

	// Разрешаем использование массивов координат вершин и нормалей
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Определяем необходимость включения текстурных координат сетки №0
	m_texCoordsEnabled = !model.GetMesh(0).HasTextureCoords();

	m_cullFace = true;

	const int materialCount = model.GetMeterialCount();

	// Пробегаемся по всем материалам модели
	for (int material = -1; material < materialCount; ++material)
	{
		// Флаг, сигнализирующий о том, что текущий материал был 
		// уже активирован.
		// Он нужен для того, чтобы не тратить время на активацию
		// материала, если он был активирован одонй из ранее
		// визуализированных сеток
		bool materialActivated = false;

		// В каждой сетке рисуем подсетку, использующую данный материал
		for (size_t mesh = 0; mesh < meshCount; ++mesh)
		{
			// При рисоваии подсетки материал нужно активировать материал,
			// если он не был ранее активирован и это не материал
			// с индексом=-1 (материал по умолчанию)
			bool needToActivateMaterial = 
				!materialActivated && material >= 0;

			// Рисуем подсетку сетки mesh модели model,
			// использующую материал с индексом material
			materialActivated |= RenderMaterialSubMesh(
				model, mesh, material, needToActivateMaterial);
		}
	}

	if (!m_cullFace)
	{
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		glEnable(GL_CULL_FACE);
	}

	// Если использование массива текстурных координат было разрешено,
	// то запрещаем его использование
	if (m_texCoordsEnabled)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	// Аналогично запрещаем использование массивов вершин и нормалей
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

bool CModelRenderer::RenderMaterialSubMesh(
	CModel const& model, 
	unsigned meshIndex, 
	int materialIndex,
	bool activateMaterial
	)const
{
	// Получаем сетку по ее индексу
	CMesh const& mesh = model.GetMesh(meshIndex);

	// Получаем индекс подсетки, использующей материал materialIndex
	const int materialSubMeshIndex = 
		mesh.GetMaterialSubMesh(materialIndex);

	// Если в данной сетке нет граней, использующих материал
	// materialIndex, то выходим, т.к. рисовать нечего
	if (materialSubMeshIndex < 0)
	{
		// т.к. мы ничего не рисуем, то сообщаем, что материал
		// мы не активировали
		return false;
	}

	bool materialActivated = false;

	// Активируем материал, если его следует активировать и текущий
	// материал не является материалом по умолчанию
	if (activateMaterial && (materialIndex >= 0))
	{
		// Получили материал модели
		CModelMaterial const& modelMaterial = model.GetMaterial(materialIndex);

		// Извлекли параметры материала OpenGL и активировали его
		modelMaterial.GetMaterial().Activate(GL_FRONT_AND_BACK);
		
		// Сигнализируем о том, что материал был активирован
		materialActivated = true;

		// Если материал двусторонний, включаем двусторонний режим освещения
		// и выключаем режим отбраковки граней
		if (modelMaterial.IsTwoSided())
		{
			if (m_cullFace)
			{
				glDisable(GL_CULL_FACE);
				m_cullFace = false;
				glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			}
		}
		else	// материал не видим с нелицевой стороны
		{
			if (!m_cullFace)
			{
				glEnable(GL_CULL_FACE);
				m_cullFace = true;
				glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
			}
		}
	}

	// Получаем адрес данных буфера вершин модели
	GLubyte const * pVertexPointer = reinterpret_cast<GLubyte const *>
		(model.GetVertexBuffer().GetBufferPointer());

	// Инициализируем указатели OpenGL на массивы вершин
	// и текстурных координат данной полигональной сетки
	SetupMeshVertexPointers(mesh, pVertexPointer);

	// Если поддерживается расширение GL_EXT_compiled_vertex_array,
	// то мы можем попросить OpenGL выполнить кеширование
	// оттрансформированных вершин полигональной сетки.
	// В том случае, когда каждая вершина в среднем используется 
	// чаще, чем дважды, это может дать прирост в скорости
	// обработки вершин, т.к. каждая вершина будет
	// оттрансформирована лишь однажды, а использована несколько раз
	bool needToUnlockArrays = false;
	if (
		(mesh.GetIndexCount() > mesh.GetVertexCount() * 2) && 
		GLEW_EXT_compiled_vertex_array
		)
	{
		glLockArraysEXT(0, mesh.GetVertexCount());
		needToUnlockArrays = true;
	}

	// Получаем адрес данных буфера индексов модели
	GLubyte const * pIndexPointer = reinterpret_cast<GLubyte const *>
		(model.GetIndexBuffer().GetBufferPointer());

	// Визуализируем подсетку, использующую текущий материал
	RenderSubMeshFaces(
		mesh, 
		materialSubMeshIndex, 
		pIndexPointer + mesh.GetIndexBufferOffset());

	// Если мы кешировали оттрансформированные вершины, то
	// надо вызвать метод glUnlockArraysEXT для освобождения
	// связанных с ними данных
	if (needToUnlockArrays)
	{
		glUnlockArraysEXT();
	}

	// Сообщаем о том, был ли активирован материал
	return materialActivated;
}

void CModelRenderer::SetupMeshVertexPointers(
	CMesh const& mesh, 
	GLubyte const * pVertexBufferData)const
{
	// Получаем смещение в вершинном и индексном буферах, по которым
	// размещаются данные текущей полигональной сетки
	unsigned vertexBufferOffset = mesh.GetVertexBufferOffset();

	// Есть ли в сетке текстурные координаты?
	bool meshUsesTexture = mesh.HasTextureCoords();

	// Вычисляем интервал между вершинами полигональной сетки
	// в зависимости от наличия текстурных координат
	unsigned stride = 
		meshUsesTexture ? sizeof(TexturedVertex) : sizeof(Vertex);

	// Задаем адреса начала массивов вершин и нормалей текущей
	// полиогональной сетки
	glVertexPointer(
		3, 
		GL_FLOAT, 
		stride, 
		pVertexBufferData + vertexBufferOffset + offsetof(Vertex, position)
		);
	glNormalPointer(
		GL_FLOAT, 
		stride, 
		pVertexBufferData + vertexBufferOffset + offsetof(Vertex, normal)
		);

	// Разрешаем, либо запрещаем использование массива
	// текстурных координат
	// При этом минимизируем количество вызовов 
	// метода glEnableClientState/glDisableClientState
	if (meshUsesTexture && !m_texCoordsEnabled)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else if (!meshUsesTexture && m_texCoordsEnabled)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	// Если сетка использует текстурные координаты, задаем
	// адрес начала массива текстурных координат
	if (meshUsesTexture)
	{
		glTexCoordPointer(
			2, 
			GL_FLOAT, 
			stride, 
			pVertexBufferData + 
			vertexBufferOffset + 
			offsetof(TexturedVertex, texCoord));
	}

	// Обновляем информацию о том, был ли включен массив текстурных координат
	m_texCoordsEnabled = meshUsesTexture;
}

void CModelRenderer::RenderSubMeshFaces(
	CMesh const & mesh,				// сетка
	unsigned subMeshIndex,			// индекс подсетки
	GLubyte const * pMeshIndices	// адрес массива индексов сетки
	)const
{
	// получаем подсетку с индексом subMeshIndex
	CMesh::SubMesh const subMesh = mesh.GetSubMesh(subMeshIndex);

	// Вычисляем адрес подсетки в индексном буфере
	GLubyte const * pSubMeshPointer = 
		pMeshIndices + (subMesh.startIndex * mesh.GetIndexSize());

	// Если поддерживается расширение GL_EXT_draw_range_elements,
	// используем его для рисования массива примитивов,
	// т.к. его реализация может быть более эффективной
	// по сравнению c glDrawElements
	if (GLEW_EXT_draw_range_elements)
	{
		// Используем более производительный способ рисования 
		glDrawRangeElements(
			mesh.GetPrimitiveType(),
			0,
			mesh.GetVertexCount() - 1,
			subMesh.indexCount,
			mesh.GetIndexType(),
			pSubMeshPointer
			);
	}
	else
	{
		// Если расширение GL_EXT_draw_range_elements не поддерживается,
		// рисуем традиционным способом
		glDrawElements(
			mesh.GetPrimitiveType(), 
			subMesh.indexCount, 
			mesh.GetIndexType(), 
			pSubMeshPointer
			);
	}
}