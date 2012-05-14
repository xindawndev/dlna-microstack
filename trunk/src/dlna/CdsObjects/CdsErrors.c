#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifndef _WIN32_WCE
#include <crtdbg.h>
#endif
#else
#include <stdlib.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "CdsErrors.h"

const char *CDS_ErrorStrings[] = 
{
    "",
    CDS_EM_ACTION_FAILED,
    CDS_EM_OBJECT_ID_NO_EXIST,
    CDS_EM_NO_SUCH_CONTAINER
};

const int CDS_ErrorCodes[] = 
{
    0,
    CDS_EC_ACTION_FAILED,
    CDS_EC_OBJECT_ID_NO_EXIST,
    CDS_EC_NO_SUCH_CONTAINER
};
