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
LIBRLB_VERSION  := 2.0.0-1562755167177
LIBRLB_BOARD_ID := BOARD_WAGO_PAC100_0768_3301_P0002027
LIBRLB          := librlb-$(LIBRLB_VERSION)
LIBRLB_URL      := $(call jfrog_template_to_url, LIBRLB)
LIBRLB_SUFFIX   := $(suffix $(LIBRLB_URL))

# When LIBRLB_SOURCE is defined, ptxdist erroneously overwrites
# the get rule, leading to a build error.
# See ptxdist-XXX/rules/post/export_src.make for further details
#LIBRLB_SOURCE		:= wago_intern/$(LIBRLB)$(LIBRLB_SUFFIX)

LIBRLB_DIR		:= $(BUILDDIR)/$(LIBRLB)
LIBRLB_MD5		= $(shell [ -f $(LIBRLB_MD5_FILE) ] && cat $(LIBRLB_MD5_FILE))
LIBRLB_SOURCE_BASEDIR := wago_intern/artifactory_sources
LIBRLB_MD5_FILE 	:= ${LIBRLB_SOURCE_BASEDIR}/$(LIBRLB)$(LIBRLB_SUFFIX).md5
LIBRLB_ARTIFACT		= $(call jfrog_get_filename,$(LIBRLB_URL))
LIBRLB_LICENSE		:= proprietary
LIBRLB_FPGA_IMAGE	:= $(call remove_quotes,$(PTXCONF_LIBRLB_FPGA_IMAGE))

LIBRLB_CMAKE_OPTS =
ifdef PTXCONF_LIBRLB_BUILD_TESTING
	LIBRLB_CMAKE_OPTS +=i -DBUILD_TESTING=ON
endif

LIBRLB_PACKAGE_NAME := librlb_$(LIBRLB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBRLB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/librlb.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES	
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
	'$(LIBRLB_URL)' ${LIBRLB_SOURCE_BASEDIR}/$(LIBRLB)$(LIBRLB_SUFFIX) '$(LIBRLB_MD5_FILE)'
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/librlb.extract:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES	
	mkdir -p $(LIBRLB_DIR) && \
    tar xvzf ${LIBRLB_SOURCE_BASEDIR}/$(LIBRLB)$(LIBRLB_SUFFIX) -C $(LIBRLB_DIR) --strip-components=1
	@$(call patchin, LIBRLB)
endif
	@$(call touch)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/librlb.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBRLB_CONF_ENV	:= $(CROSS_ENV)

#
# cmake
#
LIBRLB_CONF_TOOL:= cmake
LIBRLB_CONF_OPT	:= -DCMAKE_CXX_FLAGS:="-D$(LIBRLB_BOARD_ID)" \
                   $(CROSS_CMAKE_USR) $(LIBRLB_CMAKE_OPTS) -DFPGA_IMAGE_NAME:STRING="$(LIBRLB_FPGA_IMAGE)"

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

$(STATEDIR)/librlb.prepare:
	@$(call targetinfo)
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/librlb.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/librlb.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   # BSP mode: install ipk contents to packages/librlb
	@mkdir -p $(LIBRLB_PKGDIR) && \
    tar xvzf $(LIBRLB_PLATFORMCONFIGPACKAGEDIR)/$(LIBRLB_PACKAGE_NAME).tgz -C $(LIBRLB_PKGDIR)
else

#   # "normal" mode: call "make install"
	@$(call world/install, LIBRLB)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save only FPGA image for BSP (according to requirement)
	@cd $(LIBRLB_PKGDIR) && tar cvzf $(LIBRLB_PLATFORMCONFIGPACKAGEDIR)/$(LIBRLB_PACKAGE_NAME).tgz \
        lib/firmware/$(LIBRLB_FPGA_IMAGE)
endif

endif

	@$(call touch)



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

	@$(call install_copy, librlb, 0, 0, 0644, -, /lib/firmware/$(LIBRLB_FPGA_IMAGE))

	@$(call install_finish, librlb)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/librlb.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBRLB)

# vim: syntax=make
