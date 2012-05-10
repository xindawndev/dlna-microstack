################################################################################
## @file:       distclean.mk
## @author      张框正
## @brief       删除临时文件，中间文件
## @version     1.0
###############################################################################
.PHONY: distclean
distclean: 
	@$(ECHO) clean $(TARGET_DIRECTORY)
	@$(RM) $(TARGET_DIRECTORY)
