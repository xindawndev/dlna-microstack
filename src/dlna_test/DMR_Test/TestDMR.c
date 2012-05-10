#include <TestDMR.h>

typedef enum _eURIType
{
    UT_Unknown = 0,
    UT_Normal,
    UT_PlaySingle,
    UT_PlayListS,
    UT_PlayListV,
    UT_PlayContainer
} URIType;

struct _tDMRInfo
{
    DMR_PlayState playState;
    DMR_MediaPlayMode playMode;
    char * trackUrl;
    char * transportUrl;
    struct CdsObject * currentTrackMetadata;
    struct CdsObject * currentAVTransportMetadata;
    long trackDuration;
    int trackNumber;
    int trackCount;

    BOOL isstop;
    BOOL ispause;

#if defined(USE_PLAYLISTS)
    PlayListManager trackManager;
#endif
#if defined(INCLUDE_FEATURE_DISPLAY)
    int contrast;
    int brightness;
#endif
#if defined(INCLUDE_FEATURE_VOLUME)
    int volume;
    BOOL mute;
#endif
};

struct _tDMRInfo DMRInfo;

/**********/

char * BuildProtocolInfo(const char* infoList[])
{
    int counter;
    int length = 0;
    char* result = NULL;
    char* p;

    if(infoList == NULL)
    {
        return NULL;
    }

    counter = 0;
    p = (char*)infoList[counter];
    while(p[0] != '\0')
    {
        length += ((int)strlen(p) + 1);
        p = (char*)infoList[++counter];
    }

    result = (char*)malloc(length);
    result[0] = 0;

    counter = 0;
    p = (char*)infoList[counter];
    while(p[0] != '\0')
    {
        if(result[0] != '\0')
        {
            strcat(result, ",");
        }
        strcat(result, p);
        p = (char*)infoList[++counter];
    }

    return result;
}

void UpdateUi()
{
    printf( "----------------------------------------------------------Begin\n" );
    printf( "|PlayState: %d\n", DMRInfo.playState );
    printf( "|PlayMode: %d\n", DMRInfo.playMode );
    printf( "|TrackUrl: %s\n", DMRInfo.trackUrl ? DMRInfo.trackUrl: "" );
    printf( "|TransportUrl: %s\n", DMRInfo.transportUrl ? DMRInfo.transportUrl: "" );
    printf( "|TrackDuration: %ld\n", DMRInfo.trackDuration );
    printf( "|TrackNumber: %d\n", DMRInfo.trackNumber );
    printf( "|TrackCount: %d\n", DMRInfo.trackCount );
    printf( "|Stop: %s\n", DMRInfo.isstop ? "true" : "false" );
    printf( "|Pause: %s\n", DMRInfo.ispause ? "true" : "false" );

#if defined(USE_PLAYLISTS)
#endif
#if defined(INCLUDE_FEATURE_DISPLAY)
    printf( "|Contrast: %d\n", DMRInfo.contrast );
    printf( "|Brightness: %d\n", DMRInfo.brightness );
#endif
#if defined(INCLUDE_FEATURE_VOLUME)
    printf( "|Volume: %d\n", DMRInfo.volume );
    printf( "|Mute: %s\n", DMRInfo.mute ? "true" : "false" );
#endif
    printf( "----------------------------------------------------------End\n" );
}
/**********/

/*
验证Metadata的正确性
*/
int _verifyMetadate( struct CdsObject * data )
{
    return 1;
}

void _clearTrackUri( DMR instance, int update_dmr )
{
    String_Destroy( DMRInfo.trackUrl );
    DMRInfo.trackUrl = NULL;

    CDS_ObjRef_Release( DMRInfo.currentTrackMetadata );
    DMRInfo.currentTrackMetadata = NULL;

    DMRInfo.trackDuration = 0;
    DMRInfo.trackNumber = 0;

    if ( update_dmr != 0 )
    {
        DMR_StateChange_CurrentTrackURI( instance, DMRInfo.trackUrl);
        DMR_StateChange_CurrentTrackMetaData( instance, DMRInfo.currentTrackMetadata );
        DMR_StateChange_CurrentTrackDuration( instance, DMRInfo.trackDuration );
        DMR_StateChange_CurrentTrack( instance, DMRInfo.trackNumber );
    }
}

void _clearTransportUri( DMR instance,  int update_dmr )
{
    _clearTrackUri( instance, update_dmr );

    String_Destroy( DMRInfo.transportUrl );
    DMRInfo.transportUrl = NULL;

    CDS_ObjRef_Release( DMRInfo.currentAVTransportMetadata );
    DMRInfo.currentAVTransportMetadata = NULL;

    DMRInfo.playState = DMR_PS_NoMedia;
    DMRInfo.trackCount = 0;

    #ifdef USE_PLAYLISTS
    if ( DMRInfo.trackManager != NULL )
    {
        DMRInfo.trackManager->Destory( DMRInfo.trackManager );
        DMRInfo.trackManager = NULL;
    }
    #endif

    if ( update_dmr != 0 )
    {
        DMR_StateChange_AVTransportURI( instance, DMRInfo.transportUrl );
        DMR_StateChange_AVTransportURIMetaData( instance, DMRInfo.currentAVTransportMetadata );
        DMR_StateChange_TransportPlayState( instance, DMRInfo.playState );
        DMR_StateChange_NumberOfTracks( instance, DMRInfo.trackCount );
    }
}

int Callback_SetAVTransportURI(DMR instance, void* session, char* uri, struct CdsObject* data)
{
    printf( "\n%d:%s(%s)\n", __LINE__, __FUNCTION__, uri ? uri : "" );
    if ( DMRInfo.playState == DMR_PS_Paused || DMRInfo.playState == DMR_PS_Playing || DMRInfo.playState == DMR_PS_Transitioning )
    {
        CDS_ObjRef_Add( data );
        return 715; // 忙
    }

    if ( data != NULL && _verifyMetadate( data ) == 0 )
    {
        CDS_ObjRef_Release( data );
        return 402; // 无效参数
    }

    if ( uri == NULL )
    {
        _clearTransportUri( instance, 1 );

        if ( data != NULL )
        {
            CDS_ObjRef_Release( data );
        }

        UpdateUi();
    }
    else if ( DMRInfo.transportUrl == NULL || strcmp( DMRInfo.transportUrl, uri ) != 0 )
    {
        BOOL changeState = TRUE;
        DMR_PlayState oldState = DMRInfo.playState;
        struct DLNAProtocolInfo * protocolInfo = NULL;
        long contentLength = -1;
        URIType type = UT_Unknown;
        BOOL supportsByteRange = FALSE;
    }
    return 0;
}

int Callback_GetAVProtocolInfo(DMR instance, void* session, char** protocolInfo)
{
    printf( "\n%d:%s(%s)\n", __LINE__, __FUNCTION__, *protocolInfo ? *protocolInfo : "" );
    return 0;
}

int Callback_SetPlayMode(DMR instance, void* session, DMR_MediaPlayMode playMode)
{
    printf( "\n%d:%s(%d)\n", __LINE__, __FUNCTION__, playMode );
    DMRInfo.playMode = playMode;
    DMR_StateChange_CurrentPlayMode( instance, DMRInfo.playMode );

    return 0;
}

int Callback_Stop(DMR instance, void* session)
{
    printf( "\n%d:%s()\n", __LINE__, __FUNCTION__ );
    if ( DMRInfo.playState == DMR_PS_Playing || DMRInfo.playState == DMR_PS_Paused )
    {
        // RenderStop();
        DMRInfo.isstop = TRUE;
    }

    return 0;
}

BOOL _isImageClass( unsigned int mediaClass )
{
    unsigned int mask = CDS_MEDIACLASS_ITEM | CDS_MEDIACLASS_IMAGEITEM;
    if ( ( mediaClass & mask ) == mask )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int Callback_Play(DMR instance, void* session, char* playSpeed)
{
    printf( "\n%d:%s(%s)\n", __LINE__, __FUNCTION__, playSpeed ? playSpeed : "" );
    if (DMRInfo.playState == DMR_PS_Stopped || DMRInfo.playState == DMR_PS_Paused )
    {
        DMR_StateChange_TransportPlaySpeed( instance, "1" );
        if ( DMRInfo.playState == DMR_PS_Paused )
        {
            // RendererPause();
        }
        else
        {
            if ( DMRInfo.playMode == DMR_MPM_Intro )
            {
                // RendererPlay( DMRInfo.trackUrl, INTRO_MILLISECONDS, GetTransferModeFromURL( DMRInfo.trackUrl, NULL ) );
            }
            else
            {
                int image = 0;
#ifdef USE_PLAYLISTS
				if ( DMRInfo.trackManager == NULL )
				{
					if ( DMRInfo.currentTrackMetadata != NULL )
					{
						if ( _isImageClass( DMRInfo.currentTrackMetadata->MediaClass ) == 1 )
						{
							image = 1;
						}
					}
				}
				else
				{
					if ( DMRInfo.trackManager->TrackMetaData != NULL )
					{
						if ( _isImageClass( DMRInfo.trackManager->TrackMetaData->MediaClass ) == 1 )
						{
							image = 1;
						}
					}
				}

				if ( DMRInfo.trackManager != NULL && DMRInfo.trackManager->TrackNumber < 0 )
				{
					return 716;
				}
#else /* USE_PLAYLISTS */
				if( DMRInfo.currentTrackMetadata != NULL )
				{
					if ( _isImageClass( DMRInfo.currentTrackMetadata->MediaClass ) == 1 )
					{
						image = 1;
					}
				}
#endif /* USE_PLAYLISTS */
				if ( image == 1 )
				{
					// RenderPlay( DMRInfo.trackUrl, IMAGE_PLAY_MILLISECONDS, RENDERER_INTERACTIVE );
				}
				else
				{
					// RenderPlay( DMRInfo.trackUrl, DMRInfo.trackDuration, GetTransferModeFromURL( DMRInfo.trackUrl, NULL ) );
				}
            }
        }
    }
    return 0;
}

int Callback_Pause(DMR instance, void* session)
{
    printf( "\n%d:%s()\n", __LINE__, __FUNCTION__ );
    if ( DMRInfo.playState == DMR_PS_Playing )
    {
        DMR_StateChange_TransportPlaySpeed( instance, "1" );
        //RenderPause();
        DMRInfo.ispause = TRUE;
    }
    return 0;
}

// TRACK_NR 调用
int Callback_SeekTrack(DMR instance, void* session, unsigned int trackIndex)
{
    printf( "\n%d:%s(%d)\n", __LINE__, __FUNCTION__, trackIndex );
    UpdateUi();
    return 0;
}

// REL_TIME 调用
int Callback_SeekTrackPosition(DMR instance, void* session, long position)
{
    printf( "\n%d:%s(%ld)\n", __LINE__, __FUNCTION__, position );
    UpdateUi();
    return 0;
}

// ABS_TIME 调用
int Callback_SeekMediaPosition(DMR instance, void* session, long position)
{
    printf( "\n%d:%s(%ld)\n", __LINE__, __FUNCTION__, position );
    UpdateUi();
    return 0;
}

int Callback_Next(DMR instance, void* session)
{
    printf( "\n%d:%s()\n", __LINE__, __FUNCTION__ );
#ifdef USE_PLAYLISTS
	if ( DMRInfo.trackManager != NULL )
	{
		BOOL replay = FALSE;
		if ( /*RenderIsPlaying() == */TRUE )
		{
			//RenderStop();
			replay = TRUE;
		}
		PlayListManager_Next( DMRInfo.trackManager, TRUE );
		if ( replay == TRUE )
		{
			//int duration = GetTrackDuration( DMRInfo.trackManager->TrackUrl, DMRInfo.trackManager->TrackMetaData);
			//RenderStop();
			//RenderPlay( DMRInfo.trackManager->TrackUrl, duration, GetTransferModeFromURL(_trackManager->TrackUrl, DMRInfo.trackManager->TrackMetaData));
		}
	}
	else
	{
		return 711;
	}
    UpdateUi();
    return 0;
#else /* USE_PLAYLISTS */
	return 711;
#endif /* USE_PLAYLISTS */
}

int Callback_Previous(DMR instance, void* session)
{
    printf( "\n%d:%s()\n", __LINE__, __FUNCTION__ );
#ifdef USE_PLAYLISTS
	if ( DMRInfo.trackManager != NULL )
	{
		BOOL replay = FALSE;
		if ( /*RenderIsPlaying() ==*/ TRUE )
		{
			//RenderStop();
			replay = TRUE;
		}
		PlayListManager_Prev( DMRInfo.trackManager, TRUE );
		if ( replay == TRUE )
		{
			//int duration = GetTrackDuration( DMRInfo.trackManager->TrackUrl, DMRInfo.trackManager->TrackMetaData );
			//RenderStop();
			//RenderPlay( DMRInfo.trackManager->TrackUrl, duration, GetTransferModeFromURL( DMRInfo.trackManager->TrackUrl, DMRInfo.trackManager->TrackMetaData ) );
		}
	}
	else
	{
		return 711;
	}
    UpdateUi();
    return 0;
#else /* USE_PLAYLISTS */
	return 711;
#endif /* USE_PLAYLISTS */
}

int Callback_SelectPreset(DMR instance, void* session, char* presetName)
{
    printf( "\n%d:%s(%s)\n", __LINE__, __FUNCTION__, presetName ? presetName : "" );
	if ( strcmp( presetName, "FactoryDefault" ) == 0 )
	{
#ifdef INCLUDE_FEATURE_DISPLAY
        DMR_StateChange_Brightness( instance, BRIGHTNESS_FACTORYDEFAULT );
        DMR_StateChange_Contrast( instance, CONTRAST_FACTORYDEFAULT );
#endif
#ifdef INCLUDE_FEATURE_VOLUME
        DMR_StateChange_Volume( instance, VOLUME_FACTORYDEFAULT );
		DMR_StateChange_Mute( instance, FALSE );
#endif

		UpdateUi();
	}
    return 0;
}

#if defined(INCLUDE_FEATURE_VOLUME)

 int Callback_SetVolume(DMR instance, void* session, unsigned char volume)
 {
     printf( "\n%d:%s(%d)\n", __LINE__, __FUNCTION__, (int)volume );
     DMRInfo.volume = (int)volume;

	DMR_StateChange_Volume( instance, volume );

    UpdateUi();
    return 0;
 }

 int Callback_SetMute(DMR instance, void* session, BOOL mute)
 {
     printf( "\n%d:%s(%s)\n", __LINE__, __FUNCTION__, mute ? "true" : "false" );
    DMRInfo.mute = mute;

	DMR_StateChange_Mute( instance, mute );

    UpdateUi();
    return 0;
 }

#endif /* INCLUDE_FEATURE_VOLUME */

#if defined(INCLUDE_FEATURE_DISPLAY)

 int Callback_SetContrast(DMR instance, void* session, unsigned char contrast)
 {
     printf( "\n%d:%s(%d)\n", __LINE__, __FUNCTION__, (int)contrast );
     DMRInfo.contrast = (int)contrast;

	DMR_StateChange_Contrast( instance, contrast );

    UpdateUi();
    return 0;
 }

 int Callback_SetBrightness(DMR instance, void* session, unsigned char brightness)
 {
     printf( "\n%d:%s(%d)\n", __LINE__, __FUNCTION__, (int)brightness );
     DMRInfo.brightness = (int)brightness;

     DMR_StateChange_Brightness( instance, brightness );

     UpdateUi();
     return 0;
 }

#endif /* INCLUDE_FEATURE_DISPLAY */
