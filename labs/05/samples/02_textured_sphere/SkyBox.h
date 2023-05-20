#pragma once

#include "TexturedCube.h"

class CSkyBox
{
public:
	CSkyBox(
		std::wstring const& leftTexture,
		std::wstring const& rightTexture,
		std::wstring const& downTexture,
		std::wstring const& upTexture,
		std::wstring const& backTexture,
		std::wstring const& frontTexture);
	void Draw() const;

private:
	mutable bool m_initialized;
	mutable CTexturedCube m_cube;
	std::wstring m_leftTexture;
	std::wstring m_rightTexture;
	std::wstring m_downTexture;
	std::wstring m_upTexture;
	std::wstring m_backTexture;
	std::wstring m_frontTexture;
};
