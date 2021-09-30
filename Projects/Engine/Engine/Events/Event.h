#pragma once

#include <future>

#include "Engine/Core.h"
#include <string>
#include <vector>

#include "Engine/Objects/Object.h"

namespace Engine
{
	template <typename T, typename ... Args>
	class Engine_API EventHandler// : public Object
	{
	public:
		typedef std::function<T(Args...)> FunctionType;
		typedef uint32_t IDType;

		explicit EventHandler(
			const FunctionType& Function,
			const String& sName = "Event Handler"
		)
			: m_Function(Function)//, Object(sName)
		{
			m_ID = ++m_IDCounter;
		}

		// Copy constructor
		EventHandler(const EventHandler& Src)
			: m_Function(Src.m_Function), m_ID(Src.m_ID)//, Object(Src.GetName())
		{
		}

		// Move constructor
		EventHandler(EventHandler&& Src)
			: m_Function(std::move(Src.m_Function)), m_ID(Src.m_ID)//, Object(Src.GetName())
		{
		}

		~EventHandler() {}

		// Copy assignment operator
		EventHandler& operator=(const EventHandler& Src)
		{
			m_Function = Src.m_Function;
			m_ID = Src.m_ID;
			m_sName = Src.GetName();

			return *this;
		}

		// Move assignment operator
		EventHandler& operator=(EventHandler&& Src)
		{
			std::swap(m_Function, Src.m_Function);
			m_ID = Src.m_ID;
			m_sName = Src.GetName();

			return *this;
		}

		// Function call operator
		void operator()(Args... params) const
		{
			if (m_Function)
			{
				m_Function(params...);
			}
		}

		bool operator==(const EventHandler& Other) const
		{
			return m_ID == Other.m_ID;
		}

		operator bool() const
		{
			return m_Function;
		}

		uint32_t GetID() const
		{
			return m_ID;
		}

		const String& GetName() const
		{
			return m_sName;
		}

	protected:
		FunctionType m_Function;

		IDType m_ID;
		String m_sName;

		static std::atomic_uint m_IDCounter;
	};

// #ifdef CB_BUILD_DLL
	template <typename T, typename ... Args>
	std::atomic_uint EventHandler<T, Args...>::m_IDCounter(0);
// #endif
	
	template <typename T, typename... Args>
	class Engine_API Event : public Object
	{
	public:
		typedef EventHandler<T, Args...> HandlerType;

		virtual String ToString() const { return GetName(); }

		Event(const String& sName = "Event") : Object(sName)
		{
		}

		// copy constructor
		Event(const Event& Src)
		{
			std::lock_guard<std::mutex> Lock(Src.m_HandlersLocker);

			m_Handlers = Src.m_Handlers;
			m_sName = Src.m_sName;
		}

		// move constructor
		Event(Event&& Src)
		{
			std::lock_guard<std::mutex> Lock(Src.m_HandlersLocker);

			m_Handlers = std::move(Src.m_Handlers);
			m_sName = Src.m_sName;
		}
		
		template<typename C>
		typename HandlerType::IDType Bind(C* Object, T(C::* Function)(Args...))
		{
			return Add([Object, Function](Args... args)->T { return (Object->*Function)(args...); },
				"Event Handler " + String(
					typeid(Function).name()
				)
			);
		}
		
		bool Unbind(const typename HandlerType::IDType& HandlerID)
		{
			std::lock_guard<std::mutex> Lock(m_HandlersLocker);

			auto it = std::find_if(
				m_Handlers.begin(), m_Handlers.end(),
				[HandlerID](const HandlerType& handler)
			{
				return handler.GetID() == HandlerID;
			}
			);

			if (it != m_Handlers.end())
			{
				m_Handlers.erase(it);
				return true;
			}

			return false;
		}

		void Clear()
		{
			std::lock_guard<std::mutex> Lock(m_HandlersLocker);

			m_Handlers.clear();
		}

		void Call(Args... Params) const
		{
			HandlerCollectionType HandlersCopy = GetHandlersCopy();
			CallInternal(HandlersCopy, Params...);
		}

		std::future<T> CallAsync(Args... Params) const
		{
			return std::async(std::launch::async, [this](Args... AsyncParams) { all(AsyncParams...); }, Params...);
		}

		// copy assignment operator
		Event& operator=(const Event& Src)
		{
			std::lock_guard<std::mutex> Lock(m_HandlersLocker);
			std::lock_guard<std::mutex> Lock2(Src.m_HandlersLocker);

			m_Handlers = Src.m_Handlers;

			return *this;
		}

		// move assignment operator
		Event& operator=(Event&& Src)
		{
			std::lock_guard<std::mutex> Lock(m_HandlersLocker);
			std::lock_guard<std::mutex> Lock2(Src.m_HandlersLocker);

			std::swap(m_Handlers, Src.m_Handlers);

			return *this;
		}

		void operator()(Args... Params) const
		{
			Call(Params...);
		}

		typename HandlerType::IDType operator+=(const HandlerType& Handler)
		{
			return Add(Handler);
		}

		typename HandlerType::IDType operator+=(const typename HandlerType::FunctionType& Handler)
		{
			return Add(Handler);
		}

		bool operator-=(const HandlerType& Handler)
		{
			return Remove(Handler);
		}

	protected:
		typename HandlerType::IDType Add(const typename HandlerType::FunctionType& Handler, const String& sName)
		{
			std::lock_guard<std::mutex> lock(m_HandlersLocker);
			m_Handlers.push_back(HandlerType(Handler, sName));
			return m_Handlers.back().GetID();
		}

		bool Remove(const HandlerType& Handler)
		{
			std::lock_guard<std::mutex> Lock(m_HandlersLocker);

			auto it = std::find(m_Handlers.begin(), m_Handlers.end(), Handler);
			if (it != m_Handlers.end())
			{
				m_Handlers.erase(it);
				return true;
			}

			return false;
		}

		bool Remove(const typename HandlerType::FunctionType& Handler)
		{
			return Remove(HandlerType(Handler));
		}

		typedef std::vector<HandlerType> HandlerCollectionType;

		void CallInternal(const HandlerCollectionType& Handlers, Args... Params) const
		{
			for (auto i = Handlers.rbegin(); i != Handlers.rend(); ++i)
			{
				(*i)(Params...);
			}
		}

		HandlerCollectionType GetHandlersCopy() const
		{
			std::lock_guard<std::mutex> Lock(m_HandlersLocker);

			// Since the function return value is by copy, 
			// before the function returns (and destruct the lock_guard object),
			// it creates a copy of the m_Handlers container.

			return m_Handlers;
		}

	private:
		HandlerCollectionType m_Handlers;
		mutable std::mutex m_HandlersLocker;
	};

	template <typename T, typename ... Args>
	inline std::ostream& operator<<(std::ostream& os, const Event<T, Args...>& e)
	{
		return os << e.ToString();
	}
}
