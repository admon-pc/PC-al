#ifndef _AL_H_
#define _AL_H_

// C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// C++
#include <cmath>
#include <utility>

// al
#include "Common/alBase.h"
#include "Common/alConfig.h"
#include "Common/alAssert.h"
#include "Common/alMemory.h"
#include "Common/alLog.h"
#include "Common/alUserData.h"
#include "Common/alID.h"
#include "Common/alPtr.h"
#include "Common/alForward.h"
#include "Math/alMath.h"
#include "Geometry/alRay.h"
#include "Geometry/alAabb.h"
#include "String/alString.h"
#include "String/alUnicodeString.h"
#include "Containers/alArray.h"
#include "Containers/alList.h"
#include "Event/alEvent.h"
#include "Classes/alFileBuffer.h"

template<typename Type, uint32_t size>
class bqLIFO
{
	uint32_t m_sz = 0;
	Type m_data[size];
public:
	bqLIFO()
	{
	}

	~bqLIFO()
	{
	}

	void Push(const Type& v)
	{
		if (m_sz < size)
		{
			m_data[m_sz] = v;
			++m_sz;
		}
	}

	void Pop()
	{
		if (m_sz)
			--m_sz;
	}

	void Get(Type* out)
	{
		if (m_sz)
			*out = m_data[m_sz - 1];
		else
			*out = 0;
	}

	bool IsEmpty() { return m_sz == 0; }
	void Clear() { m_sz = 0; }
	bool IsFull() { return m_sz == size - 1; }
};

enum class alMatrixType : uint32_t
{
	World,
	View,
	Projection,
	ViewProjection, //For 3d line
	WorldViewProjection,
	ViewInvert,
	
	// 
	//GUIProjection,

	_count
	//LightView,
	//LightProjection,
};

enum class alVideoDriverType : uint32_t
{
	Unknown,
//	OpenGL33,
	Direct3D11,
};

// help structure
template<typename T1, typename T2>
struct alPair
{
	alPair() {}
	alPair(const T1& _t1, const T2& _t2) :m_first(_t1), m_second(_t2) {}
	T1 m_first;
	T2 m_second;
};

enum class alCursorType : uint32_t;

enum class alSaveImageType
{
	png,
};

enum class alDirection : uint32_t
{
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest
};

/*
* Добавлять архивы чтобы потом загружать файлы из них НЕНАДО
* Пусть это будет реализовано на стороне приложения. Так будет гибче.
* Когда я захочу реализовать то самое, когда ресурс из архива можно
* заменить, не загружать его а использовать другой, лежащий в папке,
* то я реализую его вручную. Например, хочу загрузить картинку. Сперва,
* я сам загружаю нужные архивы. Потом, при необходимости получить ресурс,
* я сам пытаюсь найти его в папке, и уже потом в архиве. Всё сам.
*/

class alLib
{
public:
	static void InitializeLib();
	static alSystemWindow* CreateSystemWindow(alSystemWindowCallback*);
	static alSystemPopup* CreateSystemPopup();
	static void Update();

	static uint32_t GetImageLoaderNum();
	static alImageLoader* GetImageLoader(uint32_t);
	static alImageLoader* GetImageLoaderPNG();

	static alImage* LoadALImage(const char*);
	static alImage* LoadALImage(alFileBuffer*, alImageLoader*);
	static void SaveImage(const char* fileName, alImage*, alSaveImageType);

	static void AddEvent(const alEvent&, bool uniq = true);
	static bool PollEvent(alEvent&);

	static alCursor* CreateCursor(alCursorType ct, const char* fn);
	static alCursor* GetCursor(alCursorType ct);
	static bool GetCursorDisableAutoChange();
	static void ResetCursor(alCursorType ct);
	static void SetCursor(alCursorType ct, alCursor* c);
	static void SetCursorClip(alVec4i* New, alVec4i* Old, alSystemWindow* w);
	// If you change your cursor, it will be changed again
	// when you move cursor over border.
	static void SetCursorDisableAutoChange(bool);
	static void ShowCursor(bool v);

	static alGS* CreateGS(alVideoDriverType);
	static alInput* GetInput();
	static alGUIContext* CreateGUIContext(alSystemWindow* window, alGS*);
	static alGUIFont* CreateGUIFont();

	static uint64_t GetTime();
	static float32_t* GetDeltaTime();

	static uint64_t GetFileSize(const char*);
	static void RemoveFile(const char*);
	static void GetFileName(const char*, alStringA*);

	static alMat4* GetMatrix(alMatrixType t);
	static void SetMatrix(alMatrixType t, alMat4* m);

	static void StringGetExtension(const char* str, bool addDot, alStringA& out);
	static void StringGetWords(const char*, alArray<alStringA>*, bool add_space = false,
		bool add_tab = false,
		bool add_newLine = false);
	static int32_t StringStrcmp(const char32_t* s1, const char32_t* s2);

	static void CopyTextToClipboard(alUnicodeString*);
	static void GetTextFromClipboard(alUnicodeString*);

	static void LoadMesh(const char*, alMeshLoaderCallback*);

	static void InitializeDefaultFont(alGS*);
	static alGUIFont* GetDefaultFont();
	static alGUIColorTheme* GetDefaultColorTheme();

	// cstd
	static uint32_t sprintf(char32_t* str, const char32_t* format, ...);
	static uint32_t vsnprintf(char32_t* s, size_t n, const char32_t* format, va_list arg);
};


#endif

