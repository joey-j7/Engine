#pragma once

#include <string>

namespace Engine
{
	class ShaderProgram
	{
	public:
		struct Descriptor
		{
			String Vertex = "";
			String Pixel = "";
			String Geometry = "";
			String Compute = "";
			String TessControl = "";
			String TessEvaluation = "";
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