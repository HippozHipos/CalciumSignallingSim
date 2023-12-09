#pragma once

#include <string>
#include <glad/glad.h>
#include "Cyclopes/Renderer/Shader.h"
#include "Cyclopes/Renderer/Cycsh.h"

namespace cyc {

	class OpenGLShader : public Shader
	{
	public:
		void Init(const Cycsh& source) override;
		void Bind() override;
		void Destroy() override;

	private:
		uint32_t MakeVertexShader(const char* source);
		uint32_t MakeFragmentShader(const char* source);
		void CompileShaders(uint32_t vs, uint32_t fs);
		void CreateProgram(uint32_t vs, uint32_t fs);
		void LinkProgram(uint32_t vs, uint32_t fs);

	public:
		void SetMat4f(const CycString& name, const glm::mat4& value) override;
		void SetInt(const CycString& name, int value) override;
		void SetIntBuffer(const CycString& name, int* value, int size) override;

	private:
		GLuint UniformLocation(const CycString& name);

	private:
		uint32_t m_Program = 0;
	};
}
