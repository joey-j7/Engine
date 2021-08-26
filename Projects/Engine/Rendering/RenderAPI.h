#pragma once

#include <memory>

#include "Engine/Files/FileDatabase.h"
#include "ShaderProgram.h"

#include "Renderers/Renderer2D.h"
#include "Renderers/Renderer3D.h"

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
		virtual bool Deinit();
		virtual void Reset();

		virtual ShaderProgram* Create(const ShaderProgram::Descriptor& descriptor) = 0;

		virtual RenderFile* Load(const String& filePath, FileLoader::Type pathType = FileLoader::Type::E_CONTENT);
		virtual bool Unload(RenderFile* resource);

		virtual bool Swap() = 0;
		virtual void Present() = 0;

		virtual CommandEngine* GetCommandEngine(const String& sName) = 0;
		RenderContext& GetRenderContext() const { return *m_pRenderContext; }

		Renderer2D* GetRenderer2D() const { return m_pRenderer2D.get(); }
		Renderer3D* GetRenderer3D() const { return m_pRenderer3D.get(); }

		bool IsResized() const { return m_bResized; }
		void OnFramebufferResize(uint32_t Width, uint32_t Height);

	protected:
		bool m_bInitialized = false;
		bool m_bRunning = false;
		bool m_bResized = false;
		
		RenderContext* m_pRenderContext = nullptr;

		std::unique_ptr<Renderer2D> m_pRenderer2D;
		std::unique_ptr<Renderer3D> m_pRenderer3D;
	};

}
