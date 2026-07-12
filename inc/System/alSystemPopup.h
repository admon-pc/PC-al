#ifndef _AL_SYSTEMPOPUP_H_
#define _AL_SYSTEMPOPUP_H_

class alSystemPopup
{
public:
	alSystemPopup() {}
	virtual ~alSystemPopup() {}

	virtual alSystemPopup* CreateSubMenu(const char32_t* text) = 0;
	virtual void AddItem(const char32_t*, uint32_t id, const char32_t* shortcut) = 0;
	virtual void AddSeparator() = 0;
	virtual void Show(alSystemWindow*, uint32_t x, uint32_t y) = 0;
};



#endif
