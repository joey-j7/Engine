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
			std::string Vertex = "";
			std::string Pixel = "";
			std::string Geometry = "";
			std::string Compute = "";
			std::string TessControl = "";
			std::string TessEvaluation = "";
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
		std::vector<ShaderResource*> m_Resources;
	};
}