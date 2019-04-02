# -*-makefile-*-
#
# Copyright (C) 2012 by Wolfram Sang <w.sang@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LTTNG_MODULES) += lttng-modules

#
# Paths and names
#
LTTNG_MODULES_VERSION	:= 2.6.0
LTTNG_MODULES_MD5		:= a058ab037daaca293a54934d7b9f9c98
LTTNG_MODULES		:= lttng-modules-$(LTTNG_MODULES_VERSION)
LTTNG_MODULES_SUFFIX	:= tar.bz2
#LTTNG_MODULES_URL		:= $(call ptx/mirror, SF, libcg/$(LTTNG_MODULES).$(LTTNG_MODULES_SUFFIX))
LTTNG_MODULES_SOURCE	:= $(SRCDIR)/$(LTTNG_MODULES).$(LTTNG_MODULES_SUFFIX)
LTTNG_MODULES_DIR		:= $(BUILDDIR)/$(LTTNG_MODULES)
LTTNG_MODULES_LICENSE	:= LGPLv2.1

ifdef PTXCONF_LTTN_MODULES
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/lttn-modules.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/lttn-modules.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/lttn-modules.compile:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(LTTN_MODULES_DIR) \
		modules
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lttn-modules.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lttn-modules.targetinstall:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(LTTN_MODULES_DIR) \
		modules_install
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/lttn-modules.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LTTN_MODULES)

# vim: syntax=make
