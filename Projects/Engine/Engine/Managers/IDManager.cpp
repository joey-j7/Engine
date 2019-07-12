#include "pch.h"
#include "IDManager.h"

namespace Engine
{
	uint32_t IDManager::Reserve()
	{
		const uint32_t uiSlot = m_uiFirstAvailable;
		m_uiIDs[uiSlot] = true;

		m_uiOccupied++;

		// Find the next available ID immediately
		uint32_t i = uiSlot + 1;

		for (; i < UINT_MAX; ++i)
		{
			if (!m_uiIDs[i])
			{
				m_uiFirstAvailable = i;
				break;
			}
		}

		// Out if IDs, may never happen
		assert(i != UINT_MAX);

		return uiSlot;
	}

	bool IDManager::Free(uint32_t uiID)
	{
		const auto it = m_uiIDs.find(uiID);

		if (it != m_uiIDs.end())
		{
			it->second = false;
			m_uiFirstAvailable = std::min(m_uiFirstAvailable, uiID);

			m_uiOccupied--;

			return true;
		}

		return false;
	}
}