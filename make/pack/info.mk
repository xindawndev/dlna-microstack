################################################################################
## @file:       info.mk
## @author      张框正
## @brief       删除临时文件，中间文件
## @version     1.0
###############################################################################
.PHONY: info
info: 
	@$(ECHO) "Name: $(LOCAL_NAME)"
	@$(ECHO) "Type: $(LOCAL_TYPE)"
	@$(ECHO) "Config: $(CONFIG_COMPILE)"
	@$(ECHO) "Directory: $(TARGET_DIRECTORY)"
	@$(ECHO) "Target: $(PACKET_TARGET)"
	@$(ECHO) "Version: $(VERSION)"
	@$(ECHO) "File: $(TARGET_FILE_FULL)"
	@$(ECHO) "Depends: $(PACKET_DEPENDS)"
