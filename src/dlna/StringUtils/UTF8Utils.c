#if defined(WIN32) || defined(_WIN32_WCE)
#include <time.h>
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include "UTF8Utils.h"
#include "ILibParsers.h"

 /*
 * Implements additional string functionality.
 */
char* EncodeToUTF8(const char* mbcsStr) 
{ 
#if defined(WIN32)
        wchar_t*  wideStr; 
        char*   utf8Str; 
        int   charLen; 

        charLen = MultiByteToWideChar(CP_ACP, 0, mbcsStr, -1, NULL, 0); 
        wideStr = (wchar_t*) malloc(sizeof(wchar_t)*charLen); 
        MultiByteToWideChar(CP_ACP, 0, mbcsStr, -1, wideStr, charLen); 

        charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL); 

        utf8Str = (char*) malloc(charLen);

        WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);

        free(wideStr); 
        return utf8Str;
#else
    return(ILibString_Copy(mbcsStr, (int)strlen(mbcsStr)));
#endif
} 

char* DecodeFromUTF8(const char* utf8Str) 
{ 
#if defined(WIN32)
        wchar_t*  wideStr; 
        char*   mbcsStr; 
        int   charLen; 

        charLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0); 
        wideStr = (wchar_t*) malloc(sizeof(wchar_t)*charLen); 
        MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wideStr, charLen); 

        charLen = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, NULL, 0, NULL, NULL); 

        mbcsStr = (char*) malloc(charLen);

        WideCharToMultiByte(CP_ACP, 0, wideStr, -1, mbcsStr, charLen, NULL, NULL);

        free(wideStr); 
        return mbcsStr;
#else
    return(ILibString_Copy(utf8Str, (int)strlen(utf8Str)));
#endif
}
