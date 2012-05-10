################################################################################
## @file:	depends.mk
## @author      张框正 
## @brief       提取项目信息的函数/用于依赖目录取
## @version     1.0
###############################################################################

include $(ROOT_MAKE_DIRECTORY)/func/info.mk

# __为了防止重名
define make_item_depends_pack
$(2)            := $(strip $($(2)) __$(1)__)
endef

define get_format_depend
        $(strip \
        $(shell $(MAKE) -C $(PLATFORM_BUILD_DIRECTORY) LOCAL_NAME=$(1) config="$(strip $(config))" info | \
                awk -F : ' 
                        BEGIN { $(foreach info,$(2),infos["$(info)"]);print "$(1)";} 
                        { if ($$1 in infos) printf ":%s",$$2; }END{printf "!"}'))
endef

define enable_item_depends_pack
$(1)
$(eval $(call make_item_depends_pack,$(1),$(2)))
$(foreach item,$(1),$(call enable_item_depends_pack,$(filter-out $(strip $($(2))),$(call get_item_depends,$(item))),$(2)))
endef



define fromat_depends
$(1)
$(eval $(call make_item_depends_pack,$(1),$(2)))
$(foreach aaa,$(1),$(eval $(call make_item_depends_pack,$(call get_format_depend,$(aaa),File Depends DependLibs),$(3))))
$(foreach item,$(1),$(call fromat_depends,$(filter-out $(strip $($(2))),$(call get_item_depends,$(item))),$(2),$(3)))
endef

# param 1 root 2 handle
define get_child
$(strip $(call get_item,$(1),Depends))
endef

define get_item_info_vaul
$(eval get_item_info_as_var_ret:=$(strip \
        $(shell $(MAKE) -C $(PLATFORM_BUILD_DIRECTORY) LOCAL_NAME=$(1) config="$(strip $(config))" info | \
                awk -F : ' 
                        BEGIN { $(foreach info,$(2),infos["$(info)"];);ii="$(1)"; } 
                        { if ($$1 in infos) printf "%s%s $$(eval %s_%s:=%s)", ii, $$1, ii, $$1, $$2; } 
                ' \
        ) \
))$(get_item_info_as_var_ret)
endef


define get_item
$($(1)_$(2))
endef


#param 1 rootdir(/ppbox/ppbox)  2 list(Depends File) 3 fun(for print) 4 variable(key) 5 fromat 
define tree_visit
$(1)
$(eval $(call make_item_depends_pack,$(1),$(4)))
$(call get_item_info_vaul,$(1),$(2))
$(eval $(call $(3),$(1),$(5)))
$(foreach item,$(call get_child,$(1)),$(if $(findstring __$(item)__,$($(4))),,$(call tree_visit,$(item),$(2),$(3),$(4),$(5))))
endef

