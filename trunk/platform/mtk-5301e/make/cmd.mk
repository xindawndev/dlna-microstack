PLATFORM_TOOL_PATH	:= /mtk-toolchain/gnuarm-4.4.2/cross-tools/arm11/bin

PLATFORM_TOOL_NAME	:= gcc44

PLATFORM_TOOL_PREFIX	:= armv6z-mediatek442_001-linux-gnueabi-

PLATFORM_TOOL_LIB_PATH          := $(PLATFORM_TOOL_DIRECTORY)/mtk-toolchain/gnuarm-4.4.2/cross-tools/arm11/lib

PLATFORM_TOOL_LIB_PATH2		:= $(PLATFORM_TOOL_DIRECTORY)/mtk-toolchain/gnuarm-4.4.2/cross-tools/arm11/i686-pc-linux-gnu/armv6z-mediatek-linux-gnueabi/lib

export LD_LIBRARY_PATH=:$(PLATFORM_TOOL_LIB_PATH):$(PLATFORM_TOOL_LIB_PATH2)
