# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
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
CPPUTEST_VERSION	:= 3.1
CPPUTEST			:= cpputest-$(CPPUTEST_VERSION)
CPPUTEST_URL		:= 
CPPUTEST_DIR		:= $(BUILDDIR)/$(CPPUTEST)
CPPUTEST_HOME		:= $(CPPUTEST_DIR)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/cpputest.extract:
	$(call targetinfo)
	mkdir -p $(CPPUTEST_DIR)
	rsync -a --exclude=".*" --exclude="*.o" --exclude="*.a" --exclude=objs/ --exclude=lib/ $(SRCDIR)/$(CPPUTEST) $(BUILDDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CPPUTEST_PATH	:= PATH=$(CROSS_PATH)
CPPUTEST_ENV 	:= $(CROSS_ENV)

#
# 
#
CPPUTEST_AUTOCONF := $(CROSS_AUTOCONF_USR)

$(STATEDIR)/cpputest.prepare:
	@$(call targetinfo)	
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cpputest.compile:
	@$(call targetinfo)
# Build CPPUTEST base
	cd $(CPPUTEST_DIR) && $(CPPUTEST_PATH) $(MAKE) all_no_tests $(PARALLELMFLAGS) \
	SILENCE="" CC=$(CROSS_CC) CXX=$(CROSS_CXX) CPPUTEST_ADDITIONAL_CPPFLAGS='$(CROSS_CPPFLAGS)' 

# build the cpputest extensions
	cd $(CPPUTEST_DIR) && $(CPPUTEST_PATH) $(MAKE) all_no_tests -f Makefile_CppUTestExt $(PARALLELMFLAGS) \
	SILENCE="" CC=$(CROSS_CC) CXX=$(CROSS_CXX) CPPUTEST_ADDITIONAL_CPPFLAGS='$(CROSS_CPPFLAGS)' 
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cpputest.install:
	@$(call targetinfo)
	@mkdir -p $(SYSROOT)/usr/include/CppUTest/
	@install -m 755 -D $(CPPUTEST_DIR)/include/CppUTest/* $(SYSROOT)/usr/include/CppUTest/
	@$(call touch)

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
