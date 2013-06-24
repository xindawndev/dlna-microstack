#include "ILibLog.h"
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

log2jni logdump = NULL;
LogConfig g_lc;

char * GetMsgByLevel(int lvl)
{
    switch (lvl) {
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARN";
        case DEBUG:
            return "DEBUG";
        case EVENT:
            return "EVENT";
        case VERBOSE:
            return "VERB";
        default:
            return "UNKNOWN";
    }
}

void LogPrintf(const char * format, ...)
{
    char         local_buffer[1024];
    unsigned int buffer_size = 1024;
    char*        buffer = local_buffer;
    va_list      args;

    va_start(args, format);

    for(;;) {
        int result;

        result = vsnprintf(buffer, buffer_size-1, format, args);
        buffer[buffer_size-1] = 0;
        if (result >= 0) break;

        buffer_size = (buffer_size+4096)*2;
        if (buffer_size > 65536) break;
        if (buffer != local_buffer) free(buffer);
        buffer = (char *)malloc(sizeof(char) * buffer_size);
        if (buffer == NULL) return;
    }

    if (logdump != NULL) { 
         logdump(buffer, strlen(buffer));
    }
    printf(buffer);

#ifdef _WIN32
    OutputDebugStringA(buffer);
#endif

    if (buffer != local_buffer) free(buffer);

    va_end(args);
}


