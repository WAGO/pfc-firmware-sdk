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
PACKAGES-$(PTXCONF_LIBCGROUP) += libcgroup

#
# Paths and names
#
LIBCGROUP_VERSION	:= 0.41
LIBCGROUP_MD5		:= 3dea9d50b8a5b73ff0bf1cdcb210f63f
LIBCGROUP		:= libcgroup-$(LIBCGROUP_VERSION)
LIBCGROUP_SUFFIX	:= tar.bz2
LIBCGROUP_URL		:= $(call ptx/mirror, SF, libcg/$(LIBCGROUP).$(LIBCGROUP_SUFFIX))
LIBCGROUP_SOURCE	:= $(SRCDIR)/$(LIBCGROUP).$(LIBCGROUP_SUFFIX)
LIBCGROUP_DIR		:= $(BUILDDIR)/$(LIBCGROUP)
LIBCGROUP_LICENSE	:= LGPLv2.1

#
# autoconf
#
LIBCGROUP_CONF_TOOL := autoconf
LIBCGROUP_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	--disable-bindings \
	--enable-tools \
	--disable-pam \
	--enable-daemon

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcgroup.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libcgroup)
	@$(call install_fixup, libcgroup,PRIORITY,optional)
	@$(call install_fixup, libcgroup,SECTION,base)
	@$(call install_fixup, libcgroup,AUTHOR,"Wolfram Sang <w.sang@pengutronix.de>")
	@$(call install_fixup, libcgroup,DESCRIPTION,missing)

	@$(call install_lib, libcgroup, 0, 0, 0644, libcgroup)

ifdef PTXCONF_LIBCGROUP_TOOLS

ifdef PTXCONF_LIBCGROUP_CGCLASSIFY
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgclassify)
endif
ifdef PTXCONF_LIBCGROUP_CGCREATE
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgcreate)
endif
ifdef PTXCONF_LIBCGROUP_CGDELETE
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgdelete)
endif
ifdef PTXCONF_LIBCGROUP_CGEXEC
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgexec)
endif
ifdef PTXCONF_LIBCGROUP_CGGET
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgget)
endif
ifdef PTXCONF_LIBCGROUP_CGSET
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgset)
endif
ifdef PTXCONF_LIBCGROUP_CGSNAPSHOT
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/cgsnapshot)
endif
ifdef PTXCONF_LIBCGROUP_LSCGROUP
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/lscgroup)
endif
ifdef PTXCONF_LIBCGROUP_LSSUBSYS
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/bin/lssubsys)
endif
ifdef PTXCONF_LIBCGROUP_CGCLEAR
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/sbin/cgclear)
endif
ifdef PTXCONF_LIBCGROUP_CGCONFIGPARSER
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/sbin/cgconfigparser)
endif
ifdef PTXCONF_LIBCGROUP_CGRULESENGD
	@$(call install_copy, libcgroup, 0, 0, 0755, -, /usr/sbin/cgrulesengd)
endif
endif

ifdef PTXCONF_LIBCGROUP_INITSCRIPT
	@$(call install_alternative, libcgroup, 0, 0, 755, /etc/init.d/cgroupsconfig)
	@$(call install_link, libcgroup, ../init.d/cgroupsconfig, /etc/rc.d/S00_cgroupsconfig)
endif

ifdef PTXCONF_LIBCGROUP_CONFIG_FILES
	@$(call install_alternative, libcgroup, 0, 0, 644, /etc/cgrules.conf)
	@$(call install_alternative, libcgroup, 0, 0, 644, /etc/cgconfig.conf)
endif

	@$(call install_finish, libcgroup)

	@$(call touch)

# vim: syntax=make
