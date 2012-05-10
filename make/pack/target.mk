################################################################################
## @file:       target.mk
## @author      张框正
## @brief       删除临时文件，中间文件
## @version     1.0
###############################################################################

include $(PACK_MAKE_DIRECTORY)/depends.mk

MAKE_DIRECTORYS         := $(TARGET_DIRECTORY) $(TARGET_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)

.PHONY: target
target: $(TARGET_FILE_FULL)

include $(ROOT_MAKE_DIRECTORY)/mkdirs.mk

.PHONY: $(PACKET_DEPEND_FILES)
$(PACKET_DEPEND_FILES): mkdirs
	@$(ECHO) $@
	@$(RM) $(TARGET_DIRECTORY)/$(notdir $@)
	@svn up $(ROOT_DIRECTORY)$@ 2>&0 > /dev/null
	@svn export $(ROOT_DIRECTORY)$@ $(TARGET_DIRECTORY)/$(notdir $@) 2>&0 > /dev/null

.PHONY: $(DEPEND_FILES)
$(DEPEND_FILES): mkdirs
	@$(ECHO) $@
	@$(STRIP) $@ -o $(TARGET_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)/$(notdir $@)

$(TARGET_FILE_FULL): $(DEPEND_FILES) $(PACKET_DEPEND_FILES) $(MAKEFILE_LIST) 
	@$(CD) $(TARGET_DIRECTORY) ; tar -czv -f $(TARGET_FILE) $(addprefix $(PLATFORM_STRATEGY_NAME)/,$(notdir $(DEPEND_FILES))) $(notdir $(PACKET_DEPEND_FILES)) 2>&0 > /dev/null
