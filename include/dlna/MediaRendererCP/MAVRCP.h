#ifndef __MAVRCP_H__
#define __MAVRCP_H__

/* av render handle define */
typedef void * AVRHandle;

/* av render info */
struct _tAVRInfo
{
    char avr_uuid[128];
    char avr_name[128];
};

/* play mode enum */
enum _ePlayModeEnum
{
    AVRCP_INVALID       =0x0000000,         // 无效
    AVRCP_NORMAL        =0x0000001,         // 正常播放
    AVRCP_REPEAT_ALL    =0x0000002,         // 重复所有
    AVRCP_REPEAT_ONE    =0x0000004,         // 单个重复
    AVRCP_RANDOM        =0x0000008,         // 随机播放
    AVRCP_SHUFFLE       =0x0000010,         // 
    AVRCP_INTRO         =0x0000020          // 
};

/* callback functions declared                                          */
typedef void (* Callback_AVRenderSink)(char * udn, char * friendlyname);

typedef void (* Callback_Common)(int ErrorCode);
typedef void (* Callback_GetDevCapSink)(int ErrorCode, char* PlayMedia, char* RecMedia, char* RecQualityModes);
typedef void (* Callback_GetMediaInfoSink)(int ErrorCode, int nrTracks, int mediaDuration, char * curUrI, char * nextURI);
typedef void (* Callback_GetPositionSink)(int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter);

/* callback functions define */
Callback_AVRenderSink       avrender_add            = NULL;
Callback_AVRenderSink       avrender_del            = NULL;

Callback_GetDevCapSink      getdevcap_callback      = NULL;
Callback_Common             play_callback           = NULL;
Callback_Common             seek_callback           = NULL;
Callback_Common             stop_callback           = NULL;
Callback_Common             pause_callback          = NULL;
Callback_Common             next_callback           = NULL;
Callback_Common             prev_callback           = NULL;
Callback_Common             seturi_callback         = NULL;
#if defined(INCLUDE_FEATURE_VOLUME)
Callback_Common             setvol_callback         = NULL;
Callback_Common             setmute_callback        = NULL;
#endif
Callback_Common             setplaymode_callback    = NULL;
Callback_GetMediaInfoSink   getmediainfo_callback   = NULL;
Callback_GetPositionSink    getposition_callback    = NULL;

/************************************************************************/
/* Interface                                                            */
/************************************************************************/
void PrintDmrList();
char * GetDlnaDoc(char * udn);
char * GetDlnaCap( char * udn );
void GetDevCap( char * udn );
int SupportPlayMode( char * udn, enum _ePlayModeEnum playmode );
int SupportVolume( char * udn );
int SupportMute( char * udn );
void Play( char * udn );
void Seek( char * udn, int pos );
void Stop( char * udn );
void Pause( char * udn );
void Next( char * udn );
void Prev( char * udn );
void SetUri( char * udn, char * uri );
#if defined(INCLUDE_FEATURE_VOLUME)
void SetVolume( char * udn, int vol );
void SetMute( char * udn, int ismute );
#endif
void SetPlayMode( char * udn, enum _ePlayModeEnum playmode );
void GetMediaInfo( char * udn );
void GetPosition( char * udn );

/************************************************************************/
/* Interface                                                            */
/************************************************************************/
AVRHandle createAVRCP(int threadpool_size);
int startAVRCP(AVRHandle avhandle);
void stopAVRCP(AVRHandle avhandle);
void destoryAVRCP(AVRHandle avhandle);

#endif // __MAVRCP_H__
