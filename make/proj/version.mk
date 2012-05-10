################################################################################
## @file:	version.mk
## @author	¹ù´ºÃ¯ <gcm.ustc.edu>
## @brief	×Ô¶¯°æ±¾ºÅ.
## @version	1.0
###############################################################################

include $(ROOT_MAKE_DIRECTORY)/func/macro.mk

TARGET_FILE		:= $(NAME_PREFIX)$(PROJECT_TARGET)-$(PLATFORM_NAME)-$(PLATFORM_TOOL_NAME)$(NAME_CONFIG)$(NAME_SUFFIX)

ifeq ($(PROJECT_VERSION),)
        ifneq ($(PROJECT_VERSION_HEADER),)
		VERSION_HEADER		:= $(firstword $(wildcard $(addsuffix /$(PROJECT_VERSION_HEADER),$(HEADER_DIRECTORYS))))

                ifeq ($(VERSION_HEADER),)
                        $(error VERSION_HEADER $(PROJECT_VERSION_HEADER) not exists)
                endif
	
		VERSION			:= $(call get_macro_info,$(VERSION_HEADER),VERSION)

		VERSION			:= $(strip $(VERSION))

                ifeq ($(VERSION),)
                        $(error VERSION not defined in $(VERSION_HEADER))
                endif
	
		VERSION_NAME		:= $(call get_macro_info,$(VERSION_HEADER),NAME)

		VERSION_NAME		:= $(strip $(VERSION_NAME))
        else
		CONFIG_build_version	:= no
        endif
else
	CONFIG_build_version	:= no
	VERSION			:= $(PROJECT_VERSION)
endif

NAME_VERSION		:= $(VERSION)

ifneq ($(NAME_VERSION),)
	NAME_VERSION		:= -$(NAME_VERSION)
endif

ifeq ($(NAME_SUFFIX),)
	TARGET_FILE_VERSION	:= $(TARGET_FILE)$(NAME_VERSION)
else    
	TARGET_FILE_VERSION	:= $(patsubst %$(NAME_SUFFIX),%$(NAME_VERSION)$(NAME_SUFFIX),$(TARGET_FILE))
endif

TARGET_FILE_FULL	:= $(TARGET_DIRECTORY)/$(TARGET_FILE_VERSION)

ifeq ($(BUILDABLE)$(CONFIG_build_version),yesyes)
        ifeq ($(VERSION_NAME),)
                $(error VERSION_NAME not defined in $(VERSION_HEADER))
        endif
	
	BUILD_VERSION		:= $(shell LANG=C svn info $(SOURCE_DIRECTORY) | awk -F : '$$1 == "Revision" { print $$2}' 2> /dev/null)

	BUILD_VERSION		:= $(strip $(BUILD_VERSION))

	BUILD_VERSION_FILE	:= build_version

        ifneq ($(BUILD_VERSION),$(shell cat $(BUILD_VERSION_FILE) 2> /dev/null))
                $(shell echo -n $(BUILD_VERSION) > $(BUILD_VERSION_FILE))
        endif

	MAKE_VERSION		:= $(TARGET_FILE_FULL).version

target: $(MAKE_VERSION)

$(MAKE_VERSION): $(TARGET_FILE_FULL) $(BUILD_VERSION_FILE)
	@$(EV) $(TARGET_FILE_FULL) $(VERSION_NAME) $(VERSION).$(BUILD_VERSION) 2> /dev/null
	@touch $(MAKE_VERSION)

endif
