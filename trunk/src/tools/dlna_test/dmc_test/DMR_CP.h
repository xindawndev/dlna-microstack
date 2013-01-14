#ifndef _DMR_CP_H_
#define _DMR_CP_H_

#include "MicroAVRCP.h"

#include "DMRCP_ControlPoint.h"

struct _tDmrInfo
{
    void * dmr_token;
    void * render;
} DmrInfo;

static void * dmrinfos = NULL;
static int g_dmr_flag = 0;

/************************************************************************/
/* Callback Functions                                                   */
/************************************************************************/

void OnGetDevCapSink(struct AVRendererConnection * avrc,int ErrorCode, void *Tag, char* PlayMedia,char* RecMedia,char* RecQualityModes)
{
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );

    if ( !ErrorCode )
    {
        printf( "Success!\n" );
        printf( "PlayMedia:%s\n", PlayMedia );	
        printf( "RecMedia:%s\n", RecMedia );	
        printf( "RecQualityModes:%s\n", RecQualityModes );
    }
}

void OnPlaySink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnPlaySink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnPlaySink: Success!\n", ErrorCode );
        printf( "ProtocolInfo:%s\n", avrc->ProtocolInfo );	
        printf( "MediaUri:%s\n", avrc->MediaUri );	
        printf( "TrackUri:%s\n", avrc->TrackUri );
    }
}

void OnSeekSink(struct AVRendererConnection *avrc,int ErrorCode, void *Tag)
{
    printf( "OnSeekSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnSeekSink: Success!\n", ErrorCode );
    }
}

void OnStopSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnStopSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnStopSink: Success!\n", ErrorCode );
    }
}

void OnPauseSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnPauseSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnPauseSink: Success!\n", ErrorCode );
    }
}

void OnNextSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnNextSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnNextSink: Success!\n", ErrorCode );
    }
}

void OnPrevSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnPrevSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnPrevSink: Success!\n", ErrorCode );
    }
}

void OnSetUriSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "SetUriSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetUriSink: Success!\n", ErrorCode );
    }
}

void OnSetVolumeSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnSetVolumeSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetVolumeSink: Success!\n", ErrorCode );
    }
}

void OnSetMuteSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnSetMuteSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetMuteSink: Success!\n", ErrorCode );
    }
}

void OnSetPlayModeSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnSetPlayModeSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetPlayModeSink: Success!\n", ErrorCode );
    }
}

void OnGetMediaInfoSink( struct AVRendererConnection * avrc,int ErrorCode, int nrTracks, int mediaDuration, char * curUrI, char * nextURI,void * Tag )
{
    printf( "OnGetMediaInfoSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnGetMediaInfoSink: Success!\n", ErrorCode );
        printf( "NrTracks = %d, MediaDuration = %d, CurUrl = %s, NextUrl = %s\n", nrTracks, mediaDuration, curUrI, nextURI );
    }
}

void OnGetPositionSink(struct AVRendererConnection * avrc, int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter,void *Tag )
{
    printf( "OnGetPositionSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnGetPositionSink: Success!\n", ErrorCode );
        printf( "RelativeSeconds = %d, AbsoluteSeconds = %d, RelativeCounter = %d, AbsoluteCounter = %d\n", RelativeSeconds, AbsoluteSeconds, RelativeCounter, AbsoluteCounter );
    }
}

void OnGetTransportInfoSink(struct AVRendererConnection * avrc, int ErrorCode, char * CurrentTransportState, char* CurrentTransportStatus, char* CurrentSpeed,void * Tag )
{
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf("CurrentTransportState = %s\n", CurrentTransportState ? CurrentTransportState : "");
        printf("CurrentTransportStatus = %s\n", CurrentTransportStatus ? CurrentTransportStatus : "");
        printf("CurrentSpeed = %s\n", CurrentSpeed ? CurrentSpeed : "");
    }
}

void OnGetMuteSink(struct AVRendererConnection *connection, int ErrorCode, int CurrentMute, void * Tag)
{
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf("Current Volume = %d\n", CurrentMute);
    }
}

void OnGetVolumeSink(struct AVRendererConnection *connection, int ErrorCode, unsigned short CurrentVolume, void * Tag)
{
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf("Current Volume = %d\n", CurrentVolume);
    }
}

/************************************************************************/
/* Interface Test Functions                                             */
/************************************************************************/

struct _tDmrInfo * _getDmrInfo( int id )
{
    char Key[128] = { 0 };
    int Len = 128, i = 0, found = 0;
    struct _tDmrInfo * Val = NULL;
    void * dmr_enum = NULL;

    if ( dmrinfos == NULL ) return NULL;

    ILibHashTree_Lock( dmrinfos );
    dmr_enum = ILibHashTree_GetEnumerator( dmrinfos );
    if ( dmr_enum == NULL ) return NULL;

    while ( !ILibHashTree_MoveNext( dmr_enum ) )
    {
        ILibHashTree_GetValue( dmr_enum, &Key, &Len, ( ( void ** )( &Val ) ) );
        if ( id == i++ )
        {
            found = 1;
            break;
        }
    }

    ILibHashTree_DestroyEnumerator( dmr_enum );

    ILibHashTree_UnLock( dmrinfos );
    return ( found != 0 ) ? Val : NULL;
}

void PrintDmrList()
{
    char Key[128] = { 0 };
    int Len = 128, i = 0;
    struct _tDmrInfo * Val = NULL;
    void * dmr_enum = NULL;

    if ( dmrinfos == NULL ) return;
    ILibHashTree_Lock( dmrinfos );

    dmr_enum = ILibHashTree_GetEnumerator( dmrinfos );
    if ( dmr_enum == NULL ) return;
    while ( !ILibHashTree_MoveNext( dmr_enum ) )
    {
        ILibHashTree_GetValue( dmr_enum, &Key, &Len, ( ( void ** )( &Val ) ) );
        printf( "\t%d. %s [%s]\n", i++, ((struct AVRenderer*)(Val->render))->FriendlyName, ((struct AVRenderer*)(Val->render))->device->UDN );
    }
    ILibHashTree_DestroyEnumerator( dmr_enum );

    ILibHashTree_UnLock( dmrinfos );
}

void GetDlnaDoc( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }

    printf( "%s\n", RCP_GetDLNADOC( Val->render ) );
}

void GetDlnaCap( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
	char * ret = NULL;
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    //printf( "%s\n", RCP_GetDLNACAP( Val->render ) );
    ret = RCP_GetDLNACAP( Val->render );
	if ( ret ) printf( ret  );
}

void GetDevCap( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    char * ret = NULL;
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_GetDeviceCap( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnGetDevCapSink );
}

void SupportPlayMode( int arg1, int arg2 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    printf( "%s\n", RCP_SupportPlayMode( Val->render, arg2 ) ? "Support this mode!" : "Not support!" );
}

void SupportVolume( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    printf( "%s\n", RCP_SupportVolume( Val->render ) ? "Support volume!" : "Not support volume!" );
}

void SupportMute( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    printf( "%s\n", RCP_SupportMute( Val->render ) ? "Support mute!" : "Not support mute!" );
}

void Play( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_Play( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnPlaySink );
}

void Seek( int arg1, int arg2 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_Seek( ((struct AVRenderer *)(Val->render))->Connection, arg2, NULL, OnSeekSink );
}

void Stop( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_Stop( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnStopSink );
}

void Pause( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_Pause( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnPauseSink );
}

void Next( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_Next( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnNextSink );
}

void Prev( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_Prev( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnPrevSink );
}

void SetUri( int arg1, char * uri )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_SetUri( ((struct AVRenderer *)(Val->render))->Connection, uri, NULL, OnSetUriSink );
}

void SetVolume( int arg1, int arg2 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_SetVolume( ((struct AVRenderer *)(Val->render))->Connection, "Master", ( char )( arg2 ), NULL, OnSetVolumeSink );
}

void SetMute( int arg1, int arg2 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_SetMute( ((struct AVRenderer *)(Val->render))->Connection, "Master", arg2, NULL, OnSetMuteSink );
}

void SetPlayMode( int arg1, int arg2 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_SetPlayMode( ((struct AVRenderer *)(Val->render))->Connection, arg2, NULL, OnSetPlayModeSink );
}

void GetMediaInfo( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_GetMediaInfo( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnGetMediaInfoSink );
}

void GetPosition( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_GetPosition( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnGetPositionSink );
}

void GetTransportInfo( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_GetTransportInfo( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnGetTransportInfoSink );
}

void GetMute( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_GetMute( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnGetMuteSink );
}

void GetVolume( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    RCP_GetVolume( ((struct AVRenderer *)(Val->render))->Connection, NULL, OnGetVolumeSink );
}

void RefreshDevice()
{
    RCP_SearchDevice();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
//static lock_t gsem;

void OnRenderStateChanged( struct AVRenderer * sender,struct AVRendererConnection * Connection )
{
    int i;
    //lock_wait( gsem );
    printf( "%s State changed                                       Begin\n", sender->FriendlyName );
    printf( "ProtocolInfo = %s\n", Connection->ProtocolInfo );
    printf( "MediaUri = %s\n", Connection->MediaUri );
    printf( "TrackUri = %s\n", Connection->TrackUri );
    printf( "TransportStatus = %s\n", Connection->TransportStatus );

    printf( "TotalTracks = %d\n", Connection->TotalTracks );
    printf( "TrackNumber = %d\n", Connection->TrackNumber );

    printf( "Play State = " );
    switch ( Connection->PlayState )
    {
    case AVRCP_PLAYING:
        printf( "AVRCP_PLAYING\n" );
        break;
    case AVRCP_STOPPED:
        printf( "AVRCP_STOPPED\n" );
        break;
    case AVRCP_PAUSED:
        printf( "AVRCP_PAUSED\n" );
        break;
    case AVRCP_RECORDING:
        printf( "AVRCP_RECORDING\n" );
        break;
    case AVRCP_TRANSITIONING:
        printf( "AVRCP_TRANSITIONING\n" );
        break;
    case AVRCP_NO_MEDIA:
        printf( "AVRCP_NO_MEDIA\n" );
        break;
    default:
        printf( "AVRCP_UNKNOWN\n" );
        break;
    }
    printf( "Play Mode = " );
    switch ( Connection->PlayMode )
    {
    case AVRCP_NORMAL:
        printf( "AVRCP_NORMAL\n" );
        break;
    case AVRCP_REPEAT_ALL:
        printf( "AVRCP_REPEAT_ALL\n" );
        break;
    case AVRCP_REPEAT_ONE:
        printf( "AVRCP_REPEAT_ONE\n" );
        break;
    case AVRCP_RANDOM:
        printf( "AVRCP_RANDOM\n" );
        break;
    case AVRCP_SHUFFLE:
        printf( "AVRCP_SHUFFLE\n" );
        break;
    case AVRCP_INTRO:
        printf( "AVRCP_INTRO\n" );
        break;
    default:
        printf( "AVRCP_INVALID\n" );
        break;
    }

    printf( "Duration = %d\n", Connection->Duration );

    printf( "TrackPosition = %s\n", Connection->TrackPosition );
    //struct CdsObject* TrackObject;

    for ( i = 0; i < Connection->SupportedRecordQualityModesLength; ++i )
    {
        printf( "SupportedRecordQualityModes[%d] = %s\n", i, Connection->SupportedRecordQualityModes[i] );
    }

    for ( i = 0; i < Connection->PlayMediaLength; ++i )
    {
        printf( "PlayMedia[%d] = %s\n", i, Connection->PlayMedia[i] );
    }

    for ( i = 0; i < Connection->RecordMediaLength; ++i )
    {
        printf( "RecordMedia[%d] = %s\n", i, Connection->RecordMedia[i] );
    }

    for ( i = 0; i < Connection->ChannelCount; ++i )
    {
        printf( "Channel[%d] = %s\n", i, Connection->Channel[i] );
    }

    if (Connection->Volume)
        printf( "Volume = %d\n", Connection->Volume[0] );
    if (Connection->Mute)
        printf( "Mute = %d\n", *( Connection->Mute ) );

    printf( "%s State changed                                       End\n", sender->FriendlyName );

    //lock_post( gsem );
}

void OnAddMediaRenderer(void* mediaRendererToken, struct AVRenderer* mediaRenderer)
{
    struct _tDmrInfo * dmr;
    if ( !g_dmr_flag )
    {
        dmrinfos = ILibInitHashTree();
        //lock_init( gsem );
    }
    g_dmr_flag++;

    dmr = ( struct _tDmrInfo * )malloc( sizeof( struct _tDmrInfo ) );

    if ( dmr == NULL ) return;

    dmr->dmr_token = mediaRendererToken;
    dmr->render = mediaRenderer;
    mediaRenderer->StateChanged = OnRenderStateChanged;

    ILibAddEntry( dmrinfos, mediaRenderer->device->UDN, strlen( mediaRenderer->device->UDN ), ( void * )dmr );
    printf( "[Leochen] AVRender USN: %s\n", mediaRenderer->device->UDN );
}

void OnRemoveMediaRenderer(void* mediaRendererToken, struct AVRenderer* mediaRenderer, int error_code)
{
    void * dmrinfo;

    if ( !dmrinfos ) return;
    dmrinfo = ILibGetEntry( dmrinfos, mediaRenderer->device->UDN, strlen( mediaRenderer->device->UDN ) );
    ILibDeleteEntry( dmrinfos, mediaRenderer->device->UDN, strlen( mediaRenderer->device->UDN ) );
    free( dmrinfo );

    if ( !( --g_dmr_flag ) )
    {
        ILibDestroyHashTree( dmrinfos );
        dmrinfos = NULL;
    }
    printf( "[Leochen] AVRender %s[%s] leave... error = %d\n", mediaRenderer->FriendlyName, mediaRenderer->device->UDN, error_code);
}

#endif // _DMR_CP_H_
