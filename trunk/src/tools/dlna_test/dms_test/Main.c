#if defined(WIN32)
#ifndef MICROSTACK_NO_STDAFX
#include "stdafx.h"
#endif
#define _CRTDBG_MAP_ALLOC
#include <TCHAR.h>
#endif

#if defined(WINSOCK2)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(WINSOCK1)
#include <winsock.h>
#include <wininet.h>
#endif

#if defined(_POSIX)
#include <pthread.h>
#endif

#include <stdio.h>

#include "MediaServer_MicroStack.h"
#include "DmsIntegration.h"
#include "CdsDidlSerializer.h"

#if defined(WIN32)
#include <crtdbg.h>
#endif

void *MicroStackChain;
void *ILib_Pool;

void *DMSObject;

#if defined(_POSIX)
int WaitForExit = 0;

void *ILib_Monitor;
int ILib_IPAddressLength;
int *ILib_IPAddressList;

void ILib_IPAddressMonitor(void *data)
{
    int length;
    int *list;

    length = ILibGetLocalIPAddressList(&list);
    if(length!=ILib_IPAddressLength || memcmp((void*)list,(void*)ILib_IPAddressList,sizeof(int)*length)!=0)
    {
        DMS_NotifyIPAddressChange(DMSObject);

        free(ILib_IPAddressList);
        ILib_IPAddressList = list;
        ILib_IPAddressLength = length;
    }
    else
    {
        free(list);
    }

    ILibLifeTime_Add(ILib_Monitor,NULL,4,(void*)&ILib_IPAddressMonitor,NULL);
}

void ILib_LinuxQuit(void *data)
{
    if(ILib_Pool!=NULL)
    {
        printf("Stopping Thread Pool...\r\n");
        ILibThreadPool_Destroy(ILib_Pool);
        printf("Thread Pool Destroyed...\r\n");
    }

    if(MicroStackChain!=NULL)
    {
        ILibStopChain(MicroStackChain);
        MicroStackChain = NULL;
    }
}

void BreakSink(int s)
{
    if(WaitForExit==0)
    {
        ILibLifeTime_Add(ILib_Monitor,NULL,0,(void*)&ILib_LinuxQuit,NULL);
        WaitForExit = 1;
    }
}
#else
HANDLE ILib_IPAddressMonitorTerminator;
HANDLE ILib_IPAddressMonitorThread;
DWORD ILib_MonitorSocketReserved;
WSAOVERLAPPED ILib_MonitorSocketStateObject;
SOCKET ILib_MonitorSocket;

void CALLBACK ILib_IPAddressMonitor(
                                    IN DWORD dwError, 
                                    IN DWORD cbTransferred, 
                                    IN LPWSAOVERLAPPED lpOverlapped, 
                                    IN DWORD dwFlags 
                                    )
{
    DMS_NotifyIPAddressChange(DMSObject);

    WSAIoctl(ILib_MonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&ILib_MonitorSocketReserved,&ILib_MonitorSocketStateObject,&ILib_IPAddressMonitor);
}

DWORD WINAPI ILib_IPAddressMonitorLoop(LPVOID args)
{
    ILib_MonitorSocket = socket(AF_INET,SOCK_DGRAM,0);
    WSAIoctl(ILib_MonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&ILib_MonitorSocketReserved,&ILib_MonitorSocketStateObject,&ILib_IPAddressMonitor);
    while(WaitForSingleObjectEx(ILib_IPAddressMonitorTerminator,INFINITE,TRUE)!=WAIT_OBJECT_0);
    return 0;
}

DWORD WINAPI Run(LPVOID args)
{
    getchar();

    closesocket(ILib_MonitorSocket);
    SetEvent(ILib_IPAddressMonitorTerminator);
    WaitForSingleObject(ILib_IPAddressMonitorThread,INFINITE);
    CloseHandle(ILib_IPAddressMonitorTerminator);

    ILibStopChain(MicroStackChain);

    return 0;
}
#endif

#if defined(_POSIX)
void* ILibPoolThread(void *args)
{
    ILibThreadPool_AddThread(ILib_Pool);
    return(0);
}
#else
DWORD WINAPI ILibPoolThread(void *args)
{
    ILibThreadPool_AddThread(ILib_Pool);
    return(0);
}
#endif

int main( int argc, char **argv ) 
{
    struct DMS_BackEndInit backendinit;
    int x;

#if defined(_POSIX)
    struct sigaction setup_action;
    sigset_t block_mask;
    pthread_t t;
#else
    DWORD ptid = 0;
    DWORD ptid2 = 0;
#endif

    MicroStackChain = ILibCreateChain();

    ILib_Pool = ILibThreadPool_Create();
    for( x = 0; x < 3; ++x )
    {
#if defined(_POSIX)
        pthread_create(&t,NULL,&ILibPoolThread,NULL);
#else
        CreateThread(NULL,0,&ILibPoolThread,NULL,0,&ptid);
#endif
    }

    MediaServer_GetConfiguration()->Manufacturer = "PPLive";
    MediaServer_GetConfiguration()->ManufacturerURL = "http://www.pptv.com";
    MediaServer_GetConfiguration()->ModelName = "PPTV";
    MediaServer_GetConfiguration()->ModelDescription = "PPLive Corp";
    MediaServer_GetConfiguration()->ModelNumber = "123456789";
    MediaServer_GetConfiguration()->ModelURL = "http://www.pptv.com";

    backendinit.Path = "E:\\testmovie";
    DMSObject = DMS_Create(
        MicroStackChain,
        ILib_Pool,
        0,
        "PPTV_DMS",
        "396c5fea-aae0-407c-b9b5-309bcc8e8c0b",
        "0000001",
        CdsFilter_ResDuration,
        CdsFilter_ResDuration,
        "",
        "http-get:*:*:*",
        &backendinit,
        NULL);

#if defined(_POSIX)
    ILib_Monitor = ILibCreateLifeTime(MicroStackChain);

    ILib_IPAddressLength = ILibGetLocalIPAddressList(&ILib_IPAddressList);
    ILibLifeTime_Add(ILib_Monitor,NULL,4,(void*)&ILib_IPAddressMonitor,NULL);

    sigemptyset (&block_mask);
    /* Block other terminal-generated signals while handler runs. */
    sigaddset (&block_mask, SIGINT);
    sigaddset (&block_mask, SIGQUIT);
    setup_action.sa_handler = BreakSink;
    setup_action.sa_mask = block_mask;
    setup_action.sa_flags = 0;
    sigaction (SIGINT, &setup_action, NULL);
    WaitForExit = 0;
#else
    CreateThread(NULL,0,&Run,NULL,0,&ptid);

    ILib_IPAddressMonitorTerminator = CreateEvent(NULL,TRUE,FALSE,NULL);
    ILib_IPAddressMonitorThread = CreateThread(NULL,0,&ILib_IPAddressMonitorLoop,NULL,0,&ptid2);
#endif
    ILibStartChain(MicroStackChain);
#if defined(_POSIX)
    free(ILib_IPAddressList);
#endif
    return 0;
}

