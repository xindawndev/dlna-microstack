################################################################################
## @file: 	all.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	make规则的入口.
## @version	1.0
###############################################################################

include $(RELETIVE_DIR)/rootdir.mk

include $(ROOT_MAKE_DIRECTORY)/global.mk

include $(ROOT_MAKE_DIRECTORY)/func/config.mk

CONFIG			:= $(config) $(GLOBAL_CONFIG) $(LOCAL_CONFIG)

$(call parse_config,$(CONFIG))

TYPE_LIST		:= dirs proj pack

ifeq ($(LOCAL_TYPE),)
	LOCAL_TYPE		:= proj
endif

TYPE			:= $(findstring $(LOCAL_TYPE),$(TYPE_LIST))

ifeq ($(TYPE),)
        $(error LOCAL_TYPE($(LOCAL_TYPE)) should be one of $(TYPE_LIST))
endif

include $(ROOT_MAKE_DIRECTORY)/$(TYPE).mk
