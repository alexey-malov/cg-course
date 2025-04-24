#include "BaseWindow.h"
#include "GLEWInitializer.h"
#include "GLFWInitializer.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>

namespace
{
class Program
{
public:
	Program()
	{
		Shader vertexShader(GL_VERTEX_SHADER);
		vertexShader.SetSource(R"(
void main()
{
	gl_Position = gl_Vertex;
}
)");
		vertexShader.Compile();
		if (!vertexShader.IsCompiled())
			throw std::runtime_error(vertexShader.GetInfoLog());
		m_program.AttachShader(vertexShader);

		Shader fragmentShader(GL_FRAGMENT_SHADER);
		fragmentShader.SetSource(R"(
// layout qualifier requires OpenGL 4.3
#version 430 core

layout (location = 0) uniform vec2 u_center;
layout (location = 1) uniform vec4 u_circleColor;
layout (location = 2) uniform vec4 u_bgColor;
layout (location = 3) uniform float u_radius;

void main()
{
	vec2 fragToMousePos = u_center - gl_FragCoord.xy;
	float dist = length(fragToMousePos);
	gl_FragColor = dist < u_radius ? u_circleColor  : u_bgColor;
}
)");
		fragmentShader.Compile();
		if (!fragmentShader.IsCompiled())
			throw std::runtime_error(fragmentShader.GetInfoLog());
		m_program.AttachShader(fragmentShader);
		m_program.Link();
		if (!m_program.IsLinked())
			throw std::runtime_error(m_program.GetInfoLog());
		m_program.Validate();
		if (!m_program.IsValid())
			throw std::runtime_error(m_program.GetInfoLog());
	}

	void SetCenter(glm::vec2 mousePos)
	{
		m_center = mousePos;
	}

	void SetCircleColor(glm::vec4 circleColor)
	{
		m_circleColor = circleColor;
	}

	void SetBgColor(glm::vec4 bgColor)
	{
		m_bgColor = bgColor;
	}

	void SetRadius(float radius)
	{
		m_radius = radius;
	}

	void Activate() const
	{
		glUseProgram(m_program);
		glUniform2fv(0, 1, glm::value_ptr(m_center));
		glUniform4fv(1, 1, glm::value_ptr(m_circleColor));
		glUniform4fv(2, 1, glm::value_ptr(m_bgColor));
		glUniform1f(3, m_radius);
	}

private:
	ShaderProgram m_program;

	glm::vec2 m_center{};
	glm::vec4 m_circleColor{};
	glm::vec4 m_bgColor{};
	float m_radius = 1.0f;
};

} // namespace

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title)
		: BaseWindow(w, h, title)
	{
	}

private:
	void OnRunStart() override
	{
		// Инициализируем GLEW
		m_glewInit.emplace();
		m_program.emplace();
	}

	void OnResize(int width, int height) override
	{
		glViewport(0, 0, width, height);
	}

	void OnMouseMove(double x, double y) override
	{
		m_mousePos = glm::vec2{ x, y };
		glfwPostEmptyEvent();
	}

	void Draw(int /*width*/, int height) override
	{
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_program->SetCenter({ m_mousePos.x, height - m_mousePos.y });
		m_program->SetBgColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		m_program->SetCircleColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		m_program->SetRadius(10.0f);
		m_program->Activate();

		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);
		glEnd();
	}

	std::optional<GLEWInitializer> m_glewInit;
	std::optional<Program> m_program;
	glm::vec2 m_mousePos{};
};

int main()
{
	try
	{
		GLFWInitializer glfwInit;
		Window win(800, 600, "Custom uniform variable");

		win.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
