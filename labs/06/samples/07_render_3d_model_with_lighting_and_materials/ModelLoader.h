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

	// Выполняем загрузку 3ds файла, заполняя переданную модель
	void Load3dsFile(const char * fileName, CModel & model);

	// игнорировать ошибки при загрузке текстур?
	void IgnoreMissingTextures(bool ignoreMissingTextures);

	// Установить режим использования вершинного буфера модели
	void SetVertexBufferUsage(GLenum vertexBufferUsage);

	// Установить режим использования индексного буфера модели
	void SetIndexBufferUsage(GLenum indexBufferUsage);

private:
	// Информация о грани полигональной сетки, 
	// используемая при расщеплении вершин
	struct MeshFace;

	// Информация о вершине полигональной сетки, используемая
	// при расщеплении вершин
	class CVertexInfo;

	// Загружаем материалы
	void LoadMaterials(
		Lib3dsFile const& file, 
		CModel & model, 
		std::string const& baseFolder = "");

	// Загружаем текстуры, связанные с материалом
	void LoadMaterialTextures(
		Lib3dsMaterial const& materialInfo, 
		CModel & model, 
		CModelMaterial & material, 
		std::string const& baseFolder = "");

	// Загружаем текстуру и добавляем ее в модель
	GLuint LoadTexture(
		std::string const & name, 
		CModel & model, 
		std::string const& baseFolder = "");

	// Инициализируем текстурную карту данными из .3ds файла
	void InitTextureMap(
		Lib3dsTextureMap const & textureMapInfo, 
		CTextureMap & textureMap);

	// Загружаем полигональные сетки
	void LoadMeshes(Lib3dsFile const& file, CModel & model);

	// Добавляем к модели полигональную сетку и заполняем
	// переданные массивы вершин и индексов данными из 3ds файла
	static void LoadMesh(
		unsigned materialCount,
		Lib3dsMesh const& mesh,
		CModel & model, 
		std::vector<unsigned char> & vertexBufferData,
		std::vector<unsigned char> & indexBufferData
		);

	// Заполняем массив вершин данными из .3ds файла
	// вычисляя нормали к вершинам, порождая новые вершины
	// и обновляя информацию о гранях полигональной сетки
	// Возвращаем количество вершин в результирующей сетке
	static unsigned FillVertexBufferData(
		Lib3dsMesh const& mesh, 
		std::vector<unsigned char> & vertexBufferData,
		std::vector<MeshFace> & outputFaces
		);

	// Заполняем массив индексов вершин
	// В качестве шаблонного параметра IndexType принимается
	// целочисленный тип, используемых для хранения индексов.
	// Возвращаем смещение к индексам сетки внутри
	// массива indexBufferData
	template <typename IndexType>
	static unsigned FillIndexBufferData(
		std::vector<MeshFace> const& faces, 
		std::vector< std::vector<unsigned> > const & materialFaces,
		std::vector<unsigned char> & indexBufferData
		);

	// Расщепляем вершины полигональной сетки с учетом
	// групп сглаживания ее граней и формируем новый массив
	// с информацией о вершинах, а также
	// обновленную информацию о гранях сетки после 
	// расщепления
	static void SplitVerticesBySmoothGroup(
		Lib3dsMesh const& mesh,
		std::vector<CVertexInfo>& outputVertices,
		std::vector<MeshFace> & outputFaces
		);

	// Заносит в вершинный буфера информацию о нормалях,
	// а также о дополнительных вершинах, получившихся в результате
	// расщепления оригинальных вершин с учетом групп сглаживания
	// Параметры:
	//	mesh - оригинальная сетка
	//	vertexBufferData - массив, содержащий данные для буфера вершин
	//	vertexBufferOffset - смещение в массиве к началу информации 
	//		о вершинах данной полигональной сетки
	//	outputFaces - выходной массив с обновленной информацией
	//		о гранях
	//	Возвращаемое значение:
	//		количество вершин в результирующей сетке
	template <class VertexType>
	static unsigned SplitVerticesAndBuildNormals(
		Lib3dsMesh const& mesh,
		std::vector<unsigned char> & vertexBufferData,
		size_t vertexBufferOffset,
		std::vector<MeshFace> & outputFaces
		);

	// Выполняем построение списков граней, использующих
	// сгруппированных по индексу материала
	static void BuildMaterialFacesList(
		unsigned meshMaterialCount,
		std::vector<MeshFace> const& faces,
		std::vector< std::vector<unsigned> > & materialFaces);

private:
	// Режим использования вершинного буфера
	GLenum m_vertexBufferUsage;

	// Режим использования буфера индексов
	GLenum m_indexBufferUsage;

	// Игнорировать ошибки загрузки текстур?
	bool m_ignoreMissingTextures;
};
