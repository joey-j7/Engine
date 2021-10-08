#pragma once

#include "Platform/PermissionManager.h"

namespace Engine
{
	class AndPermissionManager : public PermissionManager
	{
	public:
		AndPermissionManager();

		virtual Type StringToType(const String& String) override;
		virtual bool RequestPermission(Type Permission) override;
		virtual bool RequestPermissions(const std::vector<Type>& Permissions) override;
	};
}