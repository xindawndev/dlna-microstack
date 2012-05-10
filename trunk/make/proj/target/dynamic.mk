################################################################################
## @file: 	dynamic.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	生成动态链接库的规则
## @version	1.0
###############################################################################

include $(TARGET_MAKE_DIRECTORY)/link.mk

LINK_FLAGS		:= $(LINK_FLAGS) -shared -Wl,-soname=$(TARGET_FILE_VERSION)

ifeq ($(DYNAMIC_NAME_SUFFIX),.dll)
	LINK_FLAGS		:= $(LINK_FLAGS) -Wl,--out-implib,$(TARGET_FILE_FULL:%.dll=%.a)
endif

$(TARGET_FILE_FULL): $(SOURCE_OBJECTS) $(DEPEND_FILES) $(MAKEFILE_LIST)
	@$(RM) $@
	$(LD) $(LINK_FLAGS) $(PLATFORM_CRTBEGIN_DYNAMIC) $(SOURCE_OBJECTS_FULL) $(LIB_PATHS) $(LIB_NAMES) $(PLATFORM_CRTEND_DYNAMIC) -o $@
