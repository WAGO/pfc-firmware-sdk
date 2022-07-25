
# -*-makefile-*-
#
# Copyright (C) 2012 Jan Weitzel <j.weitzel@phytec.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_BAREBOX_MLO_LEGACY) += barebox_mlo_legacy

#
# Paths and names
#
BAREBOX_MLO_LEGACY_VERSION	:= $(call remove_quotes,$(PTXCONF_BAREBOX_MLO_LEGACY_VERSION))
BAREBOX_MLO_LEGACY_MD5		:= $(call remove_quotes,$(PTXCONF_BAREBOX_MLO_LEGACY_MD5))
BAREBOX_MLO_LEGACY		:= barebox-$(BAREBOX_MLO_LEGACY_VERSION)
BAREBOX_MLO_LEGACY_URL		= http://www.barebox.org/download/$(BAREBOX_MLO_LEGACY).$(BAREBOX_SUFFIX)
BAREBOX_MLO_LEGACY_DIR		:= $(BUILDDIR)/barebox_mlo_legacy-$(BAREBOX_MLO_LEGACY_VERSION)
BAREBOX_MLO_LEGACY_SOURCE	= $(SRCDIR)/$(BAREBOX_MLO_LEGACY).$(BAREBOX_SUFFIX)
BAREBOX_MLO_LEGACY_LICENSE	:= GPL-2.0

BAREBOX_MLO_LEGACY_CONFIG	:= $(call remove_quotes, \
		$(PTXDIST_PLATFORMCONFIGDIR)/$(PTXCONF_BAREBOX_MLO_LEGACY_CONFIG))

BAREBOX_MLO_LEGACY_PFC200_G1_MAX_SIZE := 61900

# ----------------------------------------------------------------------------
# Include
# ----------------------------------------------------------------------------
#include $(PTXDIST_WORKSPACE)/rules/other/definitions.make
include $(PTXDIST_PLATFORMCONFIGDIR)/rules/other/definitions.make

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

BAREBOX_MLO_LEGACY_WRAPPER_BLACKLIST := \
	TARGET_HARDEN_RELRO \
	TARGET_HARDEN_BINDNOW \
	TARGET_HARDEN_PIE \
	TARGET_DEBUG \
	TARGET_BUILD_ID

BAREBOX_MLO_LEGACY_PATH	:= PATH=$(CROSS_PATH)
BAREBOX_MLO_LEGACY_ENV	    := KCONFIG_NOTIMESTAMP=1 \
 pkg_wrapper_blacklist="$(BAREBOX_MLO_LEGACY_WRAPPER_BLACKLIST)" 

BAREBOX_MLO_LEGACY_CONF_TOOL	:= kconfig
BAREBOX_MLO_LEGACY_CONF_OPT	:= \
			V=$(PTXDIST_VERBOSE) \
			HOSTCC=$(HOSTCC) \
			ARCH=$(PTXCONF_BAREBOX_MLO_LEGACY_ARCH_STRING) \
			CROSS_COMPILE=$(BOOTLOADER_CROSS_COMPILE)

BAREBOX_MLO_LEGACY_MAKEVARS	:= $(BAREBOX_MLO_LEGACY_CONF_OPT)

ifdef PTXCONF_BAREBOX_MLO_LEGACY
$(BAREBOX_MLO_LEGACY_CONFIG):
	@echo
	@echo "****************************************************************************"
	@echo "*Please generate a barebox MLO config with 'ptxdist menuconfig barebox_mlo'*"
	@echo "****************************************************************************"
	@echo
	@echo
	@exit 1
endif

ifneq ($(wildcard $(BAREBOX_MLO_LEGACY_CONFIG)),)

$(STATEDIR)/barebox_mlo_legacy.prepare: $(BAREBOX_MLO_LEGACY_CONFIG)

else

$(STATEDIR)/barebox_mlo_legacy.prepare:
	@$(call targetinfo)

	@echo "Using defconfigs"

	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifeq ($(wildcard $(BAREBOX_MLO_LEGACY_CONFIG)),)

$(STATEDIR)/barebox_mlo_legacy.compile:
	@$(call targetinfo)

	@for i in "$(PTXCONF_BAREBOX_MLO_LEGACY_CONFIG)"; \
	do \
		$(call build_target, BAREBOX_MLO_LEGACY, $${i}); \
	done

	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox_mlo_legacy.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox_mlo_legacy.targetinstall:
	@$(call targetinfo)
	for image in `ls $(BAREBOX_MLO_LEGACY_DIR)/images/barebox-*.img`; do \
		> $(IMAGEDIR)/MLO; \
		_suffix=$$(basename $$(echo $${image} | sed 's/-mlo.img//g')); \
		_suffix=$$(echo $${_suffix} | sed 's/barebox-//g'); \
		install -D -m644 $$image $(IMAGEDIR)/`basename $$image`; \
		if [ ! -e "$(IMAGEDIR)/MLO.$${_suffix}" ]; then \
			ln -sf `basename $$image` $(IMAGEDIR)/MLO.$${_suffix}; \
		fi; \
	done
	if [ ! -e "$(IMAGEDIR)/MLO" ]; then \
		ptxd_get_path "$(BAREBOX_MLO_LEGACY_DIR)/MLO" \
			"$(BAREBOX_MLO_LEGACY_DIR)/barebox.bin.ift" && \
		install -D -m644 "$${ptxd_reply}" "$(IMAGEDIR)/MLO"; \
	else \
		rm -f $(IMAGEDIR)/MLO; \
	fi
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox_mlo_legacy.clean:
	@$(call targetinfo)
	@$(call clean_pkg, BAREBOX_MLO_LEGACY)

# ----------------------------------------------------------------------------
# oldconfig / menuconfig
# ----------------------------------------------------------------------------

barebox_mlo_legacy_oldconfig barebox_mlo_legacy_menuconfig: $(STATEDIR)/barebox_mlo_legacy.extract
	@if test -e $(BAREBOX_MLO_LEGACY_CONFIG); then \
		cp $(BAREBOX_MLO_LEGACY_CONFIG) $(BAREBOX_MLO_LEGACY_DIR)/.config; \
		$(eval COMPARE_CONFIG := $(BAREBOX_MLO_LEGACY_CONFIG)) \
	else \
		echo "** choose defconfig: $(PTXCONF_BAREBOX_MLO_LEGACY_CONFIG) "; \
		echo ""; \
		read _choosed_defconfig; \
		echo $${_choosed_defconfig} > $(BAREBOX_MLO_LEGACY_DIR)/_choosed_defconfig; \
		$(call defconfig, BAREBOX_MLO_LEGACY, $${_choosed_defconfig}); \
		cp $(BAREBOX_MLO_LEGACY_DIR)/.config $(BAREBOX_MLO_LEGACY_DIR)/tmpconfig; \
		$(eval COMPARE_CONFIG := $(BAREBOX_MLO_LEGACY_DIR)/tmpconfig) \
	fi

	cd $(BAREBOX_MLO_LEGACY_DIR) && \
		$(BAREBOX_MLO_LEGACY_PATH) $(BAREBOX_MLO_LEGACY_CONF_ENV) $(MAKE) \
			$(BAREBOX_MLO_LEGACY_CONF_OPT) $(subst barebox_mlo_,,$@)

	@if cmp -s $(BAREBOX_MLO_LEGACY_DIR)/.config $(COMPARE_CONFIG); then \
		echo "barebox_mlo configuration unchanged"; \
	elif [ -e $(BAREBOX_MLO_LEGACY_DIR)/_choosed_defconfig ]; then \
		$(call savedefconfig, BAREBOX_MLO_LEGACY); \
		mv $(BAREBOX_MLO_LEGACY_DIR)/defconfig $(BAREBOX_MLO_LEGACY_DIR)/arch/$(PTXCONF_BAREBOX_MLO_LEGACY_ARCH_STRING)/configs/$$(cat $(BAREBOX_MLO_LEGACY_DIR)/_choosed_defconfig); \
		echo "new kernel configuration saved $$(cat $(BAREBOX_MLO_LEGACY_DIR)/_choosed_defconfig)"; \
		rm -f $(COMPARE_CONFIG); \
		rm -f $(BAREBOX_MLO_LEGACY_DIR)/_choosed_defconfig; \
	else \
		cp $(BAREBOX_MLO_LEGACY_DIR)/.config $(BAREBOX_MLO_LEGACY_CONFIG); \
	fi

# vim: syntax=make
