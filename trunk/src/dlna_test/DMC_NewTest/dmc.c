#include "dmc.h"
#include <stdio.h>

void callback_add( char * udn, char * friendlyname)
{
    printf("++++++%s Add udn = %s friendlyname = %s ++++++\n", __FUNCTION__, udn, friendlyname);

}

void callback_del( char * udn, char * friendlyname)
{
    printf("++++++%s Del udn = %s friendlyname = %s ++++++\n", __FUNCTION__, udn, friendlyname);
}

void callback_update( char * udn, char * friendlyname, struct _tRenderStateInfo renderstate)
{
    printf("++++++%s Update udn = %s friendlyname = %s ++++++\n", __FUNCTION__, udn, friendlyname);
}

void callback_getdevcap( int ErrorCode, char* PlayMedia, char* RecMedia, char* RecQualityModes)
{
    printf("++++++%s ec = %d++++++ PlayMedia = %s RecMedia = %s RecQualityModes = %s ++++++\n", __FUNCTION__, ErrorCode, PlayMedia ? PlayMedia : "", RecMedia ? RecMedia : "", RecQualityModes ? RecQualityModes : "");
}

void callback_play( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_seek( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_stop( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_pause( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_next( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_prev( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_seturi( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

#if defined(INCLUDE_FEATURE_VOLUME)
void callback_setvol( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_setmute( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

#endif
void callback_setplaymode( int ErrorCode)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
}

void callback_getmediainfo( int ErrorCode, int nrTracks, int mediaDuration, char * curUrI, char * nextURI)
{
    printf("++++++%s ec = %d nrTracks = %d mediaDuration = %d curUrI = %s nextURI = %s++++++\n", __FUNCTION__, ErrorCode, nrTracks, mediaDuration, curUrI ? curUrI : "", nextURI ? nextURI : "");
}

void callback_getposition( int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter)
{
    printf("++++++%s ec = %d RelativeSeconds = %d AbsoluteSeconds = %d RelativeCounter = %d AbsoluteCounter = %d ++++++\n", __FUNCTION__, ErrorCode, RelativeSeconds, AbsoluteSeconds, RelativeCounter, AbsoluteCounter);
}

void callback_gettransportinfo( int ErrorCode, char* CurrentTransportState, char* CurrentTransportStatus, char* CurrentSpeed)
{
    printf("++++++%s ec = %d CurrentTransportState = %s CurrentTransportStatus = %s CurrentSpeed = %s ++++++\n", __FUNCTION__, ErrorCode, CurrentTransportState ? CurrentTransportState : "", CurrentTransportStatus ? CurrentTransportStatus : "", CurrentSpeed ? CurrentSpeed : "");
}
