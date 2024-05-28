#include "StdAfx.h"
#include "BoundingBox.h"

CBoundingBox::CBoundingBox()
:m_isEmpty(true)
{
}

CBoundingBox::CBoundingBox(CVector3f const& minCoord, CVector3f const& maxCoord)
:m_minCoord(minCoord)
,m_maxCoord(maxCoord)
,m_isEmpty(false)
{
}

CVector3f const & CBoundingBox::GetMinCoord()const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return m_minCoord;
}

CVector3f const & CBoundingBox::GetMaxCoord()const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return m_maxCoord;
}

CVector3f const CBoundingBox::GetSize()const
{
	if (m_isEmpty)
	{
		return CVector3f(0, 0, 0);
	}
	return m_maxCoord - m_minCoord;
}

CVector3f const CBoundingBox::GetCenter()const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return (m_minCoord + m_maxCoord) * 0.5f;
}

CBoundingBox const CBoundingBox::Union(CBoundingBox const& other)const
{
	if (m_isEmpty && other.m_isEmpty)	// оба блока пусты
	{
		return CBoundingBox();
	}
	else if (m_isEmpty)	// пустой только текущий
	{
		return other;
	}
	else if (other.m_isEmpty)	// текущий - непустой
	{
		return *this;
	}
	else	// объединяем два непустых блока
	{
		using namespace std;
		return CBoundingBox(
			CVector3f(
				min(m_minCoord.x, other.m_minCoord.x), 
				min(m_minCoord.y, other.m_minCoord.y), 
				min(m_minCoord.z, other.m_minCoord.z)
				),
			CVector3f(
				max(m_maxCoord.x, other.m_maxCoord.x), 
				max(m_maxCoord.y, other.m_maxCoord.y), 
				max(m_maxCoord.z, other.m_maxCoord.z)
			)
		);
	}
}

bool CBoundingBox::IsEmpty()const
{
	return m_isEmpty;
}
