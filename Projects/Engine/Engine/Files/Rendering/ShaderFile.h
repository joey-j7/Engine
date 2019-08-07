#pragma once

#include "Engine/Files/Rendering/RenderFile.h"
#include "Platform/FileLoader.h"

namespace Engine
{
	class Engine_API ShaderFile : public RenderFile
	{
	public:
		enum Type
		{
			PIXEL,
			VERTEX,
			COMPUTE,
			GEOMETRY,
			TESS_CONTROL,
			TESS_EVALUATION
		};

		ShaderFile(const std::string& filePath) : RenderFile(filePath)
		{
			if (filePath.find(".frag.") != std::string::npos)
			{
				m_Type = PIXEL;
			}
			else if (filePath.find(".vert.") != std::string::npos)
			{
				m_Type = VERTEX;
			}
			else if (filePath.find(".comp.") != std::string::npos)
			{
				m_Type = COMPUTE;
			}
			else if (filePath.find(".geom.") != std::string::npos)
			{
				m_Type = GEOMETRY;
			}
			else if (filePath.find(".tesc.") != std::string::npos)
			{
				m_Type = TESS_CONTROL;
			}
			else if (filePath.find(".tese.") != std::string::npos)
			{
				m_Type = TESS_EVALUATION;
			}
			else
			{
				// Unsupported
				CB_CORE_ASSERT(false, "Unsupported shader type detected!");
			}
		}

	protected:
		std::string ParseIncludes(const std::string& filePath, const char* data, uint32_t& length)
		{
			std::string s = std::string(data, length);

			std::string fileDir = FileLoader::GetPath(filePath, FileLoader::E_ROOT);
			fileDir = fileDir.substr(0, fileDir.find_last_of(FileLoader::GetDefaultSeperator()) + 1);

			static const std::string inc = "#include \"";

			uint32_t j = 0;

			uint32_t start = 0;
			uint32_t end = 0;

			std::string incFile = "";

			// Check include directories
			for (uint32_t i = 0; i < length; ++i)
			{
				if (data[i] == inc[j])
				{
					j++;

					// Found include, find file
					if (j >= inc.length())
					{
						// Store start position
						start = i + 1;

						for (uint32_t k = start; k < length; ++k)
						{
							if (data[k] == '\"')
							{
								end = k + 1;
								break;
							}

							incFile += data[k];
						}

						// Found file, delete include
						if (end > start)
						{
							start -= static_cast<uint32_t>(inc.length());
							s.erase(start, end - start);

							i = end;

							uint32_t len = 0;
							const char* r = FileLoader::Read(fileDir + incFile, len, FileLoader::E_ROOT, false);

							s.insert(start, ParseIncludes(fileDir + incFile, r, len));
							
							delete[] r;
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					j = 0;
				}
			}

			return s;
		}

		Type m_Type;
	};
}
