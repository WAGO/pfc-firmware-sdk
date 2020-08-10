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
PACKAGES-$(PTXCONF_WSTD) += wstd

#
# Paths and names
#
ifdef PTXCONF_WSTD_DEV
WSTD_BUILD_ID      := -$(call remove_quotes,$(PTXCONF_WSTD_DEV_BRANCH))
else
WSTD_BUILD_ID      := -159169057342
endif

WSTD_VERSION	:= 1.0.2$(WSTD_BUILD_ID)
WSTD		    := wstd
ifndef PTXCONF_WSTD_DEV
WSTD_URL        := $(call jfrog_template_to_url, WSTD)
else
WSTD_URL		:= file://wago_intern/wstd
endif
WSTD_SUFFIX     := $(suffix $(WSTD_URL))
WSTD_ARCHIVE    := $(WSTD)-$(WSTD_VERSION)$(WSTD_SUFFIX)
WSTD_MD5         = $(shell [ -f $(WSTD_MD5_FILE) ] && cat $(WSTD_MD5_FILE))
WSTD_MD5_FILE   := wago_intern/artifactory_sources/$(WSTD_ARCHIVE).md5
WSTD_ARTIFACT    = $(call jfrog_get_filename,$(WSTD_URL))

WSTD_GIT_URL	:= ssh://svtfs01007:22/tfs/ProductDevelopment/Linux-BSP/_git/wstd

WSTD_DIR		:= $(BUILDDIR)/$(WSTD)
WSTD_LICENSE	:= unknown
WSTD_LICENSE_FILE	:= LICENSE

# WSTD_URL		:= ssh://git@svli01001.wago.local:1022/wago-intern/wstd.git;branch=master
# WSTD_URL		:= ssh://git@svli01001.wago.local:1022/wago-intern/wstd.git;tag=v$(WSTD_VERSION)


WSTD_CMAKE_OPTS =
ifdef PTXCONF_WSTD_BUILD_TESTING
	WSTD_CMAKE_OPTS +=-DBUILD_TESTING=ON
endif

WSTD_PACKAGE_NAME := wstd_$(WSTD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WSTD_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_WSTD_DEV

$(PTXDIST_WORKSPACE)/wago_intern/wstd:
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(WSTD_GIT_URL); } \
    || rm -fr $(PTXDIST_WORKSPACE)/wago_intern/wstd
ifdef PTXCONF_WSTD_DEV_BRANCH
	{ cd $(PTXDIST_WORKSPACE)/wago_intern/wstd && git checkout $(PTXCONF_WSTD_DEV_BRANCH); } \
    || rm -fr $(PTXDIST_WORKSPACE)/wago_intern/wstd
endif

$(STATEDIR)/wstd.get: | $(PTXDIST_WORKSPACE)/wago_intern/wstd

else

$(STATEDIR)/wstd.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
    '$(WSTD_URL)' wago_intern/artifactory_sources/$(WSTD_ARCHIVE) '$(WSTD_MD5_FILE)'
endif

	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_WSTD_DEV

$(STATEDIR)/wstd.extract:
	@$(call targetinfo)
 
	@mkdir -p $(WSTD_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@tar xvzf wago_intern/artifactory_sources/$(WSTD_ARCHIVE) -C $(WSTD_DIR) --strip-components=1
	@$(call patchin, WSTD)
endif
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#WSTD_PATH	:= PATH=$(CROSS_PATH)
WSTD_CONF_TOOL	:= cmake
WSTD_MAKE_ENV	:= $(CROSS_ENV)
WSTD_CONF_OPT	:= $(CROSS_CMAKE_USR) $(WSTD_CMAKE_OPTS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wstd.prepare:
	@$(call targetinfo)
	@$(call touch)
endif


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wstd.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wstd.install:
	@$(call targetinfo)
 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   # BSP mode: install ipk contents to packages/wstd
	@mkdir -p $(WSTD_PKGDIR) && \
    tar xvzf $(WSTD_PLATFORMCONFIGPACKAGEDIR)/$(WSTD_PACKAGE_NAME).tgz -C $(WSTD_PKGDIR)
else
 
#   # "normal" mode: call "make install"
	@$(call world/install, WSTD)
 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory contents for BSP
	@mkdir -p $(WSTD_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WSTD_PKGDIR) && tar cvzf $(WSTD_PLATFORMCONFIGPACKAGEDIR)/$(WSTD_PACKAGE_NAME).tgz *
endif
 
 
endif
 
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wstd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wstd)
	@$(call install_fixup, wstd,PRIORITY,optional)
	@$(call install_fixup, wstd,SECTION,base)
	@$(call install_fixup, wstd,AUTHOR,"WAGO Kontakttechnik GmbH")
	@$(call install_fixup, wstd,DESCRIPTION,missing)

	@$(call install_lib, wstd, 0, 0, 0755, libwstd)

	@$(call install_copy, wstd, 0, 0, 0644, $(WSTD_PKGDIR)/usr/share/licenses/oss/$(WSTD_LICENSE_FILE), /usr/share/licenses/oss/license.wstd_$(WSTD_VERSION).txt)

	@$(call install_finish, wstd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wstd.clean:
	@$(call targetinfo)
	@-cd $(WSTD_DIR)-build && \
		$(WSTD_ENV) $(WSTD_PATH) $(MAKE) clean
	@$(call clean_pkg, WSTD)

# vim: syntax=make
