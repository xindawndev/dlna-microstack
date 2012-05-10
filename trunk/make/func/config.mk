################################################################################
## @file: 	config.mk
## @author	π˘¥∫√Ø <gcm.ustc.edu>
## @brief	¥¶¿Ì≈‰÷√.
## @version	1.0
###############################################################################

include $(ROOT_MAKE_DIRECTORY)/func/base.mk

get_config		= $(firstword $(strip $(foreach config,$(1),$(findstring $(config),$(2))) $(3)))

define handle_config
        $(if $(filter --enable-%,$(1)),CONFIG_$(1:--enable-%=%):=yes, \
        $(if $(filter --disable-%,$(1)),CONFIG_$(1:--disable-%=%):=no, \
        $(if $(filter --%,$(1)),$(patsubst --%,CONFIG_%,$(word 1,$(call split,=,$(1)))):=$(call split,:,$(patsubst $(word 1,$(call split,=,$(1)))=%,%,$(1))))))
endef

parse_config		= $(foreach config,$(call revert,$(1)),$(eval $(call handle_config,$(config))))
