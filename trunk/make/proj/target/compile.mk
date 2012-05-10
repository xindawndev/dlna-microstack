################################################################################
## @file:	compile.mk
## @author	π˘¥∫√Ø <gcm.ustc.edu>
## @brief	±‡“ÎπÊ‘Ú
## @version	1.0
###############################################################################

INCLUDES		:= $(HEADER_DIRECTORYS)

ifneq ($(HEADERS),)
	INCLUDES		:= $(OBJECT_DIRECTORY) $(INCLUDES)
endif

INCLUDES		:= $(addprefix -I,$(INCLUDES))

vpath %.h $(patsubst -I%,%,$(INCLUDES))

SOURCE_DEPENDS		:= $(addprefix $(DEPEND_DIRECTORY)/,$(SOURCE_DEPENDS))

HEADER_DEPENDS		:= $(addprefix $(DEPEND_DIRECTORY)/,$(HEADER_DEPENDS))

$(SOURCE_OBJECTS): %.o: $(SOURCE_DIRECTORY)/%$(SOURCE_SUFFIX) $(HEADER_OBJECTS) $(MAKEFILE_LIST) | mkdirs 
	$(CXX) $(COMPILE_FLAGS) $(INCLUDES) -c $< -o $(OBJECT_DIRECTORY)/$@

find_header_file	= $(firstword $(wildcard $(addsuffix /$(1),$(HEADER_DIRECTORYS))))

define rule_header_object
$(1) : $(call find_header_file,$(1))a $(MAKEFILE_LIST) | mkdirs
	$(CXX) $(COMPILE_FLAGS) $(INCLUDES) -c $< -o $(OBJECT_DIRECTORY)/$@
endef

#$(foreach HEADER_OBJECT,$(HEADER_OBJECTS),$(eval $(call rule_header_object,$(HEADER_OBJECT))))

$(HEADER_OBJECTS): %.gch : % $(MAKEFILE_LIST) | mkdirs
	$(CXX) $(COMPILE_FLAGS) $(INCLUDES) -c $< -o $(OBJECT_DIRECTORY)/$@

$(SOURCE_DEPENDS): $(DEPEND_DIRECTORY)/%.dep: $(SOURCE_DIRECTORY)/%$(SOURCE_SUFFIX) $(MAKEFILE_LIST) | mkdirs
	$(CXX) -MM -MP $(COMPILE_FLAGS) $(INCLUDES) -MT $(patsubst $(DEPEND_DIRECTORY)/%.dep,%.o,$@) -MT $@ $< -MF $@

$(HEADER_DEPENDS): $(DEPEND_DIRECTORY)/%.dep: % $(MAKEFILE_LIST) | mkdirs
	$(CXX) -MM -MP $(COMPILE_FLAGS) $(INCLUDES) -MT $(patsubst $(DEPEND_DIRECTORY)/%.dep,%.gch,$@) -MT $@ $< -MF $@

-include $(HEADER_DEPENDS) $(SOURCE_DEPENDS)
