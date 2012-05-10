################################################################################
## @file: 	target.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	生成工程目标
## @version	1.0
###############################################################################

.PHONY: target
target: | mkdirs

TARGET_MAKE_DIRECTORY	:= $(PROJ_MAKE_DIRECTORY)/target

include $(TARGET_MAKE_DIRECTORY)/files.mk

include $(TARGET_MAKE_DIRECTORY)/flags.mk

include $(TARGET_MAKE_DIRECTORY)/compile.mk

MAKE_DIRECTORYS		:= $(DEPEND_DIRECTORY) $(OBJECT_DIRECTORY) $(DEPEND_SUB_DIRECTORYS) $(OBJECT_SUB_DIRECTORYS)

include $(ROOT_MAKE_DIRECTORY)/mkdirs.mk

include $(PROJ_MAKE_DIRECTORY)/version.mk

DEPENDS                 := $(PROJECT_DEPENDS) $(PLATFORM_DEPENDS)

#include $(TARGET_MAKE_DIRECTORY)/link.mk


include $(TARGET_MAKE_DIRECTORY)/$(PROJECT_TYPE).mk
target: $(TARGET_FILE_FULL)
