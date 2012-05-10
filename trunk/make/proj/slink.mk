################################################################################
## @file:	slink.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	链接已发布的工程目标
## @version	1.0
###############################################################################
.PHONY: target
target: | mkdirs

MAKE_DIRECTORYS		:= $(TARGET_DIRECTORY)

include $(ROOT_MAKE_DIRECTORY)/mkdirs.mk

CONFIG_build_version	:= no

include $(PROJ_MAKE_DIRECTORY)/publish.mk

PUBLISH_FILE		:= $(PRIVATE_DIRECTORY)/$(TARGET_FILE_PRIVATE)

$(TARGET_DIRECTORY)/$(TARGET_FILE_PRIVATE): $(PUBLISH_FILE) | mkdirs
	$(LN) $(PUBLISH_FILE) $(TARGET_DIRECTORY)/$(TARGET_FILE_PRIVATE)
	
target: $(TARGET_DIRECTORY)/$(TARGET_FILE_PRIVATE)
