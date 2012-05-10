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

#include "DMR_CP.h"
#include "DMS_CP.h"
#include "DController.h"
#include "UController.h"

#include "MediaServer_MicroStack.h"
#include "DmsIntegration.h"

void *MicroStackChain;
void *ILib_Pool;

void *DMSObject;
DMD_Session dmd_session;

void Common__CP_IPAddressListChanged();

struct AVRenderer* TheRendererCP = NULL;
void* TheMediaServerCP = NULL;

enum cmdloop_dmrcmds
{
    // RCP
    HELP = 0, RCP_PRINT, RCP_GETDLNADOC, RCP_GETDLNACAP, RCP_GETDEVCAP, RCP_SUPPORTPLAYMODE,
    RCP_SUPPORTVOLUME, RCP_SUPPORTMUTE, RCP_PLAY, RCP_SEEK, RCP_STOP, RCP_PAUSE,
    RCP_NEXT, RCP_PREV, RCP_SETURI, RCP_SETVOLUME, RCP_SETMUTE, RCP_SETPLAYMODE,
    RCP_GETMEDIAINFO, RCP_GETPOSITION, MRCP_EXIT,

    // MCP
    MCP_PRINT, MCP_GETDLNADOC, MCP_GETDLNACAP, MCP_CDBROWSE,

    // DMD
    DMD_INIT, DMD_DOWNLOAD, DMD_SHUTDOWN,

    // DMU
    DMU_INIT, DMU_UPLOAD, DMU_SHUTDOWN,
};

struct cmdloop_commands
{
    char *str;
    int cmdnum;
    int numargs;
    char *args;
} cmdloop_commands;

static struct cmdloop_commands cmdloop_cmdlist[] = {
    {"Help", HELP, 1, ""},
    {"LsDmr", RCP_PRINT, 1, ""},
    {"DmrDoc", RCP_GETDLNADOC, 2, "<devnum>"},
    {"DmrCap", RCP_GETDLNACAP, 2, "<devnum>"},
    {"DmrDevCap", RCP_GETDEVCAP, 2, "<devnum>"},
    {"DmrSupPlayMode", RCP_SUPPORTPLAYMODE, 3, "<devnum> <playmode (int)>"},
    {"DmrSupVolume", RCP_SUPPORTVOLUME, 2, "<devnum>"},
    {"DmrSupMute", RCP_SUPPORTMUTE, 2, "<devnum>"},
    {"DmrPlay", RCP_PLAY, 2, "<devnum>"},
    {"DmrSeek", RCP_SEEK, 3, "<devnum> <position (int)>"},
    {"DmrStop", RCP_STOP, 2, "<devnum>"},
    {"DmrPause", RCP_PAUSE, 2, "<devnum>"},
    {"DmrNext", RCP_NEXT, 2, "<devnum>"},
    {"DmrPrev", RCP_PREV, 2, "<devnum>"},
    {"DmrSetUri", RCP_SETURI, 3, "<devnum> <any(int)> <url (string)>"},
    {"DmrSetVol", RCP_SETVOLUME, 3, "<devnum> <volume (int)>"},
    {"DmrSetMute", RCP_SETMUTE, 3, "<devnum> <ismute (bool)>"},
    {"DmrSetPlayMode", RCP_SETPLAYMODE, 3, "<devnum> <playmode (int)>"},
    {"DmrGetMediaInfo", RCP_GETMEDIAINFO, 2, "<devnum>"},
    {"DmrGetPos", RCP_GETPOSITION, 2, "<devnum>"},
    {"Exit", MRCP_EXIT, 1, ""},
    {"LsDms", MCP_PRINT, 1, ""},
    {"DmsDoc", MCP_GETDLNADOC, 2, "<dms_devnum>"},
    {"DmsCap", MCP_GETDLNACAP, 2, "<dms_devnum>"},
    {"DmsBrowse", MCP_CDBROWSE, 3, "<dms_devnum> <startid(int)> <obj_id(string)>"},
    {"DmdInit", DMD_INIT, 1, ""},
    {"DmdD", DMD_DOWNLOAD, 3, "<dms_devnum> <any(int)> <save_dir(int)> <obj_id(string)> "},
    {"DmdShutdown", DMD_SHUTDOWN, 1, ""},
    {"DmuInit", DMU_INIT, 1, ""},
    {"Dmuu", DMU_UPLOAD, 3, "<dms_devnum> <any(int)> <save_dir(int)> <obj_id(string)> "},
    {"DmuShutdown", DMU_SHUTDOWN, 1, ""},
};

void PrintShortHelp( void )
{
    int i, numofcmds = sizeof( cmdloop_cmdlist ) / sizeof( cmdloop_commands );
    printf( "Commands:\n" );

    for( i = 0; i < numofcmds; i++ )
    {
        printf( "  %s\t\t%s\n", cmdloop_cmdlist[i].str, cmdloop_cmdlist[i].args );
    }
}

int paraseCmd( char * cmdline )
{
    char cmd[1024];
    char strarg[1024];
    char strarg0[1024];
    int arg_val_err = -99999;
    int arg1 = arg_val_err;
    int arg2 = arg_val_err;
    int cmdnum = -1;
    int numofcmds = sizeof( cmdloop_cmdlist ) / sizeof( cmdloop_commands );
    int cmdfound = 0;
    int i;
    int invalidargs = 0;
    int validargs;

    if ( cmdline == NULL || cmdline[0] == '\n' ) return 0;

    validargs = sscanf( cmdline, "%s %d %d", cmd, &arg1, &arg2 );

    for( i = 0; i < numofcmds; i++ )
    {
        if( strcasecmp( cmd, cmdloop_cmdlist[i].str ) == 0 )
        {
            cmdnum = cmdloop_cmdlist[i].cmdnum;
            cmdfound++;
            if( validargs != cmdloop_cmdlist[i].numargs )
                invalidargs++;
            break;
        }
    }

    if( !cmdfound )
    {
        printf( "Command not found; try 'Help'\n" );
        return 0;
    }

    if( invalidargs )
    {
        printf( "Invalid arguments; try 'Help'\n" );
        return 0;
    }

    invalidargs = 0;

    switch ( cmdnum )
    {
    case HELP:
        PrintShortHelp();
        break;

    case RCP_PRINT:
        PrintDmrList();
        break;

    case RCP_GETDLNADOC:
        GetDlnaDoc( arg1 );
        break;

    case RCP_GETDLNACAP:
        GetDlnaCap( arg1 );
        break;

    case RCP_GETDEVCAP:
        GetDevCap( arg1 );
        break;

    case RCP_SUPPORTPLAYMODE:
        SupportPlayMode( arg1, arg2 );
        break;

    case RCP_SUPPORTVOLUME:
        SupportVolume( arg1 );
        break;

    case RCP_SUPPORTMUTE:
        SupportMute( arg1 );
        break;

    case RCP_PLAY:
        Play( arg1 );
        break;

    case RCP_SEEK:
        Seek( arg1, arg2 );
        break;

    case RCP_STOP:
        Stop( arg1 );
        break;

    case RCP_PAUSE:
        Pause( arg1 );
        break;

    case RCP_NEXT:
        Next( arg1 );
        break;

    case RCP_PREV:
        Prev( arg1 );
        break;

    case RCP_SETURI:
        validargs = sscanf( cmdline, "%s %d %s %s", cmd, &arg1, &arg2, strarg );
        if( 4 == validargs )
            SetUri( arg1, strarg );
        else
            invalidargs++;
        break;

    case RCP_SETVOLUME:
        SetVolume( arg1, arg2 );
        break;

    case RCP_SETMUTE:
        SetMute( arg1, arg2 );
        break;

    case RCP_SETPLAYMODE:
        SetPlayMode( arg1, arg2 );
        break;

    case RCP_GETMEDIAINFO:
        GetMediaInfo( arg1 );
        break;

    case RCP_GETPOSITION:
        GetPosition( arg1 );
        break;

    case MRCP_EXIT:
        return -1;
        break;

    case MCP_PRINT:
        PrintDmsList();
        break;

    case MCP_GETDLNADOC:
        MCP_GetDLNADOC( arg1 );
        break;

    case MCP_GETDLNACAP:
        MCP_GetDLNACAP( arg1 );
        break;

    case MCP_CDBROWSE:
        validargs = sscanf( cmdline, "%s %d %d %s", cmd, &arg1, &arg2, strarg );
        if( 4 == validargs )
            MCP_Browse( arg1, arg2, strarg );
        else
            invalidargs++;
        break;

    case DMD_INIT:
        //Create Downloader
        DMD_Initialize( MicroStackChain, ILib_Pool );
        break;

    case DMD_DOWNLOAD:
        validargs = sscanf( cmdline, "%s %d %s %s", cmd, &arg1, strarg0, strarg );
        if( 4 == validargs )
        {
            struct CdsResource * res = getCdsRes( strarg );
            if ( res )
            {
                DMD_DownloadRes( res, strarg0 );
            }
        }
        else
            invalidargs++;
        break;

    case DMD_SHUTDOWN:
        DMD_Shutdown( MicroStackChain );
        break;

    case DMU_INIT:
        //Create Uploader
        DMU_Initialize( MicroStackChain, ILib_Pool );
        break;

    case DMU_UPLOAD:
        validargs = sscanf( cmdline, "%s %d %s %s", cmd, &arg1, strarg0, strarg );
        if( 4 == validargs )
        {
            struct CdsObject * obj = getCdsObj( strarg );
            if ( obj )
            {
                DMD_UploadRes( obj, arg1, strarg );
            }
        }
        else
            invalidargs++;
        break;

    case DMU_SHUTDOWN:
        DMU_Shutdown( MicroStackChain );
        break;

    default:
        printf( "Command not implemented; see 'Help'\n" );
        break;
    }

    if( invalidargs )
        printf( "Invalid args in command; see 'Help'\n" );

    return 0;
}

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

DWORD WINAPI Run(LPVOID args)
{
    char cmdLine[1024] = { 0 };
    int ret = 0;

    do
    {
        printf( "DMR_DMS_CP_Program~# " );
        fgets( cmdLine, 1024, stdin );
        ret = paraseCmd( cmdLine );
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

void * DMR_CP_CommandLoop( void *args )
{
    char cmdline[1024];
    int ret = 0;

    do
    {
        printf( "DMR_DMS_CP_Program~# " );
        fgets( cmdline, 1024, stdin );
        ret = paraseCmd( cmdline );
    } while( ret != -1 );

    BreakSink( 0 );
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
    RCP_IPAddressChanged(TheRendererCP);
    MediaServerCP__CP_IPAddressListChanged(TheMediaServerCP);
    DMS_NotifyIPAddressChange( DMSObject );
}

int main( int argc, char **argv )
{
    struct DMS_BackEndInit backendinit;

    int x;

#if defined(_POSIX)
    struct sigaction setup_action;
    sigset_t block_mask;
    pthread_t t, cmdloopthread;
#else
    DWORD ptid = 0;
    DWORD ptid2 = 0;
#endif

    int dltest1 = -1, dltest2 = -1;
    dltest1 = ILibGetCurrentTimezoneOffset_Minutes();
    dltest2 = ILibIsDaylightSavingsTime();
    printf("dltest1 = %d, dltest2 = %d, daylight = %d, timezone = %ld, tzname[0] = %s, tzname[1] = %s\n", dltest1, dltest2, daylight, timezone, tzname[0], tzname[1]);
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

    /************************************************************************/
    MediaServer_GetConfiguration()->Manufacturer = "PPLive";
    MediaServer_GetConfiguration()->ManufacturerURL = "http://www.pptv.com";
    MediaServer_GetConfiguration()->ModelName = "PPTV";
    MediaServer_GetConfiguration()->ModelDescription = "PPLive Corp";
    MediaServer_GetConfiguration()->ModelNumber = "123456789";
    MediaServer_GetConfiguration()->ModelURL = "http://www.pptv.com";

    backendinit.Path = "./PPTV多媒体";
    DMSObject = DMS_Create(
        MicroStackChain,
        ILib_Pool,
        0,
        "LeoChen DMS",
        "396c5fea-aae0-407c-b9b5-309bcc8e8c0b",
        "0000001",
        0,
        0,
        "",
        "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:audio/3gpp:DLNA.ORG_PN=AMR_3GPP,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_320,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_SP_G726,http-get:*:image/png:DLNA.ORG_PN=PNG_LRG",
        &backendinit,
        NULL);
    /************************************************************************/

    printf("Digitial Media Controller\r\n\r\n");

    // Create and setup the Media Renderer Control Point's callbacks...
    TheRendererCP = CreateRendererCP(MicroStackChain);
    RendererAdded = &OnAddMediaRenderer;
    RendererRemoved = &OnRemoveMediaRenderer;

    // Create the Media Server Control Point...
    TheMediaServerCP = MediaServerCP_CreateControlPoint(MicroStackChain, &OnAddMediaServer, &OnRemoveMediaServer);

#if defined(_POSIX)
    ILib_Monitor = ILibCreateLifeTime(MicroStackChain);

    ILib_IPAddressLength = ILibGetLocalIPAddressList(&ILib_IPAddressList);
    ILibLifeTime_Add(ILib_Monitor,NULL,4,(void*)&ILib_IPAddressMonitor,NULL);

    pthread_create(&cmdloopthread,NULL,&DMR_CP_CommandLoop,NULL);

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

