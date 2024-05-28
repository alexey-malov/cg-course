#include "StdAfx.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "TextureMap.h"
#include "Mesh.h"


bool CModel::CompareTextureNames::operator()(std::string const& textureName1, std::string const& textureName2)const
{
	// Переводим имя первой текстуры к нижнему регистру
	std::string name1LowerCase(textureName1);
	std::transform(
		name1LowerCase.begin(),
		name1LowerCase.end(),
		name1LowerCase.begin(),
		tolower);

	// Переводим имя второй текстуры к нижнему регистру
	std::string name2LowerCase(textureName2);
	std::transform(
		name2LowerCase.begin(),
		name2LowerCase.end(),
		name2LowerCase.begin(),
		tolower);

	// Возвращаем результат сравнения имен в нижнем регистре
	return name1LowerCase < name2LowerCase;
}

CModel::CModel(void)
:m_boundingBoxMustBeUpdated(true)
{
}

CModelMaterial & CModel::AddMaterial()
{
	CModelMaterialPtr pMaterial(new CModelMaterial());
	m_materials.push_back(pMaterial);
	return *pMaterial;
}

size_t CModel::GetMeterialCount()const
{
	return m_materials.size();
}

CModelMaterial const & CModel::GetMaterial(size_t index)const
{
	return *m_materials.at(index);
}

CModelMaterial & CModel::GetMaterial(size_t index)
{
	return *m_materials.at(index);
}

CTexture2D & CModel::AddTextureImage(std::string const& name)
{
	// Ищем, есть ли текстура с таким именем
	CTextures::iterator it = m_textures.find(name);
	if (it != m_textures.end())
	{
		// Если есть, возвращаем ссылку на существующую
		return *it->second;
	}

	// В противном случае создаем новую текстуру
	CTexture2DPtr pTexture(new CTexture2D());
	// и добавляем ее в контейнер текстур,
	m_textures.insert(CTextures::value_type(name, pTexture));
	// а ее имя в вектор имен
	m_textureNames.push_back(name);

	// возвращаем ссылку на добавленную текстуру
	return *pTexture;
}

bool CModel::HasTexture(std::string const& name)const
{
	CTextures::const_iterator it = m_textures.find(name);
	return it != m_textures.end();
}

size_t CModel::GetTexturesCount()const
{
	return m_textures.size();
}

std::string CModel::GetTextureName(size_t index)const
{
	return m_textureNames.at(index);
}

// Возвращаем текстурный объект по его имени
CTexture2D & CModel::GetTextureByName(std::string const& name)
{
	CTextures::iterator it = m_textures.find(name);
	if (it == m_textures.end())
	{
		throw std::logic_error("Texture with the specified name does not exist");
	}
	return *it->second;
}

// Возвращаем текстурный объект по его имени
CTexture2D const & CModel::GetTextureByName(std::string const& name)const
{
	CTextures::const_iterator it = m_textures.find(name);
	if (it == m_textures.end())
	{
		throw std::logic_error("Texture with the specified name does not exist");
	}
	return *it->second;
}

// Возвращаем текстурный объект по его индексу
CTexture2D & CModel::GetTexture(size_t index)
{
	std::string textureName = m_textureNames.at(index);
	return GetTextureByName(textureName);
}

// Возвращаем текстурный объект по его индексу
CTexture2D const & CModel::GetTexture(size_t index)const
{
	std::string textureName = m_textureNames.at(index);
	return GetTextureByName(textureName);
}

CMesh & CModel::AddMesh(unsigned int vertexBufferOffset, 
						unsigned int indexBufferOffset, 
						unsigned vertexCount, 
						unsigned indexCount, 
						bool hasTexture,
						CBoundingBox const& boundingBox,
						GLenum primitiveType,
						GLenum indexType
						)
{
	CMeshPtr pMesh(new CMesh(
		vertexBufferOffset, 
		indexBufferOffset, 
		vertexCount, 
		indexCount, 
		hasTexture,
		boundingBox,
		primitiveType,
		indexType
		));
	m_meshes.push_back(pMesh);

	// При добавлении полигональной сетки старый bounding box 
	// может оказаться неактуальным, поэтому делаем пометку о необходимости
	// его обновления
	m_boundingBoxMustBeUpdated = true;

	return *pMesh;
}

size_t CModel::GetMeshCount()const
{
	return m_meshes.size();
}

CMesh const& CModel::GetMesh(size_t index)const
{
	return *m_meshes.at(index);
}

CMesh & CModel::GetMesh(size_t index)
{
	return *m_meshes.at(index);
}

CVertexBuffer & CModel::GetVertexBuffer()
{
	return m_vertexBuffer;
}

CVertexBuffer const& CModel::GetVertexBuffer()const
{
	return m_vertexBuffer;
}

CIndexBuffer & CModel::GetIndexBuffer()
{
	return m_indexBuffer;
}

CIndexBuffer const & CModel::GetIndexBuffer()const
{
	return m_indexBuffer;
}

CBoundingBox const CModel::GetBoundingBox()const
{
	if (m_boundingBoxMustBeUpdated)
	{
		// Ограничивающий блок модели равен объединению ограничивающих блоков
		// всех сеток, входящих в состав модели
		CBoundingBox box;
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			box = box.Union(m_meshes[i]->GetBoundingBox());
		}
		m_boundingBox = box;

		// ограничивающий блок теперь актуален
		m_boundingBoxMustBeUpdated = false;
	}

	return m_boundingBox;
}
