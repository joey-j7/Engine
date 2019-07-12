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
		Buffer(const std::string& sName = "Unnamed Buffer");
		virtual ~Buffer() { delete m_pData; };

		virtual void Create(Data* pBuffer, const Strides& uiStrides) = 0;

	protected:
		std::string m_sName = "";

		Data* m_pData = nullptr;
		Strides m_uiStrides;
	};
}