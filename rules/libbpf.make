# -*-makefile-*-
#
# Copyright (C) 2020 by <WAGO>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBBPF) += libbpf

#
# Paths and names
#
LIBBPF_VERSION	:= 0.2.0
LIBBPF_MD5		:= 6da24bc48685ad9db0dec6ed9dc15f1c
LIBBPF		:= libbpf-$(LIBBPF_VERSION)
LIBBPF_SUFFIX	:= tar.xz
LIBBPF_URL		:= https://github.com/libbpf/libbpf/$(LIBBPF).$(LIBBPF_SUFFIX)
LIBBPF_SOURCE	:= $(SRCDIR)/$(LIBBPF).$(LIBBPF_SUFFIX)
LIBBPF_DIR		:= $(BUILDDIR)/$(LIBBPF)
LIBBPF_LICENSE	:= BSD-2-Clause OR LGPL-2.1

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(LIBBPF_SOURCE):
#	@$(call targetinfo)
#	@$(call get, LIBBPF)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


#$(STATEDIR)/libbpf.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(LIBBPF_DIR)/config.cache)
#	cd $(LIBBPF_DIR) && \
#		$(LIBBPF_PATH) $(LIBBPF_ENV) \
#		./configure $(LIBBPF_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libbpf.compile:
	@$(call targetinfo)
	cd $(LIBBPF_DIR)/src &&  \
		$(CROSS_ENV) $(MAKE) $(PARALLELMFLAGS) all
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

INSTALL_ENV:= LIBDIR=$(PTXCONF_SYSROOT_TARGET)/usr/lib/                      \
	INCLUDEDIR=$(PTXCONF_SYSROOT_TARGET)/usr/include/

PACKAGE_ENV:= LIBDIR=$(LIBBPF_PKGDIR)/usr/lib/                               \
	INCLUDEDIR=$(LIBBPF_PKGDIR)/usr/include

$(STATEDIR)/libbpf.install:
	@$(call targetinfo)
	@cd $(LIBBPF_DIR)/src &&                                                 \
		$(INSTALL_ENV)                                                       \
		$(CROSS_ENV) $(MAKE) install

	@mkdir -p $(LIBBPF_PKGDIR)/usr/lib
	@mkdir -p $(LIBBPF_PKGDIR)/usr/include

	@cd $(LIBBPF_DIR)/src &&                                                 \
		$(PACKAGE_ENV)                                                       \
		$(CROSS_ENV) $(MAKE) install

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbpf.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libbpf)
	@$(call install_fixup, libbpf,PRIORITY,optional)
	@$(call install_fixup, libbpf,SECTION,base)
	@$(call install_fixup, libbpf,AUTHOR,"<JSo>")
	@$(call install_fixup, libbpf,DESCRIPTION,missing)

	@$(call install_lib, libbpf, 0, 0, 0644, libbpf)
	@$(call install_copy, libbpf, 0, 0, 0644, $(LIBBPF_DIR)/LICENSE.LPGL-2.1, /usr/share/licenses/oss/license.libbpf_$(LIBBPF_VERSION).txt)

	@$(call install_finish, libbpf)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/libbpf.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBBPF)

# vim: syntax=make
