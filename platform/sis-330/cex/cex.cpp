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

__END_DECLS
