################################################################################
## @file: 	bin.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	生成可执行文件的规则
## @version	1.0
###############################################################################

include $(TARGET_MAKE_DIRECTORY)/link.mk

$(TARGET_FILE_FULL): $(SOURCE_OBJECTS) $(DEPEND_FILES) $(MAKEFILE_LIST)
	@$(RM) $@
	$(LD) $(LINK_FLAGS) $(PLATFORM_CRTBEGIN_DYNAMICBIN) $(SOURCE_OBJECTS_FULL) $(LIB_PATHS) $(LIB_NAMES) $(PLATFORM_CRTEND_DYNAMICBIN) -o $@
