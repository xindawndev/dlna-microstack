#include "iconv.h"
#include "execinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Assume the worst for the width of an address.  */
#define WORD_WIDTH 16

__BEGIN_DECLS

char ***_NSGetEnviron(void)
{
    return (char ***)NULL;
}
__END_DECLS
