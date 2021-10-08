#pragma once

namespace Engine
{
	class Engine_API PermissionManager
	{
	public:
		enum Type
		{
			E_READ_EXTERNAL_STORAGE = 0,
			E_WRITE_EXTERNAL_STORAGE = 1,
			E_CAMERA = 2,
			E_UNKNOWN = 3
		};

		PermissionManager();
		static PermissionManager& Get() { return *s_Instance; }

		virtual Type StringToType(const String& String) { return E_UNKNOWN; }

		virtual bool HasPermission(Type Permission);
		virtual bool HasPermissions(const std::vector<Type>& Permissions);

		virtual bool RequestPermission(Type Permission);
		virtual bool RequestPermissions(const std::vector<Type>& Permissions);

		std::vector<Type> GetGrantedPermissions() const { return m_GrantedPermissions; }
		void OnPermission(const std::vector<String>& Permissions, const std::vector<int32_t>& Granted);

		Event<void, const std::vector<String>&, const std::vector<int32_t>&> OnPermissionEvent = Event<void, const std::vector<String>&, const std::vector<int32_t>&>("PermissionManager::OnPermissionEvent");

	protected:
		static PermissionManager* s_Instance;

		std::vector<Type> m_GrantedPermissions;
		bool m_RequestingPermissions = false;

		std::mutex m_Mutex;
	};
}