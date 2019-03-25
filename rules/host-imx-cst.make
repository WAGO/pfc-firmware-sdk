# -*-makefile-*-
#
# Copyright (C) 2014, 2015, 2016 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_IMX_CST) += host-imx-cst

#
# Paths and names
#
HOST_IMX_CST_VERSION	:= 3.0.1
HOST_IMX_CST_MD5	:= 00dfe0e4c0709af73f892ab492930d19
HOST_IMX_CST		:= cst-$(HOST_IMX_CST_VERSION)
HOST_IMX_CST_SUFFIX	:= tar
#HOST_IMX_CST_URL	:= https://www.nxp.com/webapp/sps/download/license.jsp?colCode=IMX_CST_TOOL
HOST_IMX_CST_URL	:= https://svsv01003/svn/repo14/$(HOST_IMX_CST).$(HOST_IMX_CST_SUFFIX)
HOST_IMX_CST_SOURCE	:= $(SRCDIR)/$(HOST_IMX_CST).$(HOST_IMX_CST_SUFFIX)
HOST_IMX_CST_DIR	:= $(HOST_BUILDDIR)/$(HOST_IMX_CST)
HOST_IMX_CST_LICENSE	:= proprietary

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(HOST_IMX_CST_SOURCE):
#	@$(call targetinfo)
#	@echo "************************************************************************"
#	@echo "*"
#	@echo "* Due to license restrictions please download version $(HOST_IMX_CST_VERSION) manually from:"
#	@echo "*"
#	@echo "*    $(HOST_IMX_CST_URL)"
#	@echo "*"
#	@echo "* and place it into the source directory as:"
#	@echo "*"
#	@echo "*    $(HOST_IMX_CST_SOURCE)"
#	@echo "*"
#	@echo "*"
#	@echo "************************************************************************"
#	@echo
#	@exit 1

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_IMX_CST_CONF := NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-imx-cst.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

HOST_IMX_CST_PROGS := \
	cst \
	srktool \
	x5092wtls

HOST_IMX_CST_LIBS := \
	libbackend.a \
	libfrontend.a

HOST_IMX_CST_ARCH := \
	linux$(call ptx/ifeq, GNU_BUILD, x86_64-%, 64, 32)

$(STATEDIR)/host-imx-cst.install:
	@$(call targetinfo)
	@$(foreach prog, $(HOST_IMX_CST_PROGS), \
		install -v -m0755 -D $(HOST_IMX_CST_DIR)/$(HOST_IMX_CST_ARCH)/bin/$(prog) \
		$(HOST_IMX_CST_PKGDIR)/bin/$(prog);)
	@$(foreach lib, $(HOST_IMX_CST_LIBS), \
		install -v -m0644 -D $(HOST_IMX_CST_DIR)/$(HOST_IMX_CST_ARCH)/lib/$(lib) \
		$(HOST_IMX_CST_PKGDIR)/lib/imx-cst/$(lib);)
	@$(call touch)

# vim: syntax=make
