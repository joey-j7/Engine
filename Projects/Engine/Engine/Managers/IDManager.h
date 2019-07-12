#pragma once

namespace Engine
{
	class Engine_API IDManager
	{
	public:
		uint32_t Reserve();
		bool Free(uint32_t uiID);

		uint32_t GetCount() const { return m_uiOccupied; }

	protected:
		std::unordered_map<uint32_t, bool> m_uiIDs;

		uint32_t m_uiFirstAvailable = 0;
		uint32_t m_uiOccupied = 0;
	};
}