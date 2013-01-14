// stdlib.h

#pragma once

#include <..\Include\stdlib.h> // orignal stdlib.h

__declspec(dllimport) char ** environ;

namespace cex {

    char * __cdecl getenv(
        _In_z_ const char * _VarName
        );

}
