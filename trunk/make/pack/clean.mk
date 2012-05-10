################################################################################
## @file:       clean.mk
## @author     	张框正
## @brief       删除临时文件，中间文件
## @version     1.0
###############################################################################
.PHONY: clean
clean: 
	@$(ECHO) clean "$(addprefix $(TARGET_DIRECTORY)/,$(notdir $(DEPEND_FILES) $(PACKET_DEPEND_FILES))) $(TARGET_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)"
	@$(RM) $(addprefix $(TARGET_DIRECTORY)/,$(notdir $(PACKET_DEPEND_FILES))) $(TARGET_DIRECTORY)/$(PLATFORM_STRATEGY_NAME)
