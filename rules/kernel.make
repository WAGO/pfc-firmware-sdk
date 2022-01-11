# -*-makefile-*-
#
# Copyright (C) 2002-2009 by Pengutronix e.K., Hildesheim, Germany
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_KERNEL) += kernel


#
# when using a production release,
# we use the precompiled kernel from /opt
#
ifdef PTXCONF_PROJECT_USE_PRODUCTION
KERNEL_BDIR		:= $(PTXDIST_BASE_PLATFORMDIR)/build-target
else
KERNEL_BDIR		:= $(BUILDDIR)
endif


#
# Paths and names
#
KERNEL					:= linux-$(KERNEL_VERSION)
KERNEL_MD5			:= $(call remove_quotes,$(PTXCONF_KERNEL_MD5))
KERNEL_SUFFIX		:= tar.xz
KERNEL_DIR			:= $(KERNEL_BDIR)/$(KERNEL)
KERNEL_CONFIG		:= $(call remove_quotes, $(PTXDIST_PLATFORMCONFIGDIR)/$(PTXCONF_KERNEL_CONFIG))
KERNEL_LICENSE		:= GPL-2.0
KERNEL_URL			:= $(call kernel-url, KERNEL)
KERNEL_SOURCE		:= $(SRCDIR)/$(KERNEL).$(KERNEL_SUFFIX)
KERNEL_DEVPKG		:= NO
DEFCONFIG				:= $(findstring _defconfig,$(PTXCONF_KERNEL_CONFIG))

# ----------------------------------------------------------------------------
# Include
# ----------------------------------------------------------------------------
include $(PTXDIST_WORKSPACE)/rules/other/definitions.make

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# use CONFIG_CC_STACKPROTECTOR if available. The rest makes no sense for the kernel
KERNEL_WRAPPER_BLACKLIST := \
	TARGET_HARDEN_STACK \
	TARGET_HARDEN_FORTIFY \
	TARGET_HARDEN_RELRO \
	TARGET_HARDEN_BINDNOW \
	TARGET_HARDEN_PIE \
	TARGET_DEBUG \
	TARGET_BUILD_ID

KERNEL_PATH	:= PATH=$(CROSS_PATH)
KERNEL_ENV 	:= \
	KCONFIG_NOTIMESTAMP=1 \
	HOST_EXTRACFLAGS="$(HOST_CPPFLAGS)" \
	pkg_wrapper_blacklist="$(KERNEL_WRAPPER_BLACKLIST)"

KERNEL_MAKEVARS := \
	$(PARALLELMFLAGS) \
	V=$(PTXDIST_VERBOSE) \
	HOSTCC=$(HOSTCC) \
	ARCH=$(GENERIC_KERNEL_ARCH) \
	CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) \
	INSTALL_MOD_PATH=$(KERNEL_PKGDIR) \
	PTX_KERNEL_DIR=$(KERNEL_DIR) \
	$(call remove_quotes,$(PTXCONF_KERNEL_EXTRA_MAKEVARS))

ifdef PTXCONF_KERNEL_MODULES_INSTALL
KERNEL_MAKEVARS += \
	DEPMOD=$(PTXCONF_SYSROOT_HOST)/sbin/depmod
endif

#
# support the different kernel image formats
#
KERNEL_IMAGE := $(call remove_quotes, $(PTXCONF_KERNEL_IMAGE))
KERNEL_IMAGE_SUFFIX := $(call remove_quotes, $(PTXCONF_BOOT_ADD_LINUX_SUFFIX))
KERNEL_BUILD_IMAGE = "uImage"

# these are sane default
KERNEL_IMAGE_PATH_y := $(KERNEL_DIR)/arch/$(GENERIC_KERNEL_ARCH)/boot/$(KERNEL_BUILD_IMAGE)

# vmlinux is special
KERNEL_IMAGE_PATH_$(PTXCONF_KERNEL_IMAGE_VMLINUX) := $(KERNEL_DIR)/vmlinux
# avr32 is also special
KERNEL_IMAGE_PATH_$(PTXCONF_ARCH_AVR32) := $(KERNEL_DIR)/arch/$(GENERIC_KERNEL_ARCH)/boot/images/$(KERNEL_IMAGE)


ifndef PTXCONF_PROJECT_USE_PRODUCTION

ifdef PTXCONF_KERNEL
$(KERNEL_CONFIG):
	@echo
	@echo "*************************************************************************"
	@echo "**** Please generate a kernelconfig with 'ptxdist menuconfig kernel' ****"
	@echo "*************************************************************************"
	@echo
	@echo
	@exit 1
endif


#
# when compiling the rootfs into the kernel, we just include an empty
# file for now. the rootfs isn't build yet.
#
KERNEL_INITRAMFS_SOURCE_$(PTXCONF_IMAGE_KERNEL_INITRAMFS) += $(STATEDIR)/empty.cpio
KERNEL_INITRAMFS_SOURCE_$(PTXCONF_KLIBC) += $(INITRAMFS_CONTROL)

ifneq (,$(DEFCONFIG))
$(STATEDIR)/kernel.prepare:
else
$(STATEDIR)/kernel.prepare: $(KERNEL_CONFIG)
endif
	@$(call targetinfo)

## check if the standard ptx configuration is given
ifneq ("$(wildcard $(KERNEL_CONFIG))","")
	@echo 'config found in $(PTXDIST_PLATFORMCONFIGDIR)'

	@echo "Using kernel config file: $(<)"
	@install -m 644 "$(<)" "$(KERNEL_DIR)/.config"
ifdef PTXCONF_KERNEL_IMAGE_SIMPLE
	cp $(PTXCONF_KERNEL_IMAGE_SIMPLE_DTS) \
		$(KERNEL_DIR)/arch/$(GENERIC_KERNEL_ARCH)/boot/dts/$(PTXCONF_KERNEL_IMAGE_SIMPLE_TARGET).dts
endif

ifdef KERNEL_INITRAMFS_SOURCE_y
	@touch "$(KERNEL_INITRAMFS_SOURCE_y)"
	@sed -i -e 's,^CONFIG_INITRAMFS_SOURCE.*$$,CONFIG_INITRAMFS_SOURCE=\"$(KERNEL_INITRAMFS_SOURCE_y)\",g' \
		"$(KERNEL_DIR)/.config"
endif

	@$(call ptx/oldconfig, KERNEL)
	@diff -q -I "# [^C]" "$(KERNEL_DIR)/.config" "$(<)" > /dev/null || cp "$(KERNEL_DIR)/.config" "$(<)"

#
# Don't keep the expanded path to INITRAMS_SOURCE in $(KERNEL_CONFIG),
# because it contains local workdir path which is not relevant to
# other developers.
#
ifdef KERNEL_INITRAMFS_SOURCE_y
	@sed -i -e 's,^CONFIG_INITRAMFS_SOURCE.*$$,CONFIG_INITRAMFS_SOURCE=\"# Automatically set by PTXDist\",g' \
		"$(<)"
endif

## no ptx config is given, so check if we have an defconfig
else ifneq (,$(DEFCONFIG))
	@echo '### defconfig is given, call "make $(PTXCONF_KERNEL_CONFIG)"'
	@$(call defconfig, KERNEL, $(call remove_quotes, $(PTXCONF_KERNEL_CONFIG)))

ifdef KERNEL_INITRAMFS_SOURCE_y
	@touch "$(KERNEL_INITRAMFS_SOURCE_y)"
	@sed -i -e 's,^CONFIG_INITRAMFS_SOURCE.*$$,CONFIG_INITRAMFS_SOURCE=\"$(KERNEL_INITRAMFS_SOURCE_y)\",g' \
		"$(KERNEL_DIR)/.config"
endif
#	@$(call ptx/oldconfig, KERNEL)
	@$(call ptx/silentoldconfig, KERNEL)

else
	@echo '### wrong kernel configuration file given'
	@exit 1
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# tags
# ----------------------------------------------------------------------------

$(STATEDIR)/kernel.tags:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) \
		$(KERNEL_MAKEVARS) tags TAGS cscope

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

KERNEL_TOOL_PERF_OPTS := \
	WERROR=0 \
	NO_LIBPERL=1 \
	NO_LIBPYTHON=1 \
	NO_DWARF= \
	NO_SLANG= \
	NO_GTK2=1 \
	NO_DEMANGLE= \
	NO_LIBELF= \
	NO_LIBUNWIND=1 \
	NO_BACKTRACE= \
	NO_LIBNUMA=1 \
	NO_LIBAUDIT=1 \
	NO_LIBBIONIC=1 \
	NO_LIBCRYPTO=1 \
	NO_LIBDW_DWARF_UNWIND= \
	NO_PERF_READ_VDSO32=1 \
	NO_PERF_READ_VDSOX32=1 \
	NO_ZLIB= \
	NO_LIBBABELTRACE=1 \
	NO_LZMA=1 \
	NO_AUXTRACE= \
	NO_LIBBPF=1 \
	NO_SDT=1

$(STATEDIR)/kernel.compile:
	@$(call targetinfo)
	@rm -f \
		$(KERNEL_DIR)/usr/initramfs_data.cpio.* \
		$(KERNEL_DIR)/usr/.initramfs_data.cpio.*
	@cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) \
		$(KERNEL_MAKEVARS) $(KERNEL_BUILD_IMAGE) $(call ptx/ifdef, PTXCONF_KERNEL_MODULES,modules)
ifdef PTXCONF_KERNEL_TOOL_PERF
	@+cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) \
		$(KERNEL_MAKEVARS) $(KERNEL_TOOL_PERF_OPTS) -C tools/perf
endif
ifdef PTXCONF_KERNEL_TOOL_IIO
	@+cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) \
		CPPFLAGS="-D__EXPORTED_HEADERS__ -I$(KERNEL_DIR)/include/uapi -I$(KERNEL_DIR)/include" \
		$(KERNEL_MAKEVARS) -C tools/iio
endif
	@$(call touch)

endif # !PTXCONF_PROJECT_USE_PRODUCTION

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/kernel.install:
	@$(call targetinfo)
ifdef PTXCONF_KERNEL_MODULES_INSTALL
	@rm -rf $(KERNEL_PKGDIR)
	@cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) \
		$(KERNEL_MAKEVARS) modules_install
endif
ifdef PTXCONF_KERNEL_DTC
	@install -m 755 "$(KERNEL_DIR)/scripts/dtc/dtc" "$(PTXCONF_SYSROOT_HOST)/bin/dtc"
endif
	# Install platform-specific headers

  # pac200 (header is part of pac200-specific patchset and exists only if we
	# build for pac200)
  # TODO: when the barebox is integrated into ptxdist, this header shall be placed there and
  # be referenced by both the kernel and pac-mdio via sysroot
ifeq ($(KERNEL_VERSION), "3.6.11")
	[[ -e ${KERNEL_DIR}/arch/arm/mach-omap2/pac_boot_table.h ]] && \
	install $(KERNEL_DIR)/arch/arm/mach-omap2/pac_boot_table.h $(PTXDIST_SYSROOT_TARGET)/include/pac_boot_table.h || true
else
	[[ -e ${KERNEL_DIR}/include/linux/wsysinit-boot-table.h ]] && \
	install $(KERNEL_DIR)/include/linux/wsysinit-boot-table.h $(PTXDIST_SYSROOT_TARGET)/include/wsysinit-boot-table.h || true
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/kernel.targetinstall:
	@$(call targetinfo)

# delete the kernel image, it might be out-of-date
##	@rm -f $(IMAGEDIR)/linuximage

ifneq ($(PTXCONF_KERNEL_INSTALL)$(PTXCONF_KERNEL_VMLINUX),)
	@$(call install_init,  kernel)
	@$(call install_fixup, kernel, PRIORITY,optional)
	@$(call install_fixup, kernel, SECTION,base)
	@$(call install_fixup, kernel, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, kernel, DESCRIPTION,missing)

	@$(call install_copy, kernel, 0, 0, 0755, /boot);
ifdef PTXCONF_KERNEL_INSTALL
	@$(call install_copy, kernel, 0, 0, 0644, $(KERNEL_IMAGE_PATH_y), /boot/$(KERNEL_IMAGE)$(KERNEL_IMAGE_SUFFIX), n)
endif

# install the ELF kernel image for debugging purpose
ifdef PTXCONF_KERNEL_VMLINUX
	@$(call install_copy, kernel, 0, 0, 0644, $(KERNEL_DIR)/vmlinux, /boot/vmlinux, n)
endif

ifdef PTXCONF_KERNEL_TOOL_PERF
	@$(call install_copy, kernel, 0, 0, 0755, $(KERNEL_DIR)/tools/perf/perf, \
		/usr/bin/perf)
endif

ifdef PTXCONF_KERNEL_TOOL_IIO
	@$(call install_copy, kernel, 0, 0, 0755, $(KERNEL_DIR)/tools/iio/iio_generic_buffer, \
		/usr/bin/iio_generic_buffer)
	@$(call install_copy, kernel, 0, 0, 0755, $(KERNEL_DIR)/tools/iio/lsiio, \
		/usr/bin/lsiio)
	@$(call install_copy, kernel, 0, 0, 0755, $(KERNEL_DIR)/tools/iio/iio_event_monitor, \
		/usr/bin/iio_event_monitor)
endif

	@cp $(KERNEL_DIR)/LICENSES/exceptions/Linux-syscall-note $(KERNEL_DIR)/license
	@echo -e "----------------------------------------\n" >> $(KERNEL_DIR)/license
	@cat $(KERNEL_DIR)/LICENSES/preferred/GPL-2.0 >> $(KERNEL_DIR)/license

	@$(call install_copy, kernel, 0, 0, 0644, $(KERNEL_DIR)/license, /usr/share/licenses/oss/license.kernel_$(KERNEL_VERSION).txt)

	@$(call install_finish, kernel)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install-post
# ----------------------------------------------------------------------------

ifdef PTXCONF_IMAGE_KERNEL_INSTALL_EARLY
$(STATEDIR)/kernel.targetinstall.post: $(IMAGEDIR)/linuximage
ifdef PTXCONF_IMAGE_KERNEL_LZOP
$(STATEDIR)/kernel.targetinstall.post: $(IMAGEDIR)/linuximage.lzo
endif
endif

$(STATEDIR)/kernel.targetinstall.post:
	@$(call targetinfo)

ifdef PTXCONF_KERNEL_MODULES_INSTALL
	@$(call install_init,  kernel-modules)
	@$(call install_fixup, kernel-modules, PRIORITY,optional)
	@$(call install_fixup, kernel-modules, SECTION,base)
	@$(call install_fixup, kernel-modules, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, kernel-modules, DESCRIPTION,missing)

	@$(call install_glob, kernel-modules, 0, 0, -, /lib/modules, *.ko,, k)
	@$(call install_glob, kernel-modules, 0, 0, -, /lib/modules,, *.ko */build */source, n)	

	@$(call install_finish, kernel-modules)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

ifndef PTXCONF_PROJECT_USE_PRODUCTION

$(STATEDIR)/kernel.clean:
	@$(call targetinfo)
	@$(call clean_pkg, KERNEL)
	@if [ -L $(KERNEL_DIR) ]; then \
		pushd $(KERNEL_DIR); \
		quilt pop -af; \
		rm -rf series patches .pc; \
		$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) distclean; \
		popd; \
	fi

# ----------------------------------------------------------------------------
# oldconfig / menuconfig
# ----------------------------------------------------------------------------

COMPARE_CONFIG	:= $(PTXDIST_TEMPDIR)/tmpconfig

kernel_oldconfig kernel_menuconfig kernel_nconfig: $(STATEDIR)/kernel.extract

	@if [ -e $(KERNEL_CONFIG) -a $(PTXCONF_KERNEL_CONFIG) != "kernelconfig-git" ]; then \
		cp $(KERNEL_CONFIG) $(KERNEL_DIR)/.config; \
	elif [ "$(DEFCONFIG)" = "_defconfig" ]; then \
		echo "## menuconfig for _defconfig"; \
		if [ -e $(PTXDIST_PLATFORMCONFIGDIR)/kernelconfig-$(KERNEL_VERSION) ]; then \
			echo "### "; \
			echo "### WARNING: You are using a DEFCONFIG Kernel configuration in you PLATFORMCONFIG"; \
			echo "### although a local Kernel config is present: kernelconfig-$(KERNEL_VERSION)"; \
			echo "### Please remove your local configuration or adjust PTXCONF_KERNEL_CONFIG properly!"; \
			echo "### "; \
		fi; \
		$(call defconfig, KERNEL, $(call remove_quotes, $(PTXCONF_KERNEL_CONFIG))); \
	else \
		echo "## could not get a valid kernelconfig ##"; \
		exit 1; \
	fi

#	save unchanged config for later to check if any changes were made
	@cp $(KERNEL_DIR)/.config $(COMPARE_CONFIG); \

	@cd $(KERNEL_DIR) && \
		$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) $(subst kernel_,,$@)

	@if cmp -s $(KERNEL_DIR)/.config $(COMPARE_CONFIG); then \
		echo "kernel configuration unchanged"; \
	elif [ "$(DEFCONFIG)" = "_defconfig" ]; then \
		if [ ! -e $(PTXDIST_PLATFORMCONFIGDIR)/kernelconfig-$(KERNEL_VERSION) ]; then \
			cp $(KERNEL_DIR)/.config $(PTXDIST_PLATFORMCONFIGDIR)/kernelconfig-$(KERNEL_VERSION); \
			echo "### !!! new kernel configuration saved under $(PTXDIST_PLATFORMCONFIGDIR)/kernelconfig-$(KERNEL_VERSION). Please change your platformconfig to use the new one !!! ###"; \
		else \
			echo "### "; \
			echo "### ABORT: You are using a DEFCONFIG Kernel configuration in you PLATFORMCONFIG"; \
			echo "### although a local Kernel config is present: kernelconfig-$(KERNEL_VERSION)"; \
			echo "### Please remove your local configuration or adjust PTXCONF_KERNEL_CONFIG properly!"; \
			echo "### "; \
			exit 1;\
		fi; \
	else \
		cp $(KERNEL_DIR)/.config $(KERNEL_CONFIG); \
		echo "new kernel configuration saved $(KERNEL_CONFIG)"; \
	fi

endif

# vim: syntax=make
