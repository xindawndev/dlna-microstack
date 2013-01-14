PLATFORM_TOOL_PATH	:= /mtktools/mtk-toolchain/gnuarm-4.4.1/cross-tools/usr/local/bin

PLATFORM_TOOL_NAME	:= gcc44

PLATFORM_TOOL_PREFIX	:= armv6z-mediatek-linux-gnueabi-

PLATFORM_TOOL_LIB_PATH  := $(PLATFORM_TOOL_DIRECTORY)/mtktools/mtk-toolchain/gnuarm-4.4.1/cross-tools/usr/local/i686-pc-linux-gnu/armv6z-mediatek-linux-gnueabi/lib

PLATFORM_TOOL_LIB_PATH2 := $(PLATFORM_TOOL_DIRECTORY)/mtktools/mtk-toolchain/gnuarm-4.4.1/sysroot/usr/lib

export LD_LIBRARY_PATH=:$(PLATFORM_TOOL_LIB_PATH):$(PLATFORM_TOOL_LIB_PATH2)
