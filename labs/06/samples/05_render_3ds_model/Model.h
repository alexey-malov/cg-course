#pragma once

#include "Buffer.h"
#include "texture.h"
#include "Mesh.h"
#include "BoundingBox.h"

class CModelMaterial;
class CMesh;

class CModel : boost::noncopyable
{
public:
	CModel(void);

	/************************************************************************/
	/* ������ � �������������� �������                                      */
	/************************************************************************/

	// �������� ������������� �����, �������� � ������ �� �� �������
	CMesh const& GetMesh(size_t index)const;
	CMesh & GetMesh(size_t index);

	// �������� ���������� ������������� ����� � ������
	size_t GetMeshCount()const;

	// ��������� ������������� ����� � ������
	CMesh & AddMesh(unsigned int vertexBufferOffset, 
		unsigned int indexBufferOffset, 
		unsigned vertexCount, 
		unsigned indexCount, 
		bool hasTexture,
		CBoundingBox const& boundingBox,
		GLenum primitiveType,
		GLenum indexType
	);

	/************************************************************************/
	/* ������ � �������������� ������ ������                                */
	/************************************************************************/

	// �������� bounding box, �������������� ���������� ������
	CBoundingBox const GetBoundingBox()const;

	/************************************************************************/
	/* ������ � ���������� ������                                           */
	/************************************************************************/

	// ��������� ���������� �����������
	CTexture2D & AddTextureImage(std::string const& name);

	// ���������� ���������� �������
	size_t GetTexturesCount()const;

	// ���������� ��� ����������� ����������� �� ��� �������
	std::string GetTextureName(size_t index)const;

	// ���� �� �������� � ������ ������?
	bool HasTexture(std::string const& name)const;

	// ���������� ���������� ������ �� ��� �����
	CTexture2D & GetTextureByName(std::string const& name);
	CTexture2D const & GetTextureByName(std::string const& name)const;

	// ���������� ���������� ������ �� ��� �������
	CTexture2D & GetTexture(size_t index);
	CTexture2D const & GetTexture(size_t index)const;

	/************************************************************************/
	/* ������ � ����������� ������                                          */
	/************************************************************************/
	
	// ��������� ��������
	CModelMaterial & AddMaterial();

	// �������� ���������� ����������
	size_t GetMeterialCount()const;

	// �������� �������� �� ��� �������
	CModelMaterial const & GetMaterial(size_t index)const;
	CModelMaterial & GetMaterial(size_t index);

	/************************************************************************/
	/* ������ � �������� ������ � ��������                                  */
	/************************************************************************/

	// �������� ��������� �����
	CVertexBuffer & GetVertexBuffer();
	CVertexBuffer const& GetVertexBuffer()const;

	// �������� ��������� �����
	CIndexBuffer & GetIndexBuffer();
	CIndexBuffer const & GetIndexBuffer()const;
private:
	// ������� ��� ��������� ���� ������� ��� ����� �������� ��������
	class CompareTextureNames
	{
	public:
		bool operator()(
			std::string const& textureName1, 
			std::string const& textureName2)const;
	};
private:
	// ��������� � ��������� ������
	CVertexBuffer	m_vertexBuffer;
	CIndexBuffer	m_indexBuffer;

	// ��������� ������
	typedef boost::shared_ptr<CModelMaterial> CModelMaterialPtr;
	std::vector<CModelMaterialPtr> m_materials;

	// ���������� �����������, ������������ �������
	typedef boost::shared_ptr<CTexture2D> CTexture2DPtr;
	typedef std::map<
		std::string, 
		CTexture2DPtr, 
		CompareTextureNames> CTextures;
	typedef std::vector<std::string> CTextureNames;
	CTextures m_textures;	// �������� ������ (���->��������)
	CTextureNames m_textureNames; // ������ ���������� ����

	// ������������� ����� ������
	typedef boost::shared_ptr<CMesh> CMeshPtr;
	std::vector<CMeshPtr> m_meshes;

	// ����, ��������������� � ������������� ��������������
	// ��������������� ����� ������
	mutable bool m_boundingBoxMustBeUpdated;
	// �������������� ���� ������
	mutable CBoundingBox m_boundingBox;
};
