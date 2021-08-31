# -*-makefile-*-
#
# Copyright (C) 2007 by Sascha Hauer
#               2008, 2009 by Marc Kleine-Budde
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_BAREBOX) += barebox

#
# Paths and names
#


#
# DON'T FORGET TO UPDATE FW_VERSION AND LIST OF COMPATIBLE FIRMWARES WHEN UPDATING BAREBOX!!!
# The list should include all barebox versions that don't need to be updated in field.
#

#ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	BAREBOX_COMPAT_FW_LIST := "12"
#else
BAREBOX_COMPAT_FW_LIST := "99"
#endif


BAREBOX_VERSION	:= $(call remove_quotes,$(PTXCONF_BAREBOX_VERSION))
BAREBOX_MD5	:= $(call remove_quotes,$(PTXCONF_BAREBOX_MD5))
BAREBOX		:= barebox-$(BAREBOX_VERSION)
BAREBOX_SUFFIX	:= tar.bz2
BAREBOX_URL	:= $(call barebox-url, BAREBOX)
BAREBOX_SOURCE	:= $(SRCDIR)/$(BAREBOX).$(BAREBOX_SUFFIX)
BAREBOX_DIR	:= $(BUILDDIR)/$(BAREBOX)
BAREBOX_LICENSE	:= GPL-2.0
BAREBOX_DEVPKG	:= NO

BAREBOX_CONFIG	:= $(call remove_quotes, $(PTXDIST_PLATFORMCONFIGDIR)/$(PTXCONF_BAREBOX_CONFIG))

# ----------------------------------------------------------------------------
# Include
# ----------------------------------------------------------------------------
#include $(PTXDIST_WORKSPACE)/rules/other/definitions.make
include $(PTXDIST_PLATFORMCONFIGDIR)/rules/other/definitions.make

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

BAREBOX_WRAPPER_BLACKLIST := \
	TARGET_HARDEN_RELRO \
	TARGET_HARDEN_BINDNOW \
	TARGET_HARDEN_PIE \
	TARGET_DEBUG \
	TARGET_BUILD_ID

BAREBOX_ENV := \
	KCONFIG_NOTIMESTAMP=1 \
	pkg_wrapper_blacklist="$(BAREBOX_WRAPPER_BLACKLIST)"

BAREBOX_MAKEVARS := \
	V=$(PTXDIST_VERBOSE) \
	HOSTCC=$(HOSTCC) \
	ARCH=$(PTXCONF_BAREBOX_ARCH_STRING) \
	CROSS_COMPILE=$(BOOTLOADER_CROSS_COMPILE) \
	$(PARALLELMFLAGS)

BAREBOX_TAGS_OPT := TAGS tags cscope

ifdef PTXCONF_BAREBOX
$(BAREBOX_CONFIG):
	@echo
	@echo "****************************************************************************"
	@echo "**** Please generate a barebox config with 'ptxdist menuconfig barebox' ****"
	@echo "****************************************************************************"
	@echo
	@echo
	@exit 1
endif

ifdef PTXCONF_BAREBOX_EXTRA_ENV_PATH
$(STATEDIR)/barebox.prepare: $(call remove_quotes,$(PTXCONF_BAREBOX_EXTRA_ENV_PATH))
$(STATEDIR)/barebox.prepare: $(shell find $(call remove_quotes,$(PTXCONF_BAREBOX_EXTRA_ENV_PATH)) -print 2>/dev/null)
endif

ifneq ($(wildcard $(BAREBOX_CONFIG)),)

$(STATEDIR)/barebox.prepare: $(BAREBOX_CONFIG)
	@$(call targetinfo)

	@echo "Using barebox config file: $(<)"
	@install -m 644 "$(<)" "$(BAREBOX_DIR)/.config"

	@$(call ptx/oldconfig, BAREBOX)
	@diff -q -I "# [^C]" "$(BAREBOX_DIR)/.config" "$(<)" > /dev/null || cp "$(BAREBOX_DIR)/.config" "$(<)"

ifdef PTXCONF_BAREBOX_EXTRA_ENV
	@rm -rf $(BAREBOX_DIR)/.ptxdist-defaultenv
	@ptxd_source_kconfig "${PTXDIST_PTXCONFIG}" && \
	ptxd_source_kconfig "${PTXDIST_PLATFORMCONFIG}" && \
	$(foreach path, $(call remove_quotes,$(PTXCONF_BAREBOX_EXTRA_ENV_PATH)), \
		if [ -d "$(path)" ]; then \
			ptxd_filter_dir "$(path)" \
			$(BAREBOX_DIR)/.ptxdist-defaultenv; \
		else \
			cp "$(path)" $(BAREBOX_DIR)/.ptxdist-defaultenv/; \
		fi;)
	@sed -i -e "s,^\(CONFIG_DEFAULT_ENVIRONMENT_PATH=.*\)\"$$,\1 .ptxdist-defaultenv\"," \
		$(BAREBOX_DIR)/.config
endif

else

$(STATEDIR)/barebox.prepare:

	@$(call targetinfo)

	@echo "Using defconfigs"

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox.compile:
	@$(call targetinfo)

ifneq ($(wildcard $(BAREBOX_CONFIG)),)

	@cd $(BAREBOX_DIR) && $(BAREBOX_PATH) $(BAREBOX_ENV) \
		$(MAKE) $(BAREBOX_MAKEVARS)
else
	@for i in "$(PTXCONF_BAREBOX_CONFIG)"; \
	do \
		$(call build_target, BAREBOX, $${i}); \
	done
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox.install:
	@$(call targetinfo)
ifdef PTXCONF_ARCH_X86
	@if [ -e $(BAREBOX_DIR)/scripts/setupmbr/setupmbr ]; then \
		install -v -D -m755 $(BAREBOX_DIR)/scripts/setupmbr/setupmbr \
			$(PTXCONF_SYSROOT_HOST)/bin/setupmbr; \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox.targetinstall:
	@$(call targetinfo)

	@$(call install_init, barebox)
	@$(call install_fixup, barebox,PRIORITY,optional)
	@$(call install_fixup, barebox,SECTION,base)
	@$(call install_fixup, barebox,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, barebox,DESCRIPTION,missing)

ifdef PTXCONF_BAREBOX_BAREBOXENV
	@$(call install_copy, barebox, 0, 0, 0755, $(BAREBOX_DIR)/scripts/bareboxenv-target, \
		/usr/bin/bareboxenv)
endif

	@$(call install_alternative, barebox, 0, 0, 0400, /etc/barebox-compatible-versions)
	@$(call install_replace, barebox, /etc/barebox-compatible-versions, @BAREBOX_COMPAT_FW_LIST@, $(strip $(BAREBOX_COMPAT_FW_LIST)))

	@$(call install_copy, barebox, 0, 0, 0644, $(BAREBOX_DIR)/COPYING, /usr/share/licenses/oss/license.barebox_$(shell echo $(BAREBOX_VERSION) | tr '_-' '.+').txt)

	@$(call install_finish, barebox)

	@rm -f $(IMAGEDIR)/barebox.bin*
	@for image in `ls $(BAREBOX_DIR)/images/barebox-*.img`; do \
		> $(IMAGEDIR)/barebox.bin; \
		_suffix=$$(basename $$(echo $${image} | sed 's/.img//g')); \
		_suffix=$$(echo $${_suffix} | sed 's/barebox-//g'); \
		install -D -m644 $$image $(IMAGEDIR)/`basename $$image`; \
		if [ ! -e $(IMAGEDIR)/barebox.bin.$${_suffix} ]; then \
			ln -sf `basename $$image` $(IMAGEDIR)/barebox.bin.$${_suffix}; \
		fi; \
	done
	@if [ -e $(IMAGEDIR)/barebox.bin ]; then \
		rm $(IMAGEDIR)/barebox.bin; \
	elif [ -e $(BAREBOX_DIR)/barebox-flash-image ]; then \
		install -D -m644 $(BAREBOX_DIR)/barebox-flash-image $(IMAGEDIR)/barebox-image; \
	else \
		install -D -m644 $(BAREBOX_DIR)/barebox.bin $(IMAGEDIR)/barebox-image; \
	fi
	@if [ -e $(BAREBOX_DIR)/common/barebox_default_env ]; then \
		install -D -m644 $(BAREBOX_DIR)/common/barebox_default_env $(IMAGEDIR)/barebox-default-environment; \
	elif [ -e $(BAREBOX_DIR)/barebox_default_env ]; then \
		install -D -m644 $(BAREBOX_DIR)/barebox_default_env $(IMAGEDIR)/barebox-default-environment; \
	fi

ifeq ($(PTXCONF_PLATFORM), wago-pfc-adv)
	# this is only a workaround for the bsp-build of pfc200 adv based
	# devices. Delete this lines when the bsp build script is fixed.
	@> $(IMAGEDIR)/firmware.tar
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install-Post
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox.targetinstall.post:
	@$(call targetinfo)

ifdef PTXCONF_BAREBOX_UIMAGE
#	Check if pfc200-barebox is build
	@if [ -f "$(IMAGEDIR)/barebox-am35xx-pfc-750-820x.img" ]; then \
		_image="$(IMAGEDIR)/barebox-am35xx-pfc-750-820x.img"; \
		mkimage -A arm -O u-boot -C none -T firmware -a 0xffffffff \
		-e 0xffffffff -n 'Linux-Barebox' -d $${_image} $(IMAGEDIR)/uImage; \
	else \
		echo "## ERROR: please build barebox in version >= 2014 and with pfc200-defconfig to build" \
		"an Barebox-uImage. to disable this option unselect PTXCONF_BAREBOX_UIMAGE ##"; \
		exit 1;\
	fi
endif

	@$(call touch)
#$(error "ERROR: please build barebox with pfc200-defconfig to build an Barebox-uImage")

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox.clean:
	@$(call targetinfo)
	@$(call clean_pkg, BAREBOX)
	@rm -rf $(IMAGEDIR)/barebox.bin* $(IMAGEDIR)/barebox-default-environment
	@rm -rf $(IMAGEDIR)/uImage

#       dont remove *mlo-files, they will be deleted by its corrosponding package
	@for image in $$(ls $(IMAGEDIR)/barebox-*.img); do \
		image_name=$$(basename $${image}); \
		if [ ! $$(echo $${image_name} | grep "mlo") ]; then \
			rm -f $${image}; \
		fi \
	done

# ----------------------------------------------------------------------------
# oldconfig / menuconfig
# ----------------------------------------------------------------------------

barebox_oldconfig barebox_menuconfig barebox_nconfig: $(STATEDIR)/barebox.extract
	@if test -e $(BAREBOX_CONFIG); then \
		cp $(BAREBOX_CONFIG) $(BAREBOX_DIR)/.config; \
		$(eval COMPARE_CONFIG := $(BAREBOX_CONFIG)) \
	else \
		echo "** choose defconfig: $(PTXCONF_BAREBOX_CONFIG) "; \
		echo ""; \
		read _choosed_defconfig; \
		echo $${_choosed_defconfig} > $(BAREBOX_DIR)/_choosed_defconfig; \
		$(call defconfig, BAREBOX, $${_choosed_defconfig}); \
		cp $(BAREBOX_DIR)/.config $(BAREBOX_DIR)/tmpconfig; \
		$(eval COMPARE_CONFIG := $(BAREBOX_DIR)/tmpconfig) \
	fi

	@cd $(BAREBOX_DIR) && \
		$(BAREBOX_PATH) $(BAREBOX_ENV) $(MAKE) $(BAREBOX_MAKEVARS) $(subst barebox_,,$@)

	@if cmp -s $(BAREBOX_DIR)/.config $(COMPARE_CONFIG); then \
		echo "barebox configuration unchanged"; \
	elif [ -e $(BAREBOX_DIR)/_choosed_defconfig ]; then \
		$(call savedefconfig, BAREBOX); \
		mv $(BAREBOX_DIR)/defconfig $(BAREBOX_DIR)/arch/$(PTXCONF_BAREBOX_ARCH_STRING)/configs/$$(cat $(BAREBOX_DIR)/_choosed_defconfig); \
		echo "new kernel configuration saved $$(cat $(BAREBOX_DIR)/_choosed_defconfig)"; \
		rm -f $(COMPARE_CONFIG); \
		rm -f $(BAREBOX_DIR)/_choosed_defconfig; \
	else \
		cp $(BAREBOX_DIR)/.config $(BAREBOX_CONFIG); \
	fi

# vim: syntax=make
