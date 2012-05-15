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

#if defined(WIN32)
#include <crtdbg.h>
#endif

#include "TestDMR.h"

void *MicroStackChain;
void *ILib_Pool;
DMR testdmr;

int WaitForExit = 0;

void *ILib_Monitor;
int ILib_IPAddressLength;
int *ILib_IPAddressList;

const char* ProtocolInfoList[] = {
    /* IMAGES */
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
#ifdef INCLUDE_FEATURE_PLAYSINGLEURI
    "playsingle-http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif
    /* AUDIO */
    "http-get:*:audio/L16;channels=1;rate=44100:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:audio/L16;channels=2;rate=44100:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:audio/L16;channels=1;rate=48000:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:audio/L16;channels=2;rate=48000:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
#ifdef INCLUDE_FEATURE_PLAYSINGLEURI
    "playsingle-http-get:*:audio/L16;channels=1;rate=44100:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:audio/L16;channels=2;rate=44100:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:audio/L16;channels=1;rate=48000:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:audio/L16;channels=2;rate=48000:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif

    /* AUDIO: Windows Media */
#ifdef INCLUDE_WINDOWS_MEDIA
    "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
#ifdef INCLUDE_FEATURE_PLAYSINGLEURI
    "playsingle-http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif
#endif

    /* VIDEO: PS */
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
#ifdef INCLUDE_FEATURE_PLAYSINGLEURI
    "playsingle-http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif

    /* VIDEO: TS NA */
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
#ifdef INCLUDE_FEATURE_PLAYSINGLEURI
    "playsingle-http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "playsingle-http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif

    /* VIDEO: Windows Media */
#ifdef INCLUDE_WINDOWS_MEDIA
    "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
    "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO;DLNA.ORG_FLAGS=BD300000000000000000000000000000",
#ifdef INCLUDE_FEATURE_PLAYSINGLEURI
    "playsingle-http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
    "playsingle-http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif
#endif

	/* PLAYLIST ITEMS */
#ifdef INCLUDE_FEATURE_PLAYLIST_S
    "http-get:*:text/xml:DLNA.ORG_PN=DIDL_S;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif
#ifdef INCLUDE_FEATURE_PLAYLIST_V
    "http-get:*:text/xml:DLNA.ORG_PN=DIDL_V;DLNA.ORG_FLAGS=AD300000000000000000000000000000",
#endif

	/* PLAYCONTAINER ITEMS */
#ifdef INCLUDE_FEATURE_PLAYCONTAINERURI
/* NA */
#endif /* INCLUDE_FEATURE_PLAYCONTAINERURI */

    /* End of List */
    "\0"
};

void Common__CP_IPAddressListChanged();

#if defined(_POSIX)

#if defined(_POSIX)

void ILib_IPAddressMonitor(void *data)
{
    int length;
    int *list;

    length = ILibGetLocalIPAddressList(&list);
    if(length!=ILib_IPAddressLength || memcmp((void*)list,(void*)ILib_IPAddressList,sizeof(int)*length)!=0)
    {
        Common__CP_IPAddressListChanged();

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
    Common__CP_IPAddressListChanged();

    WSAIoctl(ILib_MonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&ILib_MonitorSocketReserved,&ILib_MonitorSocketStateObject,&ILib_IPAddressMonitor);
}

DWORD WINAPI ILib_IPAddressMonitorLoop(LPVOID args)
{
    ILib_MonitorSocket = socket(AF_INET,SOCK_DGRAM,0);
    WSAIoctl(ILib_MonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&ILib_MonitorSocketReserved,&ILib_MonitorSocketStateObject,&ILib_IPAddressMonitor);
    while(WaitForSingleObjectEx(ILib_IPAddressMonitorTerminator,INFINITE,TRUE)!=WAIT_OBJECT_0);
    return 0;
}

DWORD WINAPI DMR_CommandLoop(LPVOID args)
{
    char cmdLine[1024] = { 0 };
    int ret = 0;

    do
    {
        printf( "DMR_Program~# " );
        fgets( cmdLine, 1024, stdin );
        ret = ( strcmp( cmdLine, "exit\n" ) != 0 ) ? 0 : -1;
        UpdateUi();
        /*ret = paraseCmd( cmdLine );*/
    } while ( ret != -1 );

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

void * DMR_CommandLoop( void *args )
{
    char cmdline[1024];
    int ret = 0;

    do
    {
        printf( "DMR_Program~# " );
        fgets( cmdline, 1024, stdin );
        ret = ( strcmp( cmdline, "exit\n" ) != 0 ) ? 0 : -1;
        UpdateUi();
        /*ret = paraseCmd( cmdline );*/
    } while( ret != -1 );

    BreakSink( 0 );
    return(0);
}
#else

DWORD WINAPI ILibPoolThread(void *args)
{
    ILibThreadPool_AddThread(ILib_Pool);
    return(0);
}
#endif

void Common__CP_IPAddressListChanged()
{
    DMR_Method_NotifyMicrostackOfIPAddressChange( testdmr );
}

int main( int argc, char **argv )
{
    int x;
    char * protocolInfo;

#if defined(_POSIX)
    struct sigaction setup_action;
    sigset_t block_mask;
    pthread_t t, cmdloopthread;
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
    /*
        Create DMR
    */
    DMR_GetConfiguration()->Manufacturer = "PPTV MTBU SDK";
	DMR_GetConfiguration()->ManufacturerURL = "http://www.pptv.com";
	DMR_GetConfiguration()->ModelName = "PPTV";
	DMR_GetConfiguration()->ModelDescription = "PPTV Corp";
	DMR_GetConfiguration()->ModelNumber = "1.0.0.1";
	DMR_GetConfiguration()->ModelURL = "http://www.pptv.com";
    protocolInfo = BuildProtocolInfo( ProtocolInfoList );
    testdmr = DMR_Method_Create( MicroStackChain, 0, "PPTV DMR", "xxxxx-xse-saf323-xcxvxcv", "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeeeeeee", protocolInfo, ILib_Pool, 0 );

    testdmr->Event_SetAVTransportURI = Callback_SetAVTransportURI;
    testdmr->Event_GetAVProtocolInfo = Callback_GetAVProtocolInfo;
    testdmr->Event_SetPlayMode = Callback_SetPlayMode;
    testdmr->Event_Stop = Callback_Stop;
    testdmr->Event_Play = Callback_Play;
    testdmr->Event_Pause = Callback_Pause;
    testdmr->Event_SeekTrack = Callback_SeekTrack;
    testdmr->Event_SeekTrackPosition = Callback_SeekTrackPosition;
    testdmr->Event_SeekMediaPosition = Callback_SeekMediaPosition;
    testdmr->Event_Next = Callback_Next;
    testdmr->Event_Previous = Callback_Previous;
    testdmr->Event_SelectPreset = Callback_SelectPreset;
#if defined(INCLUDE_FEATURE_VOLUME)
    testdmr->Event_SetVolume = Callback_SetVolume;
    testdmr->Event_SetMute = Callback_SetMute;
#endif /* INCLUDE_FEATURE_VOLUME */
#if defined(INCLUDE_FEATURE_DISPLAY)
    testdmr->Event_SetContrast = Callback_SetContrast;
    testdmr->Event_SetBrightness = Callback_SetBrightness;
#endif /* INCLUDE_FEATURE_DISPLAY */

#if defined(_POSIX)
    ILib_Monitor = ILibCreateLifeTime(MicroStackChain);

    ILib_IPAddressLength = ILibGetLocalIPAddressList(&ILib_IPAddressList);
    ILibLifeTime_Add(ILib_Monitor,NULL,4,(void*)&ILib_IPAddressMonitor,NULL);

    pthread_create(&cmdloopthread,NULL,&DMR_CommandLoop,NULL);

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
    CreateThread(NULL,0,&DMR_CommandLoop,NULL,0,&ptid);

    ILib_IPAddressMonitorTerminator = CreateEvent(NULL,TRUE,FALSE,NULL);
    ILib_IPAddressMonitorThread = CreateThread(NULL,0,&ILib_IPAddressMonitorLoop,NULL,0,&ptid2);
#endif
    ILibStartChain(MicroStackChain);
    free( protocolInfo );
#if defined(_POSIX)
    free(ILib_IPAddressList);
#endif
    return 0;
}
