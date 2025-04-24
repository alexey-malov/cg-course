#include "BaseWindow.h"
#include "GLEWInitializer.h"
#include "GLFWInitializer.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <numbers>
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
attribute vec2 a_amplitude;
attribute vec2 a_phase0;
attribute vec2 a_frequency;
uniform float u_time;

void main()
{
	gl_Position = vec4(
		vec2(gl_Vertex) + a_amplitude * sin(u_time * a_frequency + a_phase0),
		gl_Vertex.zw);
}
)");
		vertexShader.Compile();
		if (!vertexShader.IsCompiled())
			throw std::runtime_error(vertexShader.GetInfoLog());
		m_program.AttachShader(vertexShader);

		Shader fragmentShader(GL_FRAGMENT_SHADER);
		fragmentShader.SetSource(R"(
void main()
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
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
		m_timeLocation = m_program.GetUniformLocation("u_time");
		m_phase0Location = m_program.GetAttributeLocation("a_phase0");
		m_frequencyLocation = m_program.GetAttributeLocation("a_frequency");
		m_amplitudeLocation = m_program.GetAttributeLocation("a_amplitude");
	}

	void SetTime(float time)
	{
		m_time = time;
	}

	void Activate()
	{
		glUseProgram(m_program);
		glUniform1f(m_timeLocation, m_time);
	}

	GLint GetPhase0Location() const noexcept
	{
		return m_phase0Location;
	}

	GLint GetFrequencyLocation() const noexcept
	{
		return m_frequencyLocation;
	}

	GLint GetAmplitudeLocation() const noexcept
	{
		return m_amplitudeLocation;
	}

private:
	ShaderProgram m_program;
	float m_time = 0.0f;
	GLint m_timeLocation = 0;
	GLint m_phase0Location = 0;
	GLint m_frequencyLocation = 0;
	GLint m_amplitudeLocation = 0;
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

	void Draw(int width, int height) override
	{
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_program->SetTime(static_cast<float>(glfwGetTime()));
		m_program->Activate();
		glEnable(GL_POINT_SMOOTH);
		glPointSize(10.0f);

		glBegin(GL_POINTS);
		glVertexAttrib2fv(m_program->GetPhase0Location(), glm::value_ptr(glm::vec2(0.1f, 0.3f)));
		glVertexAttrib2fv(m_program->GetFrequencyLocation(), glm::value_ptr(glm::vec2(4.3f, 6.5f)));
		glVertexAttrib2fv(m_program->GetAmplitudeLocation(), glm::value_ptr(glm::vec2(0.1f, 0.1f)));
		glVertex2f(m_mousePos.x / width * 2 - 1, (height - m_mousePos.y) * 2 / height - 1);

		glVertexAttrib2fv(m_program->GetPhase0Location(), glm::value_ptr(glm::vec2(0.2f, 0.3f)));
		glVertexAttrib2fv(m_program->GetFrequencyLocation(), glm::value_ptr(glm::vec2(4.2f, 3.2f)));
		glVertexAttrib2fv(m_program->GetAmplitudeLocation(), glm::value_ptr(glm::vec2(0.2f, 0.3f)));
		glVertex2f(0, 0);
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
		Window win(800, 600, "Custom attribtue variables");

		win.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
