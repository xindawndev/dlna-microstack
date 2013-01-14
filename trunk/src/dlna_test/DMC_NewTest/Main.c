#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "dmc.h"

int g_stop = 0;

enum cmdloop_dmrcmds
{
    // RCP
    HELP = 0, RCP_PRINT, RCP_GETDLNADOC, RCP_GETDLNACAP, RCP_GETDEVCAP, RCP_SUPPORTPLAYMODE,
    RCP_SUPPORTVOLUME, RCP_SUPPORTMUTE, RCP_PLAY, RCP_SEEK, RCP_STOP, RCP_PAUSE,
    RCP_NEXT, RCP_PREV, RCP_SETURI, RCP_SETVOLUME, RCP_SETMUTE, RCP_SETPLAYMODE,
    RCP_GETMEDIAINFO, RCP_GETPOSITION, RCP_GETTRANSPORTINFO, MRCP_EXIT,
};

struct cmdloop_commands
{
    char    * str;
    int     cmdnum;
    int     numargs;
    char    * args;
} cmdloop_commands;

static struct cmdloop_commands cmdloop_cmdlist[] = {
    {"Help", HELP, 1, ""},
    {"LsDmr", RCP_PRINT, 1, ""},
    {"DmrDoc", RCP_GETDLNADOC, 2, "<dev_udn>"},
    {"DmrCap", RCP_GETDLNACAP, 2, "<dev_udn>"},
    {"DmrDevCap", RCP_GETDEVCAP, 2, "<dev_udn>"},
    {"DmrSupPlayMode", RCP_SUPPORTPLAYMODE, 3, "<dev_udn> <playmode (int)>"},
    {"DmrSupVolume", RCP_SUPPORTVOLUME, 2, "<dev_udn>"},
    {"DmrSupMute", RCP_SUPPORTMUTE, 2, "<dev_udn>"},
    {"DmrPlay", RCP_PLAY, 2, "<dev_udn>"},
    {"DmrSeek", RCP_SEEK, 3, "<dev_udn> <position (int)>"},
    {"DmrStop", RCP_STOP, 2, "<dev_udn>"},
    {"DmrPause", RCP_PAUSE, 2, "<dev_udn>"},
    {"DmrNext", RCP_NEXT, 2, "<dev_udn>"},
    {"DmrPrev", RCP_PREV, 2, "<dev_udn>"},
    {"DmrSetUri", RCP_SETURI, 3, "<dev_udn> <url (string)>"},
    {"DmrSetVol", RCP_SETVOLUME, 3, "<dev_udn> <volume (int)>"},
    {"DmrSetMute", RCP_SETMUTE, 3, "<dev_udn> <ismute (bool)>"},
    {"DmrSetPlayMode", RCP_SETPLAYMODE, 3, "<dev_udn> <playmode (int)>"},
    {"DmrGetMediaInfo", RCP_GETMEDIAINFO, 2, "<dev_udn>"},
    {"DmrGetPos", RCP_GETPOSITION, 2, "<dev_udn>"},
    {"DmrGetTra", RCP_GETTRANSPORTINFO, 2, "<dev_udn>"},
    {"Exit", MRCP_EXIT, 1, ""},
};

void printShortHelp( void )
{
    int i, numofcmds = sizeof( cmdloop_cmdlist ) / sizeof( cmdloop_commands );
    printf( "Commands:\n" );

    for( i = 0; i < numofcmds; i++ ) {
        printf( "  %s\t\t%s\n", cmdloop_cmdlist[i].str, cmdloop_cmdlist[i].args );
    }
}

int to_int(char * str)
{
    char *ptr = str;
    int ret = 0;
    if (!str) return -1;
    do {
        if (*ptr < '0' || *ptr > '9') return -1;
        ret *= 10;
        ret += *ptr - '0';
    } while (*(++ptr));

    return ret;
}

int paraseCmd( char * cmdline )
{
    char cmd[1024];
    char arg_val_err[256] = {0};
    char arg1[256] = {0};
    char arg2[256] = {0};
    int cmdnum = -1;
    int numofcmds = sizeof( cmdloop_cmdlist ) / sizeof( cmdloop_commands );
    int cmdfound = 0;
    int i;
    int invalidargs = 0;
    int validargs;
    char * ret = NULL;
    int nret;

    if ( cmdline == NULL || cmdline[0] == '\n' ) return 0;

    validargs = sscanf( cmdline, "%s %s %s", cmd, arg1, arg2 );

    for( i = 0; i < numofcmds; i++ ) {
#if defined(WIN32)
        if( _stricmp( cmd, cmdloop_cmdlist[i].str ) == 0 ) {
#else
        if( strcasecmp( cmd, cmdloop_cmdlist[i].str ) == 0 ) {
#endif
            cmdnum = cmdloop_cmdlist[i].cmdnum;
            cmdfound++;
            if( validargs != cmdloop_cmdlist[i].numargs )
                invalidargs++;
            break;
        }
    }

    if( !cmdfound ) {
        printf( "Command not found; try 'Help'\n" );
        return 0;
    }

    if( invalidargs ) {
        printf( "Invalid arguments; try 'Help'\n" );
        return 0;
    }

    invalidargs = 0;

    switch ( cmdnum )
    {
    case HELP:
        printShortHelp();
        break;

    case RCP_PRINT:
        printDmrList();
        break;

    case RCP_GETDLNADOC:
        ret = dmrGetDlnaDoc( arg1 );
        printf("++++++%s dmrGetDlnaDoc = %s ++++++", __FUNCTION__, ret ? ret : "");
        break;

    case RCP_GETDLNACAP:
        ret = dmrGetDlnaCap( arg1 );
        printf("++++++%s dmrGetDlnaCap = %s ++++++", __FUNCTION__, ret ? ret : "");
        break;

    case RCP_GETDEVCAP:
        dmrGetDevCap( arg1 );
        break;

    case RCP_SUPPORTPLAYMODE:
        nret = dmrSupportPlayMode( arg1, to_int(arg2) );
        printf("++++++%s dmrSupportPlayMode = %s ++++++", __FUNCTION__, nret ? "true" : "false");
        break;

    case RCP_SUPPORTVOLUME:
        nret = dmrSupportVolume( arg1 );
        printf("++++++%s dmrSupportVolume = %s ++++++", __FUNCTION__, nret ? "true" : "false");

        break;

    case RCP_SUPPORTMUTE:
        nret = dmrSupportMute( arg1 );
        printf("++++++%s dmrSupportMute = %s ++++++", __FUNCTION__, nret ? "true" : "false");
        break;

    case RCP_PLAY:
        dmrPlay( arg1 );
        break;

    case RCP_SEEK:
        dmrSeek( arg1, to_int(arg2) );
        break;

    case RCP_STOP:
        dmrStop( arg1 );
        break;

    case RCP_PAUSE:
        dmrPause( arg1 );
        break;

    case RCP_NEXT:
        dmrNext( arg1 );
        break;

    case RCP_PREV:
        dmrPrev( arg1 );
        break;

    case RCP_SETURI:
        dmrSetUri( arg1, arg2 );
        break;

    case RCP_SETVOLUME:
        dmrSetVolume( arg1, to_int(arg2) );
        break;

    case RCP_SETMUTE:
        dmrSetMute( arg1, to_int(arg2) );
        break;

    case RCP_SETPLAYMODE:
        dmrSetPlayMode( arg1, to_int(arg2) );
        break;

    case RCP_GETMEDIAINFO:
        dmrGetMediaInfo( arg1 );
        break;

    case RCP_GETPOSITION:
        dmrGetPosition( arg1 );
        break;

    case RCP_GETTRANSPORTINFO:
        dmrGetTransportInfo( arg1 );
        break;

    case MRCP_EXIT:
        return -1;
        break;

    default:
        printf( "Command not implemented; see 'Help'\n" );
        break;
    }

    if( invalidargs )
        printf( "Invalid args in command; see 'Help'\n" );

    return 0;
}

#if defined(WIN32)
#include <windows.h>

DWORD WINAPI run(LPVOID args)
#else
void * run(void * args)
#endif
{
    char cmdLine[1024] = { 0 };
    int ret = 0;

    do
    {
        printf( "DMC_Program~# " );
        fgets( cmdLine, 1024, stdin );
        ret = paraseCmd( cmdLine );
    } while ( ret != -1 );

    stopAVRCP();
    g_stop = 1;
    return 0;
}

int main(int argc, char **argv)
{
#if defined(WIN32)
    DWORD ptid;
#else
    pthread_t cmdloop_thread;
#endif

    // regist callback functions
    avrender_add         = callback_add;
    avrender_del         = callback_del;
    avrender_update      = callback_update;
    getdevcap_callback   = callback_getdevcap;
    play_callback        = callback_play;
    seek_callback        = callback_seek;
    stop_callback        = callback_stop;
    pause_callback       = callback_pause;
    next_callback        = callback_next;
    prev_callback        = callback_prev;
    seturi_callback      = callback_seturi;
#if defined(INCLUDE_FEATURE_VOLUME)
    setvol_callback      = callback_setvol;
    setmute_callback     = callback_setmute;
#endif
    setplaymode_callback = callback_setplaymode;
    getmediainfo_callback= callback_getmediainfo;
    getposition_callback = callback_getposition;
    gettransportinfo_callback = callback_gettransportinfo;

#if defined(WIN32)
    // command thread
    CreateThread(NULL, 0, &run, NULL, 0, &ptid);
#else
    pthread_create(&cmdloop_thread, NULL, &run, NULL);
#endif

    startAVRCP(3);

    while (g_stop != 1);

    return 0;
}
