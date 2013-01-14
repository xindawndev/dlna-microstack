PLATFORM_INCLUDE_DIRECTORY	:= $(PLATFORM_TOOL_DIRECTORY)/opt/timesys/toolchains/mipsisa32r2-linux/mipsisa32r2-linux-uclibc/include

PLATFORM_LIB_DIRECTORY		:= $(PLATFORM_TOOL_DIRECTORY)/opt/timesys/toolchains/mipsisa32r2-linux/mipsisa32r2-linux-uclibc/lib

PLATFORM_INCLUDE_DIRECTORYS	:= $(PLATFORM_INCLUDE_DIRECTORY)/c++/4.2.0/ \
				$(PLATFORM_INCLUDE_DIRECTORY)/c++/4.2.0/mipsisa32r2-linux-uclibc/

PLATFORM_LINK_FLAGS		:= -L$(PLATFORM_LIB_DIRECTORY)
