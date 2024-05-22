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
	explicit CSceneObject(std::shared_ptr<IGeometryObject const> obj);

	/*
	Конструирует объект сцены, связывая с ним шейдер
	*/
	CSceneObject(std::shared_ptr<IGeometryObject const> obj, std::shared_ptr<IShader const> shader);

	/*
	Возвращает ссылку на геометрический объект
	*/
	IGeometryObject const& GetGeometryObject() const noexcept
	{
		return *m_object;
	}

	// Есть ли шейдер у объекта сцены?
	bool HasShader() const noexcept
	{
		return m_shader != NULL;
	}

	// Возвращает связанный шейдер
	IShader const& GetShader() const noexcept
	{
		assert(HasShader());
		return *m_shader;
	}

private:
	std::shared_ptr<IGeometryObject const> m_object;
	std::shared_ptr<IShader const> m_shader;
};
