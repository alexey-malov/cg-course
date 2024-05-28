#pragma once

#include "Vector3.h"

class CModel;
class CModelMaterial;
class CTextureMap;

class CModelLoader
{
	class CFile3ds;
public:
	CModelLoader();

	// ��������� �������� 3ds �����, �������� ���������� ������
	void Load3dsFile(const char * fileName, CModel & model);

	// ������������ ������ ��� �������� �������?
	void IgnoreMissingTextures(bool ignoreMissingTextures);

	// ���������� ����� ������������� ���������� ������ ������
	void SetVertexBufferUsage(GLenum vertexBufferUsage);

	// ���������� ����� ������������� ���������� ������ ������
	void SetIndexBufferUsage(GLenum indexBufferUsage);

private:
	// ���������� � ����� ������������� �����, 
	// ������������ ��� ����������� ������
	struct MeshFace;

	// ���������� � ������� ������������� �����, ������������
	// ��� ����������� ������
	class CVertexInfo;

	// ��������� ���������
	void LoadMaterials(
		Lib3dsFile const& file, 
		CModel & model, 
		std::string const& baseFolder = "");

	// ��������� ��������, ��������� � ����������
	void LoadMaterialTextures(
		Lib3dsMaterial const& materialInfo, 
		CModel & model, 
		CModelMaterial & material, 
		std::string const& baseFolder = "");

	// ��������� �������� � ��������� �� � ������
	GLuint LoadTexture(
		std::string const & name, 
		CModel & model, 
		std::string const& baseFolder = "");

	// �������������� ���������� ����� ������� �� .3ds �����
	void InitTextureMap(
		Lib3dsTextureMap const & textureMapInfo, 
		CTextureMap & textureMap);

	// ��������� ������������� �����
	void LoadMeshes(Lib3dsFile const& file, CModel & model);

	// ��������� � ������ ������������� ����� � ���������
	// ���������� ������� ������ � �������� ������� �� 3ds �����
	static void LoadMesh(
		Lib3dsMesh const& mesh, 
		CModel & model, 
		std::vector<unsigned char> & vertexBufferData,
		std::vector<unsigned char> & indexBufferData
		);

	// ��������� ������ ������ ������� �� .3ds �����
	// �������� ������� � ��������, �������� ����� �������
	// � �������� ���������� � ������ ������������� �����
	// ���������� ���������� ������ � �������������� �����
	static unsigned FillVertexBufferData(
		Lib3dsMesh const& mesh, 
		std::vector<unsigned char> & vertexBufferData,
		std::vector<MeshFace> & outputFaces
		);

	// ��������� ������ �������� ������
	// � �������� ���������� ��������� IndexType �����������
	// ������������� ���, ������������ ��� �������� ��������.
	// ���������� �������� � �������� ����� ������
	// ������� indexBufferData
	template <typename IndexType>
	static unsigned FillIndexBufferData(
		std::vector<MeshFace> const& faces, 
		std::vector<unsigned char> & indexBufferData);

	// ���������� ������� ������������� ����� � ������
	// ����� ����������� �� ������ � ��������� ����� ������
	// � ����������� � ��������, � �����
	// ����������� ���������� � ������ ����� ����� 
	// �����������
	static void SplitVerticesBySmoothGroup(
		Lib3dsMesh const& mesh,
		std::vector<CVertexInfo>& outputVertices,
		std::vector<MeshFace> & outputFaces
		);

	// ������� � ��������� ������ ���������� � ��������,
	// � ����� � �������������� ��������, ������������ � ����������
	// ����������� ������������ ������ � ������ ����� �����������
	// ���������:
	//	mesh - ������������ �����
	//	vertexBufferData - ������, ���������� ������ ��� ������ ������
	//	vertexBufferOffset - �������� � ������� � ������ ���������� 
	//		� �������� ������ ������������� �����
	//	outputFaces - �������� ������ � ����������� �����������
	//		� ������
	//	������������ ��������:
	//		���������� ������ � �������������� �����
	template <class VertexType>
	static unsigned SplitVerticesAndBuildNormals(
		Lib3dsMesh const& mesh,
		std::vector<unsigned char> & vertexBufferData,
		size_t vertexBufferOffset,
		std::vector<MeshFace> & outputFaces
		);


private:
	// ����� ������������� ���������� ������
	GLenum m_vertexBufferUsage;

	// ����� ������������� ������ ��������
	GLenum m_indexBufferUsage;

	// ������������ ������ �������� �������?
	bool m_ignoreMissingTextures;
};
