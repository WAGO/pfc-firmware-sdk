#
# Copyright (C) 2007 by Ladislav Michl
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_UNZIP) += unzip

#
# Paths and names
# (unzip is packaged a bit unusual way, that's why two version variables exist)
#
UNZIP_VERSION	:= 6.0
UNZIP_MD5	:= 62b490407489521db863b523a7f86375
UNZIP_AVERSION	:= 60
UNZIP_ARCHIVE	:= unzip$(UNZIP_AVERSION).tgz
UNZIP		:= unzip-$(UNZIP_VERSION)
UNZIP_URL	:= \
	$(call ptx/mirror, SF, infozip/$(UNZIP_ARCHIVE)) \
	ftp://ftp.info-zip.org/pub/infozip/src/$(UNZIP_ARCHIVE)
UNZIP_SOURCE	:= $(SRCDIR)/$(UNZIP_ARCHIVE)
UNZIP_DIR	:= $(BUILDDIR)/$(UNZIP)
UNZIP_LICENSE	:= Info-ZIP
UNZIP_LICENSE_FILES := file://LICENSE;md5=94caec5a51ef55ef711ee4e8b1c69e29

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

UNZIP_PATH		:= PATH=$(CROSS_PATH)
UNZIP_MAKE_OPT		:= $(CROSS_ENV_CC) -f unix/Makefile generic

UNZIP_INSTALL_OPT	:= -f unix/Makefile prefix=$(UNZIP_PKGDIR)/usr install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/unzip.targetinstall:
	@$(call targetinfo)

	@$(call install_init, unzip)
	@$(call install_fixup, unzip,PRIORITY,optional)
	@$(call install_fixup, unzip,SECTION,base)
	@$(call install_fixup, unzip,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, unzip,DESCRIPTION,missing)

ifdef PTXCONF_UNZIP_UNZIP
	@$(call install_copy, unzip, 0, 0, 0755, -, /usr/bin/unzip)
endif
ifdef PTXCONF_UNZIP_FUNZIP
	@$(call install_copy, unzip, 0, 0, 0755, -, /usr/bin/funzip)
endif
ifdef PTXCONF_UNZIP_UNZIPSFX
	@$(call install_copy, unzip, 0, 0, 0755, -, /usr/bin/unzipsfx)
endif

	@$(call install_finish, unzip)

	@$(call touch)

# vim: syntax=make
