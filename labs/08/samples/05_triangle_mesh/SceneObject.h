#pragma once

class IGeometryObject;
class IShader;

/*
Объект сцены - геометрический объект, с которым связан некоторый шейдер,
выполняющий расчет цвета
*/
class CSceneObject
{
public:
	/*
	Конструирует объект сцены, не имеющий шейдер
	*/
	CSceneObject(IGeometryObject const & obj);

	/*
	Конструирует объект сцены, связывая с ним шейдер
	*/
	CSceneObject(IGeometryObject const & obj, IShader const& shader);

	/*
	Возвращает ссылку на геометрический объект
	*/
	IGeometryObject const& GetGeometryObject()const throw()
	{
		return m_object;
	}

	// Есть ли шейдер у объекта сцены?
	bool HasShader()const throw()
	{
		return m_pShader != NULL;
	}

	// Возвращает связанный шейдер
	IShader const& GetShader()const throw()
	{
		assert(HasShader());
		return *m_pShader;
	}

private:
	IGeometryObject const & m_object;
	IShader const * m_pShader;
};
