#include "StdAfx.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "TextureLoader.h"
#include "TextureMap.h"
#include "BoundingBox.h"

// ����� ��� ��������������� ������������ ��������, ��������� � .3ds ������
class CModelLoader::CFile3ds : boost::noncopyable
{
public:
	CFile3ds(const char * fileName)
		:m_pFile(lib3ds_file_open(fileName))
	{
		if (m_pFile == NULL)
		{
			throw std::runtime_error(std::string("Unable to open ") + fileName);
		}
	}

	Lib3dsFile const& GetFile()const
	{
		return *m_pFile;
	}

	~CFile3ds()
	{
		if (m_pFile)
		{
			lib3ds_file_free(m_pFile);
		}
	}
private:
	Lib3dsFile * m_pFile;
};

CModelLoader::CModelLoader()
:m_vertexBufferUsage(GL_STATIC_DRAW)
,m_indexBufferUsage(GL_STATIC_DRAW)
,m_ignoreMissingTextures(true)
{
}

// ���������� ����� ������������� ���������� ������ ������
void CModelLoader::SetVertexBufferUsage(GLenum vertexBufferUsage)
{
	m_vertexBufferUsage = vertexBufferUsage;
}

// ���������� ����� ������������� ���������� ������ ������
void CModelLoader::SetIndexBufferUsage(GLenum indexBufferUsage)
{
	m_indexBufferUsage = indexBufferUsage;
}

void CModelLoader::Load3dsFile(const char * fileName, CModel & model)
{
	// ��������� ����
	CFile3ds file(fileName);

	// ����������� ���� � �������� � .3ds ������.
	// �� ����������� ��� ������ �������
	std::string filePath = fileName;
	// ������� ����� ����� (������, ���� ��������)
	size_t slashPos = filePath.find_last_of("/\\");

	// fileFolder ����� ��������� ���� ������ ������, ���� �������,
	// ���������� 3ds ����
	std::string fileFolder = 
		(slashPos == std::string::npos) ? "" :
		filePath.substr(0, slashPos + 1);

	// ��������� ���������
	LoadMaterials(file.GetFile(), model, fileFolder);

	// ��������� ������������� �����
	LoadMeshes(file.GetFile(), model);
}

void CModelLoader::LoadMeshes(Lib3dsFile const& file, CModel & model)
{
	// ��������� ������� ������ � ��������, 
	// ������� ����� ��������� ������� ���� ����� 3ds �����
	std::vector<unsigned char> vertexBufferData;
	std::vector<unsigned short> indexBufferData;

	const int meshCount = file.nmeshes;

	for (int i = 0; i < meshCount; ++i)
	{
		Lib3dsMesh const & mesh = *file.meshes[i];

		// ��������� ������ ������������� ����� �� 3ds �����
		// � ������� ������, � ���������� � �������� � ��������
		// ��������� � ������� ������ � ��������
		LoadMesh(mesh, model, vertexBufferData, indexBufferData);
	}

	// ������� ��������� �����
	model.GetVertexBuffer().Create();
	// � ��������� ��� ������� � ��������, ���������� �� ���� 
	// ������������� ����� ������
	model.GetVertexBuffer().BufferData(
		vertexBufferData.size() * sizeof(vertexBufferData[0]), 
		&vertexBufferData[0], 
		m_vertexBufferUsage);

	// ������� ����� ��������
	model.GetIndexBuffer().Create();
	// � ��������� ��� ������� �� �������� ������, ������������ �����,
	// ���������� �� ���� ������������� ����� ������
	model.GetIndexBuffer().BufferData(
		indexBufferData.size() * sizeof(indexBufferData[0]), 
		&indexBufferData[0], 
		m_indexBufferUsage);
}

void CModelLoader::LoadMesh(
	Lib3dsMesh const& mesh, 
	CModel & model, 
	std::vector<unsigned char> & vertexBufferData,
	std::vector<unsigned short> & indexBufferData
	)
{
	// ��������� �������� � ������ ������ ������� ������������� �����
	const unsigned int vertexBufferOffset = 
		sizeof(unsigned char) * vertexBufferData.size();
	// ��������� �������� � ������ �������� ������� ������������� �����
	const unsigned int indexBufferOffset = 
		sizeof(unsigned short) * indexBufferData.size();

	// ��������� ��������� ������ ������� �� .3ds �����
	FillVertexBufferData(mesh, vertexBufferData);
	// ��������� ������ �������� ������� �� .3ds �����
	FillIndexBufferData(mesh, indexBufferData);

	// ��������� �������������� ���� ������� ������������� �����
	// ��� ������ ������� ���������� lib3ds
	float minMeshBound[3];
	float maxMeshBound[3];
	lib3ds_mesh_bounding_box(
		const_cast<Lib3dsMesh*>(&mesh), 
		minMeshBound, maxMeshBound);

	// ������� Bounding box �� ������ ������, ������������ lib3ds
	CBoundingBox meshBoundingBox((CVector3f(minMeshBound)), (CVector3f(maxMeshBound)));

	// ��������� � ������ ������������� �����
	model.AddMesh(
		vertexBufferOffset, 
		indexBufferOffset, 
		mesh.nvertices, 
		mesh.nfaces * 3, 
		mesh.texcos != NULL,
		meshBoundingBox,
		GL_TRIANGLES,
		GL_UNSIGNED_SHORT
		);
}

void CModelLoader::FillIndexBufferData(Lib3dsMesh const& mesh, std::vector<unsigned short> & indexBufferData)
{
	const int numberOfFaces = mesh.nfaces;

	// ����������� � ������� ����� ��� �������� ������ N ������
	indexBufferData.reserve(indexBufferData.size() + numberOfFaces * 3);

	for (int i = 0; i < numberOfFaces; ++i)
	{
		Lib3dsFace const& inputFace = mesh.faces[i];

		unsigned short v0 = inputFace.index[0];
		assert(v0 < mesh.nvertices);
		unsigned short v1 = inputFace.index[1];
		assert(v1 < mesh.nvertices);
		unsigned short v2 = inputFace.index[2];
		assert(v2 < mesh.nvertices);

		indexBufferData.push_back(v0);
		indexBufferData.push_back(v1);
		indexBufferData.push_back(v2);
	}
}

void CModelLoader::FillVertexBufferData(Lib3dsMesh const& mesh, std::vector<unsigned char> & vertexBufferData)
{
	const int numberOfVertices = mesh.nvertices;

	// ����� ������� ������ � 3ds-�����
	float (*pInputVertices)[3] = mesh.vertices;

	// ����� ������� ���������� ��������� � 3ds �����
	float (*pInputTexCoords)[2] = mesh.texcos;

	// �������� �� ������ ������ � 3ds �����
	size_t vertexBufferOffset = vertexBufferData.size();

	// ���������, ���� �� � ����� ���������� ����������
	if (pInputTexCoords != NULL)
	{
		// ����� � ����������� ������������

		// ����������� ������ �� ������, ���������� ��������� 
		// � ����������� ������������
		vertexBufferData.resize(
			vertexBufferOffset + sizeof(TexturedVertex) * numberOfVertices);

		// ������ ����� ������������ ��������� ������ ������������� �����
		TexturedVertex * outputVertices = 
			reinterpret_cast<TexturedVertex*>(
				&vertexBufferData[vertexBufferOffset]);

		// ��������� ������ ������� � �������� � ������� ��������� TexturedVertex
		for (int v = 0; v < numberOfVertices; ++v)
		{
			float * inputPosition = pInputVertices[v];
			float * inputTexCoord = pInputTexCoords[v];

			TexturedVertex & outputVertex = outputVertices[v];
			Vector3 & outputPosition = outputVertex.position;
			// ������ ���������� ������� � ������������
			outputPosition.x = inputPosition[0];
			outputPosition.y = inputPosition[1];
			outputPosition.z = inputPosition[2];

			// ������ ���������� ���������� �������
			outputVertex.texCoord.x = inputTexCoord[0];
			outputVertex.texCoord.y = inputTexCoord[1];

			// TODO: ����� ��������� �������� ������� �������
			Vector3 & outputNormal = outputVertex.normal;
			outputNormal.x = outputNormal.y = outputNormal.z = 0;
		}
	}
	else
	{
		// ����� ��� ���������� ���������

		// ����������� ������ �� ������, ���������� ��������� 
		// ��� ���������� ���������
		vertexBufferData.resize(vertexBufferOffset + sizeof(Vertex) * numberOfVertices);

		// ������ ����� ������������ ��������� ������ ������������� �����
		Vertex * outputVertices = 
			reinterpret_cast<Vertex*>(&vertexBufferData[vertexBufferOffset]);

		// ��������� ������ ������� � �������� � ������� ��������� Vertex
		for (int v = 0; v < numberOfVertices; ++v)
		{
			float * inputPosition = pInputVertices[v];

			Vertex & outputVertex = outputVertices[v];
			Vector3 & outputPosition = outputVertex.position;
			// ������ ���������� ������� � ������������
			outputPosition.x = inputPosition[0];
			outputPosition.y = inputPosition[1];
			outputPosition.z = inputPosition[2];

			// TODO: ��������� �������� ������� �������
			Vector3 & outputNormal = outputVertex.normal;
			outputNormal.x = outputNormal.y = outputNormal.z = 0;
		}
	}
}

void CModelLoader::LoadMaterials(Lib3dsFile const& file, CModel & model, std::string const& baseFolder)
{
	const int materialsCount = file.nmaterials;

	for (int i = 0; i < materialsCount; ++i)
	{
		Lib3dsMaterial const * pMaterial = file.materials[i];

		// ��������� ����� �������� � ������
		CModelMaterial & material = model.AddMaterial();
		// � �������� ��������� � ���� ��������� ��������
		CMaterial & materialInfo = material.GetMaterial();

		// ������ ������� ���� ���������
		{
			const float * ambient = pMaterial->ambient;
			materialInfo.SetAmbient(ambient[0],ambient[1], ambient[2]);
		}

		// ������ ��������� ���� ���������
		{
			const float * diffuse = pMaterial->diffuse;
			materialInfo.SetDiffuse(diffuse[0], diffuse[1], diffuse[2]);
		}

		// ������ ���������� ���� ��������� � ������� ������
		{
			const float * specular = pMaterial->specular;
			materialInfo.SetSpecular(specular[0], specular[1], specular[2]);
			materialInfo.SetShininess(pMaterial->shininess);
		}

		// ��������� �������� ���������
		LoadMaterialTextures(*pMaterial, model, material, baseFolder);
	}
}

void CModelLoader::LoadMaterialTextures(
	Lib3dsMaterial const& materialInfo, 
	CModel & model, 
	CModelMaterial & material, 
	std::string const& baseFolder)
{
	// �������� ��������� ���������� ����� �1
	{
		Lib3dsTextureMap const & tex1 = materialInfo.texture1_map;

		// ���������, ���� �� ��� � ������ ��������?
		if (*tex1.name)
		{
			try
			{
				// ��������� ��������
				CTexture2DHandle texture1 = 
					LoadTexture(tex1.name, model, baseFolder);
				// ��������� ���������� �����
				CTextureMap & textureMap1 = material.AddTextureMap1(texture1);
				// ��������� �� ���������� �� .3ds �����
				InitTextureMap(tex1, textureMap1);
			}
			catch (std::runtime_error const&)
			{
				if (!m_ignoreMissingTextures)
				{
					throw;
				}
			}
		}
	}
}

GLuint CModelLoader::LoadTexture(std::string const & name, CModel & model, std::string const& baseFolder)
{
	// ��������� �������� � �������� ������ � ������
	CTexture2D & texture = model.AddTextureImage(name);
	// ��� ������ ���� ������ �� ������������ ���������� ������,
	// ���� ������ �� ����� ���������

	if (!texture)	// ���� ��� �������� ��� �� ������ ���������� �����������
	{
		CTextureLoader loader;

		std::string textureFilePath = baseFolder + name;

		// ��������� ���������� ����������� � ������������ ��� � ��������
		// ��-�� ����������� �������� ����� �� string � wstring 
		// ��������� ����������� ����� ������ �����, � ���� ������� �� ���������� 
		// ������� �� ��������� ������� ������� ASCII
		texture.Attach(
			loader.LoadTexture2D(
				std::wstring(textureFilePath.begin(), 
				textureFilePath.end())
				)
			);
	}

	return texture;
}

void CModelLoader::InitTextureMap(Lib3dsTextureMap const & textureMapInfo, CTextureMap & textureMap)
{
	textureMap.SetOffset(textureMapInfo.offset[0], textureMapInfo.offset[1]);
	textureMap.SetScale(textureMapInfo.scale[0], textureMapInfo.scale[1]);
	textureMap.SetRotation(textureMapInfo.rotation);
}

void CModelLoader::IgnoreMissingTextures(bool ignoreMissingTextures)
{
	m_ignoreMissingTextures = ignoreMissingTextures;
}

