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
PACKAGES-$(PTXCONF_LIBNVRAM) += libnvram

#
# Paths and names
#
LIBNVRAM_VERSION	:= 0
LIBNVRAM_MD5		:=
LIBNVRAM			:= libnvram
LIBNVRAM_URL		:= file://src/$(LIBNVRAM_TEST)
LIBNVRAM_DIR		:= $(BUILDDIR)/$(LIBNVRAM)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libnvram.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBNVRAM_DIR))
	cp -r $(SRCDIR)/$(LIBNVRAM) $(LIBNVRAM_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBNVRAM_PATH	:= PATH=$(CROSS_PATH)
LIBNVRAM_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBNVRAM_AUTOCONF := $(CROSS_AUTOCONF_USR)

$(STATEDIR)/libnvram.prepare:
	@$(call targetinfo)
	@$(call clean, $(LIBNVRAM_DIR)/config.cache)
	cd $(LIBNVRAM_DIR) && \
	./autogen.sh && \
		$(LIBNVRAM_PATH) $(LIBNVRAM_ENV) \
		./configure $(LIBNVRAM_AUTOCONF)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libnvram.compile:
#	@$(call targetinfo)
#	cd $(LIBNVRAM_DIR) && $(LIBNVRAM_PATH) $(MAKE) $(PARALLELMFLAGS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libnvram.install:
#	@$(call targetinfo)
#	cp $(SRCDIR)/$(LIBNVRAM)/src/nvram.h $(PTXCONF_SYSROOT_TARGET)/usr/include
#	cp -d $(LIBNVRAM_DIR)/src/.libs/libnvram.a $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	cp -d $(LIBNVRAM_DIR)/src/.libs/libnvram.so $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	cp -d $(LIBNVRAM_DIR)/src/.libs/libnvram.so.0.0.0 $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libnvram.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libnvram)
	@$(call install_fixup, libnvram,PRIORITY,optional)
	@$(call install_fixup, libnvram,SECTION,base)
	@$(call install_fixup, libnvram,AUTHOR,"<HFS>")
	@$(call install_fixup, libnvram,DESCRIPTION,missing)

	echo libnvramdir: $(LIBNVRAM_DIR)
	@$(call install_copy, libnvram, 0, 0, 0755, $(LIBNVRAM_DIR)/src/.libs/libnvram.so.0.0.0, /lib/libnvram.so.0.0.0)
	@$(call install_link, libnvram, libnvram.so.0.0.0, /lib/libnvram.so.0)
	@$(call install_link, libnvram, libnvram.so.0.0.0, /lib/libnvram.so)

	@$(call install_finish, libnvram)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

libnvram_clean:
	rm -rf $(STATEDIR)/libnvram.*
	rm -rf $(PKGDIR)/libnvram_*
	rm -rf $(LIBNVRAM_DIR)

# vim: syntax=make
