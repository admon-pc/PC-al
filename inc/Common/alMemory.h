#ifndef _AL_MEMORY_H_
#define _AL_MEMORY_H_

class alMemory
{
public:
	static void* Malloc(size_t);
	static void* Calloc(size_t);
	static void* Realloc(void*, size_t);
	static void Free(void*);
};

template<typename Type, typename... Args>
Type* alCreate(Args&&... args)
{
	Type* o = (Type*)alMemory::Malloc(sizeof(Type));
	if(o)
		new(o)Type(std::forward<Args>(args)...);
	return o;
}

template<typename Type>
void alDestroy(Type* p)
{
	AL_ASSERT_ST(p);
	p->~Type();
	alMemory::Free(p);
}

#define AL_DESTROY(x)if(x){alDestroy(x);x=0;}

class alAllocatorDefault
{
public:
	alAllocatorDefault() {}
	~alAllocatorDefault() {}

	template<typename _type>
	_type* Allocate()
	{
		_type* _o = (_type*)alMemory::Malloc(sizeof(_type));
		new(_o)_type();
		return _o;
	}

	template<typename _type>
	void Deallocate(_type* o)
	{
		AL_ASSERT_ST(o);
		//delete o;
		o->~_type();
		alMemory::Free(o);
	}
};

#define AL_DECLARE_DEFAULT_ALLOCATOR(x)void* x::operator new(std::size_t s){return alMemory::Malloc(s);}\
void x::operator delete(void*p){alMemory::Free(p);}

#endif
