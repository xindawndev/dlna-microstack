################################################################################
## @file:       depends.mk for pack
## @author      张框正 
## @brief       packet 依赖关系 只取出.so与可执行文件
## @version     1.0
###############################################################################

include $(ROOT_MAKE_DIRECTORY)/func/info.mk

include $(PACK_MAKE_DIRECTORY)/func/depends.mk

#iiiii                   := $(call enable_item_depends_pack,$(LOCAL_NAME),dirs_z)

define pack_print
$(2) := $(strip $($(2)) $(call get_item,$(1),File))
endef

iiii   := $(call tree_visit,$(LOCAL_NAME),File Depends,pack_print,dirs_z,depends_z)
dirs_z := $(patsubst __%,%,$(patsubst %__,%,$(dirs_z)))

DEPEND_FILES := $(strip $(foreach item,$(dirs_z),$(addprefix $(item)/,$(call get_item_info,$(item),File))))
DEPEND_FILES := $(filter-out %.tar.gz %.a,$(DEPEND_FILES))
DEPEND_FILES  := $(addprefix $(PLATFORM_BUILD_DIRECTORY),$(DEPEND_FILES))

