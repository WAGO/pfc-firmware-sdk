# -*-makefile-*-
#
# Copyright (C) 2006, 2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2016 by Uwe Kleine-Koenig <u.kleine-koenig@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBELF) += libelf

#
# Paths and names
#
LIBELF_VERSION	:= 0.176
LIBELF_MD5	:= 077e4f49320cad82bf17a997068b1db9
LIBELF		:= elfutils-$(LIBELF_VERSION)
LIBELF_SUFFIX	:= tar.bz2
LIBELF_URL	:= https://sourceware.org/elfutils/ftp/$(LIBELF_VERSION)/$(LIBELF).$(LIBELF_SUFFIX)
LIBELF_SOURCE	:= $(SRCDIR)/$(LIBELF).$(LIBELF_SUFFIX)
LIBELF_DIR	:= $(BUILDDIR)/$(LIBELF)
LIBELF_LICENSE	:= (LGPL-3.0-or-later OR GPL-2.0-or-later) AND GPL-3.0-or-later

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBELF_CONF_TOOL := autoconf
LIBELF_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-deterministic-archives \
	--disable-thread-safety \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-debugpred \
	--disable-gprof \
	--disable-gcov \
	--disable-sanitize-undefined \
	--disable-valgrind \
	--disable-tests-rpath \
	--enable-libebl-subdir=elfutils \
	--enable-textrelcheck \
	--enable-symbol-versioning \
	--disable-nls \
	--with-zlib \
	--without-bzlib \
	--without-lzma \
	--without-biarch

LIBELF_ARCH := $(call remove_quotes,$(PTXCONF_ARCH_STRING))
ifdef PTXCONF_ARCH_ARM64
LIBELF_ARCH := aarch64
endif

ifdef PTXDIST_ICECC
LIBELF_CFLAGS := -C
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libelf.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libelf)
	@$(call install_fixup, libelf,PRIORITY,optional)
	@$(call install_fixup, libelf,SECTION,base)
	@$(call install_fixup, libelf,AUTHOR,"Uwe Kleine-Koenig <u.kleine-koenig@pengutronix.de>")
	@$(call install_fixup, libelf,DESCRIPTION,missing)

	@$(call install_lib, libelf, 0, 0, 0644, libelf-$(LIBELF_VERSION))

ifdef PTXCONF_LIBELF_LIBDW
	@$(call install_lib, libelf, 0, 0, 0644, libdw-$(LIBELF_VERSION))
	@$(call install_lib, libelf, 0, 0, 0644, \
		elfutils/libebl_$(LIBELF_ARCH)-$(LIBELF_VERSION))
endif

ifdef PTXCONF_LIBELF_LIBASM
	@$(call install_lib, libelf, 0, 0, 0644, libasm-$(LIBELF_VERSION))
endif

ifdef PTXCONF_LIBELF_ELFSUTILS
	@$(foreach bin, addr2line ar elfcmp elfcompress elflint findtextrel \
		make-debug-archive nm objdump ranlib readelf size stack strings \
		strip unstrip, \
		$(call install_copy, libelf, 0, 0, 0755, -, \
		/usr/bin/eu-$(bin));)
endif

	@$(call install_finish, libelf)

	@$(call touch)

# vim: syntax=make
