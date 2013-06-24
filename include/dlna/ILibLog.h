#ifndef __ILIBLOG_H__
#define __ILIBLOG_H__

#include <stdio.h>

typedef void (*log2jni)(const char *, int);
extern log2jni logdump;

#define ERROR   0
#define WARNING 1
#define DEBUG   2
#define EVENT   3
#define VERBOSE 4

typedef struct _tLogConfig {
    int with_file;
    int with_function;
    int with_line;
    int max_level;
    int mute;
} LogConfig;
extern LogConfig g_lc;

char * GetMsgByLevel(int lvl);
void LogPrintf(const char * format, ...);

#define DbgPrint(LEVEL, ...) \
    do { \
    if (g_lc.mute == 0 && (LEVEL) <= g_lc.max_level) { \
    LogPrintf("[%s] ", GetMsgByLevel(LEVEL)); \
    if (g_lc.with_file) { \
    LogPrintf("[%s] ", __FILE__); } \
    if (g_lc.with_function) { \
    LogPrintf("[%s] ", __FUNCTION__); }\
    if (g_lc.with_line) { \
    LogPrintf("[%d] ", __LINE__); }\
    LogPrintf(__VA_ARGS__); \
    }\
    } while(0)

#endif
