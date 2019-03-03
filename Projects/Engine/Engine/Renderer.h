#pragma once

namespace Engine {
	class Engine_API Renderer
	{
	public:
		Renderer() {
			CB_CORE_INFO_T(5.0f, "{0} rendering API has been initialized", m_Name);
		};

		virtual ~Renderer() {}

		virtual void Clear() = 0;
		static Renderer* Create();

		static const char* m_Name;
	};

}