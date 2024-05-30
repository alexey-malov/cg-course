#pragma once

class CRotationController
{
public:
	CRotationController(int windowWidth, int windowHeight);
	~CRotationController(void);
	bool LeftButtonIsPressed() const;
	void ResizeWindow(int windowWidth, int windowHeight);
	void OnMouse(int button, int state, int x, int y);
	void OnMotion(int x, int y);

private:
	static void NormalizeModelViewMatrix(void);
	static void RotateCamera(GLfloat rotateX, GLfloat rotateY);
	bool m_leftButtonPressed;
	int m_mouseX;
	int m_mouseY;
	int m_windowWidth;
	int m_windowHeight;
};
