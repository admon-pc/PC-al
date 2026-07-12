#ifndef _AL_PTR_H_
#define _AL_PTR_H_


template<typename type>
class alPtr
{
public:
	alPtr() {}
	alPtr(type* ptr)
		:
		m_data(ptr)
	{}

	~alPtr()
	{
		if (m_data)
			alDestroy(m_data);
	}

	type* m_data = nullptr;

};

#endif
