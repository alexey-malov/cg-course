#pragma once

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
		Lib3dsMesh const& mesh, 
		CModel & model, 
		std::vector<unsigned char> & vertexBufferData,
		std::vector<unsigned short> & indexBufferData
		);

	// Заполняем массив вершин данными из .3ds файла
	static void FillVertexBufferData(
		Lib3dsMesh const& mesh, 
		std::vector<unsigned char> & vertexBufferData);

	// Заполняем массив индексов данными из .3ds файла
	static void FillIndexBufferData(
		Lib3dsMesh const& mesh, 
		std::vector<unsigned short> & indexBufferData);
private:
	// Режим использования вершинного буфера
	GLenum m_vertexBufferUsage;

	// Режим использования буфера индексов
	GLenum m_indexBufferUsage;

	// Игнорировать ошибки загрузки текстур?
	bool m_ignoreMissingTextures;
};
