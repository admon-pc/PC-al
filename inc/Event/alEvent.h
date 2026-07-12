#ifndef _AL_EVENTH_
#define _AL_EVENTH_

enum class alEventType
{
	Engine,
	Window,
	System,
	User
};

struct alEvent_User
{
	void* m_data;
	int32_t m_id;

	bool is_equal(const alEvent_User& other)
	{
		return m_id == other.m_id;
	}
};

struct alEvent_Engine
{
	int32_t i;

	bool is_equal(const alEvent_Engine& other)
	{
		return false;
	}
};

struct alEvent_Window
{
	enum _event
	{
		size_changed
	}
	m_event;

	bool is_equal(const alEvent_Window& other)
	{
		if (m_event != other.m_event)
			return false;
		return true;
	}
};

struct alEvent_System
{
	int32_t i;

	bool is_equal(const alEvent_System& other)
	{
		return false;
	}
};

class alEvent
{
public:
	alEvent() {}
	~alEvent() {}

	alEventType m_type = alEventType::Engine;

	union
	{
		alEvent_Engine m_event_engine;
		alEvent_Window m_event_window;
		alEvent_System m_event_system;
		alEvent_User   m_event_user;
	};

	bool IsEqual(const alEvent& event)
	{
		if (m_type != event.m_type) return false;
		switch (m_type)
		{
		case alEventType::Engine:
			return m_event_engine.is_equal(event.m_event_engine);
		case alEventType::Window:
			return m_event_window.is_equal(event.m_event_window);
		case alEventType::System:
			return m_event_system.is_equal(event.m_event_system);
		case alEventType::User:
			return m_event_user.is_equal(event.m_event_user);
		default:
			break;
		}
		return true;
	}
};



#endif

