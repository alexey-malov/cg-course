#include "stdafx.h"
#include "SceneObject.h"

CSceneObject::CSceneObject(IGeometryObject const & obj)
:m_object(obj)
,m_pShader(NULL)
{
}

CSceneObject::CSceneObject(IGeometryObject const & obj, IShader const& shader)
:m_object(obj)
,m_pShader(&shader)
{
}

