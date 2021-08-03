#pragma once

#include "Engine/Objects/Object.h"
#include <functional>

namespace Engine {
    template <class T>
    class Engine_API VkHandle : public Object {
    public:
        using Handle = T;
        using Disposer = std::function<void(Handle)>;

        VkHandle(const String& sName = typeid(T).name()) : Object(sName) {}

        VkHandle(Handle handle, const Disposer& disposer = nullptr, const String& sName = typeid(T).name()) :
            Object(sName)
        {
            m_Handle = handle;
            m_Disposer = disposer;
        }

        VkHandle(VkHandle&& other) : Object(other.m_sName)
        {
            m_Handle = other.m_Handle;
            m_Disposer = other.m_Disposer;
        	
            other.m_Handle = VK_NULL_HANDLE;
            other.m_Disposer = nullptr;
        }

        ~VkHandle() { DisposeIfNecessary(); }

        VkHandle& operator=(VkHandle&& other) {
            if (m_Handle != other.m_Handle) {
                DisposeIfNecessary();
            }

            m_Handle = other.m_Handle;
            m_Disposer = other.m_Disposer;

            other.m_Handle = VK_NULL_HANDLE;
            other.m_Disposer = nullptr;

            return *this;
        }

        operator bool() const { return m_Handle != VK_NULL_HANDLE; }
        operator Handle() const { return m_Handle; }

        /// Relinquish responsibility of collecting the underlying handle when this
        /// object is collected. It is the responsibility of the caller to ensure that
        /// the lifetime of the handle extends past the lifetime of this object.
        void ReleaseOwnership() { m_Disposer = nullptr; }
        void Reset() { DisposeIfNecessary(); }

    private:
        Handle m_Handle = VK_NULL_HANDLE;
        Disposer m_Disposer = nullptr;

        void DisposeIfNecessary()
    	{
            if (m_Handle == VK_NULL_HANDLE) {
                return;
            }

        	if (m_Disposer) {
                m_Disposer(m_Handle);
            }
        	
            m_Handle = VK_NULL_HANDLE;
            m_Disposer = nullptr;
        }
    };
} 