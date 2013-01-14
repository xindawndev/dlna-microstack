#include "airplay/Util.h"

#if defined(_WIN32)
//#include <iphlpapi.h>
//#include <stdio.h>
//#include <time.h>

//#pragma comment(lib, "iphlpapi.lib")

char * get_mac_addr()
{
    DWORD Err;
    char * ret_str;

    PIP_ADAPTER_INFO pAdapterInfo, pAdapt;
    DWORD AdapterInfoSize;
    PIP_ADDR_STRING pAddrStr;

    AdapterInfoSize = 0;
    if ((Err = GetAdaptersInfo(NULL, &AdapterInfoSize)) != 0) {
        if (Err != ERROR_BUFFER_OVERFLOW) {
            printf("GetAdaptersInfo sizing failed with error %d\n", Err);
            return "";
        }
    }

    // Allocate memory from sizing information
    if ((pAdapterInfo = (PIP_ADAPTER_INFO) GlobalAlloc(GPTR, AdapterInfoSize)) == NULL) {
        printf("Memory allocation error\n");
        return "";
    }

    // Get actual adapter information
    if ((Err = GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) != 0) {
        printf("GetAdaptersInfo failed with error %d\n", Err);
        return "";
    }

    pAdapt = pAdapterInfo;

    char buf[8] = {0};
    while (pAdapt) {
        for (UINT i=0; i<pAdapt->AddressLength; i++) {
            if (i == (pAdapt->AddressLength - 1))
                sprintf(buf, "%.2X\n",(int)pAdapt->Address[i]);
            else
                sprintf(buf, "%.2X-",(int)pAdapt->Address[i]);
            ret_str += buf;
        }

        pAdapt = pAdapt->Next;
    }

    return buf;
}
#endif