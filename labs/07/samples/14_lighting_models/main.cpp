#include "BaseWindow.h"
#include "GLEWInitializer.h"
#include "GLFWInitializer.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/gtc/matrix_inverse.hpp>
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
	struct LightSource
	{
		glm::vec3 pos{};
	};

	Program()
	{
		Shader vertexShader(GL_VERTEX_SHADER);
		vertexShader.SetSource(R"(
#version 430 core

layout(location = 0) uniform mat4 u_modelViewProjectionMatrix;
layout(location = 1) uniform mat4 u_modelViewMatrix;
layout(location = 2) uniform mat3 u_normalMatrix;

layout(location = 0) in vec4 a_pos;
layout(location = 1) in vec3 a_normal;

varying vec3 v_normal;
varying vec3 v_pos;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_pos;
	v_normal = u_normalMatrix * a_normal;
	v_pos = vec3(u_modelViewMatrix * a_pos);
}
)");
		vertexShader.Compile();
		if (!vertexShader.IsCompiled())
			throw std::runtime_error(vertexShader.GetInfoLog());
		m_program.AttachShader(vertexShader);

		Shader fragmentShader(GL_FRAGMENT_SHADER);
		fragmentShader.SetSource(R"(
#version 430 core

struct LightSource
{
    vec3 pos;
	vec3 intensity;
};

layout(location = 3) uniform LightSource u_light;

varying vec3 v_normal;
varying vec3 v_pos;

void main()
{
	vec3 n = normalize(v_normal);
	vec3 l = normalize(u_light.pos - v_pos);

	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * max(dot(n, l), 0.0);
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

	void SetModelViewMatrix(const glm::dmat4& m)
	{
		m_modelViewMatrix = m;
	}

	void SetProjectionMatrix(const glm::dmat4& m)
	{
		m_projectionMatrix = m;
	}

	void SetLightSource(const LightSource& light)
	{
		m_lightSource = light;
	}

	void Activate() const noexcept
	{
		glUseProgram(m_program);
		auto modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix;
		auto normalMatrix = glm::inverseTranspose(glm::dmat3{ m_modelViewMatrix });
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(glm::mat4{ modelViewProjectionMatrix }));
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(glm::mat4{ m_modelViewMatrix }));
		glUniformMatrix3fv(2, 1, GL_FALSE, glm::value_ptr(glm::mat3{ normalMatrix }));
		glUniform3fv(2, 1, glm::value_ptr(m_lightSource.pos));
	}

private:
	ShaderProgram m_program;
	glm::dmat4 m_modelViewMatrix{};
	glm::dmat4 m_projectionMatrix{};
	LightSource m_lightSource;
};

class SceneRenderer
{
public:
	struct VertexLayout
	{
		GLint position = 0;
		GLint normal = 1;
	};

	SceneRenderer()
	{
	}

	void RenderScene(const aiScene& scene)
	{
		RenderNode(scene, *scene.mRootNode);
	}

private:
	void RenderNode(const aiScene& scene, const aiNode& node)
	{
		for (unsigned int i = 0; i < node.mNumMeshes; ++i)
		{
			unsigned int meshIndex = node.mMeshes[i];
			const aiMesh* mesh = scene.mMeshes[meshIndex];
			RenderMesh(*mesh);
		}
		for (unsigned int i = 0; i < node.mNumChildren; ++i)
		{
			RenderNode(scene, *node.mChildren[i]);
		}
	}

	void RenderMesh(const aiMesh& mesh)
	{
		glBegin(GL_TRIANGLES);
		for (unsigned faceIndex = 0; faceIndex < mesh.mNumFaces; ++faceIndex)
		{
			const aiFace& face = mesh.mFaces[faceIndex];
			for (unsigned int i = 0; i < face.mNumIndices; ++i)
			{
				unsigned int vertexIndex = face.mIndices[i];
				const aiVector3D& vertex = mesh.mVertices[vertexIndex];
				const aiVector3D& normal = mesh.mNormals[vertexIndex];
				glVertexAttrib3f(m_layout.normal, normal.x, normal.y, normal.z);
				glVertex4f(vertex.x, vertex.y, vertex.z, 1.0f);
			}
		}
		glEnd();
	}

	VertexLayout m_layout;
};

} // namespace

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title, const aiScene& scene)
		: BaseWindow(w, h, title)
		, m_scene(scene)
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
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);

		m_program->SetProjectionMatrix(glm::perspective(60.0 * std::numbers::pi / 180.0, static_cast<double>(width) / height, 0.1, 100.0));
		m_program->SetModelViewMatrix(glm::lookAt(glm::dvec3{ 1.0, 2.0, 5.0 }, glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }));
		m_program->SetLightSource({ .pos = glm::vec3{ 0.0, 0.0, 0.0 } });

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(glm::value_ptr(glm::ortho(-10.0, 10.0, -5.0, 5.0, 0.1, 100.0)));

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(glm::value_ptr(glm::lookAt(glm::dvec3{ 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 })));

		m_program->Activate();

		SceneRenderer renderer;
		renderer.RenderScene(m_scene);
	}

	std::optional<GLEWInitializer> m_glewInit;
	std::optional<Program> m_program;
	glm::vec2 m_mousePos{};

	const aiScene& m_scene;
};


int main()
{
	try
	{
		Assimp::Importer importer;
		auto* scene = importer.ReadFile("res/monkey.obj",
			aiProcess_Triangulate
				| aiProcess_JoinIdenticalVertices
				| aiProcess_SortByPType);

		GLFWInitializer glfwInit;
		Window win(800, 600, "Custom attribtue variables", *scene);

		win.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
