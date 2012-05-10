################################################################################
## @file: 	clean.mk
## @author	郭春茂 <gcm.ustc.edu>
## @brief	删除临时文件，中间文件
## @version	1.0
###############################################################################
.PHONY: clean 
clean:
	@$(ECHO) clean $(OBJECT_DIRECTORY) $(DEPEND_DIRECTORY) $(TARGET_FILE_FULL)
	@$(RM) $(TARGET_FILE_FULL) $(OBJECT_DIRECTORY) $(DEPEND_DIRECTORY)
