# *-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GSL) += gsl

#
# Paths and names
#
GSL_VERSION	:= 3.0.1
GSL_MD5	    := 163bcffaa8beb1a43a31ea7ae3665aaa
GSL		    := GSL-$(GSL_VERSION)
GSL_SUFFIX	:= tar.gz
GSL_URL	    := https://github.com/microsoft/GSL/archive/v$(GSL_VERSION).$(GSL_SUFFIX)
GSL_SOURCE	:= $(SRCDIR)/$(GSL).$(GSL_SUFFIX)
GSL_DIR	    := $(BUILDDIR)/$(GSL)
GSL_LICENSE	:= MIT
GSL_LICENSE_FILE	:= LICENSE

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GSL_CONF_TOOL	:= cmake
GSL_MAKE_ENV	:= $(CROSS_ENV)
GSL_CONF_OPT	:= $(CROSS_CMAKE_USR) $(GSL_CMAKE_OPTS)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gsl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gsl)
	@$(call install_fixup, gsl,PRIORITY,optional)
	@$(call install_fixup, gsl,SECTION,base)
	@$(call install_fixup, gsl,AUTHOR,"WAGO GmbH")
	@$(call install_fixup, gsl,DESCRIPTION,missing)

	@$(call install_copy, gsl, 0, 0, 0644, $(GSL_DIR)/$(GSL_LICENSE_FILE), /usr/share/licenses/oss/license.gsl_$(GSL_VERSION).txt)

	@$(call install_finish, gsl)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/gsl.clean:
	@$(call targetinfo)
	@-cd $(GSL_DIR)-build && \
		$(GSL_ENV) $(GSL_PATH) $(MAKE) clean
	@$(call clean_pkg, GSL)

# vim: syntax=make
