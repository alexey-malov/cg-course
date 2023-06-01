// MyApplication.cpp: implementation of the CMyApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "MyApplication.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyApplication::CMyApplication(int* pArgc, char** argv, int width, int height, const char* title)
	: CApplication(pArgc, argv, width, height, title)
	, m_vertexShader(NULL)
	, m_program(NULL)
	, m_phase(0)
	, m_tickCount(GetTickCount64())
	, m_phaseUniformLocation(0)
	, m_amplitudeUniformLocation(0)
	, m_frequencyUniformLocation(0)

{
}

CMyApplication::~CMyApplication()
{
	// deinitialize shaders
	Deinitialize();
}

void CMyApplication::Init()
{
	InitGLEW();

	CreateVertexShader();

	SetShaderSource();

	CompileShader();

	CreateProgram();

	AttachShaders();

	LinkProgram();

	ValidateProgram();

	InitUniforms();

	EnableIdleHandler();
}

void CMyApplication::OnDraw()
{
	ULONGLONG currentTick = GetTickCount64();
	int timeDelta = int(currentTick - m_tickCount);
	m_tickCount = (ULONGLONG)currentTick;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_phase += (timeDelta * 0.001f) * 3;
	if (m_phase >= 2 * M_PI)
	{
		m_phase -= 2 * M_PI;
	}

	glUseProgramObjectARB(m_program);

	glUniform1fARB(m_amplitudeUniformLocation, 0.3f);
	glUniform1fARB(m_phaseUniformLocation, (GLfloat)m_phase);
	glUniform1fARB(m_frequencyUniformLocation, (GLfloat)M_PI);

	glBegin(GL_LINE_STRIP);
	{
		for (double y = -1; y <= 1.05; y += 0.01)
		{
			glVertex3f(0, (GLfloat)y, 0);
		}
	}
	glEnd();

	glUseProgramObjectARB(NULL);

	glutSwapBuffers();
}

void CMyApplication::OnIdle()
{
	glutPostRedisplay();
}

void CMyApplication::InitGLEW()
{
	std::cout <<"Checking for supported OpenGL extensions\n";
	glewInit();

	// load shaders
	if (!GLEW_ARB_shader_objects)
	{
		throw std::runtime_error("Shader objects are not supported");
	}
	std::cout << "Ok\n";
}

void CMyApplication::CreateVertexShader()
{
	// create vertex shader
	std::cout << "Creating shader...\n";
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER_ARB);
	if (!m_vertexShader)
	{
		throw std::runtime_error("Failed to create vertex shader");
	}
	std::cout << "Ok\n";
}

void CMyApplication::SetShaderSource()
{
	std::cout << "Setting shader source\n";
	GLcharARB shaderSource[] = "uniform float phase;				\n"
							   "uniform float amplitude;			\n"
							   "uniform float frequency;				\n"
							   "void main()						\n"
							   "{									\n"
							   "	vec4 v = gl_Vertex;	\n"
							   "										\n"
							   "	v.x += amplitude * sin(frequency * v.y + phase);	\n"
							   "	gl_Position = gl_ModelViewProjectionMatrix * v;		\n"
							   "	gl_FrontColor = gl_Color;		\n"
							   "}\n";

	GLcharARB const* pShaderSource = shaderSource;
	GLcharARB const** ppShaderSource = &pShaderSource;

	// set shader source code
	glShaderSourceARB(m_vertexShader, 1, ppShaderSource, NULL);
	std::cout << "Ok\n";
}

void CMyApplication::CompileShader()
{
	std::cout << "Compiling shader...\n";
	glCompileShaderARB(m_vertexShader);
	GLint compileStatus;
	glGetObjectParameterivARB(m_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
		throw std::runtime_error(GetInfoLog(m_vertexShader).c_str());
	}
	std::cout << "Ok\n";
}

void CMyApplication::CreateProgram()
{
	std::cout << "Creating program object\n";
	m_program = glCreateProgramObjectARB();
	if (!m_program)
	{
		throw std::runtime_error("Failed to create program");
	}
	std::cout << "Ok\n";
}

void CMyApplication::AttachShaders()
{
	std::cout << "Attaching vertex shader to the program\n";
	glAttachObjectARB(m_program, m_vertexShader);
	std::cout << "Ok\n";
}

void CMyApplication::LinkProgram()
{
	std::cout << "Linking program\n";
	glLinkProgramARB(m_program);
	GLint linkStatus;
	glGetObjectParameterivARB(m_program, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		throw std::runtime_error(GetInfoLog(m_program));
	}
	std::cout << "Ok\n";
}

void CMyApplication::ValidateProgram()
{
	std::cout << "Validating program\n";
	glValidateProgramARB(m_program);
	GLint validationStatus;
	glGetObjectParameterivARB(m_program, GL_OBJECT_VALIDATE_STATUS_ARB, &validationStatus);
	if (validationStatus != GL_TRUE)
	{
		throw std::runtime_error(GetInfoLog(m_program));
	}
	std::cout << "Ok\n";
}

void CMyApplication::InitUniforms()
{
	std::cout << "Getting uniform variables locations\n";
	m_phaseUniformLocation = glGetUniformLocationARB(m_program, "phase");
	if (m_phaseUniformLocation < 0)
	{
		throw std::runtime_error("Cannot get 'phase' uniform variable location");
	}

	m_amplitudeUniformLocation = glGetUniformLocationARB(m_program, "amplitude");
	if (m_amplitudeUniformLocation < 0)
	{
		throw std::runtime_error("Cannot get 'amplitude' uniform variable location");
	}

	m_frequencyUniformLocation = glGetUniformLocationARB(m_program, "frequency");
	if (m_frequencyUniformLocation < 0)
	{
		throw std::runtime_error("Cannot get 'frequency' uniform variable location");
	}

	std::cout << "Ok\n";
}

std::string CMyApplication::GetInfoLog(GLhandleARB object)
{
	GLcharARB buffer[10000];
	GLsizei length;
	glGetInfoLogARB(object, sizeof(buffer) - 1, &length, buffer);
	return std::string(buffer, length);
}

void CMyApplication::Deinitialize()
{
	if (m_vertexShader)
	{
		glDeleteObjectARB(m_vertexShader);
	}

	if (!m_program)
	{
		glDeleteObjectARB(m_program);
	}
}
