#pragma once

class CModel;
class CMesh;

class CModelRenderer : private boost::noncopyable
{
public:
	CModelRenderer(void);
	void RenderModel(CModel const& model)const;
private:
	// ������������� ��������, ����� ������� ���������� ��������
	// materialIndex
	// ���������� true, ���� ��� ������ ������ ��� ����������� ��������
	bool RenderMaterialSubMesh(
		CModel const& model,	// ������
		unsigned meshIndex,		// ������ �����
		int materialIndex,		// ������ ���������
		bool activateMaterial	// ����� �� ������������ ��������?
		)const;

	// �������������� ��������� OpenGL �������� �������� ������, ��������
	// � ���������� ��������� ������������� �����
	void SetupMeshVertexPointers(
		CMesh const& mesh,					// �����
		GLubyte const * pVertexBufferData	// ����� ������ 
		)const;

	// ������������ ������ ��������
	// ��������������, ��� ������ �������� ������, �������� � ����������
	// ��������� ��� ��������� �� ������� �����
	void RenderSubMeshFaces(
		CMesh const & mesh,				// �����
		unsigned subMeshIndex,			// ������ �������� ��������
		GLubyte const * pMeshIndices	// ����� ������� �������� �����
		)const;

	// ��� �� ������� ������ ���������� ���������?
	mutable bool m_texCoordsEnabled;

	// ��� �� ������� ����� ���������� ������?
	mutable bool m_cullFace;
};
