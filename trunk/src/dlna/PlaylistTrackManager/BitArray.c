#if defined(WIN32)
    #define _CRTDBG_MAP_ALLOC
#endif

#include "DMRCommon.h"
#include "BitArray.h"

const unsigned char __Bits[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
const unsigned char __Mask[8] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
const unsigned char __Comp[8] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};


struct _BitArray
{
    int MaxSize;
    int Size;
    unsigned char* Array;
};


BitArray BitArray_Create(int maxSize, int initialValue)
{
    struct _BitArray* array = NULL;
    int bytes = 0;
    
    if(maxSize < 0)
    {
        return NULL;
    }

    array = (struct _BitArray*)malloc(sizeof(struct _BitArray));
    if(array == NULL)
    {
        return NULL;
    }

    bytes = maxSize / 8;
    if((maxSize % 8) != 0)
    {
        bytes++;
    }
    array->Array = (unsigned char*)malloc((size_t)bytes);
    if(array->Array == NULL)
    {
        free(array);
        return NULL;
    }
    array->Size = maxSize;
    array->MaxSize = maxSize;

    if(initialValue == 0)
    {
        memset(array->Array, 0, (size_t)bytes);
    }
    else
    {
        memset(array->Array, -1, (size_t)bytes);
    }

    return (BitArray)array;
}

void BitArray_ChangeSize(BitArray array, int size)
{
    struct _BitArray* instance = (struct _BitArray*)array;
    if(size <= instance->MaxSize && size >= 0)
    {
        instance->Size = size;
    }
}

void BitArray_Destroy(BitArray array)
{
    struct _BitArray* instance = (struct _BitArray*)array;
    free(instance->Array);
    free(instance);
}

int BitArray_SetBit(BitArray array, int index, int value)
{
    struct _BitArray* instance = (struct _BitArray*)array;
    if(instance == NULL || index < 0 || index >= instance->Size)
    {
        return 0;
    }
    if(value == 0)
    {
        instance->Array[index / 8] = instance->Array[index / 8] & __Mask[index % 8];
    }
    else
    {
        instance->Array[index / 8] = instance->Array[index / 8] | __Bits[index % 8];
    }
    return 1;
}

int BitArray_GetBit(BitArray array, int index)
{
    struct _BitArray* instance = (struct _BitArray*)array;
    if(instance == NULL || index < 0 || index >= instance->Size)
    {
        return -1;
    }
    return ((instance->Array[index / 8] & __Bits[index % 8]) == __Bits[index % 8])?1:0;
}

int BitArray_TestAllBitsEqualTo(BitArray array, int bit)
{
    struct _BitArray* instance = (struct _BitArray*)array;
    int byteSize = instance->Size / 8;
    int compTo = 0;
    int i;
    int result = 1;

    compTo = bit?255:0;

    for(i = 0; i < byteSize; i++)
    {
        if((int)(instance->Array[i]) != compTo)
        {
            result = 0;
            break;
        }
    }

    if(result == 1 && (instance->Size % 8) != 0)
    {
        compTo = bit?__Comp[(instance->Size % 8)-1]:0;
        if((int)(instance->Array[i] & __Comp[(instance->Size % 8)-1]) != compTo)
        {
            result = 0;
        }
    }

    return result;
}

void BitArray_Reset(BitArray array, int bit)
{
    struct _BitArray* instance = (struct _BitArray*)array;
    int byteSize = instance->Size / 8;
    int value = bit?255:0;
    int i;

    for(i = 0; i < byteSize; i++)
    {
        instance->Array[i] = value;
    }
    if((instance->Size % 8) != 0)
    {
        instance->Array[i] = value;
    }
}
