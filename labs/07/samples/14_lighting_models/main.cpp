#include "BaseWindow.h"
#include "GLEWInitializer.h"
#include "GLFWInitializer.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "TextureLoader.h"
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <numbers>
#include <optional>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/orthonormalize.hpp>

namespace
{

// Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m)
{
	// Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
	const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{ m });
	// Заменяем 3 столбца исходной матрицы
	return {
		glm::dvec4{ normalizedMatrix[0], 0.0 },
		glm::dvec4{ normalizedMatrix[1], 0.0 },
		glm::dvec4{ normalizedMatrix[2], 0.0 },
		m[3]
	};
}

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
layout(location = 1) uniform mat4 u_modelMatrix;
layout(location = 2) uniform mat3 u_normalMatrix; // inverse transpose of u_modelMatrix

layout(location = 0) in vec4 a_pos;
layout(location = 1) in vec3 a_normal;

varying vec3 v_worldPos;
varying vec3 v_worldNormal;

void main()
{
	v_worldPos = (u_modelMatrix * a_pos).xyz;
	v_worldNormal = u_normalMatrix * a_normal;

	gl_Position = u_modelViewProjectionMatrix * a_pos;
}
)");
		vertexShader.Compile();
		if (!vertexShader.IsCompiled())
			throw std::runtime_error(vertexShader.GetInfoLog());
		m_program.AttachShader(vertexShader);

		Shader fragmentShader(GL_FRAGMENT_SHADER);
		fragmentShader.SetSource(R"(
#version 430 core
layout (location = 3) uniform vec3 u_worldCamera;
layout (location = 4) uniform samplerCube u_envMap; 

varying vec3 v_worldPos;
varying vec3 v_worldNormal;

const vec3 WorldLightPos = vec3(10.0, 10.0, 10.0);
const float ambientFactor = 0.2;

void main()
{
	vec3 worldLightDir = normalize(WorldLightPos - v_worldPos);
	vec3 worldNormal = normalize(v_worldNormal);

	vec3 reflected = reflect(v_worldPos - u_worldCamera, worldNormal);
	vec4 color = textureCube(u_envMap, reflected);
	
	float diffuseFactor = max(0.0, dot(worldNormal, worldLightDir));
	gl_FragColor = (diffuseFactor + ambientFactor) * color;
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

	void SetModelMatrix(const glm::dmat4& m)
	{
		m_modelMatrix = m;
	}

	void SetViewMatrix(const glm::dmat4& m)
	{
		m_viewMatrix = m;
	}

	void SetProjectionMatrix(const glm::dmat4& m)
	{
		m_projectionMatrix = m;
	}

	void SetEnvCubeTextureUnit(GLint cubeTextureIndex)
	{
		m_envCubeTextureIndex = cubeTextureIndex;
	}

	void Activate() const noexcept
	{
		glUseProgram(m_program);

		const auto modelViewProj = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(glm::mat4{ modelViewProj }));

		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(glm::mat4({ m_modelMatrix })));

		const auto normalMatrix = glm::inverseTranspose(glm::dmat3{ m_modelMatrix });
		glUniformMatrix3fv(2, 1, GL_FALSE, glm::value_ptr(glm::mat3{ normalMatrix }));

		const auto cameraPos = glm::vec3{ glm::inverse(m_viewMatrix)[3] };
		glUniform3fv(3, 1, glm::value_ptr(cameraPos));

		assert(m_envCubeTextureIndex >= 0);
		glUniform1i(4, m_envCubeTextureIndex);
	}

private:
	ShaderProgram m_program;
	glm::dmat4 m_modelMatrix{ 1.0 };
	glm::dmat4 m_viewMatrix{ 1.0 };
	glm::dmat4 m_projectionMatrix{ 1.0 };
	GLint m_envCubeTextureIndex = -1;
};

class SceneRenderer
{
public:
	struct VertexLayout
	{
		GLint position = 0;
		GLint normal = 1;
	};

	SceneRenderer(VertexLayout layout = {})
		: m_layout{ layout }
	{
	}

	void RenderScene(const aiScene& scene)
	{
		RenderNode(scene, *scene.mRootNode);
	}

private:
	VertexLayout m_layout;

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
};

using CubeTextureFiles = std::array<std::filesystem::path, 6>;

Texture LoadCubeMap(const CubeTextureFiles& files)
{
	Texture tex;
	tex.Create();
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	TextureLoader loader;

	for (int i = 0; i < 6; ++i)
	{
		loader.LoadTexture2D(files[i], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
	}

	glGenerateTextureMipmap(tex);
	auto errorCode = glGetError();
	assert(errorCode == GL_NO_ERROR);

	return tex;
}

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

		m_cubeTexture = LoadCubeMap({
			"res/brightday2_positive_x.png",
			"res/brightday2_negative_x.png",
			"res/brightday2_positive_y.png",
			"res/brightday2_negative_y.png",
			"res/brightday2_positive_z.png",
			"res/brightday2_negative_z.png",
		});
	}

	void OnResize(int width, int height) override
	{
		glViewport(0, 0, width, height);
	}

	void OnMouseMove(double x, double y) override
	{
		const glm::dvec2 mousePos{ x, y };
		if (m_leftButtonPressed)
		{
			const auto windowSize = GetFramebufferSize();

			const auto mouseDelta = mousePos - m_mousePos;
			const double xAngle = mouseDelta.y * std::numbers::pi / windowSize.y;
			const double yAngle = mouseDelta.x * std::numbers::pi / windowSize.x;
			RotateCamera(xAngle, yAngle);
		}
		m_mousePos = mousePos;
		glfwPostEmptyEvent();
	}

	void RotateCamera(double xAngleRadians, double yAngleRadians)
	{
		// Извлекаем из 1 и 2 строки матрицы камеры направления осей вращения,
		// совпадающих с экранными осями X и Y.
		// Строго говоря, для этого надо извлекать столбцы их обратной матрицы камеры, но так как
		// матрица камеры ортонормированная, достаточно транспонировать её подматрицу 3*3
		const glm::dvec3 xAxis{
			m_cameraMatrix[0][0], m_cameraMatrix[1][0], m_cameraMatrix[2][0]
		};
		const glm::dvec3 yAxis{
			m_cameraMatrix[0][1], m_cameraMatrix[1][1], m_cameraMatrix[2][1]
		};
		m_cameraMatrix = glm::rotate(m_cameraMatrix, xAngleRadians, xAxis);
		m_cameraMatrix = glm::rotate(m_cameraMatrix, yAngleRadians, yAxis);

		m_cameraMatrix = Orthonormalize(m_cameraMatrix);
	}

	void OnMouseButton(int button, int action, int mods) override
	{
		if (button == GLFW_MOUSE_BUTTON_1)
		{
			m_leftButtonPressed = (action & GLFW_PRESS) != 0;
		}
	}

	void Draw(int width, int height) override
	{
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);

		m_program->SetProjectionMatrix(glm::perspective(60.0 * std::numbers::pi / 180.0, static_cast<double>(width) / height, 0.1, 100.0));
		m_program->SetViewMatrix(m_cameraMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTexture);
		m_program->SetEnvCubeTextureUnit(0);

		m_program->Activate();

		SceneRenderer renderer;
		renderer.RenderScene(m_scene);
	}

	std::optional<GLEWInitializer> m_glewInit;
	std::optional<Program> m_program;

	Texture m_cubeTexture;

	glm::dvec2 m_mousePos{};
	bool m_leftButtonPressed = false;
	glm::dmat4x4 m_cameraMatrix = glm::lookAt(
		glm::dvec3{ 0.0, 0.0, 3.0 },
		glm::dvec3{ 0.0, 0.0, 0.0 },
		glm::dvec3{ 0.0, 1.0, 0.0 });

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
		Window win(800, 600, "Reflection", *scene);

		win.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
