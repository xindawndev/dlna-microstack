################################################################################
## @file:       publish.mk
## @author      张框正
## @brief       删除临时文件，中间文件
## @version     1.0
###############################################################################

.PHONY: publish
publish : $(TARGET_FILE_FULL)
	@$(ECHO) "$(MKDIR) $(ROOT_PUBLISH_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)"
	@$(MKDIR) $(ROOT_PUBLISH_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)
	@$(ECHO) "$(LN) $(TARGET_FILE_FULL) $(ROOT_PUBLISH_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)/$(TARGET_FILE)"
	@$(LN) $(TARGET_FILE_FULL) $(ROOT_PUBLISH_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)/$(TARGET_FILE)
