#include "StdAfx.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "TextureLoader.h"
#include "TextureMap.h"
#include "BoundingBox.h"

// Информация о грани полигональной сетки
struct CModelLoader::MeshFace
{
	unsigned vertices[3];
	int materialIndex;
};

// Класс хранящий информацию о вершине, 
// ее прототипе, порожденных вершинах и нормали
class CModelLoader::CVertexInfo
{
public:
	// Конструктор оригинальной вершины
	CVertexInfo()
		:m_originalVertexIndex(-1)
		,m_derivedVertexIndex(-1)
		,m_normalIsDefined(false)
	{}

	// Конструктор порожденной вершины
	CVertexInfo(CVector3f const& normal, int originalVertexIndex)
		:m_originalVertexIndex(originalVertexIndex)
		,m_derivedVertexIndex(-1)
		,m_normalIsDefined(true)
		,m_normal(normal)
	{
		assert(originalVertexIndex >= 0);
	}

	// Доступ к вектору нормали
	CVector3f const& GetNormal()const
	{
		assert(NormalIsDefined());
		return m_normal;
	}

	// Позволяет задать нормаль для вершины
	void SetNormal(CVector3f const& normal)
	{
		assert(!m_normalIsDefined);
		m_normal = normal;
		m_normalIsDefined = true;
	}


	// Была ли задана нормаль к грани?
	bool NormalIsDefined()const
	{
		return m_normalIsDefined;
	}

	// Индекс оригинальной вершины, породившей данную
	// Отрицательный индекс оригинальной вершины имеют оригинальные вершины
	// полигональной сетки
	int GetOriginalVertexIndex()const
	{
		return m_originalVertexIndex;
	}

	// Возвращает индекс следующей порожденной вершины в списке вершин,
	// либо -1, если текущая вершина не имеет порожденных вершин
	int GetDerivedVertex()const
	{
		return m_derivedVertexIndex;
	}

	// Задает индекс унаследованной вершины
	void DeriveVertex(int vertexIndex)
	{
		assert(vertexIndex >= 0);
		assert(m_derivedVertexIndex == -1);
		m_derivedVertexIndex = vertexIndex;
	}

private:
	int m_originalVertexIndex;	// индекс базовой вершины
	int m_derivedVertexIndex;	// индекс порожденной вершины
	bool m_normalIsDefined;		// задан ли вектор нормали?
	CVector3f m_normal;
};


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
	std::vector<unsigned char> indexBufferData;

	const int meshCount = file.nmeshes;

	for (int i = 0; i < meshCount; ++i)
	{
		Lib3dsMesh const & mesh = *file.meshes[i];

		// Добавляем данные полигональной сетки из 3ds файла
		// к текущей модели, а информацию о вершинах и индексах
		// добавляем в массивы вершин и индексов
		LoadMesh(file.nmaterials, mesh, model, vertexBufferData, indexBufferData);
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
	unsigned materialCount,
	Lib3dsMesh const& mesh, 
	CModel & model, 
	std::vector<unsigned char> & vertexBufferData,
	std::vector<unsigned char> & indexBufferData
	)
{
	// Вычисляем смещение в буфере вершин текущей полигональной сетки
	const unsigned int vertexBufferOffset = 
		static_cast<unsigned>(sizeof(unsigned char) * vertexBufferData.size());

	// Обновленный массив граней
	std::vector<MeshFace> updatedFaces;

	// Заполняем массив вершин, выполняя при необходимости 
	// их расщепление
	unsigned const numberOfVertices = 
		FillVertexBufferData(mesh, vertexBufferData, updatedFaces);

	// Тип, используемый для хранения индексов вершин
	GLenum indexType = 0;

	// Инициализируем смещение к данным полигональной сетки
	// в буфере индексов
	unsigned int indexBufferOffset = 0;

	// Строим списки граней, сгруппированные по используемому
	// индексу материала
	std::vector< std::vector<unsigned> > materialFaces;
	BuildMaterialFacesList(materialCount, updatedFaces, materialFaces);

	// В зависимости от количества вершин после расщепления
	// сохраняем индексы в виде 8, 16 или 32 битных чисел
	if (numberOfVertices <= UCHAR_MAX + 1)	// достаточно 8 бит?
	{
		indexType = GL_UNSIGNED_BYTE;
		indexBufferOffset = FillIndexBufferData<GLubyte>(
			updatedFaces, materialFaces, indexBufferData);
	}
	else if (numberOfVertices <= USHRT_MAX + 1) // достаточно 16 бит?
	{
		indexType = GL_UNSIGNED_SHORT;
		indexBufferOffset = FillIndexBufferData<GLushort>(
			updatedFaces, materialFaces, indexBufferData);
	}
	else	// Используем 32 битные индексы
	{
		indexType = GL_UNSIGNED_INT;
		indexBufferOffset = FillIndexBufferData<GLuint>(
			updatedFaces, materialFaces, indexBufferData);
	}

	// Вычисляем ограничивающий блок текущей полигональной сетки
	// при помощи средств библиотеки lib3ds
	float minMeshBound[3];
	float maxMeshBound[3];
	lib3ds_mesh_bounding_box(
		const_cast<Lib3dsMesh*>(&mesh), 
		minMeshBound, maxMeshBound);

	// Создаем Bounding box на основе данных, возвращенных lib3ds
	CBoundingBox meshBoundingBox(
		(CVector3f(minMeshBound)), 
		(CVector3f(maxMeshBound)));

	// Добавляем к модели полигональную сетку
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

	// Порядковый номер индекса самой первой подсетки равен 0
	unsigned submeshStartIndex = 0;

	// Добавляем в сетук информацию о содержащихся в ней подсетках
	// и материалах
	for (unsigned materialIndex = 0; materialIndex <= materialCount; 
		++materialIndex)
	{
		// Массив граней, использующих материал materialIndex
		std::vector<unsigned> const& subMeshFaces = 
			materialFaces[materialIndex];

		// количество граней, имеющих материал materialIndex
		size_t const subMeshFaceCount = subMeshFaces.size();

		// Если подсетка не пустая, то добавляем ее в виде подсетки
		if (subMeshFaceCount != 0)
		{
			// Т.к. грани треугольный, количество индексов в 3 раза больше
			// количества граней
			unsigned const subMeshIndexCount = static_cast<unsigned>(subMeshFaceCount * 3);

			// Добавляем подсетку
			unsigned subMeshIndex = addedMesh.AddSubMesh(
				submeshStartIndex, subMeshIndexCount);

			// Задаем в сетке связь между текущим индексом материала и 
			// добавленной подсеткой
			addedMesh.SetMaterialSubMesh(
				static_cast<int>(materialIndex) - 1, subMeshIndex);

			// вычисляем порядковый номер индекса следующей подсетки
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
	// количество граней
	const unsigned numberOfFaces = static_cast<unsigned>(faces.size());

	// размер (в байтах), требуемых для хранения одного индекса
	unsigned indexSize = sizeof(IndexType);

	// Проверка на то, что indexSize - степень двойки
	assert((indexSize & (indexSize - 1)) == 0);

	// Размер данных, занимаемый всеми индексами полигональной сетки
	unsigned meshIndexDataSize = numberOfFaces * 3 * indexSize;

	// Вычисляем смещение в буфере индексов таким образом, чтобы индексы
	// сетки находились по смещению, кратному размеру индекса
	// (для ускорения доступа к данным)
	// 8-битные индексы будут выровнены по границе байтов
	// 16-битные индексы - по двухбайтной границе
	// 32-битные индексы - по смещению, кратному 4
	unsigned const indexBufferOffset = static_cast<unsigned>(
		((indexBufferData.size() + indexSize - 1) / indexSize) * indexSize);

	// Увеличиваем размер буфера индексов так, чтобы по смещению
	// indexBufferOffset разместить данные размером meshIndexDataSize
	indexBufferData.resize(indexBufferOffset + meshIndexDataSize);

	// Указатель на позицию в массиве индексов, соответствующей началу
	// данных текущей полигональной сетки
	IndexType * pOutputIndex = 
		reinterpret_cast<IndexType*>(&indexBufferData[indexBufferOffset]);

	// Из-за конфликтов между макросом max из windows.h 
	// и методом std::numeric_limits::max() придется использовать
	// следующий способ опеределения максимального значения беззнакового
	// целочисленного типа IndexType
	IndexType const maxIndexValue = IndexType(~0);
	// Дополнительная проверка на то, что для хранения индексов
	// используется беззнаковый тип
	assert(maxIndexValue > 0);

	size_t const materialCount = materialFaces.size();
	// Пробегаемся по списку материалов
	for (
		size_t materialIndex = 0; 
		materialIndex < materialCount; 
		++materialIndex)
	{
		// Массив граней, входящих в состав подсетки с текущим индексом
		// материала
		std::vector<unsigned> const& subMeshFaces = 
			materialFaces[materialIndex];

		size_t const subMeshFaceCount = subMeshFaces.size();

		// Пробегаем по массиву граней текущей подсетки
		for (unsigned i = 0; i < subMeshFaceCount; ++i)
		{
			// индекс грани подсетки
			unsigned faceIndex = subMeshFaces[i];

			MeshFace const& inputFace = faces[faceIndex];

			// Пробегаем по индексам текущей грани
			for (unsigned j = 0; j < 3; ++j)
			{
				// Получаем индекс текущей вершины грани
				unsigned vertexIndex = inputFace.vertices[j];

				// проверяем, что индекс вершины может быть представлены
				// при помощи типа IndexType
				if (vertexIndex > (unsigned)maxIndexValue)
				{
					throw std::logic_error("Vertex index is out of range");
				}

				// все нормально, сохраняем индекс вершины в массив индексов
				// и переходим к следующей вершине
				*pOutputIndex++ = static_cast<IndexType>(vertexIndex);
			}
		}
	}

	// Возвращаем смещение в массиве индексов, по которому располагаются
	// индексы данной полигональной сетки
	return indexBufferOffset;
}

void CModelLoader::SplitVerticesBySmoothGroup(
   Lib3dsMesh const& mesh,
   std::vector<CVertexInfo>& outputVertices,
   std::vector<MeshFace> & outputFaces
	)
{
	// Массив для хранения нормалей вершин всех граней полигональной сетки
	std::vector<float> faceVertexNormals(mesh.nfaces * 3 * 3);

	// Функция lib3ds_mesh_calculate_vertex_normals вычисляет нормаль каждой 
	// вершины каждой треугольной грани сетки с учетом групп сглаживания грани 
	// и прилегающих к ней граней
	// Для ее вызова нам пришлось добавить немного "черной магии":
	//	а) В качестве первого параметра функция принимает указатель Lib3dsMesh*
	//		вместо Lib3dsMesh const*, хотя не модифицирует переданную
	//		полигональную сетку. Судя по всему, это недоработка автора библиотеки.
	//		Придется снять константность с указателя с использованием const_cast
	//	б) В качестве второго параметра функция принимает указатель
	//		на первый элемент массива трехкомпонентных массивов 
	//		float-ов (float[][3]).
	//		Поскольку контейнер faceVertexNormals хранит элементы типа float, для 
	//		преобразования указателя float* к нужному типу float(*)[3] 
	//		воспользуемся оператором reinterpret_cast
	float (*pCalculatedNormals)[3] = 
		reinterpret_cast<float(*)[3]>(&faceVertexNormals[0]);
	// Выполняем вычисление нормалей в вершинах при помощи lib3ds
	lib3ds_mesh_calculate_vertex_normals(const_cast<Lib3dsMesh*>(&mesh),
		pCalculatedNormals);

	// резервируем память под максимальное возможное количество вершин
	// в данной полигональной сетке(количество граней * 3)
	outputVertices.clear();
	outputVertices.reserve(mesh.nfaces * 3);
	// Массив изначально будет заполнен информацией об оригинальных вершинах
	// (метод std::vector::resize выполнит инициализацию добавленных в массив
	// элементов с использованием конструктора по умолчанию, который
	// инициализирует вершину как оригинальную)
	outputVertices.resize(mesh.nvertices);

	// Очищаем массив выходных вершин (на случай, если он окажется непустым)
	outputFaces.clear();
	// Заранее резервируем в нем место для хранения всех граней полигональной 
	// сетки, чтобы добавление в него новых элементов не приводило к повторному
	// выделению памяти в куче (небольшая оптимизация)
	outputFaces.reserve(mesh.nfaces);

	// Индекс нормали в массиве вычисленных при помощи lib3ds нормалей
	size_t calculatedNormalIndex = 0;
	// В цикле будут последовательно обработаны все грани сетки
	for (size_t faceIndex = 0; faceIndex < mesh.nfaces; ++faceIndex)
	{
		// Ссылка на текущую грань
		Lib3dsFace const& face = mesh.faces[faceIndex];

		// Выходная грань для помещения в массив outputFaces, которая будет 
		// заполнена индексами вершин (с изменением оригинальной нумерации 
		// при добавлении порожденных вершин)
		MeshFace outputFace;
		// Задаем индекс материала
		outputFace.materialIndex = face.material;

		// цикл по индексам вершин текущей грани
		for (unsigned i = 0; i < 3; ++i)
		{
			// Получаем вычисленный вектор нормали текущей вершины
			// из массива вычисленных нормалей, увеличивая индекс нормали
			CVector3f calculatedNormal(
				pCalculatedNormals[calculatedNormalIndex++]);

			// Получаем исходный индекс текущей вершины обрабатываемой грани
			unsigned vertexIndex = face.index[i];

			// Проверяем, не выходит ли индекс вершины за пределы массива вершин.
			// В нормальной ситуации такое невозможно, хотя при повреждении
			// входного файла не исключено
			if (vertexIndex >= mesh.nvertices)
			{
				throw std::runtime_error("Vertex index is out of vertex array");
			}

			// Цикл поиска вершины, нормаль которой совпадает с вычисленным
			// значением нормали.
			while (true)
			{
				// Получаем информацию о существующей вершине
				CVertexInfo & vertex = outputVertices[vertexIndex];

				// задана ли для нее нормаль?
				if (!vertex.NormalIsDefined())	
				{
					// Нормаль в проверяемой вершине еще не задана, значит, в нее
					// нужно записать вычисленное значение нормали
					vertex.SetNormal(calculatedNormal);
					// Поиск завершен. Индекс искомой вершины - в переменной
					// vertexIndex
					break;
				}
				else // нормаль в вершине была определена
				{
					// сравниваем вычbсленный вектор нормали с существующим.
					// Т.к. вычисление нормалей происходит с некоторой погрешностью,
					// используем проверку векторов на приблизительное равенство:
					// их разность по абсолютной величине должна быть меньше 0.00001
					// Число 0.00001 вполне подходит для сравнения единичных векторов.
					// Использовать бОльшую точность для чисел типа float особого
					// смысла не имеет
					if ((calculatedNormal - vertex.GetNormal()).GetLength() < 1e-5f)
					{
						// выходим из цикла - поиск окончен
						// Индекс искомой вершины находится в переменной vertexIndex
						break;
					}
					else
					{	// вычисленная нормаль отличается от нормали проверяемой
						// вершины
						
						// Получаем индекс вершины, порожденной от данной
						int derivedVertexIndex = vertex.GetDerivedVertex();

						// Есть ли вообще порожденная вершина?
						if (derivedVertexIndex < 0)
						{	// Порожденных вершин больше нет, а т.к. вычисленная
							// нормаль найдена не была, придется добавить
							// порожденную вершину, породив ее от текущей
							
							// Порождаем новую вершину от текущей вершины
							CVertexInfo newVertex(calculatedNormal, vertexIndex);

							// Индекс *искомой* вершины будет равен индексу
							// созданной вершины. Т.к. вершины добавляются в конец
							// массива вершин, индекс будет равен текущему
							// количеству вершин в массиве
							vertexIndex = static_cast<int>(outputVertices.size());

							// Для текущей вершины задаем индекс порожденной 
							// (найденной) вершины
							vertex.DeriveVertex(vertexIndex);

							// добавляем новую вершину в массив векторов
							outputVertices.push_back(newVertex);

							// выходим из цикла - поиск окончен
							// Индекс искомой вершины находится в переменной
							// vertexIndex
							break;
						}
						else
						{
							// порожденная вершина у вершины с индексом vertexIndex
							// имеется. Следовательно вышеописанные действия нужно
							// проделать над порожденной вершиной на следующей 
							// итерации данного цикла, пока вершина с вычисленным
							// значением нормали не будет найдена среди цепочки
							// порожденных вершин, либо пока цепочка не закончится
							vertexIndex = derivedVertexIndex;

							// переходим к следующей итерации цикла поиска вершины 
							// с совпадающем вектором нормали
						}
					}
				}
			}	// while (true)

			// При выходе из предыдущего цикла while в переменной vertexIndex 
			// будет находиться индекс вершины (либо существующей, либо
			// порожденной)
			outputFace.vertices[i] = vertexIndex;

		}	// for (unsigned i = 0; i < 3; ++i)

		// добавляем грань в массив граней
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
	// Создаем массив для хранения информациеи о вершинах
	// полигональной сетки
	std::vector<CVertexInfo> vertices;

	// Вычисляем нормали к вершинам полигональной сетки,
	// при необходимости выполняя порождение новых вершин
	// с обновлением индексов граней
	SplitVerticesBySmoothGroup(mesh, vertices, outputFaces);

	size_t const numberOfVertices = vertices.size();

	// Обновляем размер массива вершин, принимая во внимание количество
	// вершин, получившееся после расщепления вершин
	vertexBufferData.resize(
		vertexBufferOffset + sizeof(VertexType) * numberOfVertices);

	// вычисляем адрес расположения вершинных данных полигональной сетки
	// в буфере
	VertexType * outputVertices = 
		reinterpret_cast<VertexType*>(&vertexBufferData[vertexBufferOffset]);

	// Цикл по результирующим вершинам, в котором значение данные о
	// порожденных вершинах, а также вычисленные нормали будут
	// скопированы в выходной буфер
	for (size_t vertexIndex = 0; vertexIndex < numberOfVertices; ++vertexIndex)
	{
		// информация о текущей вершине
		CVertexInfo const & vertexInfo = vertices[vertexIndex];
		// ссылка на данные об этой вершине в вершинном буфере
		VertexType & outputVertex = outputVertices[vertexIndex];

		// Если вершина является порожденной, то копируем в нее данные из 
		// породившей ее вершины
		int originalVertexIndex = vertexInfo.GetOriginalVertexIndex();
		if (originalVertexIndex >= 0)
		{
			// удостоверяемся (в отладочной конфигурации), что данные копируются
			// из ранее обработанной вершины
			assert(static_cast<unsigned>(originalVertexIndex) < vertexIndex);
			// ссылка на вершину, породившую данную
			VertexType const& originalVertex = 
				outputVertices[originalVertexIndex];
			// копируем порождающую вершину в текущую
			outputVertex = originalVertex;
		}

		// Если у вершины был определен вектор нормали, то копируем его в
		// вершинный буфер
		// У вершины вектор нормали задан, если вершина входила в состав
		// хотя бы одной грани сетки
		if (vertexInfo.NormalIsDefined())
		{
			CVector3f const& normal = vertexInfo.GetNormal();
			outputVertex.normal.x = normal.x;
			outputVertex.normal.y = normal.y;
			outputVertex.normal.z = normal.z;
		}
	}

	// Возвращаем количество вершин, полученных после расщепления
	return static_cast<unsigned>(numberOfVertices);
}


unsigned CModelLoader::FillVertexBufferData(
	Lib3dsMesh const& mesh, 
	std::vector<unsigned char> & vertexBufferData,
	std::vector<MeshFace> & outputFaces)
{
	const int numberOfVertices = mesh.nvertices;

	// адрес массива вершин в 3ds-файле
	float (*pInputVertices)[3] = mesh.vertices;

	// адрес массива текстурных координат в 3ds файле
	float (*pInputTexCoords)[2] = mesh.texcos;

	// смещение до начала данных в 3ds файле
	size_t const vertexBufferOffset = vertexBufferData.size();

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
		}
		// Добавляем недостающую информацию о нормалях и порожденных вершинах
		// и возвращаем количество полученных вершин в сетке
		return SplitVerticesAndBuildNormals<TexturedVertex>
			(mesh, vertexBufferData, vertexBufferOffset, outputFaces);
	}
	else	// сетка без текстурных координат
	{
		// увеличиваем массив на размер, занимаемых вершинами 
		// без текстурных координат
		vertexBufferData.resize(
			vertexBufferOffset + sizeof(Vertex) * numberOfVertices);

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
		}
		// Добавляем недостающую информацию о нормалях и порожденных вершинах
		// и возвращаем количество полученных вершин в сетке
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

		{
			material.SetTwoSided(pMaterial->two_sided != 0);
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

void CModelLoader::BuildMaterialFacesList(
	unsigned materialCount,
	std::vector<MeshFace> const& faces,
	std::vector< std::vector<unsigned> > & materialFaces)
{
	// обнуляем счетчики граней каждого материала 
	// (включая материал с индексом -1, используемый
	// для граней, без материала)
	std::vector<unsigned> materialFaceCount(materialCount + 1);

	unsigned const faceCount = static_cast<unsigned>(faces.size());

	// Пробегаем по всем граням полигональной сетки
	for (size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex)
	{
		// индекс материала грани
		int faceMaterial = faces[faceIndex].materialIndex;

		if (faceMaterial < -1 || 
			faceMaterial >= static_cast<int>(materialCount))
		{
			// некорректый индекс материала
			throw std::out_of_range(
				"Face material index is out of range");
		}

		// Увеличиваем счетчик граней, использующих данный материал
		// (0 индекс соответствует материалу=-1)
		++materialFaceCount[faceMaterial + 1];
	}

	// Каждому материалу сетки изначально будет соответствовать 
	// пустой массив индексов граней
	materialFaces.assign(materialCount + 1, std::vector<unsigned>());

	// Резервируем место в массивах индексов граней для хранения
	// требуемого количества граней (чтобы избежать перевыделения памяти)
	// при росте размера массивов
	// Цикл от 0 до materialCount (включительно, для обработки материала
	// с индексом равным -1)
	for (size_t materialIndex = 0; 
		materialIndex <= materialCount; 
		++materialIndex)
	{
		std::vector<unsigned> & faces = materialFaces[materialIndex];
		faces.reserve(materialFaceCount[materialIndex]);
	}

	// пробегаем по граням сетки, добавляя индекс грани
	// в массив граней, использующих соответствующий материал
	for (unsigned faceIndex = 0; faceIndex < faceCount; ++faceIndex)
	{
		// индекс материала грани
		int faceMaterial = faces[faceIndex].materialIndex;

		assert(static_cast<unsigned>(faceMaterial) + 1 <= materialCount);

		// ссылка на массив граней, использующих данный материал
		std::vector<unsigned> & faces = materialFaces[faceMaterial + 1];

		// заносим в полученный массив граней индекс текущей грани
		faces.push_back(faceIndex);
	}
}