#include "pch.h"
#include "PermissionManager.h"

namespace Engine
{

	PermissionManager* PermissionManager::s_Instance = nullptr;

	PermissionManager::PermissionManager()
	{
		CB_CORE_ASSERT(!s_Instance, "Permission Manager already exists!");
		s_Instance = this;
	}

	bool PermissionManager::HasPermission(Type Permission)
	{
		return std::count(
			m_GrantedPermissions.begin(),
			m_GrantedPermissions.end(),
			Permission
		) != 0;
	}

	bool PermissionManager::HasPermissions(const std::vector<Type>& Permissions)
	{
		bool Granted = true;

		for (auto Permission : Permissions)
		{
			if (!HasPermission(Permission))
			{
				Granted = false;
				break;
			}
		}

		return Granted;
	}

	bool PermissionManager::RequestPermission(Type Permission)
	{
		if (!HasPermission(Permission))
		{
			m_GrantedPermissions.push_back(Permission);
			OnPermission({ "" }, {1});
		}

		return true;
	}

	bool PermissionManager::RequestPermissions(const std::vector<Type>& Permissions)
	{
		std::vector<String> PermissionStrings;
		std::vector<int32_t> Granted;

		for (auto Permission : Permissions)
		{
			if (!HasPermission(Permission))
			{
				m_GrantedPermissions.push_back(Permission);

				PermissionStrings.push_back("");
				Granted.push_back(1);
			}
		}

		OnPermission(PermissionStrings, Granted);

		return true;
	}
	
	void PermissionManager::OnPermission(const std::vector<String>& Permissions, const std::vector<int32_t>& Granted)
	{
		std::lock_guard<std::mutex> Guard(m_Mutex);

		for (int32_t i = 0; i < Granted.size(); ++i)
		{
			Type PermissionType = StringToType(Permissions[i]);

			// If granted
			if (Granted[i] >= 0)
			{
				if (!HasPermission(PermissionType))
				{
					m_GrantedPermissions.push_back(PermissionType);
				}
			}
			// Not granted
			else
			{
				auto It = find(m_GrantedPermissions.begin(), m_GrantedPermissions.end(), PermissionType);
				
				if (It != m_GrantedPermissions.end())
				{
					m_GrantedPermissions.erase(It);
				}
			}
		}

		m_RequestingPermissions = false;
		OnPermissionEvent(Permissions, Granted);

		OnPermissionEvent.Clear();
	}
}