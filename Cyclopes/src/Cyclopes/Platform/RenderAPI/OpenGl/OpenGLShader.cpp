#include "Cycpch.h"
#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include "OpenGLError.h"

#include "Cyclopes/Core/Log.h"
#include "Cyclopes/Core/Assert.h"

namespace cyc {

	uint32_t OpenGLShader::MakeVertexShader(const char* source)
	{
		uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &source, NULL);
		return vs;
	}

	unsigned int OpenGLShader::MakeFragmentShader(const char* source)
	{
		uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &source, NULL);
		return fs;
	}

	void OpenGLShader::CompileShaders(uint32_t vs, uint32_t fs)
	{
		glCompileShader(vs);
		CYC_CHECK_SHADER_COMPILATION(vs);

		glCompileShader(fs);
		CYC_CHECK_SHADER_COMPILATION(fs);
	}

	void OpenGLShader::CreateProgram(uint32_t vs, uint32_t fs)
	{
		m_Program = glCreateProgram();
		CYC_ASSERT_NOBREAK(m_Program != 0, "glCreateProgram failed to create a valid program");
		glAttachShader(m_Program, vs);
		glAttachShader(m_Program, fs);
	}
	
	void OpenGLShader::LinkProgram(uint32_t vs, uint32_t fs)
	{
		glLinkProgram(m_Program);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void OpenGLShader::Init(const Cycsh& source)
	{
		uint32_t vs = MakeVertexShader(source.vertex.c_str());
		uint32_t fs = MakeFragmentShader(source.fragment.c_str());

		CompileShaders(vs, fs);
		CreateProgram(vs, fs);
		LinkProgram(vs, fs);
	}

	void OpenGLShader::Bind()
	{
		glUseProgram(m_Program);
	}

	void OpenGLShader::Destroy()
	{
		glDeleteProgram(m_Program);
	}

	void OpenGLShader::SetMat4f(const CycString& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::SetInt(const CycString& name, int value)
	{
		glUniform1i(UniformLocation(name), value);
	}

	void OpenGLShader::SetIntBuffer(const CycString& name, int* value, int size)
	{
		glUniform1iv(UniformLocation(name), size, value);
	}

	GLuint OpenGLShader::UniformLocation(const CycString& name)
	{
		return glGetUniformLocation(m_Program, name.c_str());
	}

}