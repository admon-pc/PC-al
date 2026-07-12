#ifndef _AL_CURSOR_H_
#define _AL_CURSOR_H_

enum class alCursorType : uint32_t
{
	Arrow,
	Cross,
	Hand,
	Help,
	IBeam,
	No,
	Size,
	SizeNESW,
	SizeNS,
	SizeNWSE,
	SizeWE,
	UpArrow,
	Wait,

	User,
	_count
};

class alCursor
{
protected:
	alCursorType m_type;
public:
	alCursor() {}
	virtual ~alCursor() {}

	virtual void OnCreate(alCursorType ct) = 0;
	virtual void Activate() = 0;
	virtual void* GetHandle() = 0;
};

#endif
