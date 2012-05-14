#ifndef __BITARRAY_H__
#define __BITARRAY_H__

/**
    \file BitArray.h
    \ingroup DLNADMR
    \brief Utility API for creating a bit array.

    \addtogroup DLNADMR DLNA MediaRenderer Microstack (DMR)

    \{
*/

/** \brief The type 'BitArray' is a 'void*'.
*/
typedef void *BitArray;


/** \brief Create a BitArray object.
    \param size The maximum size of the bit array in bits.
    \param initialValue The initial value of the bits in the array
    (must be 0 for an off state, and non-zero for a on state).

    \returns The created BitArray object or NULL if an error occurred.

    \warning The maximum size of the array is fixed but the usable size of the
             array is initially set to zero.  This allows growth up to the maximum
             size without a memory allocation and memory copy penilty.  Use the
             \ref BitArray_ChangeSize method to change the current size.
*/
BitArray BitArray_Create(int size, int initialValue);

/** \brief Destroys all memory associated with the \ref BitArray object.
    \param array This is the 'this' pointer to the \ref BitArray instance.
*/
void BitArray_Destroy(BitArray array);

/** \brief Sets a specified bit to the specified value.
    \param array This is the 'this' pointer to the \ref BitArray instance.
    \param index The zero based index of the bit to set/reset.
    \param value Etiher 0 to reset the bit or non-zero to set the bit.

    \returns Zero (0) if the method fails or 1 if the method succeeds.
*/
int BitArray_SetBit(BitArray array, int index, int value);

/** \brief Gets the bit value at the specific index.
    \param array This is the 'this' pointer to the \ref BitArray instance.
    \param index The zero based index of the bit to get the state of.
    \returns 0 if the bit is reset, 1 if the bit is set or, -1 if the index
             was out of range.
*/
int BitArray_GetBit(BitArray array, int index);

/** \brief Tests to see if all the bits in the useable array have the given
           value.
    \param array This is the 'this' pointer to the \ref BitArray instance.
    \param bit The value to compare to.  Use 0 to test for the bits being reset
           (0) or non-zero to test for the bits beign set (1).
    \returns Zero (0) if the bits are not all equal to the test value or (1) if
             all the bits are equal to the test value.
*/
int BitArray_TestAllBitsEqualTo(BitArray array, int bit);

/** \brief Sets all bits in the useable array size to the given value.
    \param array This is the 'this' pointer to the \ref BitArray instance.
    \param bit The value to set all of the bits in the array to.
*/
void BitArray_Reset(BitArray array, int bit);

/** \brief Change the size of the usable array to the given size.
    \param array This is the 'this' pointer to the \ref BitArray instance.
    \param size The new size of the array.  Must be 0-MaxSize inclusive.
*/
void BitArray_ChangeSize(BitArray array, int size);

/* \} */
#endif /* __BITARRAY_H__ */
