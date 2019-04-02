# -*-makefile-*-
#
# Copyright (C) 2012 by <Hans Florian Scholz>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OMS) += oms

#
# Paths and names
#
ifdef PTXCONF_OMS_TRUNK
OMS_VERSION	:= 0.1
else
OMS_VERSION	:= 0.1
endif
OMS		:= oms-$(OMS_VERSION)
OMS_URL		:= file://$(SRCDIR)/$(OMS)
OMS_SRC		:= $(SRCDIR)/$(OMS)
OMS_DIR		:= $(BUILDDIR)/$(OMS)
OMS_BUILD_OOT	:= NO
OMS_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(OMS_DIR))
	#@$(call extract, OMS)
	@mkdir -p $(OMS_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(OMS_SRC) $(BUILDDIR)
#	@$(call patchin, OMS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#OMS_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
OMS_CONF_TOOL	:= autoconf
OMS_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

ifndef PTXCONF_OMS_OMSD
	OMS_CONF_OPT += --disable-omsd
endif

$(STATEDIR)/oms.prepare:
	@$(call targetinfo)

ifdef PTXCONF_OMS_TRUNK
	cd $(OMS_DIR) && sh autogen.sh
else
	#cd $(OMS_DIR) && [ -f configure ] || sh autogen.sh
	cd $(OMS_DIR) && sh autogen.sh
endif

	@$(call world/prepare, OMS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.compile:
	@$(call targetinfo)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(OMS_SRC) $(BUILDDIR)
	@$(call world/compile, OMS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/oms.install:
#	@$(call targetinfo)
#	@$(call world/install, OMS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.targetinstall:
	@$(call targetinfo)

	@$(call install_init, oms)
	@$(call install_fixup, oms, PRIORITY, optional)
	@$(call install_fixup, oms, SECTION, base)
	@$(call install_fixup, oms, AUTHOR, "<Hans Florian Scholz>")
	@$(call install_fixup, oms, DESCRIPTION, missing)

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(OMS_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, oms, 0, 0, 0755, -, /$$i); \
	done
#	@for i in $(shell cd $(OMS_PKGDIR) && find lib usr/lib -name "*.so*"); do \
#		$(call install_copy, oms, 0, 0, 0644, -, /$$i); \
#	done
	@$(call install_copy, oms, 0, 0, 0644, -, /usr/lib/liboms.so)
#	@$(call install_copy, oms, 0, 0, 0644, $(OMS_PKGDIR)/usr/lib/wide/liboms_codesys.so, /usr/lib/wide/liboms.so)

	@links="$(shell cd $(OMS_PKGDIR) && find lib usr/lib -type l)"; \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(OMS_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, oms, $$from, $$to); \
		done; \
	fi
	@$(call install_alternative, oms, 0, 0, 0644, /etc/oms.d/omsd.conf,n)
	@$(call install_alternative, oms, 0, 0, 0770, /etc/oms.d/power_on_reset.sh,n)
ifdef PTXCONF_OMS_FIX_IP
	@$(call install_alternative, oms, 0, 0, 0644, /etc/oms.d/pfc/fix_ip.conf,n)
	@$(call install_alternative, oms, 0, 0, 0770, /etc/oms.d/fix_ip.sh,n)
endif
ifdef PTXCONF_OMS_FACTORY_DEFUALTS
	@$(call install_alternative, oms, 0, 0, 0644, /etc/oms.d/pfc/factory_defaults.conf,n)
	#@$(call install_alternative, oms, 0, 0, 0644, /etc/oms.d/src/factory_defaults.conf,n)
	@$(call install_alternative, oms, 0, 0, 0770, /etc/oms.d/set_factory_defaults.sh,n)
endif
	@$(call install_copy, oms, 0, 0, 0755, -, /etc/init.d/omsdaemon)

#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, oms)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.clean:
	@$(call targetinfo)
	#rm -f $(OMS_DIR)/{configure,config.sub,config.guess}
	@$(call clean_pkg, OMS)
	@-rm -rf $(OMS_DIR)


# vim: syntax=make
