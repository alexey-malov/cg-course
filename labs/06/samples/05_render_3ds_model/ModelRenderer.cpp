#include "StdAfx.h"
#include "ModelRenderer.h"
#include "Mesh.h"
#include "Model.h"

CModelRenderer::CModelRenderer(void)
{
}

void CModelRenderer::RenderModel(CModel const& model)
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

	// Получаем базовые адреса массивов вершин и индексов
	GLubyte const * pVertexPointer = reinterpret_cast<GLubyte const *>
		(model.GetVertexBuffer().GetBufferPointer());
	GLubyte const * pIndexPointer = reinterpret_cast<GLubyte const *>
		(model.GetIndexBuffer().GetBufferPointer());

	// Разрешаем использование массивов координат вершин и нормалей
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Определяем необходимость включения текстурных координат сетки №0
	bool enableTexture = !model.GetMesh(0).HasTextureCoords();

	// Цикл по всем полигональным сеткам модели
	for (size_t i = 0; i < meshCount; ++i)
	{
		CMesh const& mesh = model.GetMesh(i);

		// Получаем смещение в вершинном и индексном буферах, по которым
		// размещаются данные текущей полигональной сетки
		unsigned vertexBufferOffset = mesh.GetVertexBufferOffset();
		unsigned indexBufferOffset = mesh.GetIndexBufferOffset();

		// Есть ли в сетке текстурные координаты?
		bool meshUsesTexture = mesh.HasTextureCoords();

		// Вычисляем интервал между вершинами полигональной сетки
		// в зависимости от наличия текстурных координат
		unsigned stride = meshUsesTexture ? sizeof(TexturedVertex) : sizeof(Vertex);

		// Задаем адреса начала массивов вершин и нормалей текущей
		// полиогональной сетки
		glVertexPointer(
			3, 
			GL_FLOAT, 
			stride, 
			pVertexPointer + vertexBufferOffset + offsetof(Vertex, position)
			);
		glNormalPointer(
			GL_FLOAT, 
			stride, 
			pVertexPointer + vertexBufferOffset + offsetof(Vertex, normal)
			);

		// Разрешаем, либо запрещаем использование массива
		// текстурных координат
		// При этом минимизируем количество вызовов 
		// метода glEnableClientState/glDisableClientState
		if (meshUsesTexture && !enableTexture)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else if (!meshUsesTexture && enableTexture)
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
				pVertexPointer + 
					vertexBufferOffset + 
					offsetof(TexturedVertex, texCoord));
		}

		enableTexture = meshUsesTexture;

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

		// Если поддерживается расширение GL_EXT_draw_range_elements,
		// испольузем его для рисования массива примитивов,
		// т.к. его реализация может быть более эффективной
		// по сравнению c glDrawElements
		if (GLEW_EXT_draw_range_elements)
		{
			glDrawRangeElements(
				mesh.GetPrimitiveType(),
				0,
				mesh.GetVertexCount() - 1,
				mesh.GetIndexCount(),
				mesh.GetIndexType(),
				pIndexPointer + indexBufferOffset
				);
		}
		else
		{
			// Если расширение GL_EXT_draw_range_elements не поддерживается,
			// рисуем традиционным способом
			glDrawElements(
				mesh.GetPrimitiveType(), 
				mesh.GetIndexCount(), 
				mesh.GetIndexType(), 
				pIndexPointer + indexBufferOffset
				);
		}

		// Если мы кешировали оттрансформирвоанные вершины, то
		// надо вызвать метод glUnlockArraysEXT для освобождения
		// связанных с ними данных
		if (needToUnlockArrays)
		{
			glUnlockArraysEXT();
		}

	}

	// Если использование массива текстурных координат было разрешено,
	// то запрещаем его использование
	if (enableTexture)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	// Аналогично запрещаем использование массивов вершин и нормалей
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
