#ifndef _AL_SYSTEMWINDOW_H_
#define _AL_SYSTEMWINDOW_H_

class alSystemWindowCallback;

class alSystemWindow : public alUserData, public alID
{
	bool m_isFullscreen = false;
	alSystemWindowCallback* m_callback = 0;
	void* m_OSData = 0;
public:
	alSystemWindow(alSystemWindowCallback*);
	virtual ~alSystemWindow();
	void Show();
	void Hide();
	void SetTitle(const char*);
	void SetPositionAndSize(uint32_t posX, uint32_t posY, uint32_t sizeX, uint32_t sizeY);
	void ToFullscreenMode();
	void ToWindowMode();

	alSystemWindowCallback* GetWindowCallback() { return m_callback; }
	void* GetOSData() { return m_OSData; }

	alVec2i m_clientSize;
	alVec2i m_clientSizeOld;
	void* m_GPUData = 0;
};

class alSystemWindowCallback
{
public:
	alSystemWindowCallback() {}
	virtual ~alSystemWindowCallback() {}

	// on close window
	virtual void OnClose(alSystemWindow*) {}
	virtual void OnPopupCommand(uint32_t cmd) {}

	virtual void OnMaximize(alSystemWindow*) {}
	virtual void OnMinimize(alSystemWindow*) {}
	virtual void OnRestore(alSystemWindow*) {}
	virtual void OnSize(alSystemWindow*) {}
	virtual void OnMove(alSystemWindow*) {}
	virtual void OnSizeChanged(alSystemWindow*) {}
	virtual void OnActivate(alSystemWindow*) {}
	virtual void OnDeactivate(alSystemWindow*) {}
	virtual void OnPaint(alSystemWindow*) {}
	virtual alVec2i OnMinMaxInfo(alSystemWindow*) { return alVec2i(800, 600); }

	// if we drop 10 files then 
	//   num is == 10
	//   and i is current file
	// x y is coordinates where
	virtual void OnDropFiles(alSystemWindow*, uint32_t num, uint32_t i, const wchar_t*, int32_t x, int32_t y) {}

	// Change GPU image resolution here.
	// Will be called by gpu lib when user will call gs->UpdateWindowData()
	//  inside this callback ... it must be in OnSize.
	virtual alVec2i OnGPUUpdateSize(alSystemWindow* w) { return w->m_clientSize; }

};

#endif
