ifneq ($(PROJECT_DEPEND_SHELL),)
    $(foreach sfile,$(PROJECT_DEPEND_SHELL),$(shell $(ROOT_PROJECT_DIRECTORY)$(LOCAL_NAME)/$(sfile)))
endif
