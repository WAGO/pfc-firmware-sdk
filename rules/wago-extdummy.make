# -*-makefile-*-
#
# Copyright (C) 2014 by <>WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_EXTDUMMY) += wago-extdummy

#
# Paths and names
#
WAGO_EXTDUMMY_VERSION	:= 1.0.0
WAGO_EXTDUMMY		:= Wagodummy-$(WAGO_EXTDUMMY_VERSION)
WAGO_EXTDUMMY_SUFFIX	:= zip
WAGO_EXTDUMMY_MD5	= $(shell wget --timeout=1 -q -O - "$$@" http://svsv01003.wago.local/wbm/$(WAGO_EXTDUMMY).$(WAGO_EXTDUMMY_SUFFIX).md5 | sed -e 's/*$(WAGO_EXTDUMMY).$(WAGO_EXTDUMMY_SUFFIX)//g')
WAGO_EXTDUMMY_URL	:= http://svsv01003.wago.local/wbm/$(WAGO_EXTDUMMY).$(WAGO_EXTDUMMY_SUFFIX)
WAGO_EXTDUMMY_SOURCE	:= $(SRCDIR)/$(WAGO_EXTDUMMY).$(WAGO_EXTDUMMY_SUFFIX)
WAGO_EXTDUMMY_DIR	:= $(BUILDDIR)/$(WAGO_EXTDUMMY)
WAGO_EXTDUMMY_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WAGO_EXTDUMMY_SOURCE):
	@$(call targetinfo)
	@$(call get, WAGO_EXTDUMMY)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
$(STATEDIR)/wago-extdummy.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-extdummy.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-extdummy.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-extdummy.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-extdummy)
	@$(call install_fixup, wago-extdummy,PRIORITY,optional)
	@$(call install_fixup, wago-extdummy,SECTION,base)
	@$(call install_fixup, wago-extdummy,AUTHOR,"<WAGO GmbH \& Co. KG>")
	@$(call install_fixup, wago-extdummy,DESCRIPTION,missing)
	
	@$(call install_copy, wago-extdummy, 0, 0, 0755, $(WAGO_EXTDUMMY_DIR)/app.js, /var/www/extdummy/app.js)
	@$(call install_copy, wago-extdummy, 0, 0, 0755, $(WAGO_EXTDUMMY_DIR)/index.html, /var/www/extdummy/index.html)
	@$(call install_tree, wago-extdummy, 0, 0, $(WAGO_EXTDUMMY_DIR)/resources, /var/www/extdummy/resources)

	@$(call install_finish, wago-extdummy)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-extdummy.clean:
	@$(call targetinfo)
	@$(shell rm src/$(WAGO_EXTDUMMY).$(WAGO_EXTDUMMY_SUFFIX))
	@$(call clean_pkg, WAGO_EXTDUMMY)

vim: syntax=make
