#pragma once

#include "Vector3.h"

/*
����� "�������������� ����"
*/
class CBoundingBox
{
public:
	// ������� ������ ����
	CBoundingBox();

	// ������� ���� �� ���� ������, �������� �����������
	// � ������������ ����������
	CBoundingBox(CVector3f const& minCoord, CVector3f const& maxCoord);

	// �������� � ���, �������� �� ���� ������
	bool IsEmpty()const;

	// ���������� ��������� ������������ ���� �������������� ������
	CBoundingBox const Union(CBoundingBox const& other)const;

	// ���������� ����������� � ������������ ����������
	CVector3f const & GetMinCoord()const;
	CVector3f const & GetMaxCoord()const;

	// ���������� ������ �����: (min - max) 
	CVector3f const GetSize()const;

	// ���������� ���������� ������ �����
	CVector3f const GetCenter()const;
private:
	bool	m_isEmpty;
	CVector3f m_minCoord;
	CVector3f m_maxCoord;
};
