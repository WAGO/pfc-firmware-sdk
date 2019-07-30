# -*-makefile-*-
#
# Copyright (C) 2019 by WAGO Kontakttechnik GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBRLB) += librlb

#
# Paths and names
#
LIBRLB_VERSION	:= 0.0.9454-9599_8
LIBRLB		:= librlb-$(LIBRLB_VERSION)
LIBRLB_URL			:= $(call jfrog_template_to_url, LIBRLB)
LIBRLB_SUFFIX		:= $(suffix $(LIBRLB_URL))
LIBRLB_SOURCE		:= $(SRCDIR)/$(LIBRLB)$(LIBRLB_SUFFIX)
LIBRLB_DIR			:= $(BUILDDIR)/$(LIBRLB)
LIBRLB_MD5			= $(shell [ -f $(LIBRLB_MD5_FILE) ] && cat $(LIBRLB_MD5_FILE))
LIBRLB_MD5_FILE    := $(LIBRLB_SOURCE).md5
LIBRLB_ARTIFACT	= $(call jfrog_get_filename,$(LIBRLB_URL))
LIBRLB_LICENSE		:= proprietary



LIBRLB_CMAKE_OPTS = 
ifdef PTXCONF_LIBRLB_BUILD_TESTING
	LIBRLB_CMAKE_OPTS +=i -DBUILD_TESTING=ON
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(LIBRLB_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(LIBRLB_URL)' '$@' '$(LIBRLB_MD5_FILE)'

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBRLB_CONF_ENV	:= $(CROSS_ENV)

#
# cmake
#
LIBRLB_CONF_TOOL	:= cmake
LIBRLB_CONF_OPT	:= $(CROSS_CMAKE_USR) $(LIBRLB_CMAKE_OPTS)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/librlb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, librlb)
	@$(call install_fixup, librlb, PRIORITY, optional)
	@$(call install_fixup, librlb, SECTION, base)
	@$(call install_fixup, librlb, AUTHOR, "WAGO Kontakttechnik GmbH")
	@$(call install_fixup, librlb, DESCRIPTION, missing)

#	#
#	# example code:; copy all binaries
#	#

	@for i in $(shell cd $(LIBRLB_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, librlb, 0, 0, 0755, -, /$$i); \
	done

#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, librlb)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/librlb.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBRLB)

# vim: syntax=make
