#pragma once

#include <string>

namespace Engine
{
	class ShaderProgram
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

		ShaderProgram(const Descriptor& descriptor)
		{
			m_Descriptor = descriptor;
		}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		bool IsLinked() const { return m_bLinked; }

	protected:
		Descriptor m_Descriptor;
		bool m_bLinked = false;
	};
}