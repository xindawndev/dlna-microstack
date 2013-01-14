#ifndef _CXL_YETI_UTIL_H_
#define _CXL_YETI_UTIL_H_

#if defined(_WIN32)
#include <iphlpapi.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "iphlpapi.lib")

char * get_mac_addr();
#endif

#endif // _CXL_YETI_UTIL_H_
