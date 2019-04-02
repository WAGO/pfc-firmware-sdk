# -*-makefile-*-
#
# Copyright (C) 2005 by Shahar Livne <shahar@livnex.com>
#               2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ARCH_X86)-$(PTXCONF_VALGRIND) += valgrind
PACKAGES-$(PTXCONF_ARCH_PPC)-$(PTXCONF_VALGRIND) += valgrind
PACKAGES-$(PTXCONF_ARCH_ARM)-$(PTXCONF_VALGRIND) += valgrind

#
# Paths and names
#
VALGRIND_VERSION	:= 3.13.0
VALGRIND_MD5		:= 817dd08f1e8a66336b9ff206400a5369
VALGRIND			:= valgrind-$(VALGRIND_VERSION)
VALGRIND_SUFFIX		:= tar.bz2
VALGRIND_URL		:= http://valgrind.org/downloads/$(VALGRIND).$(VALGRIND_SUFFIX)
VALGRIND_SOURCE		:= $(SRCDIR)/$(VALGRIND).$(VALGRIND_SUFFIX)
VALGRIND_DIR		:= $(BUILDDIR)/$(VALGRIND)
VALGRIND_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef KERNEL_VERSION
VALGRIND_KERNEL_VERSION := $(KERNEL_VERSION)
endif

ifdef KERNEL_HEADER_VERSION
VALGRIND_KERNEL_VERSION := $(KERNEL_HEADER_VERSION)
endif

VALGRIND_PATH	:= PATH=$(CROSS_PATH)
VALGRIND_ENV	:= \
	$(CROSS_ENV) \
	valgrind_cv_sys_kernel_version=$(VALGRIND_KERNEL_VERSION)

#
# autoconf
#
VALGRIND_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--enable-tls

$(STATEDIR)/valgrind.extract.post: $(STATEDIR)/autogen-tools

$(STATEDIR)/valgrind.prepare:
	@$(call targetinfo)
	@$(call clean, $(VALGRIND_DIR)/config.cache)
	cd $(VALGRIND_DIR) && \
		sh autogen.sh &&\
		sed -ir "s/armv7/arm/g" configure && \
		$(VALGRIND_PATH) $(VALGRIND_ENV) \
		./configure $(VALGRIND_AUTOCONF)
	@$(call touch)
# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/valgrind.targetinstall:
	@$(call targetinfo)
ifndef PTXCONF_VALGRIND_SKIP_TARGETINSTALL

	@$(call install_init, valgrind)
	@$(call install_fixup, valgrind,PRIORITY,optional)
	@$(call install_fixup, valgrind,SECTION,base)
	@$(call install_fixup, valgrind,AUTHOR,"Shahar Livne <shahar@livnex.com>")
	@$(call install_fixup, valgrind,DESCRIPTION,missing)

	@cd $(VALGRIND_PKGDIR) && \
		find usr/bin/ -type f -a -name "*" | while read file; do \
		$(call install_copy, valgrind, 0, 0, 0755, -, /$$file, n) \
	done

	@cd $(VALGRIND_PKGDIR) && \
		find usr/lib/valgrind -name "*.supp" -o -name "*.so" | while read file; do \
		$(call install_copy, valgrind, 0, 0, 0644, -, /$$file, n) \
	done

	@cd $(VALGRIND_PKGDIR) && \
		find usr/lib/valgrind -type f \
			\! -wholename "*.a" \! -wholename "*.supp" \! -wholename "*.so" | while read file; do \
		$(call install_copy, valgrind, 0, 0, 0755, -, /$$file) \
	done

	@$(call install_finish, valgrind)

endif
	@$(call touch)

# vim: syntax=make
