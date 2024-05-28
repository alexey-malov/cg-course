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

	// ���� ��� ������������� �����, �� ������ ��������
	if (meshCount == 0)
	{
		return;
	}

	// ������������ �������� � ��������� � ��������� ������� 
	// �������� ������
	model.GetVertexBuffer().Bind();
	model.GetIndexBuffer().Bind();

	// �������� ������� ������ �������� ������ � ��������
	GLubyte const * pVertexPointer = reinterpret_cast<GLubyte const *>
		(model.GetVertexBuffer().GetBufferPointer());
	GLubyte const * pIndexPointer = reinterpret_cast<GLubyte const *>
		(model.GetIndexBuffer().GetBufferPointer());

	// ��������� ������������� �������� ��������� ������ � ��������
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// ���������� ������������� ��������� ���������� ��������� ����� �0
	bool enableTexture = !model.GetMesh(0).HasTextureCoords();

	// ���� �� ���� ������������� ������ ������
	for (size_t i = 0; i < meshCount; ++i)
	{
		CMesh const& mesh = model.GetMesh(i);

		// �������� �������� � ��������� � ��������� �������, �� �������
		// ����������� ������ ������� ������������� �����
		unsigned vertexBufferOffset = mesh.GetVertexBufferOffset();
		unsigned indexBufferOffset = mesh.GetIndexBufferOffset();

		// ���� �� � ����� ���������� ����������?
		bool meshUsesTexture = mesh.HasTextureCoords();

		// ��������� �������� ����� ��������� ������������� �����
		// � ����������� �� ������� ���������� ���������
		unsigned stride = meshUsesTexture ? sizeof(TexturedVertex) : sizeof(Vertex);

		// ������ ������ ������ �������� ������ � �������� �������
		// �������������� �����
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

		// ���������, ���� ��������� ������������� �������
		// ���������� ���������
		// ��� ���� ������������ ���������� ������� 
		// ������ glEnableClientState/glDisableClientState
		if (meshUsesTexture && !enableTexture)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else if (!meshUsesTexture && enableTexture)
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		// ���� ����� ���������� ���������� ����������, ������
		// ����� ������ ������� ���������� ���������
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

		// ���� �������������� ���������� GL_EXT_compiled_vertex_array,
		// �� �� ����� ��������� OpenGL ��������� �����������
		// �������������������� ������ ������������� �����.
		// � ��� ������, ����� ������ ������� � ������� ������������ 
		// ����, ��� ������, ��� ����� ���� ������� � ��������
		// ��������� ������, �.�. ������ ������� �����
		// ������������������ ���� �������, � ������������ ��������� ���
		bool needToUnlockArrays = false;
		if (
			(mesh.GetIndexCount() > mesh.GetVertexCount() * 2) && 
			GLEW_EXT_compiled_vertex_array
			)
		{
			glLockArraysEXT(0, mesh.GetVertexCount());
			needToUnlockArrays = true;
		}

		// ���� �������������� ���������� GL_EXT_draw_range_elements,
		// ���������� ��� ��� ��������� ������� ����������,
		// �.�. ��� ���������� ����� ���� ����� �����������
		// �� ��������� c glDrawElements
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
			// ���� ���������� GL_EXT_draw_range_elements �� ��������������,
			// ������ ������������ ��������
			glDrawElements(
				mesh.GetPrimitiveType(), 
				mesh.GetIndexCount(), 
				mesh.GetIndexType(), 
				pIndexPointer + indexBufferOffset
				);
		}

		// ���� �� ���������� �������������������� �������, ��
		// ���� ������� ����� glUnlockArraysEXT ��� ������������
		// ��������� � ���� ������
		if (needToUnlockArrays)
		{
			glUnlockArraysEXT();
		}

	}

	// ���� ������������� ������� ���������� ��������� ���� ���������,
	// �� ��������� ��� �������������
	if (enableTexture)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	// ���������� ��������� ������������� �������� ������ � ��������
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
