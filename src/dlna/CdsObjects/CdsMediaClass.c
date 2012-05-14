#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifndef _WIN32_WCE
#include <crtdbg.h>
#endif
#else
#include <stdlib.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "CdsMediaClass.h"

/*
 *    The relative order of strings within these arrays must correspond to the MSCP_CLASS_MASK_xxx bitmask mappings.
 *    The first element must always be an empty string, to account for the fact that a sub-bitstring of the 
 *    mediaClass value may evaluate to 0.
 *
 *    All of these strings represent the normative set of media classes defined in UPnP AV.
 *
 *    It should be noted that none of these strings should EVER need to be change for XML escaping.
 */
const char* CDS_CLASS_OBJECT_TYPE[] = {"", "object.container", "object.item", };
const char* CDS_CLASS_MAJOR_TYPE[] = {"", "album", "genre", "person", "playlistContainer", "storageFolder", "storageSystem", "storageVolume", "audioItem", "imageItem", "playlistItem", "textItem", "videoItem"};
const char* CDS_CLASS_MINOR1_TYPE[] = {"", "musicAlbum", "photoAlbum", "movieGenre", "musicGenre", "musicArtist", "audioBook", "audioBroadcast", "musicTrack", "photo", "movie", "musicVideoClip", "videoBroadcast"};
const char* CDS_CLASS_MINOR2_TYPE[] = {""};
