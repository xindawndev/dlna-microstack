################################################################################
## @file: 	depends.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	寻找依赖项.
## @version	1.0
###############################################################################

include $(ROOT_MAKE_DIRECTORY)/func/info.mk
include $(ROOT_MAKE_DIRECTORY)/func/depends.mk

include $(PROJ_MAKE_DIRECTORY)/func/info.mk

#iiiii := $(call fromat_depends,$(LOCAL_NAME),dirs_z,depends_z)

define proj_print
$(2) := $(strip $($(2)) $(1):$(call get_item,$(1),Type):$(call get_item,$(1),File):$(call get_item,$(1),Depends):$(call get_item,$(1),DependLibs)!)
endef

iiii := $(call tree_visit,$(LOCAL_NAME),Type File Depends DependLibs,proj_print,dirs_z,depends_z)

DEPEND_FILES := $(shell echo "$(depends_z)" | awk -f $(ROOT_MAKE_DIRECTORY)/awk/depend.awk)

DEPEND_FILES		:= $(patsubst %.dll,%.a,$(DEPEND_FILES))

SYSTEM_LIB := $(filter-out %.a %$(DYNAMIC_NAME_SUFFIX),$(DEPEND_FILES))
DEPEND_FILES := $(filter %.a %$(DYNAMIC_NAME_SUFFIX),$(DEPEND_FILES))
DEPEND_FILES  := $(addprefix $(PLATFORM_BUILD_DIRECTORY),$(DEPEND_FILES))

