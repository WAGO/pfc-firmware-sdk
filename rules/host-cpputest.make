# -*-makefile-*-
#
# Copyright (C) 2012 by <andrej.gantvorg@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_CPPUTEST) += host-cpputest

#
# Paths and names
#
HOST_CPPUTEST_VERSION	:= 3.1
HOST_CPPUTEST		:= cpputest-$(HOST_CPPUTEST_VERSION)
HOST_CPPUTEST_SUFFIX	:= 
HOST_CPPUTEST_URL	:= file://$(SRCDIR)/$(HOST_CPPUTEST)
#HOST_CPPUTEST_SOURCE	:= $(SRCDIR)/cpputest-$(HOST_CPPUTEST_VERSION)
HOST_CPPUTEST_DIR	:= $(HOST_BUILDDIR)/$(HOST_CPPUTEST)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-cpputest.extract:
	@$(call targetinfo)
	@$(call clean, $(HOST_CPPUTEST_DIR))
	mkdir -p $(HOST_CPPUTEST_DIR)
	rsync -a --exclude=".*" --exclude=objs/ --exclude="*.d" --exclude="*.o" \
  $(SRCDIR)/$(HOST_CPPUTEST)/* $(HOST_CPPUTEST_DIR)
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#HOST_CPPUTEST_CONF_ENV	:= $(HOST_ENV)

#
# autoconf
#
HOST_CPPUTEST_CONF_TOOL	:= autoconf
#HOST_CPPUTEST_CONF_OPT	:= $(HOST_AUTOCONF)

$(STATEDIR)/host-cpputest.prepare:
	@$(call targetinfo)
#	@$(call clean, $(HOST_CPPUTEST_DIR)/config.cache)
#	cd $(HOST_CPPUTEST_DIR) && \
#		$(HOST_CPPUTEST_PATH) $(HOST_CPPUTEST_ENV) \
#		./configure $(HOST_CPPUTEST_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-cpputest.compile:
	@$(call targetinfo)
	@$(call world/compile, HOST_CPPUTEST)
   # now make the extensions
	cd $(HOST_CPPUTEST_DIR) && $(HOST_CPPUTEST_PATH) $(MAKE) -f Makefile_CppUTestExt $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-cpputest.install:
	@$(call targetinfo)
	# The project's build dir under build-host has to be referenced via
   # CPPUTEST_HOME variable.
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/host-cpputest.clean:
	@$(call targetinfo)
	@$(call clean_pkg, HOST_CPPUTEST)

# vim: syntax=make
