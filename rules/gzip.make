# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GZIP) += gzip

#
# Paths and names
#
GZIP_VERSION	:= 1.10
GZIP_MD5	:= 691b1221694c3394f1c537df4eee39d3
GZIP		:= gzip-$(GZIP_VERSION)
GZIP_SUFFIX	:= tar.xz
GZIP_URL	:= $(call ptx/mirror, GNU, gzip/$(GZIP).$(GZIP_SUFFIX))
GZIP_SOURCE	:= $(SRCDIR)/$(GZIP).$(GZIP_SUFFIX)
GZIP_DIR	:= $(BUILDDIR)/$(GZIP)
GZIP_LICENSE	:= GPL-3.0-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
GZIP_CONF_TOOL	:= autoconf
GZIP_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--runstatedir=/run \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-threads=posix \
	--disable-rpath \
	--disable-gcc-warnings

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gzip.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gzip)
	@$(call install_fixup, gzip,PRIORITY,optional)
	@$(call install_fixup, gzip,SECTION,base)
	@$(call install_fixup, gzip,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, gzip,DESCRIPTION,missing)

	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/gzip)

# WAGO: do not install additional tools
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/gunzip)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/gzexe)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/uncompress)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zcat)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zcmp)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zdiff)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zegrep)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zfgrep)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zforce)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zgrep)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zless)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/zmore)
#	@$(call install_copy, gzip, 0, 0, 0755, -, /usr/bin/znew)

	@$(call install_finish, gzip)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/gzip.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, GZIP)

# vim: syntax=make
