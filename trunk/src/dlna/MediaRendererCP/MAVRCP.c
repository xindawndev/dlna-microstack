#include "MAVRCP.h"

#include "ILibParsers.h"
#include "ILibThreadPool.h"
#include "MicroAVRCP.h"
#include "DMRCP_ControlPoint.h"

struct _tDmrInfo
{
    void * dmr_token;
    void * render;
} DmrInfo;

struct _tMAVRCP
{
    int mavrcp_start;
    sem_t mavrcp_lock;
    void * mavrcp_dmr_tables;

    void * mavrcp_stackchain;
    void * mavrcp_threadpool;
    void * mavrcp_rendercp;

    void * mavrcp_ip_monitor_timer;
    int mavrcp_ip_addr_len;
    int * mavrcp_ip_addr_list;
};

/* callback functions define */
Callback_AVRenderSink           avrender_add            = NULL;
Callback_AVRenderSink           avrender_del            = NULL;
Callback_AVRenderUpdateSink     avrender_update         = NULL;

Callback_GetDevCapSink          getdevcap_callback      = NULL;
Callback_CommonSink             play_callback           = NULL;
Callback_CommonSink             seek_callback           = NULL;
Callback_CommonSink             stop_callback           = NULL;
Callback_CommonSink             pause_callback          = NULL;
Callback_CommonSink             next_callback           = NULL;
Callback_CommonSink             prev_callback           = NULL;
Callback_CommonSink             seturi_callback         = NULL;
#if defined(INCLUDE_FEATURE_VOLUME)
Callback_CommonSink             setvol_callback         = NULL;
Callback_CommonSink             setmute_callback        = NULL;
#endif
Callback_CommonSink             setplaymode_callback    = NULL;
Callback_GetMediaInfoSink       getmediainfo_callback   = NULL;
Callback_GetPositionSink        getposition_callback    = NULL;

/************************************************************************/
/* Callback Functions                                                   */
/************************************************************************/
void OnGetDevCapSink(struct AVRendererConnection * avrc,int ErrorCode, void *Tag, char* PlayMedia,char* RecMedia,char* RecQualityModes)
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "Success!\n" );
        printf( "PlayMedia:%s\n", PlayMedia );
        printf( "RecMedia:%s\n", RecMedia );
        printf( "RecQualityModes:%s\n", RecQualityModes );
    }
#endif

    if ( getdevcap_callback != NULL )
    {
        getdevcap_callback(Tag, ErrorCode, PlayMedia, RecMedia, RecQualityModes);
    }
}

void OnPlaySink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnPlaySink: Success!\n", ErrorCode );
        printf( "ProtocolInfo:%s\n", avrc->ProtocolInfo );	
        printf( "MediaUri:%s\n", avrc->MediaUri );	
        printf( "TrackUri:%s\n", avrc->TrackUri );
    }
#endif

    if ( play_callback != NULL )
    {
        play_callback(Tag, ErrorCode);
    }
}

void OnSeekSink(struct AVRendererConnection * avrc,int ErrorCode, void *Tag)
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSeekSink: Success!\n", ErrorCode );
    }
#endif

    if ( seek_callback != NULL )
    {
        seek_callback(Tag, ErrorCode);
    }
}

void OnStopSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnStopSink: Success!\n", ErrorCode );
    }
#endif

    if ( stop_callback != NULL )
    {
        stop_callback(Tag, ErrorCode);
    }
}

void OnPauseSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnPauseSink: Success!\n", ErrorCode );
    }
#endif

    if ( pause_callback != NULL )
    {
        pause_callback(Tag, ErrorCode);
    }
}

void OnNextSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnNextSink: Success!\n", ErrorCode );
    }
#endif

    if ( next_callback != NULL )
    {
        next_callback(Tag, ErrorCode);
    }
}

void OnPrevSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnPrevSink: Success!\n", ErrorCode );
    }
#endif

    if ( prev_callback != NULL )
    {
        prev_callback(Tag, ErrorCode);
    }
}

void OnSetUriSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetUriSink: Success!\n", ErrorCode );
    }
#endif

    if ( seturi_callback != NULL )
    {
        seturi_callback(Tag, ErrorCode);
    }
}

#if defined(INCLUDE_FEATURE_VOLUME)
void OnSetVolumeSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetVolumeSink: Success!\n", ErrorCode );
    }
#endif

    if ( setvol_callback != NULL )
    {
        setvol_callback(Tag, ErrorCode);
    }
}

void OnSetMuteSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetMuteSink: Success!\n", ErrorCode );
    }
#endif

    if ( setmute_callback != NULL )
    {
        setmute_callback(Tag, ErrorCode);
    }
}
#endif

void OnSetPlayModeSink( struct AVRendererConnection * avrc, int ErrorCode, void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnSetPlayModeSink: Success!\n", ErrorCode );
    }
#endif

    if ( setplaymode_callback != NULL )
    {
        setplaymode_callback(Tag, ErrorCode);
    }
}

void OnGetMediaInfoSink( struct AVRendererConnection * avrc,int ErrorCode, int nrTracks, int mediaDuration, char * curUrI, char * nextURI,void * Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnGetMediaInfoSink: Success!\n", ErrorCode );
        printf( "NrTracks = %d, MediaDuration = %d, CurUrl = %s, NextUrl = %s\n", nrTracks, mediaDuration, curUrI, nextURI );
    }
#endif

    if ( getmediainfo_callback != NULL )
    {
        getmediainfo_callback(Tag, ErrorCode, nrTracks, mediaDuration, curUrI, nextURI);
    }
}

void OnGetPositionSink(struct AVRendererConnection * avrc, int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter,void *Tag )
{
#if defined(_DEBUG) || defined(DEBUG)
    printf( "(%d) %s: ErrorCode:%d\n", __LINE__, __FUNCTION__, ErrorCode );
    if ( !ErrorCode )
    {
        printf( "OnGetPositionSink: Success!\n", ErrorCode );
        printf( "RelativeSeconds = %d, AbsoluteSeconds = %d, RelativeCounter = %d, AbsoluteCounter = %d\n", RelativeSeconds, AbsoluteSeconds, RelativeCounter, AbsoluteCounter );
    }
#endif

    if ( getposition_callback != NULL )
    {
        getposition_callback(Tag, ErrorCode, RelativeSeconds, AbsoluteSeconds, RelativeCounter, AbsoluteCounter);
    }
}

/************************************************************************/
/* Interface                                                            */
/************************************************************************/
struct _tDmrInfo * _getDmrInfo( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * ret_val = NULL;
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)avhandle;
    if (p_mavrcp == NULL)
        return NULL;

    if ( NULL == udn )
        return NULL;

    if ( p_mavrcp->mavrcp_dmr_tables == NULL )
        return NULL;

    ILibHashTree_Lock( p_mavrcp->mavrcp_dmr_tables );
    ret_val = (struct _tDmrInfo *)ILibGetEntry(p_mavrcp->mavrcp_dmr_tables, (void *)udn, strlen(udn));
    ILibHashTree_UnLock( p_mavrcp->mavrcp_dmr_tables );

    return ret_val;
}

void PrintDmrList( AVRHandle avhandle )
{
    char Key[128] = { 0 };
    int Len = 128, i = 0;
    struct _tDmrInfo * Val = NULL;
    void * dmr_enum = NULL;
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)avhandle;
    if (p_mavrcp == NULL)
        return;

    if ( p_mavrcp->mavrcp_dmr_tables == NULL ) return;
    ILibHashTree_Lock( p_mavrcp->mavrcp_dmr_tables );

    dmr_enum = ILibHashTree_GetEnumerator( p_mavrcp->mavrcp_dmr_tables );
    if ( dmr_enum == NULL ) return;
    while ( !ILibHashTree_MoveNext( dmr_enum ) )
    {
        ILibHashTree_GetValue( dmr_enum, (char **)&Key, &Len, ( ( void ** )( &Val ) ) );
        printf( "\t%d. %s [%s]\n", i++, ((struct AVRenderer*)(Val->render))->FriendlyName, ((struct AVRenderer*)(Val->render))->device->UDN );
    }
    ILibHashTree_DestroyEnumerator( dmr_enum );

    ILibHashTree_UnLock( p_mavrcp->mavrcp_dmr_tables );
}

char * GetDlnaDoc( AVRHandle avhandle, char * udn)
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return NULL;
    }

    return RCP_GetDLNADOC( Val->render );
}

char * GetDlnaCap( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return NULL;
    }
    return RCP_GetDLNACAP( Val->render );
}

void GetDevCap( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    char * ret = NULL;
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_GetDeviceCap( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnGetDevCapSink );
}

int SupportPlayMode( AVRHandle avhandle, char * udn, enum PlayModeEnum playmode )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return -1;
    }
    return RCP_SupportPlayMode( Val->render, playmode );
}

int SupportVolume( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return -1;
    }
    return RCP_SupportVolume( Val->render );
}

int SupportMute( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return -1;
    }
    return RCP_SupportMute( Val->render );
}

void Play( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_Play( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnPlaySink );
}

void Seek( AVRHandle avhandle, char * udn, int pos )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_Seek( ((struct AVRenderer *)(Val->render))->Connection, pos, avhandle, OnSeekSink );
}

void Stop( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_Stop( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnStopSink );
}

void Pause( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_Pause( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnPauseSink );
}

void Next( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_Next( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnNextSink );
}

void Prev( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_Prev( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnPrevSink );
}

void SetUri( AVRHandle avhandle, char * udn, char * uri )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_SetUri( ((struct AVRenderer *)(Val->render))->Connection, uri, avhandle, OnSetUriSink );
}

#if defined(INCLUDE_FEATURE_VOLUME)
void SetVolume( AVRHandle avhandle, char * udn, int vol )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_SetVolume( ((struct AVRenderer *)(Val->render))->Connection, "Master", ( char )( vol ), NULL, OnSetVolumeSink );
}

void SetMute( AVRHandle avhandle, char * udn, int ismute )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_SetMute( ((struct AVRenderer *)(Val->render))->Connection, "Master", ismute, avhandle, OnSetMuteSink );
}
#endif

void SetPlayMode( AVRHandle avhandle, char * udn, enum PlayModeEnum playmode )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_SetPlayMode( ((struct AVRenderer *)(Val->render))->Connection, playmode, avhandle, OnSetPlayModeSink );
}

void GetMediaInfo( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_GetMediaInfo( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnGetMediaInfoSink );
}

void GetPosition( AVRHandle avhandle, char * udn )
{
    struct _tDmrInfo * Val = _getDmrInfo( avhandle, udn );
    if ( Val == NULL )
    {
        printf( "You choice a invalid device!\n" );
        return;
    }
    RCP_GetPosition( ((struct AVRenderer *)(Val->render))->Connection, avhandle, OnGetPositionSink );
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void OnRenderStateChanged( struct AVRenderer * sender,struct AVRendererConnection * Connection )
{
    int i;
    struct _tRenderStateInfo renderstate;
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)RCPToken_GetTag(sender->Tag);

    memset( (void *)&renderstate, 0, sizeof(struct _tRenderStateInfo) );

    sem_wait( &(p_mavrcp->mavrcp_lock) );
#if defined(_DEBUG) || defined(DEBUG)
    //printf("/************************************************************************/\n");
    //printf("========%s: %x\n", __FUNCTION__, (int)p_mavrcp);
    //printf("/************************************************************************/\n");

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
    case MAVRCP_PLAYING:
        printf( "AVRCP_PLAYING\n" );
        break;
    case MAVRCP_STOPPED:
        printf( "AVRCP_STOPPED\n" );
        break;
    case MAVRCP_PAUSED:
        printf( "AVRCP_PAUSED\n" );
        break;
    case MAVRCP_RECORDING:
        printf( "AVRCP_RECORDING\n" );
        break;
    case MAVRCP_TRANSITIONING:
        printf( "AVRCP_TRANSITIONING\n" );
        break;
    case MAVRCP_NO_MEDIA:
        printf( "AVRCP_NO_MEDIA\n" );
        break;
    default:
        printf( "AVRCP_UNKNOWN\n" );
        break;
    }
    printf( "Play Mode = " );
    switch ( Connection->PlayMode )
    {
    case MAVRCP_NORMAL:
        printf( "AVRCP_NORMAL\n" );
        break;
    case MAVRCP_REPEAT_ALL:
        printf( "AVRCP_REPEAT_ALL\n" );
        break;
    case MAVRCP_REPEAT_ONE:
        printf( "AVRCP_REPEAT_ONE\n" );
        break;
    case MAVRCP_RANDOM:
        printf( "AVRCP_RANDOM\n" );
        break;
    case MAVRCP_SHUFFLE:
        printf( "AVRCP_SHUFFLE\n" );
        break;
    case MAVRCP_INTRO:
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
#endif
    renderstate.CMID                                    =Connection->CMID;
    renderstate.AVTID                                   =Connection->AVTID;
    renderstate.RCID                                    =Connection->RCID;
    renderstate.Error                                   =Connection->Error;
    renderstate.ProtocolInfo                            =Connection->ProtocolInfo;
    renderstate.MediaUri                                =Connection->MediaUri;
    renderstate.TrackUri                                =Connection->TrackUri;
    renderstate.TransportStatus                         =Connection->TransportStatus;
    renderstate.TotalTracks                             =Connection->TotalTracks;
    renderstate.TrackNumber                             =Connection->TrackNumber;
    renderstate.PlayState                               =Connection->PlayState;
    renderstate.PlayMode                                =Connection->PlayMode;
    renderstate.Duration                                =Connection->Duration;
    memcpy(renderstate.TrackPosition, Connection->TrackPosition, 32);
    renderstate.SupportedRecordQualityModes             =Connection->SupportedRecordQualityModes;
    renderstate.SupportedRecordQualityModesLength       =Connection->SupportedRecordQualityModesLength;
    renderstate.RecordMedia                             =Connection->RecordMedia;
    renderstate.RecordMediaLength                       =Connection->RecordMediaLength;
    renderstate.PlayMedia                               =Connection->PlayMedia;
    renderstate.PlayMediaLength                         =Connection->PlayMediaLength;
    renderstate.ChannelCount                            =Connection->ChannelCount;
    renderstate.Channel                                 =Connection->Channel;
    renderstate.Volume                                  =Connection->Volume;
    renderstate.Mute                                    =Connection->Mute;
    sem_post( &(p_mavrcp->mavrcp_lock ) );
    if (avrender_update != NULL)
    {
        avrender_update(p_mavrcp, sender->device->UDN, sender->device->FriendlyName, renderstate);
    }
}

void OnAddMediaRenderer(void* mediaRendererToken, struct AVRenderer* mediaRenderer)
{
    struct _tDmrInfo * dmr = NULL;
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)RCPToken_GetTag(mediaRendererToken);

    printf("/************************************************************************/\n");
    printf("========%s: %x\n", __FUNCTION__, (int)p_mavrcp);
    printf("/************************************************************************/\n");

    dmr = ( struct _tDmrInfo * )malloc( sizeof( struct _tDmrInfo ) );
    if ( dmr == NULL )
    {
        fprintf(stderr, "malloc dmrinfo fail!\n");
        return;
    }

    dmr->dmr_token = mediaRendererToken;
    dmr->render = mediaRenderer;
    mediaRenderer->StateChanged = OnRenderStateChanged;

    ILibAddEntry( p_mavrcp->mavrcp_dmr_tables, mediaRenderer->device->UDN, strlen( mediaRenderer->device->UDN ), ( void * )dmr );
    printf( "Add AVRender %s[%s]\n", mediaRenderer->FriendlyName, mediaRenderer->device->UDN );
    if ( avrender_add != NULL )
    {
        avrender_add((void *)p_mavrcp, mediaRenderer->device->UDN, mediaRenderer->device->FriendlyName);
    }
}

void OnRemoveMediaRenderer(void* mediaRendererToken, struct AVRenderer* mediaRenderer)
{
    void * dmrinfo = NULL;
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)RCPToken_GetTag(mediaRendererToken);

    dmrinfo = ILibGetEntry( p_mavrcp->mavrcp_dmr_tables, mediaRenderer->device->UDN, strlen( mediaRenderer->device->UDN ) );
    ILibDeleteEntry( p_mavrcp->mavrcp_dmr_tables, mediaRenderer->device->UDN, strlen( mediaRenderer->device->UDN ) );
    free( dmrinfo );

    printf( "Remove AVRender %s[%s]...\n", mediaRenderer->FriendlyName, mediaRenderer->device->UDN );
    if ( avrender_del != NULL )
    {
        avrender_del((void *)p_mavrcp, mediaRenderer->device->UDN, mediaRenderer->device->FriendlyName);
    }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

/* thread pool add */
#if defined(_POSIX)
void* pool_thread(void *args)
{
    ILibThreadPool_AddThread(args);
    return (0);
}
#else
DWORD WINAPI pool_thread(void *args)
{
    ILibThreadPool_AddThread(args);
    return (0);
}
#endif

void IPAddressMonitor(void *data)
{
    int length;
    int *list;

    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)data;
    length = ILibGetLocalIPAddressList(&list);
    if (length != p_mavrcp->mavrcp_ip_addr_len || memcmp((void *)list, (void *)p_mavrcp->mavrcp_ip_addr_list, sizeof(int)*length) != 0)
    {
        RCP_IPAddressChanged(p_mavrcp->mavrcp_rendercp);

        free(p_mavrcp->mavrcp_ip_addr_list);
        p_mavrcp->mavrcp_ip_addr_list = list;
        p_mavrcp->mavrcp_ip_addr_len = length;
    }
    else
    {
        free(list);
    }

    ILibLifeTime_Add(p_mavrcp->mavrcp_ip_monitor_timer, data, 4, (void *)&IPAddressMonitor, NULL);
}

AVRHandle createAVRCP(int threadpool_size)
{
    int count;
#if defined(_POSIX)
    pthread_t t;
#else
    DWORD ptid = 0, ptid2 = 0;
#endif

    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)malloc(sizeof(struct _tMAVRCP));
    memset(p_mavrcp, 0, sizeof(struct _tMAVRCP));

    p_mavrcp->mavrcp_start = 0;
    sem_init(&(p_mavrcp->mavrcp_lock), 0, 1);
    p_mavrcp->mavrcp_dmr_tables = ILibInitHashTree();

    p_mavrcp->mavrcp_stackchain = ILibCreateChain();
    p_mavrcp->mavrcp_threadpool = ILibThreadPool_Create();

    for( count = 0; count < threadpool_size; ++count )
    {
#if defined(WIN32)
        CreateThread(NULL, 0, &pool_thread, p_mavrcp->mavrcp_threadpool, 0, &ptid);
#else
        pthread_create(&t,NULL,&pool_thread,p_mavrcp->mavrcp_threadpool);
#endif
    }

    p_mavrcp->mavrcp_rendercp = CreateRendererCP(p_mavrcp->mavrcp_stackchain);
    RCPToken_SetTag((void *)p_mavrcp->mavrcp_rendercp, (void *)p_mavrcp);

    RendererAdded = &OnAddMediaRenderer;
    RendererRemoved = &OnRemoveMediaRenderer;

    return (AVRHandle)(p_mavrcp);
}

#if defined(_POSIX)
void* start_chain(void *args)
{
    ILibStartChain(args);
    return (0);
}
#else
DWORD WINAPI start_chain(void *args)
{
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)args;
    p_mavrcp->mavrcp_start = 1;

    ILibStartChain(p_mavrcp->mavrcp_stackchain);
    p_mavrcp->mavrcp_start = 0;
    return (0);
}
#endif

int startAVRCP(AVRHandle avhandle)
{
#if defined(_POSIX)
    pthread_t t;
#else
    DWORD ptid = 0;
#endif
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)avhandle;
    if (p_mavrcp == NULL) return -1;

    p_mavrcp->mavrcp_ip_monitor_timer = ILibCreateLifeTime(p_mavrcp->mavrcp_stackchain);
    p_mavrcp->mavrcp_ip_addr_len = ILibGetLocalIPAddressList(&(p_mavrcp->mavrcp_ip_addr_list));
    ILibLifeTime_Add(p_mavrcp->mavrcp_ip_monitor_timer, p_mavrcp, 4, (void*)&IPAddressMonitor,NULL);

//#if defined(_POSIX)
//    pthread_create(&t,NULL,&start_chain,p_mavrcp);
//#else
//    CreateThread(NULL, 0, &start_chain, p_mavrcp, 0, &ptid);
//#endif
    start_chain(p_mavrcp);

    return 0;
}

void stopAVRCP(AVRHandle avhandle)
{
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)avhandle;
    if (p_mavrcp == NULL) return;
    if (!p_mavrcp->mavrcp_start) return;
    ILibStopChain(p_mavrcp->mavrcp_stackchain);
    p_mavrcp->mavrcp_start = 0;
    freesafe(p_mavrcp->mavrcp_ip_addr_list);
}

void destoryAVRCP(AVRHandle avhandle)
{
    struct _tMAVRCP * p_mavrcp = (struct _tMAVRCP *)avhandle;
    if (p_mavrcp == NULL) return;
    if (p_mavrcp->mavrcp_start)
    {
        stopAVRCP(avhandle);
    }
    sem_destroy(&(p_mavrcp->mavrcp_lock));
    ILibDestroyHashTree( p_mavrcp->mavrcp_dmr_tables );

    freesafe(p_mavrcp);
}