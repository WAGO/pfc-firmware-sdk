# -*-makefile-*-
#
# Copyright (C) 2012 by <Andrej.Gantvorg@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_CT_MAKE_XML_CONFIG) += host-ct-make-xml-config

#
# Paths and names
#
HOST_CT_MAKE_XML_CONFIG_VERSION	:= 1.0.0
HOST_CT_MAKE_XML_CONFIG		:= ct-make-xml-config
HOST_CT_MAKE_XML_CONFIG_SUFFIX	:= 
HOST_CT_MAKE_XML_CONFIG_URL	:= /$(HOST_CT_MAKE_XML_CONFIG)
HOST_CT_MAKE_XML_CONFIG_SRC	:= $(SRCDIR)/$(HOST_CT_MAKE_XML_CONFIG)
HOST_CT_MAKE_XML_CONFIG_DIR	:= \
$(HOST_BUILDDIR)/$(HOST_CT_MAKE_XML_CONFIG)-$(HOST_CT_MAKE_XML_CONFIG_VERSION)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ct-make-xml-config.extract:
	@$(call targetinfo)
	@$(call clean, $(HOST_CT_MAKE_XML_CONFIG_DIR))
	mkdir -p $(HOST_CT_MAKE_XML_CONFIG_DIR)
	rsync -a --exclude=".*" --exclude=objs/ --exclude="*.d" --exclude="*.o" --exclude="plclinux_rt" \
  $(HOST_CT_MAKE_XML_CONFIG_SRC)/ $(HOST_CT_MAKE_XML_CONFIG_DIR)
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# if host-python is selected (i.e. vtp-ctp), use it.
# Otherwise (i.e. pfcXXX) use the version installed on the host
#
ifdef PTXCONF_HOST_PYTHON
HOST_PYTHON_CPPFLAGS = $(shell $(PTXCONF_SYSROOT_HOST)/bin/python2 $(PTXCONF_SYSROOT_HOST)/bin/python2-config --cflags)
HOST_PYTHON_LDFLAGS = $(shell $(PTXCONF_SYSROOT_HOST)/bin/python2 $(PTXCONF_SYSROOT_HOST)/bin/python2-config --ldflags)
HOST_CT_MAKE_XML_CONFIG__ENV=EXTRA_CPPFLAGS="$(HOST_PYTHON_CPPFLAGS)"  \
                             EXTRA_LDFLAGS="$(HOST_PYTHON_LDFLAGS)" \
                             PYTHON=$(PTXCONF_SYSROOT_HOST)/bin/python2

$(STATEDIR)/host-ct-make-xml-config.prepare: $(STATEDIR)/host-python.install.post
else
HOST_CT_MAKE_XML_CONFIG__ENV = EXTRA_CPPFLAGS="$(shell python2-config --cflags)"  \
                             EXTRA_LDFLAGS="$(shell python2-config --ldflags)" \
                             PYTHON=/usr/bin/python2

endif

#DEBUG: kick optimization flags out of python-config output
#HOST_PYTHON_CPPFLAGS:=$(subst O2,O0,$(HOST_PYTHON_CPPFLAGS))
#HOST_PYTHON_LDFLAGS:=$(subst O2,O0,$(HOST_PYTHON_LDFLAGS))
#HOST_PYTHON_CPPFLAGS:=$(subst O3,O0,$(HOST_PYTHON_CPPFLAGS))
#HOST_PYTHON_LDFLAGS:=$(subst O3,O0,$(HOST_PYTHON_LDFLAGS))
#END DEBUG

HOST_CT_MAKE_XML_CONFIG_PATH	:= PATH=$(HOST_PATH)

$(STATEDIR)/host-ct-make-xml-config.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

 $(STATEDIR)/host-ct-make-xml-config.compile:
	@$(call targetinfo)
#	@$(call world/compile, HOST_CT_MAKE_XML_CONFIG)
#	$(call compile, HOST_CT_MAKE_XML_CONFIG, "$(HOST_CT_MAKE_XML_CONFIG__ENV)")
#  $(PTXCONF_SYSROOT_HOST)/bin/python/$(HOST_PYTHON_VERSION)
	cd $(HOST_CT_MAKE_XML_CONFIG_DIR) && $(HOST_CT_MAKE_XML_CONFIG__ENV) $(HOST_CT_MAKE_XML_CONFIG_PATH) $(MAKE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ct-make-xml-config.install:
	@$(call targetinfo)
	$(call install, HOST_CT_MAKE_XML_CONFIG,$(PTXCONF_HOST_PREFIX),h)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ct-make-xml-config.clean:
	@$(call targetinfo)
	@-make clean -C $(HOST_CT_MAKE_XML_CONFIG_DIR)
	@$(call clean_pkg, HOST_CT_MAKE_XML_CONFIG)

# vim: syntax=make
