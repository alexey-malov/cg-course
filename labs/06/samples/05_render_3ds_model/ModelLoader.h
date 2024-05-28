#pragma once

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
		std::vector<unsigned short> & indexBufferData
		);

	// ��������� ������ ������ ������� �� .3ds �����
	static void FillVertexBufferData(
		Lib3dsMesh const& mesh, 
		std::vector<unsigned char> & vertexBufferData);

	// ��������� ������ �������� ������� �� .3ds �����
	static void FillIndexBufferData(
		Lib3dsMesh const& mesh, 
		std::vector<unsigned short> & indexBufferData);
private:
	// ����� ������������� ���������� ������
	GLenum m_vertexBufferUsage;

	// ����� ������������� ������ ��������
	GLenum m_indexBufferUsage;

	// ������������ ������ �������� �������?
	bool m_ignoreMissingTextures;
};
