# $1 = <package> e.g. BAREBOX
# $2 = *_defconfig
define build_target
$(call defconfig, $(1), $(2)) && \
	$(call build, $(1))
endef

# $1 = <package> e.g. BAREBOX
# $2 = *_defconfig
define defconfig
cd "$($(strip $(1))_DIR)" && $($(strip $(1))_PATH) $($(strip $(1))_ENV) $(MAKE) \
	$($(strip $(1))_MAKEVARS) $(2)
endef

# $1 = <package> e.g. BAREBOX
define build
cd "$($(strip $(1))_DIR)" && $($(strip $(1))_PATH) $($(strip $(1))_ENV) $(MAKE) \
	$($(strip $(1))_MAKEVARS)
endef

# $1 = <package> e.g. BAREBOX
define savedefconfig
cd "$($(strip $(1))_DIR)" && $($(strip $(1))_PATH) $($(strip $(1))_ENV) $(MAKE) \
	$($(strip $(1))_MAKEVARS) savedefconfig
endef
