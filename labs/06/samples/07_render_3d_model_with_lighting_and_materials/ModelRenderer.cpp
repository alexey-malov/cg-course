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

	// ���� ��� ������������� �����, �� ������ ��������
	if (meshCount == 0)
	{
		return;
	}

	// ������������ �������� � ��������� � ��������� ������� 
	// �������� ������
	model.GetVertexBuffer().Bind();
	model.GetIndexBuffer().Bind();

	// ��������� ������������� �������� ��������� ������ � ��������
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// ���������� ������������� ��������� ���������� ��������� ����� �0
	m_texCoordsEnabled = !model.GetMesh(0).HasTextureCoords();

	m_cullFace = true;

	const int materialCount = model.GetMeterialCount();

	// ����������� �� ���� ���������� ������
	for (int material = -1; material < materialCount; ++material)
	{
		// ����, ��������������� � ���, ��� ������� �������� ��� 
		// ��� �����������.
		// �� ����� ��� ����, ����� �� ������� ����� �� ���������
		// ���������, ���� �� ��� ����������� ����� �� �����
		// ����������������� �����
		bool materialActivated = false;

		// � ������ ����� ������ ��������, ������������ ������ ��������
		for (size_t mesh = 0; mesh < meshCount; ++mesh)
		{
			// ��� �������� �������� �������� ����� ������������ ��������,
			// ���� �� �� ��� ����� ����������� � ��� �� ��������
			// � ��������=-1 (�������� �� ���������)
			bool needToActivateMaterial = 
				!materialActivated && material >= 0;

			// ������ �������� ����� mesh ������ model,
			// ������������ �������� � �������� material
			materialActivated |= RenderMaterialSubMesh(
				model, mesh, material, needToActivateMaterial);
		}
	}

	if (!m_cullFace)
	{
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		glEnable(GL_CULL_FACE);
	}

	// ���� ������������� ������� ���������� ��������� ���� ���������,
	// �� ��������� ��� �������������
	if (m_texCoordsEnabled)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	// ���������� ��������� ������������� �������� ������ � ��������
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
	// �������� ����� �� �� �������
	CMesh const& mesh = model.GetMesh(meshIndex);

	// �������� ������ ��������, ������������ �������� materialIndex
	const int materialSubMeshIndex = 
		mesh.GetMaterialSubMesh(materialIndex);

	// ���� � ������ ����� ��� ������, ������������ ��������
	// materialIndex, �� �������, �.�. �������� ������
	if (materialSubMeshIndex < 0)
	{
		// �.�. �� ������ �� ������, �� ��������, ��� ��������
		// �� �� ������������
		return false;
	}

	bool materialActivated = false;

	// ���������� ��������, ���� ��� ������� ������������ � �������
	// �������� �� �������� ���������� �� ���������
	if (activateMaterial && (materialIndex >= 0))
	{
		// �������� �������� ������
		CModelMaterial const& modelMaterial = model.GetMaterial(materialIndex);

		// �������� ��������� ��������� OpenGL � ������������ ���
		modelMaterial.GetMaterial().Activate(GL_FRONT_AND_BACK);
		
		// ������������� � ���, ��� �������� ��� �����������
		materialActivated = true;

		// ���� �������� ������������, �������� ������������ ����� ���������
		// � ��������� ����� ���������� ������
		if (modelMaterial.IsTwoSided())
		{
			if (m_cullFace)
			{
				glDisable(GL_CULL_FACE);
				m_cullFace = false;
				glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			}
		}
		else	// �������� �� ����� � ��������� �������
		{
			if (!m_cullFace)
			{
				glEnable(GL_CULL_FACE);
				m_cullFace = true;
				glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
			}
		}
	}

	// �������� ����� ������ ������ ������ ������
	GLubyte const * pVertexPointer = reinterpret_cast<GLubyte const *>
		(model.GetVertexBuffer().GetBufferPointer());

	// �������������� ��������� OpenGL �� ������� ������
	// � ���������� ��������� ������ ������������� �����
	SetupMeshVertexPointers(mesh, pVertexPointer);

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

	// �������� ����� ������ ������ �������� ������
	GLubyte const * pIndexPointer = reinterpret_cast<GLubyte const *>
		(model.GetIndexBuffer().GetBufferPointer());

	// ������������� ��������, ������������ ������� ��������
	RenderSubMeshFaces(
		mesh, 
		materialSubMeshIndex, 
		pIndexPointer + mesh.GetIndexBufferOffset());

	// ���� �� ���������� �������������������� �������, ��
	// ���� ������� ����� glUnlockArraysEXT ��� ������������
	// ��������� � ���� ������
	if (needToUnlockArrays)
	{
		glUnlockArraysEXT();
	}

	// �������� � ���, ��� �� ����������� ��������
	return materialActivated;
}

void CModelRenderer::SetupMeshVertexPointers(
	CMesh const& mesh, 
	GLubyte const * pVertexBufferData)const
{
	// �������� �������� � ��������� � ��������� �������, �� �������
	// ����������� ������ ������� ������������� �����
	unsigned vertexBufferOffset = mesh.GetVertexBufferOffset();

	// ���� �� � ����� ���������� ����������?
	bool meshUsesTexture = mesh.HasTextureCoords();

	// ��������� �������� ����� ��������� ������������� �����
	// � ����������� �� ������� ���������� ���������
	unsigned stride = 
		meshUsesTexture ? sizeof(TexturedVertex) : sizeof(Vertex);

	// ������ ������ ������ �������� ������ � �������� �������
	// �������������� �����
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

	// ���������, ���� ��������� ������������� �������
	// ���������� ���������
	// ��� ���� ������������ ���������� ������� 
	// ������ glEnableClientState/glDisableClientState
	if (meshUsesTexture && !m_texCoordsEnabled)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else if (!meshUsesTexture && m_texCoordsEnabled)
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
			pVertexBufferData + 
			vertexBufferOffset + 
			offsetof(TexturedVertex, texCoord));
	}

	// ��������� ���������� � ���, ��� �� ������� ������ ���������� ���������
	m_texCoordsEnabled = meshUsesTexture;
}

void CModelRenderer::RenderSubMeshFaces(
	CMesh const & mesh,				// �����
	unsigned subMeshIndex,			// ������ ��������
	GLubyte const * pMeshIndices	// ����� ������� �������� �����
	)const
{
	// �������� �������� � �������� subMeshIndex
	CMesh::SubMesh const subMesh = mesh.GetSubMesh(subMeshIndex);

	// ��������� ����� �������� � ��������� ������
	GLubyte const * pSubMeshPointer = 
		pMeshIndices + (subMesh.startIndex * mesh.GetIndexSize());

	// ���� �������������� ���������� GL_EXT_draw_range_elements,
	// ���������� ��� ��� ��������� ������� ����������,
	// �.�. ��� ���������� ����� ���� ����� �����������
	// �� ��������� c glDrawElements
	if (GLEW_EXT_draw_range_elements)
	{
		// ���������� ����� ���������������� ������ ��������� 
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
		// ���� ���������� GL_EXT_draw_range_elements �� ��������������,
		// ������ ������������ ��������
		glDrawElements(
			mesh.GetPrimitiveType(), 
			subMesh.indexCount, 
			mesh.GetIndexType(), 
			pSubMeshPointer
			);
	}
}