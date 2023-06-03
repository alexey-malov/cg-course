#pragma once

class CAnimationController
{
public:
	CAnimationController();
	// ��������� ����� �����������
	void Reset();
	// ������ ����� ������� �������� � ������ ������� �����
	void Tick();
	// ���������� ���������� ������� (� ��), ��������� � �������
	// ����������� �����
	ULONGLONG GetTimeDelta() const;

private:
	ULONGLONG m_lastTick;
	ULONGLONG m_delta;
};
