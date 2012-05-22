#ifndef _TESTDMR_H_
#define _TESTDMR_H_

#include <DMR.h>

extern struct _tDMRInfo g_dmrInfo;

char * BuildProtocolInfo(const char* infoList[]);
void UpdateUi();

int Callback_SetAVTransportURI(DMR instance, void* session, char* uri, struct CdsObject* data);
int Callback_GetAVProtocolInfo(DMR instance, void* session, char** protocolInfo);
int Callback_SetPlayMode(DMR instance, void* session, DMR_MediaPlayMode playMode);
int Callback_Stop(DMR instance, void* session);
int Callback_Play(DMR instance, void* session, char* playSpeed);
int Callback_Pause(DMR instance, void* session);
int Callback_SeekTrack(DMR instance, void* session, unsigned int trackIndex);
int Callback_SeekTrackPosition(DMR instance, void* session, long position);
int Callback_SeekMediaPosition(DMR instance, void* session, long position);
int Callback_Next(DMR instance, void* session);
int Callback_Previous(DMR instance, void* session);
int Callback_SelectPreset(DMR instance, void* session, char* presetName);

#if defined(INCLUDE_FEATURE_VOLUME)

 int Callback_SetVolume(DMR instance, void* session, unsigned char volume);
 int Callback_SetMute(DMR instance, void* session, BOOL mute);

#endif /* INCLUDE_FEATURE_VOLUME */

#if defined(INCLUDE_FEATURE_DISPLAY)

 int Callback_SetContrast(DMR instance, void* session, unsigned char contrast);
 int Callback_SetBrightness(DMR instance, void* session, unsigned char brightness);

#endif /* INCLUDE_FEATURE_DISPLAY */

#endif

