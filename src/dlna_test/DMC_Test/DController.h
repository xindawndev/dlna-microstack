#ifndef _DLCONTROLLER_H_
#define _DLCONTROLLER_H_

#include "DownloadController.h"

DMD_Session dmds = NULL;

void printHelp()
{
    printf( "exit             Exit download mode.\n" );
    printf( "supresume        Check whether the server supports resume using RANGE requests.\n" );
    printf( "ispaused         Find out if the download session is paused by disconnecting.\n" );
    printf( "ispausedfc       Find out if the download session is paused using TCP flow control.\n" );
    printf( "pause            Pause download session by disconnecting and resume using RANGE.\n" );
    printf( "pausefc          Pause download session using TCP flow control.\n" );
    printf( "resume           Allows the application to resume the download session from the last downloaded byte position.\n" );
    printf( "resumefc         Allows the application to resume the download session using TCP flow control.\n" );
    printf( "abort            Allows the application to abort the transfer progress of the download session.\n" );
    printf( "static           Get download process.\n" );
    printf( "help             Display me.\n" );
}

int procCmd( DMD_Session session, char * cmdline )
{
    int ret = 0;
    int total = 1, cursize = 0;
    if ( strcmp( cmdline, "exit\n" ) == 0 )
    {
        ret = -1;
    }
    else if ( strcmp( cmdline, "supresume\n" ) == 0 )
    {
        if ( DMD_IsResumeSupported( session ) )
        {
            printf( "The server supports resume using RANGE requests.\n" );
        }
        else
        {
            printf( "The server NOT supports resume using RANGE requests.\n" );
        }
    } 
    else if ( strcmp( cmdline, "ispaused\n" ) == 0 )
    {
        if ( DMD_IsPaused( session ) )
        {
            printf( "The transfer is PAUSED.\n" );
        }
        else
        {
            printf( "The transfer is NOT PAUSED.\n" );
        }
    }
    else if ( strcmp( cmdline, "ispausedfc\n" ) == 0 )
    {
        if ( DMD_IsPausedByFlowControl( session ) )
        {
            printf( "The transfer is PAUSED.(TCP)\n" );
        }
        else
        {
            printf( "The transfer is NOT PAUSED.(TCP)\n" );
        }
    }
    else if ( strcmp( cmdline, "pause\n" ) == 0 )
    {
        if ( DMD_PauseTransfer( session ) )
        {
            printf( "The transfer is PAUSED SUCCESS!\n" );
        }
        else
        {
            printf( "The transfer is PAUSED FAIL!\n" );
        }
    }
    else if ( strcmp( cmdline, "pausefc\n" ) == 0 )
    {
        if ( DMD_PauseTransferByFlowControl( session ) )
        {
            printf( "The transfer is PAUSED SUCCESS!(TCP)\n" );
        }
        else
        {
            printf( "The transfer is PAUSED FAIL!(TCP)\n" );
        }
    }
    else if ( strcmp( cmdline, "resume\n" ) == 0 )
    {
        if ( DMD_ResumeTransfer( session ) )
        {
            printf( "The transfer is RESUME SUCCESS!\n" );
        }
        else
        {
            printf( "The transfer is RESUME FAIL!\n" );
        }
    }
    else if ( strcmp( cmdline, "resumefc\n" ) == 0 )
    {
        if ( DMD_ResumeTransferByFlowControl( session ) )
        {
            printf( "The transfer is RESUME SUCCESS!(TCP)\n" );
        }
        else
        {
            printf( "The transfer is RESUME FAIL!\n(TCP)" );
        }
    }
    else if ( strcmp( cmdline, "abort\n" ) == 0 )
    {
        if ( DMD_AbortTransfer( session ) )
        {
            printf( "The transfer is ABORT SUCCESS!\n" );
        }
        else
        {
            printf( "The transfer is ABORT FAIL!\n" );
        }
    }
    else if ( strcmp( cmdline, "static\n" ) == 0 )
    {
        cursize = DMD_GetBytesReceived(session);
        total = DMD_GetTotalBytesExpected(session);
        printf( "Received:%d/%d                     (%f\%%)\n", cursize, total, 100.0f * cursize/total );
    }
    else if ( strcmp( cmdline, "help\n" ) == 0 )
    {
        printHelp();
    }
    else
    {
        printf( "Unknown Command!\n" );
    }

    return ret;
}

void DMD_CommandLoop( DMD_Session session )
{
    char cmdline[1024];
    int ret = 0;

    do
    {
        printf( "DMD_Program~# " );
        fgets( cmdline, 1024, stdin );
        ret = procCmd( session, cmdline );
    } while( ret != -1 );
}

void On_DownloadResp(DMD_Session session, enum DMD_Errors error_code, int http_error_code, void* user_obj)
{
    printf( "\n(%d)%s: http_error_code:%d\n", __LINE__, __FUNCTION__, http_error_code );
    switch ( error_code )
    {
    case DMD_ERROR_None:
        printf( "Download Success!\n" );
        break;
    case DMD_ERROR_FailedOrAborted:
        printf( "Transfer aborted or failed.\n" );
        break;
    case DMD_ERROR_FileNotExist:
        printf( "File does not exist on server.\n" );
        break;
    case DMD_ERROR_HttpError:
        printf( "Http errors.\n" );
        break;
    case DMD_ERROR_Unknown:
        printf( "Unknown error.\n" );
        break;
    case DMD_ERROR_DISK_FULL:
        printf( "Disk is full.\n" );
        break;
    default:
        printf( "NIL\n" );
        return;
    }
};

void DMD_DownloadRes( struct CdsResource * res, char * save_as_file_name )
{
    dmds = DMD_DownloadObject( res, 0, save_as_file_name, On_DownloadResp, NULL );
    if ( !dmds )
    {
        printf( "DownloadObj is NULL!\n" );
        return;
    }
    DMD_CommandLoop( dmds );
}

#endif // _DLCONTROLLER_H_
