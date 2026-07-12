#ifndef _AL_ID_H_
#define _AL_ID_H_

class alID
{
	uint32_t m_id = 0;
public:
	alID() {}
	virtual  ~alID() {}

	virtual uint32_t GetID() { return m_id; }
	virtual void SetID(uint32_t newID) { m_id = newID; }
};

#endif
