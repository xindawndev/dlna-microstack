#ifndef _ILIBSSDPCLIENT_H_
#define _ILIBSSDPCLIENT_H_

#include "UPnPControlPointStructs.h"

void* ILibCreateSSDPClientModule(void *chain, char* DeviceURN, int DeviceURNLength, void (*FunctionCallback)(void *sender, char* UDN, int Alive, char* LocationURL, int Timeout, UPnPSSDP_MESSAGE m, void *user),void *user);
void ILibSSDPDeviceSearch(void * SSDPToken);
void ILibSSDP_IPAddressListChanged(void *SSDPToken);

#endif
