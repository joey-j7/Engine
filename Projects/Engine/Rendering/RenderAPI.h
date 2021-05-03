#pragma once

#include <memory>

#include "Engine/Files/FileDatabase.h"
#include "ShaderProgram.h"

#include "Renderers/2D/Renderer2D.h"

namespace Engine
{
	class RenderContext;
	class CommandEngine;

	class Engine_API RenderAPI : public Object
	{
	public:
		enum Type
		{
			E_NONE,
			E_OPENGL,
			E_VULKAN,
			E_DIRECTX
		};

		RenderAPI(RenderContext& renderContext);
		virtual ~RenderAPI()
		{
		}

		virtual bool Init();

		virtual ShaderProgram* Create(const ShaderProgram::Descriptor& descriptor) = 0;

		virtual RenderFile* Load(const std::string& filePath, FileLoader::Type pathType = FileLoader::Type::E_CONTENT);
		virtual bool Unload(RenderFile* resource);

		virtual void Swap() = 0;
		virtual void Present() = 0;

		virtual void Suspend() = 0;
		virtual void Resume() = 0;

		virtual CommandEngine* GetCommandEngine(const std::string& sName) = 0;
		RenderContext& GetRenderContext() const { return *m_pRenderContext; }

	protected:
		bool m_bInitialized = false;
		bool m_bRunning = false;
		
		RenderContext* m_pRenderContext = nullptr;

		std::unique_ptr<Renderer2D> m_pRenderer2D;
	};

}
