################################################################################
## @file:	dirs.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	目录处理函数
## @version	1.0
###############################################################################

include $(ROOT_MAKE_DIRECTORY)/func/base.mk

# 取路径目录部分，不带最后的/
# argment1:	路径（集合）
# return:	目录部分（集合）

directory		= $(patsubst %/,%,$(dir $(1)))

# 计算两个目录间的反向相对目录路径
# argment1:	相对目录路径
# argment2: 	原先被相对的目录（默认，pwd）
# return:	反向相对目录路径
# desc:
#   a=revert($(2))
#   b=addsuffix(..,a)
#   c=split(/,c)
#   d=addsuffix(.,c)
#   e=join(b,d)
#   f=patsubst(%.....,%,e)
#   g=patsubst(%..,,f)
#   h=patsubst(%.,..,g)
#   return joinlist(/,h) 

revert_directory    	= $(call joinlist,/,$(call revert,$(patsubst %.,..,$(patsubst %..,,$(patsubst %.....,%,$(join $(addsuffix ..,$(call revert,$(subst /, ,$(if $(2),$(2),$(shell pwd))))),$(addsuffix .,$(filter-out .,$(call split,/,$(1))))))))))  

# 计算相对路径作用后的结果
# argment1:	相对目录路径（列表）
# argment2: 	原先被相对的目录（默认，pwd）
# return:	相对路径作用后的结果（列表）

reletive_directory	= $(subst //,/,$(foreach path,$(1),$(call reletive_directory2,$(call split,/,$(path)),$(if $(2),$(patsubst %/,%,$(2)),$(shell pwd)))))

reletive_directory1	= $(if $(1),$(if $(findstring ..,$(1)),$(call directory,$(2)),$(if $(findstring .,$(1)),$(2),$(2)/$(1))),$(2))

reletive_directory2	= $(if $(1),$(call reletive_directory2,$(wordlist 2,$(words $(1)),$(1)),$(call reletive_directory1,$(word 1,$(1)),$(2))),$(2))

# 计算路径的根目录
# argment1:	路径（列表）
# argment2:	深度（默认1）
# return:	根目录（列表）

root_directory		= $(foreach dir,$(1),$(if $(filter /%,$(dir)),/)$(call joinlist,/,$(wordlist 1,$(if $(2),$(2),1),$(call split,/,$(dir)))))

# 计算路径的直接，间接根目录
# argment1:	路径
# return:	直接，间接根目录列表

root_directories	= $(call directory,$(1)) $(if $(1),$(call root_directories,$(call directory,$(1))))

