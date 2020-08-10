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
PACKAGES-$(PTXCONF_WRETAIN) += wretain

#
# Paths and names
#
WRETAIN_MAJOR_VERSION := 1
WRETAIN_MINOR_VERSION := 0
WRETAIN_PATCH_VERSION := 3

ifdef PTXCONF_WRETAIN_DEV
WRETAIN_BUILD_ID      := -$(call remove_quotes,$(PTXCONF_WRETAIN_DEV_BRANCH))
else
WRETAIN_BUILD_ID      := -159229425162
endif

WRETAIN		          := wretain
WRETAIN_SO_VERSION	  := $(WRETAIN_MAJOR_VERSION).$(WRETAIN_MINOR_VERSION).$(WRETAIN_PATCH_VERSION)
WRETAIN_VERSION	      := $(WRETAIN_SO_VERSION)$(WRETAIN_BUILD_ID)

ifndef PTXCONF_WRETAIN_DEV
WRETAIN_URL           := $(call jfrog_template_to_url, WRETAIN)
else
WRETAIN_URL		      := file://wago_intern/wretain
endif
WRETAIN_SUFFIX        := $(suffix $(WRETAIN_URL))
WRETAIN_MD5            = $(shell [ -f $(WRETAIN_MD5_FILE) ] && cat $(WRETAIN_MD5_FILE))
WRETAIN_MD5_FILE      := wago_intern/artifactory_sources/$(WRETAIN)$(WRETAIN_SUFFIX).md5
WRETAIN_ARTIFACT       = $(call jfrog_get_filename,$(WRETAIN_URL))
WRETAIN_ARCHIVE       := $(WRETAIN)-$(WRETAIN_VERSION)$(WRETAIN_SUFFIX)

WRETAIN_GIT_URL	      := ssh://svtfs01007:22/tfs/ProductDevelopment/Linux-BSP/_git/wretain

WRETAIN_DIR		      := $(BUILDDIR)/$(WRETAIN)
WRETAIN_LICENSE	      := unknown


WRETAIN_CMAKE_OPTS =
ifdef PTXCONF_WRETAIN_BUILD_TESTING
	WRETAIN_CMAKE_OPTS +=-DBUILD_TESTING=ON
endif


WRETAIN_PACKAGE_NAME := wretain_$(WRETAIN_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WRETAIN_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_WRETAIN_DEV

$(PTXDIST_WORKSPACE)/wago_intern/wretain:
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(WRETAIN_GIT_URL); } \
    || rm -fr $(PTXDIST_WORKSPACE)/wago_intern/wretain
ifdef PTXCONF_WRETAIN_DEV_BRANCH
	{ cd $(PTXDIST_WORKSPACE)/wago_intern/wretain && git checkout $(PTXCONF_WRETAIN_DEV_BRANCH); } \
    || rm -fr $(PTXDIST_WORKSPACE)/wago_intern/wretain
endif

$(STATEDIR)/wretain.get: | $(PTXDIST_WORKSPACE)/wago_intern/wretain

else
$(STATEDIR)/wretain.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
    '$(WRETAIN_URL)' wago_intern/artifactory_sources/$(WRETAIN_ARCHIVE) '$(WRETAIN_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_WRETAIN_DEV

$(STATEDIR)/wretain.extract:
	@$(call targetinfo)

	@mkdir -p $(WRETAIN_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@tar xvzf wago_intern/artifactory_sources/$(WRETAIN_ARCHIVE) -C $(WRETAIN_DIR) --strip-components=1
	@$(call patchin, WRETAIN)
endif
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#WRETAIN_PATH	:= PATH=$(CROSS_PATH)
WRETAIN_CONF_TOOL	:= cmake
WRETAIN_MAKE_ENV	:= $(CROSS_ENV)
WRETAIN_CONF_OPT	:= $(CROSS_CMAKE_USR) $(WRETAIN_CMAKE_OPTS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wretain.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wretain.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wretain.install:
	@$(call targetinfo)
 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   # BSP mode: install ipk contents to packages/wretain
	@mkdir -p $(WRETAIN_PKGDIR) && \
    tar xvzf $(WRETAIN_PLATFORMCONFIGPACKAGEDIR)/$(WRETAIN_PACKAGE_NAME).tgz -C $(WRETAIN_PKGDIR)
else
 
#   # "normal" mode: call "make install"
	@$(call world/install, WRETAIN)
 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory contents for BSP
	@cd $(WRETAIN_PKGDIR) && tar cvzf $(WRETAIN_PLATFORMCONFIGPACKAGEDIR)/$(WRETAIN_PACKAGE_NAME).tgz *
endif
 
 
endif
 
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wretain.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wretain)
	@$(call install_fixup, wretain,PRIORITY,optional)
	@$(call install_fixup, wretain,SECTION,base)
	@$(call install_fixup, wretain,AUTHOR,"WAGO Kontakttechnik GmbH")
	@$(call install_fixup, wretain,DESCRIPTION,missing)

ifdef PTXCONF_WRETAIN_LINK_TYPE_DIRECT
	@$(call install_lib, wretain, 0, 0, 0644, libwretain_direct)
	@$(call install_alternative, wretain, 0, 0, 0644, /etc/wretain_direct.conf)
	@$(call install_link, wretain, libwretain_direct.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so)
	@$(call install_link, wretain, libwretain_direct.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so.$(WRETAIN_MAJOR_VERSION))
	@$(call install_link, wretain, libwretain_direct.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so.$(WRETAIN_SO_VERSION))
else
	@$(call install_lib, wretain, 0, 0, 0644, libwretain_serial)
	@$(call install_alternative, wretain, 0, 0, 0644, /etc/wretain_serial.conf)
	@$(call install_link, wretain, libwretain_serial.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so)
	@$(call install_link, wretain, libwretain_serial.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so.$(WRETAIN_MAJOR_VERSION))
	@$(call install_link, wretain, libwretain_serial.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so.$(WRETAIN_SO_VERSION))
endif

	@$(call install_copy, wretain, 0, 0, 0755, -, /usr/sbin/retain_data)

	@$(call install_finish, wretain)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wretain.clean:
	@$(call targetinfo)
	@-cd $(WRETAIN_DIR)-build && \
		$(WRETAIN_ENV) $(WRETAIN_PATH) $(MAKE) clean
	@$(call clean_pkg, WRETAIN)

# vim: syntax=make
