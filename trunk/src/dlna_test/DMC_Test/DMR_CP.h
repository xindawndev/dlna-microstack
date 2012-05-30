#ifndef _DMR_CP_H_
#define _DMR_CP_H_

extern "C" {
#include "MicroAVRCP.h"
#include "DMRCP_ControlPoint.h"
};

#include <map>
#include <string>

struct _tDmrInfo
{
    int index;
    void * dmr_token;
    void * render;
} DmrInfo;

static std::map<std::string, struct _tDmrInfo *> dmrinfos;
static int g_dmr_count = 0;

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
        printf( "OnPlaySink: Success!\n" );
        printf( "ProtocolInfo:%s\n", avrc->ProtocolInfo );	
        printf( "MediaUri:%s\n", avrc->MediaUri );	
        printf( "TrackUri:%s\n", avrc->TrackUri );
    }
}

void OnSeekSink(struct AVRendererConnection *sender,int ErrorCode, void *Tag)
{
    printf( "OnSeekSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnSeekSink: Success!\n" );
    }
}

void OnStopSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnStopSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnStopSink: Success!\n" );
    }
}

void OnPauseSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnPauseSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnPauseSink: Success!\n" );
    }
}

void OnNextSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnNextSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnNextSink: Success!\n" );
    }
}

void OnPrevSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnPrevSink:ErrorCode:%d\n", ErrorCode );

    if ( !ErrorCode )
    {
        printf( "OnPrevSink: Success!\n" );
    }
}

void OnSetUriSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "SetUriSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetUriSink: Success!\n" );
    }
}

void OnSetVolumeSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnSetVolumeSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetVolumeSink: Success!\n" );
    }
}

void OnSetMuteSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnSetMuteSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetMuteSink: Success!\n" );
    }
}

void OnSetPlayModeSink( struct AVRendererConnection * avrc, int ErrorCode, void * tag )
{
    printf( "OnSetPlayModeSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetPlayModeSink: Success!\n" );
    }
}

void OnGetMediaInfoSink( struct AVRendererConnection * avrc,int ErrorCode, int nrTracks, int mediaDuration, char * curUrI, char * nextURI,void * Tag )
{
    printf( "OnGetMediaInfoSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnGetMediaInfoSink: Success!\n" );
        printf( "NrTracks = %d, MediaDuration = %d, CurUrl = %s, NextUrl = %s\n", nrTracks, mediaDuration, curUrI, nextURI );
    }
}

void OnGetPositionSink(struct AVRendererConnection * avrc, int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter,void *Tag )
{
    printf( "OnGetPositionSink:ErrorCode:%d\n", ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnGetPositionSink: Success!\n" );
        printf( "RelativeSeconds = %d, AbsoluteSeconds = %d, RelativeCounter = %d, AbsoluteCounter = %d\n", RelativeSeconds, AbsoluteSeconds, RelativeCounter, AbsoluteCounter );
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

    if ( dmrinfos.empty() ) return NULL;

    std::map<std::string, struct _tDmrInfo * >::iterator iter;
    for (iter = dmrinfos.begin(); iter != dmrinfos.end(); ++iter)
    {
        if (iter->second->index == id)
        {
            return iter->second;
        }
    }

    return NULL;

}

void PrintDmrList()
{
    char Key[128] = { 0 };
    int Len = 128;
    struct _tDmrInfo * Val = NULL;
    void * dmr_enum = NULL;

    if ( dmrinfos.empty() ) return;

    std::map<std::string, struct _tDmrInfo * >::iterator iter;
    for (iter = dmrinfos.begin(); iter != dmrinfos.end(); ++iter)
    {
        printf( "\t%d. %s [%s]\n", iter->second->index, ((struct AVRenderer*)(iter->second->render))->FriendlyName, ((struct AVRenderer*)(iter->second->render))->device->UDN );
    }
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
    printf( "%s\n", RCP_SupportPlayMode( (struct AVRenderer *)Val->render, (enum PlayModeEnum)arg2 ) ? "Support this mode!" : "Not support!" );
}

void SupportVolume( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    printf( "%s\n", RCP_SupportVolume( (struct AVRenderer *)Val->render ) ? "Support volume!" : "Not support volume!" );
}

void SupportMute( int arg1 )
{
    struct _tDmrInfo * Val = _getDmrInfo( arg1 );
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }
    printf( "%s\n", RCP_SupportMute( (struct AVRenderer *)Val->render ) ? "Support mute!" : "Not support mute!" );
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
    RCP_SetPlayMode( ((struct AVRenderer *)(Val->render))->Connection, (enum PlayModeEnum)arg2, NULL, OnSetPlayModeSink );
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

/************************************************************************/
/*                                                                      */
/************************************************************************/
//static sem_t gsem;

void OnRenderStateChanged( struct AVRenderer * sender,struct AVRendererConnection * Connection )
{
    int i;
    //sem_wait( gsem );
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

    printf( "Volume = %d\n", Connection->Volume[0] );
    printf( "Mute = %d\n", *( Connection->Mute ) );

    printf( "%s State changed                                       End\n", sender->FriendlyName );

    //sem_post( gsem );
}

void deleteKey(std::string const & key)
{
    if (dmrinfos.empty()) return;
    std::map<std::string, struct _tDmrInfo * >::iterator iter;
    for (iter = dmrinfos.begin(); iter != dmrinfos.end(); ++iter)
    {
        if (iter->first == key)
        {
            free(iter->second);
            dmrinfos.erase(iter);
            return;
        }
    }
}

void OnAddMediaRenderer(void* mediaRendererToken, struct AVRenderer* mediaRenderer)
{
    struct _tDmrInfo * dmr;

    dmr = ( struct _tDmrInfo * )malloc( sizeof( struct _tDmrInfo ) );

    if ( dmr == NULL ) return;

    dmr->index = g_dmr_count++;
    dmr->dmr_token = mediaRendererToken;
    dmr->render = mediaRenderer;
    mediaRenderer->StateChanged = OnRenderStateChanged;
    deleteKey(mediaRenderer->device->UDN);
    dmrinfos[mediaRenderer->device->UDN] = dmr;
}

void OnRemoveMediaRenderer(void* mediaRendererToken, struct AVRenderer* mediaRenderer)
{
    if ( dmrinfos.empty() ) return;
    deleteKey(mediaRenderer->device->UDN);
}

#endif // _DMR_CP_H_
