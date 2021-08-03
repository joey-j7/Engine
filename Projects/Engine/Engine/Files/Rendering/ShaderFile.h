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

		ShaderFile(const String& filePath) : RenderFile(filePath)
		{
			if (filePath.find(".frag.") != String::npos)
			{
				m_Type = PIXEL;
			}
			else if (filePath.find(".vert.") != String::npos)
			{
				m_Type = VERTEX;
			}
			else if (filePath.find(".comp.") != String::npos)
			{
				m_Type = COMPUTE;
			}
			else if (filePath.find(".geom.") != String::npos)
			{
				m_Type = GEOMETRY;
			}
			else if (filePath.find(".tesc.") != String::npos)
			{
				m_Type = TESS_CONTROL;
			}
			else if (filePath.find(".tese.") != String::npos)
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
		String ParseIncludes(const String& filePath, const char* data, uint32_t& length)
		{
			String s = String(data, length);

			String fileDir = FileLoader::GetPath(filePath, FileLoader::E_ROOT);
			fileDir = fileDir.substr(0, fileDir.find_last_of(FileLoader::GetDefaultSeperator()) + 1);

			static const String inc = "#include \"";

			uint32_t j = 0;

			uint32_t start = 0;
			uint32_t end = 0;

			String incFile = "";

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
