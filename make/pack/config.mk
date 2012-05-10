################################################################################
## @file:       config.mk for pack
## @author      张框正 
## @brief       packet 依赖关系 只取出.so与可执行文件
## @version     1.0
###############################################################################
CONFIG                  := $(config) $(PACKET_CONFIG)

get_config              = $(firstword $(strip $(foreach config,$(1),$(findstring $(config),$(2))) $(3)))

CONIFG_COMPILE          := $(call get_config,$(CONFIG),debug release,release)

TARGET_DIRECTORY        := $(CONIFG_COMPILE)

DEPENDS                 := $(PACKET_DEPENDS)

config                  := $(CONFIG)


VERSION_DEPEND          := $(call get_item_info,$(PACKET_VERSION_DEPEND),File)
VERSION_DEPEND          := $(PLATFORM_BUILD_DIRECTORY)$(PACKET_VERSION_DEPEND)/$(VERSION_DEPEND) 

VERSION                 := $(strip $(shell $(EV) $(VERSION_DEPEND) $(call get_item_info,$(PACKET_VERSION_DEPEND),Target)))

TARGET_FILE             := $(PACKET_TARGET)-$(subst .,-,$(PLATFORM_STRATEGY_NAME))-$(CONIFG_COMPILE)-$(VERSION).tar.gz

TARGET_FILE_FULL        := $(TARGET_DIRECTORY)/$(TARGET_FILE)

PACKET_DEPEND_FILES     := $(shell echo -n $(PACKET_DEPEND_FILES) | iconv -f gbk)
