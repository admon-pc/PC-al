#ifndef _AL_USERDATA_H_
#define _AL_USERDATA_H_

class alUserData
{
	void* m_data = 0;
public:
	alUserData() {}
	virtual  ~alUserData() {}

	virtual void* GetUserData() { return m_data; }
	virtual void SetUserData(void* d) { m_data = d; }
};

#endif
