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
	/* Работа с полигональными сетками                                      */
	/************************************************************************/

	// Получаем полигональную сетку, входящую в модель по ее индексу
	CMesh const& GetMesh(size_t index)const;
	CMesh & GetMesh(size_t index);

	// Получаем количество полигональных сеток в модели
	size_t GetMeshCount()const;

	// Добавляем полигональную сетку к модели
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
	/* Работа с ограничивающим блоком модели                                */
	/************************************************************************/

	// Получаем bounding box, ограничивающий трехмерную модель
	CBoundingBox const GetBoundingBox()const;

	/************************************************************************/
	/* Работа с текстурами модели                                           */
	/************************************************************************/

	// Добавляем текстурное изображение
	CTexture2D & AddTextureImage(std::string const& name);

	// Возвращаем количество текстур
	size_t GetTexturesCount()const;

	// Возвращаем имя текстурного изображения по его индексу
	std::string GetTextureName(size_t index)const;

	// Есть ли текстура с данным именем?
	bool HasTexture(std::string const& name)const;

	// Возвращаем текстурный объект по его имени
	CTexture2D & GetTextureByName(std::string const& name);
	CTexture2D const & GetTextureByName(std::string const& name)const;

	// Возвращаем текстурный объект по его индексу
	CTexture2D & GetTexture(size_t index);
	CTexture2D const & GetTexture(size_t index)const;

	/************************************************************************/
	/* Работа с материалами модели                                          */
	/************************************************************************/
	
	// Добавляем материал
	CModelMaterial & AddMaterial();

	// Получаем количество материалов
	size_t GetMeterialCount()const;

	// Получаем материал по его индексу
	CModelMaterial const & GetMaterial(size_t index)const;
	CModelMaterial & GetMaterial(size_t index);

	/************************************************************************/
	/* Работа с буферами вершин и индексов                                  */
	/************************************************************************/

	// Получаем вершинный буфер
	CVertexBuffer & GetVertexBuffer();
	CVertexBuffer const& GetVertexBuffer()const;

	// Получаем индексный буфер
	CIndexBuffer & GetIndexBuffer();
	CIndexBuffer const & GetIndexBuffer()const;
private:
	// Функтор для сравнения имен текстур без учета регистра символов
	class CompareTextureNames
	{
	public:
		bool operator()(
			std::string const& textureName1, 
			std::string const& textureName2)const;
	};
private:
	// Вершинный и индексный буферы
	CVertexBuffer	m_vertexBuffer;
	CIndexBuffer	m_indexBuffer;

	// Материалы модели
	typedef boost::shared_ptr<CModelMaterial> CModelMaterialPtr;
	std::vector<CModelMaterialPtr> m_materials;

	// Текстурные изображения, используемые моделью
	typedef boost::shared_ptr<CTexture2D> CTexture2DPtr;
	typedef std::map<
		std::string, 
		CTexture2DPtr, 
		CompareTextureNames> CTextures;
	typedef std::vector<std::string> CTextureNames;
	CTextures m_textures;	// текстуры модели (имя->текстура)
	CTextureNames m_textureNames; // массив текстурных имен

	// полигональные сетки модели
	typedef boost::shared_ptr<CMesh> CMeshPtr;
	std::vector<CMeshPtr> m_meshes;

	// Флаг, сигнализирующий о необходимости перевычисления
	// ограничивающего блока модели
	mutable bool m_boundingBoxMustBeUpdated;
	// Ограничивающий блок модели
	mutable CBoundingBox m_boundingBox;
};
