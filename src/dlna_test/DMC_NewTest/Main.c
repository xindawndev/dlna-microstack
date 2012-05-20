#include <stdio.h>
#include <stdlib.h>
#include "MAVRCP1.h"

#if defined(WIN32)
#include <windows.h>
#endif

void AVRenderSink(char * udn, char * friendlyname)
{

}

int main(int argc, char **argv)
{
    while(1)
    {
        startAVRCP(3);
        system("pause");

        stopAVRCP();
    }

    return 0;
}
