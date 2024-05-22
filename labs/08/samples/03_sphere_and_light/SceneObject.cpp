#include "stdafx.h"
#include "SceneObject.h"

CSceneObject::CSceneObject(std::shared_ptr<IGeometryObject const> obj)
	: m_object(obj)
	, m_shader(NULL)
{
}

CSceneObject::CSceneObject(std::shared_ptr<IGeometryObject const> obj, std::shared_ptr<IShader const> shader)
	: m_object(std::move(obj))
	, m_shader(std::move(shader))
{
}

