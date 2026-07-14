#pragma once

#include "Input/alInput.h"

#include "System/alCursor.h"
#include "GUI/alGUI.h"

#ifdef AL_PLATFORM_WIN32
#include "System/alCursorWin32.h"

//NOMINMAX for std::min std::max
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef LoadImage
#undef LoadImage
#endif

#ifdef DrawText
#undef DrawText
#endif

#endif

#define AL_EVENT_MAX 30
#define AL_MAX_BONES 250

class alLibGlobalData
{
public:
	alLibGlobalData();

	~alLibGlobalData();

#ifdef AL_PLATFORM_WIN32
	HANDLE m_processHeap = 0;
	HINSTANCE m_moduleHandle = 0;
#endif
};


class alLibImpl
{
public:
	alLibImpl();
	~alLibImpl();

	alInput* m_input = 0;
	bool m_cursorDisableAutoChange = false;
	bool m_showCursor = true;

	alSystemWindow* CreateSystemWindow(alSystemWindowCallback*);
	alEvent m_events[AL_EVENT_MAX];
	uint32_t m_events_num = 0;
	uint32_t m_events_current = 0;

	alMat4* m_matrixPtrs[(uint32_t)alMatrixType::_count];
	alMat4 m_matrixBones[AL_MAX_BONES];
	alMat4 m_defaultMatrix[(uint32_t)alMatrixType::_count];

	alArray<alImageLoader*> m_imageLoaders;
	alImageLoader* m_imageLoader_PNG = 0;
	
	alArray<alMeshLoader*> m_meshLoaders;

	alCursor* m_cursors[(uint32_t)alCursorType::_count];
	alCursor* m_cursorsDefault[(uint32_t)alCursorType::_count];
	alVec4i m_cursorClip;

	float32_t m_dt = 0.f;

	alGUIFont* m_fontMini = 0;
	alGUIColorTheme m_colorTheme;
};


