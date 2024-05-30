#include "StdAfx.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "TextureLoader.h"
#include "TextureMap.h"
#include "BoundingBox.h"

// Класс для автоматического освобождения ресурсов, связанных с .3ds файлом
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

// Установить режим использования вершинного буфера модели
void CModelLoader::SetVertexBufferUsage(GLenum vertexBufferUsage)
{
	m_vertexBufferUsage = vertexBufferUsage;
}

// Установить режим использования индексного буфера модели
void CModelLoader::SetIndexBufferUsage(GLenum indexBufferUsage)
{
	m_indexBufferUsage = indexBufferUsage;
}

void CModelLoader::Load3dsFile(const char * fileName, CModel & model)
{
	// Открываем файл
	CFile3ds file(fileName);

	// Опеределяем путь к каталогу с .3ds файлом.
	// Он понадобится для поиска текстур
	std::string filePath = fileName;
	// позиция косой черты (прямой, либо обратной)
	size_t slashPos = filePath.find_last_of("/\\");

	// fileFolder будет содержать либо пустую строку, либо каталог,
	// содержащий 3ds файл
	std::string fileFolder = 
		(slashPos == std::string::npos) ? "" :
		filePath.substr(0, slashPos + 1);

	// Загружаем материалы
	LoadMaterials(file.GetFile(), model, fileFolder);

	// Загружаем полигональные сетки
	LoadMeshes(file.GetFile(), model);
}

void CModelLoader::LoadMeshes(Lib3dsFile const& file, CModel & model)
{
	// Временные массивы вершин и индексов, 
	// которые будут заполнены данными всех сеток 3ds файла
	std::vector<unsigned char> vertexBufferData;
	std::vector<unsigned short> indexBufferData;

	const int meshCount = file.nmeshes;

	for (int i = 0; i < meshCount; ++i)
	{
		Lib3dsMesh const & mesh = *file.meshes[i];

		// Добавляем данные полигональной сетки из 3ds файла
		// к текущей модели, а информацию о вершинах и индексах
		// добавляем в массивы вершин и индексов
		LoadMesh(mesh, model, vertexBufferData, indexBufferData);
	}

	// Создаем вершинный буфер
	model.GetVertexBuffer().Create();
	// и заполняем его данными о вершинах, собранными со всех 
	// полигональных сеток модели
	model.GetVertexBuffer().BufferData(
		vertexBufferData.size() * sizeof(vertexBufferData[0]), 
		&vertexBufferData[0], 
		m_vertexBufferUsage);

	// Создаем буфер индексов
	model.GetIndexBuffer().Create();
	// и заполняем его данными об индексах вершин, составляющих грани,
	// собранными со всех полигональных сеток модели
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
	// Вычисляем смещение в буфере вершин текущей полигональной сетки
	const unsigned int vertexBufferOffset = static_cast<unsigned>(
		sizeof(unsigned char) * vertexBufferData.size());
	// Вычисляем смещение в буфере индексом текущей полигональной сетки
	const unsigned int indexBufferOffset = static_cast<unsigned>(
		sizeof(unsigned short) * indexBufferData.size());

	// Заполняем вершинный массив данными из .3ds файла
	FillVertexBufferData(mesh, vertexBufferData);
	// Заполняем массив индексов данными из .3ds файла
	FillIndexBufferData(mesh, indexBufferData);

	// Вычисляем ограничивающий блок текущей полигональной сетки
	// при помощи средств библиотеки lib3ds
	float minMeshBound[3];
	float maxMeshBound[3];
	lib3ds_mesh_bounding_box(
		const_cast<Lib3dsMesh*>(&mesh), 
		minMeshBound, maxMeshBound);

	// Создаем Bounding box на основе данных, возвращенных lib3ds
	CBoundingBox meshBoundingBox((CVector3f(minMeshBound)), (CVector3f(maxMeshBound)));

	// Добавляем к модели полигональную сетку
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

	// Резервируем в массиве место для хранения вершин N граней
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

	// адрес массива вершин в 3ds-файле
	float (*pInputVertices)[3] = mesh.vertices;

	// адрес массива текстурных координат в 3ds файле
	float (*pInputTexCoords)[2] = mesh.texcos;

	// смещение до начала данных в 3ds файле
	size_t vertexBufferOffset = vertexBufferData.size();

	// проверяем, есть ли у сетки текстурные координаты
	if (pInputTexCoords != NULL)
	{
		// сетка с текстурными координатами

		// увеличиваем массив на размер, занимаемых вершинами 
		// с текстурными координатами
		vertexBufferData.resize(
			vertexBufferOffset + sizeof(TexturedVertex) * numberOfVertices);

		// задаем адрес расположения вершинных данных полигональной сетки
		TexturedVertex * outputVertices = 
			reinterpret_cast<TexturedVertex*>(
				&vertexBufferData[vertexBufferOffset]);

		// Заполняем массив данными о вершинах в формате структуры TexturedVertex
		for (int v = 0; v < numberOfVertices; ++v)
		{
			float * inputPosition = pInputVertices[v];
			float * inputTexCoord = pInputTexCoords[v];

			TexturedVertex & outputVertex = outputVertices[v];
			Vector3 & outputPosition = outputVertex.position;
			// задаем координаты вершины в пространстве
			outputPosition.x = inputPosition[0];
			outputPosition.y = inputPosition[1];
			outputPosition.z = inputPosition[2];

			// задаем текстурные координаты вершины
			outputVertex.texCoord.x = inputTexCoord[0];
			outputVertex.texCoord.y = inputTexCoord[1];

			// TODO: нужно вычислить значение вектора нормали
			Vector3 & outputNormal = outputVertex.normal;
			outputNormal.x = outputNormal.y = outputNormal.z = 0;
		}
	}
	else
	{
		// сетка без текстурных координат

		// увеличиваем массив на размер, занимаемых вершинами 
		// без текстурных координат
		vertexBufferData.resize(vertexBufferOffset + sizeof(Vertex) * numberOfVertices);

		// задаем адрес расположения вершинных данных полигональной сетки
		Vertex * outputVertices = 
			reinterpret_cast<Vertex*>(&vertexBufferData[vertexBufferOffset]);

		// Заполняем массив данными о вершинах в формате структуры Vertex
		for (int v = 0; v < numberOfVertices; ++v)
		{
			float * inputPosition = pInputVertices[v];

			Vertex & outputVertex = outputVertices[v];
			Vector3 & outputPosition = outputVertex.position;
			// задаем координаты вершины в пространстве
			outputPosition.x = inputPosition[0];
			outputPosition.y = inputPosition[1];
			outputPosition.z = inputPosition[2];

			// TODO: вычислить значение вектора нормали
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

		// Добавляем новый материал к модели
		CModelMaterial & material = model.AddMaterial();
		// и получаем связанное с этим материлом описание
		CMaterial & materialInfo = material.GetMaterial();

		// Задаем фоновый цвет материала
		{
			const float * ambient = pMaterial->ambient;
			materialInfo.SetAmbient(ambient[0],ambient[1], ambient[2]);
		}

		// Задаем диффузный цвет материала
		{
			const float * diffuse = pMaterial->diffuse;
			materialInfo.SetDiffuse(diffuse[0], diffuse[1], diffuse[2]);
		}

		// Задаем зеркальный цвет материала и степень блеска
		{
			const float * specular = pMaterial->specular;
			materialInfo.SetSpecular(specular[0], specular[1], specular[2]);
			materialInfo.SetShininess(pMaterial->shininess);
		}

		// Загружаем текстуры материала
		LoadMaterialTextures(*pMaterial, model, material, baseFolder);
	}
}

void CModelLoader::LoadMaterialTextures(
	Lib3dsMaterial const& materialInfo, 
	CModel & model, 
	CModelMaterial & material, 
	std::string const& baseFolder)
{
	// Пытаемся загрузить текстурную карту №1
	{
		Lib3dsTextureMap const & tex1 = materialInfo.texture1_map;

		// Проверяем, есть ли имя у первой текстуры?
		if (*tex1.name)
		{
			try
			{
				// Загружаем текстуру
				CTexture2DHandle texture1 = 
					LoadTexture(tex1.name, model, baseFolder);
				// Добавляем текстурную карту
				CTextureMap & textureMap1 = material.AddTextureMap1(texture1);
				// Наполняем ее сведениями из .3ds файла
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
	// Добавляем текстуру с заданным именем к модели
	CTexture2D & texture = model.AddTextureImage(name);
	// Нам вернут либо ссылку на существующий текстурный объект,
	// либо ссылку на вновь созданный

	if (!texture)	// Если для текстуры еще не задано текстурное изображение
	{
		CTextureLoader loader;

		std::string textureFilePath = baseFolder + name;

		// Загружаем текстурное изображение и присоединяем его к текстуре
		// Из-за простейшего перевода имени из string в wstring 
		// корректно загружаться будут только файлы, в пути которых не содержатся 
		// символы за пределами кодовой таблицы ASCII
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

