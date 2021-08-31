

# out dir for all license files
# all files will be installed by ptxdist makefile
LICENSE_OUT_DIR := $(PROJECT_ROOT)/out/licenses
# desitnation of OSS licnese files in the PFC platform 
LICENSE_DEST_DIR := $(DESTDIR)/usr/share/licenses/oss

# $(call gen_license_file_path, packagename, version)
# Generate the target file path for a lincense on the PFC, 
# based on the package name and the version. 
gen_license_file_path=/usr/share/licenses/oss/license.$(strip $1)_$(strip $2).txt

# $(call create_license_target, packagename, version, license_file)
create_license_target = $$(DESTDIR)$(call gen_license_file_path, $1, $2) : | $$(GEN_DIRS) ;\
	mkdir -p $$(dir $$@); cp $3 $$@;

# If a build target has _INSTALL_LICENSE set, add the target filename to the install target list.
INSTALL_TARGETS += \
	$(strip \
		$(foreach BUILDTARGET,$(BUILDTARGETS),\
			$(if $($(BUILDTARGET)_INSTALL_LICENSE),\
				$(DESTDIR)$(call gen_license_file_path,$(basename $(BUILDTARGET)),$($(BUILDTARGET)_VERSION))\
			,)\
		)\
	)

# Add the out folder to the gen dirs in order to get it created and cleaned for us.
GEN_DIRS += $(LICENSE_OUT_DIR)

ifneq ($(strip $(MAKECMDGOALS)),clean)
-include $(LICENSE_OUT_DIR)/license_install_rules.mk
endif

# Rule generation
$(LICENSE_OUT_DIR)/license_install_rules.mk: $(MK_FILES) | $(GEN_DIRS)
	$(SILENT) \
	echo 'Generate $(LICENSE_OUT_DIR)/license_install_rules.mk'; \
	{ \
		$(foreach BUILDTARGET,$(BUILDTARGETS), \
			$(if $($(BUILDTARGET)_INSTALL_LICENSE), \
				echo '$(call create_license_target,$(basename $(BUILDTARGET)),$($(BUILDTARGET)_VERSION),$($(BUILDTARGET)_INSTALL_LICENSE))'; \
				echo ; \
			,) \
		) \
		echo "no-op to avoid syntax error if the foreach loop yields an empty list" > /dev/null; \
	} > $@
