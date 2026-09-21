#ifndef _AL_GUI2H_
#define _AL_GUI2H_

#ifdef CreateWindow
#undef CreateWindow
#endif

class alGUI2WindowMessage
{
public:
	enum
	{
		wm_paint = 1,
	};

	uint32_t m_flags1 = 0;
	uint32_t m_flags2 = 0;
};

class alGUI2WindowCallback
{
public:
	alGUI2WindowCallback() {}
	virtual ~alGUI2WindowCallback() {}

	virtual uint32_t WndProc(alGUI2Window*, const alGUI2WindowMessage&) = 0;
};

class alGUI2Window
{
	alUnicodeString m_windowTitle;
	alVec2i m_windowSize;
	alVec2i m_windowPosition;
	alGUI2WindowCallback* m_cbUser = 0;
	alGUI2WindowCallback* m_cbDefault = 0;


	alGUI2Window* m_parent = 0;
	alList<alGUI2Window*> m_children;
public:
	alGUI2Window(alGUI2WindowCallback* cbUser, alGUI2WindowCallback* cbDefault)
		:
	m_cbUser(cbUser),
	m_cbDefault(cbDefault)
	{}
	~alGUI2Window() {}

	void SendMessage(const alGUI2WindowMessage&);
	void ShowWindow();
};

class alGUI2
{
public:
	static alGUI2Window* CreateWindow(alGUI2WindowCallback*, alGUI2Window* parent);
};

#endif

