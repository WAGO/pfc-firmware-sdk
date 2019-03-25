# -*-makefile-*-
#
# Copyright (C) 2010 by Bart vdr. Meulen <bartvdrmeulen@gmail.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_TZ_DATABASE) += host-tz-database

#
# Paths and names
#
TZCODE_VERSION	:= 2013d
TZCODE		:= tzcode$(TZCODE_VERSION)
TZCODE_SUFFIX	:= tar.gz
TZCODE_URL	:= \
ftp://elsie.nci.nih.gov/pub/$(TZCODE).$(TZCODE_SUFFIX) \
ftp://munnari.oz.au/pub/oldtz/$(TZCODE).$(TZCODE_SUFFIX)
TZCODE_SOURCE	:= $(SRCDIR)/$(TZCODE).$(TZCODE_SUFFIX)

TZDATA_VERSION	:= 2013d
TZDATA		:= tzdata$(TZDATA_VERSION)
TZDATA_SUFFIX	:= tar.gz
TZDATA_URL	:= \
ftp://elsie.nci.nih.gov/pub/$(TZDATA).$(TZDATA_SUFFIX) \
ftp://munnari.oz.au/pub/oldtz/$(TZDATA).$(TZDATA_SUFFIX)
TZDATA_SOURCE	:= $(SRCDIR)/$(TZDATA).$(TZDATA_SUFFIX)

TZ_STRIPPER_SRC := $(SRCDIR)/strip_timezone

HOST_TZDATABASE		:= tz-database
HOST_TZDATABASE_DIR	:= $(HOST_BUILDDIR)/$(HOST_TZDATABASE)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(TZCODE_SOURCE):
	@$(call get, TZCODE)

$(TZDATA_SOURCE):
	@$(call get, TZDATA)

$(STATEDIR)/host-tz-database.get: $(TZCODE_SOURCE) $(TZDATA_SOURCE)
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-tz-database.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(HOST_TZDATABASE_DIR))
# This doesn't work due to a bug in ptxdist-2013.03.0's ptxd_make_extract_world function
#	@$(call extract, TZCODE, $(HOST_TZDATABASE_DIR))
#	@$(call extract, TZDATA, $(HOST_TZDATABASE_DIR))
	mkdir -p $(HOST_TZDATABASE_DIR)
	tar xvf $(TZCODE_SOURCE) -C $(HOST_TZDATABASE_DIR)
	tar xvf $(TZDATA_SOURCE) -C $(HOST_TZDATABASE_DIR)

	@mkdir -p $(HOST_TZDATABASE_DIR)/strip_timezone
	@rsync -a --exclude=".project" --exclude=".cproject"  $(TZ_STRIPPER_SRC) $(HOST_TZDATABASE_DIR)
# See above
#	@$(call extract, TZDATA, $(HOST_TZDATABASE_DIR)/strip_timezone/unstripped)
	mkdir -p $(HOST_TZDATABASE_DIR)/strip_timezone/unstripped
	tar xvf $(TZDATA_SOURCE) -C $(HOST_TZDATABASE_DIR)/strip_timezone/unstripped
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_TZDATABASE_PATH	:= PATH=$(HOST_PATH)
HOST_TZDATABASE_ENV	:= $(HOST_ENV)
HOST_TZDATABASE_CONF_TOOL    := autoconf
HOST_TZDATABASE_CONF_OPT     := $(HOST_AUTOCONF_USR)


$(STATEDIR)/host-tz-database.prepare:
	@$(call targetinfo, $@)
	cd $(HOST_TZDATABASE_DIR)/strip_timezone && sh autogen.sh
	@cd $(HOST_TZDATABASE_DIR) && echo -e '#!/bin/bash\ncd strip_timezone\n./configure $$@\ncd -\n' > configure && chmod +x configure
	@$(call world/prepare, HOST_TZDATABASE)	
#	cd $(HOST_TZDATABASE_DIR)/strip_timezone && sh configure
#	cd $(HOST_TZDATABASE_DIR)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-tz-database.compile:
	@$(call targetinfo)
	cd $(HOST_TZDATABASE_DIR) && $(HOST_TZDATABASE_PATH) $(MAKE)
	cd $(HOST_TZDATABASE_DIR)/strip_timezone && $(MAKE)	
	mkdir -p $(PTXDIST_SYSROOT_HOST)/usr/share/zoneinfo
	cd $(HOST_TZDATABASE_DIR)/strip_timezone && ./src/tzstrip unstripped stripped \
	&& cd $(HOST_TZDATABASE_DIR)/strip_timezone \
	&& ../zic -d $(PTXDIST_SYSROOT_HOST)/usr/share/zoneinfo  $(HOST_TZDATABASE_DIR)/strip_timezone/stripped/tzstripped -v
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-tz-database.install:
	$(call targetinfo)
	$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/host-tz-database.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, HOST_TZDATABASE)

# vim: syntax=make
