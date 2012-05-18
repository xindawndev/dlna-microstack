#include <stdio.h>
#include <stdlib.h>
#include "MAVRCP1.h"
#include <windows.h>

void AVRenderSink(char * udn, char * friendlyname)
{

}

DWORD WINAPI run(void *args)
{
    return (0);
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