################################################################################
## @file: 	static2.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	生成合并的静态链接库的规则
## @version	1.0
###############################################################################

include $(TARGET_MAKE_DIRECTORY)/link.mk

$(TARGET_FILE_FULL): $(SOURCE_OBJECTS) $(HEADER_OBJECTS) $(MAKEFILE_LIST) $(DEPEND_FILES)
	@$(RM) $@
	$(LT) $(LINK_FLAGS) $(SOURCE_OBJECTS_FULL) $(LIB_PATHS) $(LIB_NAMES) -o $@
