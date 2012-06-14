#ifndef __DMC_H__
#define __DMC_H__

#include "MAVRCP.h"

void callback_add( char * udn, char * friendlyname);
void callback_del( char * udn, char * friendlyname);

void callback_update( char * udn, char * friendlyname, struct _tRenderStateInfo renderstate);
void callback_getdevcap( int ErrorCode, char* PlayMedia, char* RecMedia, char* RecQualityModes);

void callback_play( int ErrorCode);
void callback_seek( int ErrorCode);
void callback_stop( int ErrorCode);
void callback_pause( int ErrorCode);
void callback_next( int ErrorCode);
void callback_prev( int ErrorCode);
void callback_seturi( int ErrorCode);
#if defined(INCLUDE_FEATURE_VOLUME)
void callback_setvol( int ErrorCode);
void callback_setmute( int ErrorCode);
#endif
void callback_setplaymode( int ErrorCode);

void callback_getmediainfo( int ErrorCode, int nrTracks, int mediaDuration, char * curUrI, char * nextURI);
void callback_getposition( int ErrorCode, int RelativeSeconds, int AbsoluteSeconds, int RelativeCounter, int AbsoluteCounter);


#endif // __DMC_H__
