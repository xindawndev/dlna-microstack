#include "dmc.h"

void callback_add( char * udn, char * friendlyname)
{
    printf("++++++%s udn = %s friendlyname = %s\n", __FUNCTION__, udn, friendlyname);
}

void callback_del( char * udn, char * friendlyname)
{
    printf("++++++%s udn = %s friendlyname = %s\n", __FUNCTION__, udn, friendlyname);
}

void callback_update( char * udn, char * friendlyname, struct _tRenderStateInfo renderstate)
{
    printf("++++++%s udn = %s friendlyname = %s\n", __FUNCTION__, udn, friendlyname);
}

void callback_getdevcap( int ErrorCode, char* PlayMedia, char* RecMedia, char* RecQualityModes)
{
    printf("++++++%s ec = %d++++++\n", __FUNCTION__, ErrorCode);
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

}

void callback_getposition( int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter)
{

}
