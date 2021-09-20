#pragma once

#include <vector>

#include "Engine/Objects/Object.h"

namespace Engine
{
	class Engine_API Buffer : public Object
	{
		typedef void Data;
		typedef std::vector<uint32_t> Strides;

	public:
		Buffer(const String& sName = "Buffer");
		virtual ~Buffer() {
			// TODO: Remove actual data
		};

		virtual void Create(Data* pBuffer, const Strides& uiStrides) = 0;

	protected:
		String m_sName = "";

		Data* m_pData = nullptr;
		Strides m_uiStrides;
	};
}