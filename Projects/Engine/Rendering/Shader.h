#pragma once

#include <stdint.h>

namespace Engine
{
	class ShaderResource;

	class Shader
	{
	public:
		struct Descriptor
		{
			ShaderResource* Vertex = nullptr;
			ShaderResource* Pixel = nullptr;
			ShaderResource* Geometry = nullptr;
			ShaderResource* Compute = nullptr;
			ShaderResource* TessControl = nullptr;
			ShaderResource* TessEvaluation = nullptr;
		};

		Shader(const Descriptor& descriptor)
		{
			m_Descriptor = descriptor;
		}

		virtual ~Shader() {}

		uint32_t GetHandle() const { return m_Handle; }
		bool IsLinked() const { return m_Linked; }

	protected:
		Descriptor m_Descriptor;
		uint32_t m_Handle = UINT_MAX;

		bool m_Linked = false;
	};
}