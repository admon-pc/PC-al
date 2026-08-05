#ifndef _AL_GUID_H_
#define _AL_GUID_H_

struct alGUID
{
    uint32_t Data1 = 0;
    uint16_t Data2 = 0;
    uint16_t Data3 = 0;
    uint8_t Data4[8] = {0,0,0,0,0,0,0,0};
    
    uint8_t* Data()const{ return (uint8_t*)&Data1; }
    // Use alLib::GUIDIsEqual
    //bool IsEqual(const alGUID& other);
};
#define AL_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const alGUID name =     {l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8}

#endif
