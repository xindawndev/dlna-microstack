#ifndef _UPCONTROLLER_H_
#define _UPCONTROLLER_H_

#include "UploadController.h"

DMU_Session dmus = NULL;

void printDmuHelp()
{
    printf( "exit             Exit upload mode.\n" );
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

int procDmuCmd( DMU_Session session, char * cmdline )
{
    int ret = 0;
    int total = 1, cursize = 0;
    if ( strcmp( cmdline, "exit\n" ) == 0 )
    {
        ret = -1;
    }
    else if ( strcmp( cmdline, "supresume\n" ) == 0 )
    {
        if ( DMU_IsResumeSupported( session ) )
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
        if ( DMU_IsPaused( session ) )
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
        if ( DMU_IsPausedByFlowControl( session ) )
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
        if ( DMU_PauseTransfer( session ) )
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
        if ( DMU_PauseTransferByFlowControl( session ) )
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
        if ( DMU_ResumeTransfer( session ) )
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
        if ( DMU_ResumeTransferByFlowControl( session ) )
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
        if ( DMU_AbortTransfer( session ) )
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
        cursize = DMU_GetBytesSent(session);
        total = DMU_GetTotalBytes(session);
        printf( "Received:%d/%d                     (%f\%%)\n", cursize, total, 100.0f * cursize/total );
    }
    else if ( strcmp( cmdline, "help\n" ) == 0 )
    {
        printDmuHelp();
    }
    else
    {
        printf( "Unknown Command!\n" );
    }

    return ret;
}

void DMU_CommandLoop( DMU_Session session )
{
    char cmdline[1024];
    int ret = 0;

    do
    {
        printf( "DMU_Program~# " );
        fgets( cmdline, 1024, stdin );
        ret = procDmuCmd( session, cmdline );
    } while( ret != -1 );
}

void On_UploadResp(DMU_Session session, enum DMU_Errors error_code, int http_error_code, struct CdsObject *result, void* user_obj)
{
    printf( "\n(%d)%s: http_error_code:%d\n", __LINE__, __FUNCTION__, http_error_code );
    switch ( error_code )
    {
    case DMD_ERROR_None:
        printf( "Upload Success!\n" );
        break;
    case DMU_ERROR_FailedOrAborted:
        printf( "Transfer aborted or failed.\n" );
        break;
    case DMU_ERROR_ResumeOrRetryFailed:
        printf( "Resume or retry transfer failed.\n" );
        break;
    case DMU_ERROR_ServerNotExist:
        printf( "Server for this container is no longer valid.\n" );
        break;
    case DMU_ERROR_HttpError:
        printf( "Http errors.\n" );
        break;
    case DMU_ERROR_InvalidContext:
        printf( "Specified container change is not allowed.\n" );
        break;

    case DMU_ERROR_XmlNotWellFormed:
        printf( "XML returned not well formed.\n" );
        break;
    case DMU_ERROR_CreateObjectFailed:
        printf( "Failed to create object.\n" );
        break;
    case DMU_ERROR_ImportUriNotAvailble:
        printf( "IimportUri is not returned in the response.\n" );
        break;
    default:
        printf( "Unknown Errors: %d\n", error_code );
        return;
    }
};

void DMD_UploadRes( struct CdsObject * parent, int is_item, char * save_as_file_name )
{
    struct CdsObject * new_object = NULL;

    dmus = DMU_CreateObjectAndUpload(
        1, is_item ? 1 : 0, parent, new_object, is_item ? save_as_file_name : NULL , On_UploadResp, NULL);

    if ( !dmus )
    {
        printf( "UploadObj is NULL!\n" );
        return;
    }
    DMU_CommandLoop( dmus );
}

#endif // _UPCONTROLLER_H_
