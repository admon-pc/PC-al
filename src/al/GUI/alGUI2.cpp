#include "al.h"
#include "GUI/alGUI2.h"
#include "GS/alGSTexture.h"
#include "GS/alGS.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

class alGUI2WindowCallback_Window : public alGUI2WindowCallback
{
public:
	alGUI2WindowCallback_Window() {}
	virtual ~alGUI2WindowCallback_Window() {}

	virtual uint32_t WndProc(alGUI2Window* wnd, const alGUI2WindowMessage& msg) override
	{
		return 0;
	}
};
alGUI2WindowCallback_Window g_defaultWindowCallback;

alGUI2Window* alGUI2::CreateGUI2Window(alGUI2WindowCallback* cb, alGUI2Window* parent)
{
	alGUI2Window* w = new alGUI2Window(cb, &g_defaultWindowCallback);

	return w;
}

void alGUI2Window::SendGUI2Message(const alGUI2WindowMessage&)
{
}

