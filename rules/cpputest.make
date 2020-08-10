# -*-makefile-*-
#
# Copyright (C) 2010 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CPPUTEST) += cpputest

#
# Paths and names
#
CPPUTEST_VERSION	:= 3.7.2
CPPUTEST_MD5		:= f4f7d62cb78e360ee3c979ee834f88b4
CPPUTEST		:= cpputest-$(CPPUTEST_VERSION)
CPPUTEST_SUFFIX    	:= tar.gz
# Note that prefix 'v' is missing in URL of version 3.7.2: URL must b fixed on update 
CPPUTEST_URL       	:= https://github.com/cpputest/cpputest/releases/download/$(CPPUTEST_VERSION)/cpputest-$(CPPUTEST_VERSION).$(CPPUTEST_SUFFIX)
CPPUTEST_SOURCE    	:= $(SRCDIR)/cpputest-$(CPPUTEST_VERSION).$(CPPUTEST_SUFFIX)
CPPUTEST_DIR		:= $(BUILDDIR)/$(CPPUTEST)
CPPUTEST_HOME		:= $(CPPUTEST_DIR)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CPPUTEST_PATH	:= PATH=$(CROSS_PATH)
CPPUTEST_ENV 	:= $(CROSS_ENV)

#
# cmake 
#
CPPUTEST_CONF_TOOL := cmake
CPPUTEST_CONF_OPT += -DC++11=ON
CPPUTEST_CONF_OPT += -DCPPUTEST_FLAGS=OFF 

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# $(STATEDIR)/cpputest.prepare:
#	@$(call targetinfo)	
#	@$(call world/prepare, CPPUTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/cpputest.compile:
#	@$(call targetinfo)
#	@$(call world/compile, CPPUTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/cpputest.install:
#	@$(call targetinfo)
#	@$(call world/install, CPPUTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cpputest.targetinstall:
	@$(call targetinfo)

# Packet contains no artifcats.

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

cpputest_clean:
	rm -rf $(STATEDIR)/cpputest.*
	rm -rf $(PKGDIR)/cpputest_*
	rm -rf $(CPPUTEST_DIR)

# vim: syntax=make
