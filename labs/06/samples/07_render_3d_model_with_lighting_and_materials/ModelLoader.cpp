#include "StdAfx.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "TextureLoader.h"
#include "TextureMap.h"
#include "BoundingBox.h"

// ���������� � ����� ������������� �����
struct CModelLoader::MeshFace
{
	unsigned vertices[3];
	int materialIndex;
};

// ����� �������� ���������� � �������, 
// �� ���������, ����������� �������� � �������
class CModelLoader::CVertexInfo
{
public:
	// ����������� ������������ �������
	CVertexInfo()
		:m_originalVertexIndex(-1)
		,m_derivedVertexIndex(-1)
		,m_normalIsDefined(false)
	{}

	// ����������� ����������� �������
	CVertexInfo(CVector3f const& normal, int originalVertexIndex)
		:m_originalVertexIndex(originalVertexIndex)
		,m_derivedVertexIndex(-1)
		,m_normalIsDefined(true)
		,m_normal(normal)
	{
		assert(originalVertexIndex >= 0);
	}

	// ������ � ������� �������
	CVector3f const& GetNormal()const
	{
		assert(NormalIsDefined());
		return m_normal;
	}

	// ��������� ������ ������� ��� �������
	void SetNormal(CVector3f const& normal)
	{
		assert(!m_normalIsDefined);
		m_normal = normal;
		m_normalIsDefined = true;
	}


	// ���� �� ������ ������� � �����?
	bool NormalIsDefined()const
	{
		return m_normalIsDefined;
	}

	// ������ ������������ �������, ���������� ������
	// ������������� ������ ������������ ������� ����� ������������ �������
	// ������������� �����
	int GetOriginalVertexIndex()const
	{
		return m_originalVertexIndex;
	}

	// ���������� ������ ��������� ����������� ������� � ������ ������,
	// ���� -1, ���� ������� ������� �� ����� ����������� ������
	int GetDerivedVertex()const
	{
		return m_derivedVertexIndex;
	}

	// ������ ������ �������������� �������
	void DeriveVertex(int vertexIndex)
	{
		assert(vertexIndex >= 0);
		assert(m_derivedVertexIndex == -1);
		m_derivedVertexIndex = vertexIndex;
	}

private:
	int m_originalVertexIndex;	// ������ ������� �������
	int m_derivedVertexIndex;	// ������ ����������� �������
	bool m_normalIsDefined;		// ����� �� ������ �������?
	CVector3f m_normal;
};


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
	std::vector<unsigned char> indexBufferData;

	const int meshCount = file.nmeshes;

	for (int i = 0; i < meshCount; ++i)
	{
		Lib3dsMesh const & mesh = *file.meshes[i];

		// ��������� ������ ������������� ����� �� 3ds �����
		// � ������� ������, � ���������� � �������� � ��������
		// ��������� � ������� ������ � ��������
		LoadMesh(file.nmaterials, mesh, model, vertexBufferData, indexBufferData);
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
	unsigned materialCount,
	Lib3dsMesh const& mesh, 
	CModel & model, 
	std::vector<unsigned char> & vertexBufferData,
	std::vector<unsigned char> & indexBufferData
	)
{
	// ��������� �������� � ������ ������ ������� ������������� �����
	const unsigned int vertexBufferOffset = 
		sizeof(unsigned char) * vertexBufferData.size();

	// ����������� ������ ������
	std::vector<MeshFace> updatedFaces;

	// ��������� ������ ������, �������� ��� ������������� 
	// �� �����������
	unsigned const numberOfVertices = 
		FillVertexBufferData(mesh, vertexBufferData, updatedFaces);

	// ���, ������������ ��� �������� �������� ������
	GLenum indexType = 0;

	// �������������� �������� � ������ ������������� �����
	// � ������ ��������
	unsigned int indexBufferOffset = 0;

	// ������ ������ ������, ��������������� �� �������������
	// ������� ���������
	std::vector< std::vector<unsigned> > materialFaces;
	BuildMaterialFacesList(materialCount, updatedFaces, materialFaces);

	// � ����������� �� ���������� ������ ����� �����������
	// ��������� ������� � ���� 8, 16 ��� 32 ������ �����
	if (numberOfVertices <= UCHAR_MAX + 1)	// ���������� 8 ���?
	{
		indexType = GL_UNSIGNED_BYTE;
		indexBufferOffset = FillIndexBufferData<GLubyte>(
			updatedFaces, materialFaces, indexBufferData);
	}
	else if (numberOfVertices <= USHRT_MAX + 1) // ���������� 16 ���?
	{
		indexType = GL_UNSIGNED_SHORT;
		indexBufferOffset = FillIndexBufferData<GLushort>(
			updatedFaces, materialFaces, indexBufferData);
	}
	else	// ���������� 32 ������ �������
	{
		indexType = GL_UNSIGNED_INT;
		indexBufferOffset = FillIndexBufferData<GLuint>(
			updatedFaces, materialFaces, indexBufferData);
	}

	// ��������� �������������� ���� ������� ������������� �����
	// ��� ������ ������� ���������� lib3ds
	float minMeshBound[3];
	float maxMeshBound[3];
	lib3ds_mesh_bounding_box(
		const_cast<Lib3dsMesh*>(&mesh), 
		minMeshBound, maxMeshBound);

	// ������� Bounding box �� ������ ������, ������������ lib3ds
	CBoundingBox meshBoundingBox(
		(CVector3f(minMeshBound)), 
		(CVector3f(maxMeshBound)));

	// ��������� � ������ ������������� �����
	CMesh & addedMesh = model.AddMesh(
		vertexBufferOffset, 
		indexBufferOffset, 
		numberOfVertices,
		mesh.nfaces * 3, 
		mesh.texcos != NULL,
		meshBoundingBox,
		GL_TRIANGLES,
		indexType
		);

	// ���������� ����� ������� ����� ������ �������� ����� 0
	unsigned submeshStartIndex = 0;

	// ��������� � ����� ���������� � ������������ � ��� ���������
	// � ����������
	for (unsigned materialIndex = 0; materialIndex <= materialCount; 
		++materialIndex)
	{
		// ������ ������, ������������ �������� materialIndex
		std::vector<unsigned> const& subMeshFaces = 
			materialFaces[materialIndex];

		// ���������� ������, ������� �������� materialIndex
		size_t const subMeshFaceCount = subMeshFaces.size();

		// ���� �������� �� ������, �� ��������� �� � ���� ��������
		if (subMeshFaceCount != 0)
		{
			// �.�. ����� �����������, ���������� �������� � 3 ���� ������
			// ���������� ������
			unsigned const subMeshIndexCount = subMeshFaceCount * 3;

			// ��������� ��������
			unsigned subMeshIndex = addedMesh.AddSubMesh(
				submeshStartIndex, subMeshIndexCount);

			// ������ � ����� ����� ����� ������� �������� ��������� � 
			// ����������� ���������
			addedMesh.SetMaterialSubMesh(
				static_cast<int>(materialIndex) - 1, subMeshIndex);

			// ��������� ���������� ����� ������� ��������� ��������
			submeshStartIndex += subMeshIndexCount;
		}
	}
}

template <typename IndexType>
unsigned CModelLoader::FillIndexBufferData(
	std::vector<MeshFace> const& faces,
	std::vector< std::vector<unsigned> > const & materialFaces,
	std::vector<unsigned char> & indexBufferData)
{
	// ���������� ������
	const unsigned numberOfFaces = faces.size();

	// ������ (� ������), ��������� ��� �������� ������ �������
	unsigned indexSize = sizeof(IndexType);

	// �������� �� ��, ��� indexSize - ������� ������
	assert((indexSize & (indexSize - 1)) == 0);

	// ������ ������, ���������� ����� ��������� ������������� �����
	unsigned meshIndexDataSize = numberOfFaces * 3 * indexSize;

	// ��������� �������� � ������ �������� ����� �������, ����� �������
	// ����� ���������� �� ��������, �������� ������� �������
	// (��� ��������� ������� � ������)
	// 8-������ ������� ����� ��������� �� ������� ������
	// 16-������ ������� - �� ����������� �������
	// 32-������ ������� - �� ��������, �������� 4
	unsigned const indexBufferOffset = 
		((indexBufferData.size() + indexSize - 1) / indexSize) * indexSize;

	// ����������� ������ ������ �������� ���, ����� �� ��������
	// indexBufferOffset ���������� ������ �������� meshIndexDataSize
	indexBufferData.resize(indexBufferOffset + meshIndexDataSize);

	// ��������� �� ������� � ������� ��������, ��������������� ������
	// ������ ������� ������������� �����
	IndexType * pOutputIndex = 
		reinterpret_cast<IndexType*>(&indexBufferData[indexBufferOffset]);

	// ��-�� ���������� ����� �������� max �� windows.h 
	// � ������� std::numeric_limits::max() �������� ������������
	// ��������� ������ ������������ ������������� �������� ������������
	// �������������� ���� IndexType
	IndexType const maxIndexValue = IndexType(~0);
	// �������������� �������� �� ��, ��� ��� �������� ��������
	// ������������ ����������� ���
	assert(maxIndexValue > 0);

	size_t const materialCount = materialFaces.size();
	// ����������� �� ������ ����������
	for (
		size_t materialIndex = 0; 
		materialIndex < materialCount; 
		++materialIndex)
	{
		// ������ ������, �������� � ������ �������� � ������� ��������
		// ���������
		std::vector<unsigned> const& subMeshFaces = 
			materialFaces[materialIndex];

		size_t const subMeshFaceCount = subMeshFaces.size();

		// ��������� �� ������� ������ ������� ��������
		for (unsigned i = 0; i < subMeshFaceCount; ++i)
		{
			// ������ ����� ��������
			unsigned faceIndex = subMeshFaces[i];

			MeshFace const& inputFace = faces[faceIndex];

			// ��������� �� �������� ������� �����
			for (unsigned j = 0; j < 3; ++j)
			{
				// �������� ������ ������� ������� �����
				unsigned vertexIndex = inputFace.vertices[j];

				// ���������, ��� ������ ������� ����� ���� ������������
				// ��� ������ ���� IndexType
				if (vertexIndex > (unsigned)maxIndexValue)
				{
					throw std::logic_error("Vertex index is out of range");
				}

				// ��� ���������, ��������� ������ ������� � ������ ��������
				// � ��������� � ��������� �������
				*pOutputIndex++ = static_cast<IndexType>(vertexIndex);
			}
		}
	}

	// ���������� �������� � ������� ��������, �� �������� �������������
	// ������� ������ ������������� �����
	return indexBufferOffset;
}

void CModelLoader::SplitVerticesBySmoothGroup(
   Lib3dsMesh const& mesh,
   std::vector<CVertexInfo>& outputVertices,
   std::vector<MeshFace> & outputFaces
	)
{
	// ������ ��� �������� �������� ������ ���� ������ ������������� �����
	std::vector<float> faceVertexNormals(mesh.nfaces * 3 * 3);

	// ������� lib3ds_mesh_calculate_vertex_normals ��������� ������� ������ 
	// ������� ������ ����������� ����� ����� � ������ ����� ����������� ����� 
	// � ����������� � ��� ������
	// ��� �� ������ ��� �������� �������� ������� "������ �����":
	//	�) � �������� ������� ��������� ������� ��������� ��������� Lib3dsMesh*
	//		������ Lib3dsMesh const*, ���� �� ������������ ����������
	//		������������� �����. ���� �� �����, ��� ����������� ������ ����������.
	//		�������� ����� ������������� � ��������� � �������������� const_cast
	//	�) � �������� ������� ��������� ������� ��������� ���������
	//		�� ������ ������� ������� ���������������� �������� 
	//		float-�� (float[][3]).
	//		��������� ��������� faceVertexNormals ������ �������� ���� float, ��� 
	//		�������������� ��������� float* � ������� ���� float(*)[3] 
	//		������������� ���������� reinterpret_cast
	float (*pCalculatedNormals)[3] = 
		reinterpret_cast<float(*)[3]>(&faceVertexNormals[0]);
	// ��������� ���������� �������� � �������� ��� ������ lib3ds
	lib3ds_mesh_calculate_vertex_normals(const_cast<Lib3dsMesh*>(&mesh),
		pCalculatedNormals);

	// ����������� ������ ��� ������������ ��������� ���������� ������
	// � ������ ������������� �����(���������� ������ * 3)
	outputVertices.clear();
	outputVertices.reserve(mesh.nfaces * 3);
	// ������ ���������� ����� �������� ����������� �� ������������ ��������
	// (����� std::vector::resize �������� ������������� ����������� � ������
	// ��������� � �������������� ������������ �� ���������, �������
	// �������������� ������� ��� ������������)
	outputVertices.resize(mesh.nvertices);

	// ������� ������ �������� ������ (�� ������, ���� �� �������� ��������)
	outputFaces.clear();
	// ������� ����������� � ��� ����� ��� �������� ���� ������ ������������� 
	// �����, ����� ���������� � ���� ����� ��������� �� ��������� � ����������
	// ��������� ������ � ���� (��������� �����������)
	outputFaces.reserve(mesh.nfaces);

	// ������ ������� � ������� ����������� ��� ������ lib3ds ��������
	size_t calculatedNormalIndex = 0;
	// � ����� ����� ��������������� ���������� ��� ����� �����
	for (size_t faceIndex = 0; faceIndex < mesh.nfaces; ++faceIndex)
	{
		// ������ �� ������� �����
		Lib3dsFace const& face = mesh.faces[faceIndex];

		// �������� ����� ��� ��������� � ������ outputFaces, ������� ����� 
		// ��������� ��������� ������ (� ���������� ������������ ��������� 
		// ��� ���������� ����������� ������)
		MeshFace outputFace;
		// ������ ������ ���������
		outputFace.materialIndex = face.material;

		// ���� �� �������� ������ ������� �����
		for (unsigned i = 0; i < 3; ++i)
		{
			// �������� ����������� ������ ������� ������� �������
			// �� ������� ����������� ��������, ���������� ������ �������
			CVector3f calculatedNormal(
				pCalculatedNormals[calculatedNormalIndex++]);

			// �������� �������� ������ ������� ������� �������������� �����
			unsigned vertexIndex = face.index[i];

			// ���������, �� ������� �� ������ ������� �� ������� ������� ������.
			// � ���������� �������� ����� ����������, ���� ��� �����������
			// �������� ����� �� ���������
			if (vertexIndex >= mesh.nvertices)
			{
				throw std::runtime_error("Vertex index is out of vertex array");
			}

			// ���� ������ �������, ������� ������� ��������� � �����������
			// ��������� �������.
			while (true)
			{
				// �������� ���������� � ������������ �������
				CVertexInfo & vertex = outputVertices[vertexIndex];

				// ������ �� ��� ��� �������?
				if (!vertex.NormalIsDefined())	
				{
					// ������� � ����������� ������� ��� �� ������, ������, � ���
					// ����� �������� ����������� �������� �������
					vertex.SetNormal(calculatedNormal);
					// ����� ��������. ������ ������� ������� - � ����������
					// vertexIndex
					break;
				}
				else // ������� � ������� ���� ����������
				{
					// ���������� ���b������� ������ ������� � ������������.
					// �.�. ���������� �������� ���������� � ��������� ������������,
					// ���������� �������� �������� �� ��������������� ���������:
					// �� �������� �� ���������� �������� ������ ���� ������ 0.00001
					// ����� 0.00001 ������ �������� ��� ��������� ��������� ��������.
					// ������������ ������� �������� ��� ����� ���� float �������
					// ������ �� �����
					if ((calculatedNormal - vertex.GetNormal()).GetLength() < 1e-5f)
					{
						// ������� �� ����� - ����� �������
						// ������ ������� ������� ��������� � ���������� vertexIndex
						break;
					}
					else
					{	// ����������� ������� ���������� �� ������� �����������
						// �������
						
						// �������� ������ �������, ����������� �� ������
						int derivedVertexIndex = vertex.GetDerivedVertex();

						// ���� �� ������ ����������� �������?
						if (derivedVertexIndex < 0)
						{	// ����������� ������ ������ ���, � �.�. �����������
							// ������� ������� �� ����, �������� ��������
							// ����������� �������, ������� �� �� �������
							
							// ��������� ����� ������� �� ������� �������
							CVertexInfo newVertex(calculatedNormal, vertexIndex);

							// ������ *�������* ������� ����� ����� �������
							// ��������� �������. �.�. ������� ����������� � �����
							// ������� ������, ������ ����� ����� ��������
							// ���������� ������ � �������
							vertexIndex = static_cast<int>(outputVertices.size());

							// ��� ������� ������� ������ ������ ����������� 
							// (���������) �������
							vertex.DeriveVertex(vertexIndex);

							// ��������� ����� ������� � ������ ��������
							outputVertices.push_back(newVertex);

							// ������� �� ����� - ����� �������
							// ������ ������� ������� ��������� � ����������
							// vertexIndex
							break;
						}
						else
						{
							// ����������� ������� � ������� � �������� vertexIndex
							// �������. ������������� ������������� �������� �����
							// ��������� ��� ����������� �������� �� ��������� 
							// �������� ������� �����, ���� ������� � �����������
							// ��������� ������� �� ����� ������� ����� �������
							// ����������� ������, ���� ���� ������� �� ����������
							vertexIndex = derivedVertexIndex;

							// ��������� � ��������� �������� ����� ������ ������� 
							// � ����������� �������� �������
						}
					}
				}
			}	// while (true)

			// ��� ������ �� ����������� ����� while � ���������� vertexIndex 
			// ����� ���������� ������ ������� (���� ������������, ����
			// �����������)
			outputFace.vertices[i] = vertexIndex;

		}	// for (unsigned i = 0; i < 3; ++i)

		// ��������� ����� � ������ ������
		outputFaces.push_back(outputFace);
	}	// for (size_t faceIndex = 0; faceIndex < mesh.nfaces; ++faceIndex)
}

template <class VertexType>
unsigned CModelLoader::SplitVerticesAndBuildNormals(
   Lib3dsMesh const& mesh,
   std::vector<unsigned char> & vertexBufferData,
   size_t vertexBufferOffset,
   std::vector<MeshFace> & outputFaces
   )
{
	// ������� ������ ��� �������� ����������� � ��������
	// ������������� �����
	std::vector<CVertexInfo> vertices;

	// ��������� ������� � �������� ������������� �����,
	// ��� ������������� �������� ���������� ����� ������
	// � ����������� �������� ������
	SplitVerticesBySmoothGroup(mesh, vertices, outputFaces);

	size_t const numberOfVertices = vertices.size();

	// ��������� ������ ������� ������, �������� �� �������� ����������
	// ������, ������������ ����� ����������� ������
	vertexBufferData.resize(
		vertexBufferOffset + sizeof(VertexType) * numberOfVertices);

	// ��������� ����� ������������ ��������� ������ ������������� �����
	// � ������
	VertexType * outputVertices = 
		reinterpret_cast<VertexType*>(&vertexBufferData[vertexBufferOffset]);

	// ���� �� �������������� ��������, � ������� �������� ������ �
	// ����������� ��������, � ����� ����������� ������� �����
	// ����������� � �������� �����
	for (size_t vertexIndex = 0; vertexIndex < numberOfVertices; ++vertexIndex)
	{
		// ���������� � ������� �������
		CVertexInfo const & vertexInfo = vertices[vertexIndex];
		// ������ �� ������ �� ���� ������� � ��������� ������
		VertexType & outputVertex = outputVertices[vertexIndex];

		// ���� ������� �������� �����������, �� �������� � ��� ������ �� 
		// ���������� �� �������
		int originalVertexIndex = vertexInfo.GetOriginalVertexIndex();
		if (originalVertexIndex >= 0)
		{
			// �������������� (� ���������� ������������), ��� ������ ����������
			// �� ����� ������������ �������
			assert(static_cast<unsigned>(originalVertexIndex) < vertexIndex);
			// ������ �� �������, ���������� ������
			VertexType const& originalVertex = 
				outputVertices[originalVertexIndex];
			// �������� ����������� ������� � �������
			outputVertex = originalVertex;
		}

		// ���� � ������� ��� ��������� ������ �������, �� �������� ��� �
		// ��������� �����
		// � ������� ������ ������� �����, ���� ������� ������� � ������
		// ���� �� ����� ����� �����
		if (vertexInfo.NormalIsDefined())
		{
			CVector3f const& normal = vertexInfo.GetNormal();
			outputVertex.normal.x = normal.x;
			outputVertex.normal.y = normal.y;
			outputVertex.normal.z = normal.z;
		}
	}

	// ���������� ���������� ������, ���������� ����� �����������
	return numberOfVertices;
}


unsigned CModelLoader::FillVertexBufferData(
	Lib3dsMesh const& mesh, 
	std::vector<unsigned char> & vertexBufferData,
	std::vector<MeshFace> & outputFaces)
{
	const int numberOfVertices = mesh.nvertices;

	// ����� ������� ������ � 3ds-�����
	float (*pInputVertices)[3] = mesh.vertices;

	// ����� ������� ���������� ��������� � 3ds �����
	float (*pInputTexCoords)[2] = mesh.texcos;

	// �������� �� ������ ������ � 3ds �����
	size_t const vertexBufferOffset = vertexBufferData.size();

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
		}
		// ��������� ����������� ���������� � �������� � ����������� ��������
		// � ���������� ���������� ���������� ������ � �����
		return SplitVerticesAndBuildNormals<TexturedVertex>
			(mesh, vertexBufferData, vertexBufferOffset, outputFaces);
	}
	else	// ����� ��� ���������� ���������
	{
		// ����������� ������ �� ������, ���������� ��������� 
		// ��� ���������� ���������
		vertexBufferData.resize(
			vertexBufferOffset + sizeof(Vertex) * numberOfVertices);

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
		}
		// ��������� ����������� ���������� � �������� � ����������� ��������
		// � ���������� ���������� ���������� ������ � �����
		return SplitVerticesAndBuildNormals<Vertex>
			(mesh, vertexBufferData, vertexBufferOffset, outputFaces);
	}
}

void CModelLoader::LoadMaterials(
	Lib3dsFile const& file, CModel & model, std::string const& baseFolder)
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

		{
			material.SetTwoSided(pMaterial->two_sided != 0);
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

void CModelLoader::BuildMaterialFacesList(
	unsigned materialCount,
	std::vector<MeshFace> const& faces,
	std::vector< std::vector<unsigned> > & materialFaces)
{
	// �������� �������� ������ ������� ��������� 
	// (������� �������� � �������� -1, ������������
	// ��� ������, ��� ���������)
	std::vector<unsigned> materialFaceCount(materialCount + 1);

	size_t const faceCount = faces.size();

	// ��������� �� ���� ������ ������������� �����
	for (size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex)
	{
		// ������ ��������� �����
		int faceMaterial = faces[faceIndex].materialIndex;

		if (faceMaterial < -1 || 
			faceMaterial >= static_cast<int>(materialCount))
		{
			// ����������� ������ ���������
			throw std::out_of_range(
				"Face material index is out of range");
		}

		// ����������� ������� ������, ������������ ������ ��������
		// (0 ������ ������������� ���������=-1)
		++materialFaceCount[faceMaterial + 1];
	}

	// ������� ��������� ����� ���������� ����� ��������������� 
	// ������ ������ �������� ������
	materialFaces.assign(materialCount + 1, std::vector<unsigned>());

	// ����������� ����� � �������� �������� ������ ��� ��������
	// ���������� ���������� ������ (����� �������� ������������� ������)
	// ��� ����� ������� ��������
	// ���� �� 0 �� materialCount (������������, ��� ��������� ���������
	// � �������� ������ -1)
	for (size_t materialIndex = 0; 
		materialIndex <= materialCount; 
		++materialIndex)
	{
		std::vector<unsigned> & faces = materialFaces[materialIndex];
		faces.reserve(materialFaceCount[materialIndex]);
	}

	// ��������� �� ������ �����, �������� ������ �����
	// � ������ ������, ������������ ��������������� ��������
	for (size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex)
	{
		// ������ ��������� �����
		int faceMaterial = faces[faceIndex].materialIndex;

		assert(static_cast<unsigned>(faceMaterial) + 1 <= materialCount);

		// ������ �� ������ ������, ������������ ������ ��������
		std::vector<unsigned> & faces = materialFaces[faceMaterial + 1];

		// ������� � ���������� ������ ������ ������ ������� �����
		faces.push_back(faceIndex);
	}
}