################################################################################
## @file:	flags.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	设定编译参数.
## @version	1.0
###############################################################################

ARCHIVE_FLAGS		:= rcs

ifeq ($(DEFAULT_FLAGS),)
	DEFAULT_FLAGS		:= 1
endif

ifeq ($(DEFAULT_FLAGS),1)
        ifeq ($(CONFIG_THREAD),multi)
		CONFIG_COMPILE_FLAGS		:= -pthread $(CONFIG_COMPILE_FLAGS)
		CONFIG_LINK_FLAGS		:= -pthread $(CONFIG_LINK_FLAGS)
        endif

        ifeq ($(CONFIG_COMPILE),debug)
		CONFIG_COMPILE_FLAGS		:= -Wall -g -DDEBUG $(CONFIG_COMPILE_FLAGS)
		CONFIG_LINK_FLAGS		:= $(CONFIG_LINK_FLAGS)
        else
		CONFIG_COMPILE_FLAGS		:= -Wall -Os -DNDEBUG $(CONFIG_COMPILE_FLAGS)
		CONFIG_LINK_FLAGS		:= -Wl,-Os $(CONFIG_LINK_FLAGS)
        endif

endif

COMPILE_FLAGS		:= -fPIC $(CONFIG_COMPILE_FLAGS) $(GLOBAL_COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(PROJECT_COMPILE_FLAGS)

ifneq ($(CONFIG_LIB),static)
LINK_FLAGS		:= $(CONFIG_LINK_FLAGS) $(GLOBAL_LINK_FLAGS) $(PLATFORM_LINK_FLAGS) $(PROJECT_LINK_FLAGS)
else
LINK_FLAGS		:= $(CONFIG_STATIC_LINK_FLAGS) $(GLOBAL_STATIC_LINK_FLAGS) $(PLATFORM_STATIC_LINK_FLAGS) $(PROJECT_LINK_FLAGS)
endif

COMPILE_FLAGS		:= $(filter-out $(addsuffix %,$(PLATFORM_DISABLE_FLAGS)),$(COMPILE_FLAGS))

LINK_FLAGS		:= $(filter-out $(addsuffix %,$(PLATFORM_DISABLE_FLAGS)),$(LINK_FLAGS))

COMPILE_FLAGS		:= $(strip $(COMPILE_FLAGS))

LINK_FLAGS		:= $(strip $(LINK_FLAGS))

