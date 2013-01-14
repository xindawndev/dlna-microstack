#include "iconv.h"
#include "execinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

/* Assume the worst for the width of an address.  */
#define WORD_WIDTH 16


__BEGIN_DECLS

int get_nprocs(void) __THROW
{
    return 1;
}

iconv_t iconv_open(__const char *__tocode, __const char *__fromcode)
{
    return iconv_t(-1);
}

size_t iconv(
    iconv_t __cd, 
    char **__restrict __inbuf,
    size_t *__restrict __inbytesleft,
    char **__restrict __outbuf,
    size_t *__restrict __outbytesleft)
{
    return (size_t)(-1);
}

int iconv_close(iconv_t __cd)
{
    return -1;
}

int backtrace (void **__array, int __size)
{
    return 0;
}

char ** backtrace_symbols(void *__const *__array, int __size)
    __THROW
{
    int cnt;
    size_t total = 0;
    char **result;

    /* We can compute the text size needed for the symbols since we print
    them all as "[+0x<addr>]".  */
    total = __size * (WORD_WIDTH + 6);

    /* Allocate memory for the result.  */
    result = (char **)malloc(__size * sizeof (char *) + total);
    if (result != NULL)
    {
        char *last = (char *) (result + __size);

        for (cnt = 0; cnt < __size; ++cnt)
        {
            result[cnt] = last;
            last += 1 + sprintf(last, "[+%p]", __array[cnt]);
        }
    }

    return result;
}

void backtrace_symbols_fd(void *__const *__array, int __size, int __fd)
    __THROW
{
}

__END_DECLS
