// := framework_config.h

#define FRAMEWORK_NO_SYSTEM_V_IPC

#define FRAMEWORK_NO_POSIX_IPC

#include <unistd.h>
#undef major
#undef minor
