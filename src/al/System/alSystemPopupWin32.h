#ifndef _AL_SYSTEMPOPUPW32_H_
#define _AL_SYSTEMPOPUPW32_H_

class alSystemPopupWin32 : public alSystemPopup
{
	HMENU m_hPopupMenu = 0;
	alArray<alSystemPopup*> m_subMenus;
public:
	alSystemPopupWin32();
	virtual ~alSystemPopupWin32();

	virtual alSystemPopup* CreateSubMenu(const char32_t* text) override;
	virtual void AddItem(const char32_t*, uint32_t id, const char32_t* shortcut) override;
	virtual void AddSeparator() override;
	virtual void Show(alSystemWindow*, uint32_t x, uint32_t y) override;


};



#endif
