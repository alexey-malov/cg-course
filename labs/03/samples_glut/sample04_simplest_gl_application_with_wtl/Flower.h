#pragma once

class CFlower
{
public:
	CFlower(float minRadius, float maxRadius, int petals, COLORREF centerColor, COLORREF petalColor);
	void Draw()const;
	~CFlower(void);
	CFlower(CFlower const& f);
private:
	// запрещаем присваивание объектов CFlower
	CFlower& operator=(CFlower const&);
	mutable GLuint m_displayList;
	float m_minRadius;
	float m_maxRadius;
	int m_petals;
	COLORREF m_centerColor;
	COLORREF m_petalColor;
};
