################################################################################
## @file:       link.mk
## @author      张框正
## @brief       生成动态链接库的规则
## @version     1.0
###############################################################################

ifneq ($(CONFIG_LIB)-$(CONFIG_combine_static_lib),static-no)

include $(PROJ_MAKE_DIRECTORY)/depends.mk

LIB_PATHS               := $(addprefix -L,$(dir $(DEPEND_FILES)))
LIB_NAMES               := $(notdir $(DEPEND_FILES))
LIB_NAMES               := $(patsubst lib%.a,%,$(LIB_NAMES))
LIB_NAMES               := $(patsubst lib%.so,%,$(LIB_NAMES))
LIB_NAMES               := $(addprefix -l,$(LIB_NAMES))

ifneq ($(CONFIG_LIB)-$(CONFIG_combine_static_lib),static-yes)

LIB_NAMES               := $(LIB_NAMES) $(addprefix -l,$(SYSTEM_LIB))
LIB_NAMES               := $(LIB_NAMES) $(addprefix -l,$(PLATFORM_DEPEND_LIBRARYS))

ifeq ($(CONFIG_COMPILE),release)
        ifneq ($(PROJECT_VERSION_SCRIPT),)
		VERSION_SCRIPT		:= $(SOURCE_DIRECTORY)/$(PROJECT_VERSION_SCRIPT)
        endif
endif

ifneq ($(VERSION_SCRIPT),)
	LINK_FLAGS              := $(LINK_FLAGS) -Wl,--version-script=$(VERSION_SCRIPT)
	DEPEND_FILES		:= $(VERSION_SCRIPT) $(DEPEND_FILES)
endif

LINK_FLAGS		:= $(LINK_FLAGS) -Wl,-rpath=.

endif # ifneq ($(CONFIG_LIB)-$(CONFIG_combine_static_lib),static-yes)

LINK_FLAGS		:= $(strip $(LINK_FLAGS))

endif # ifneq ($(CONFIG_LIB)-$(CONFIG_combine_static_lib),static-no)

SOURCE_OBJECTS_FULL	:= $(addprefix $(OBJECT_DIRECTORY)/, $(SOURCE_OBJECTS))
