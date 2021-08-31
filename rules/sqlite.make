# -*-makefile-*-
#
# Copyright (C) 2004 by Ladislav Michl
#               2009 by Juergen Beisert <j.beisert@pengtronix.de>
#               2009 by Erwin Rol <erwin@erwinrol.com>
#               2010, 2012 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2011 by Markus Rathgeb <rathgeb.markus@googlemail.com>
#               2016 by Clemens Gruber <clemens.gruber@pqgruber.com>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SQLITE) += sqlite

#
# Paths and names
#
SQLITE_VERSION	:= 3280000
SQLITE_MD5	:= 3c68eb400f8354605736cd55400e1572
SQLITE		:= sqlite-autoconf-$(SQLITE_VERSION)
SQLITE_SUFFIX	:= tar.gz
SQLITE_URL	:= https://www.sqlite.org/2019/$(SQLITE).$(SQLITE_SUFFIX)
SQLITE_SOURCE	:= $(SRCDIR)/$(SQLITE).$(SQLITE_SUFFIX)
SQLITE_DIR	:= $(BUILDDIR)/$(SQLITE)
SQLITE_LICENSE	:= public_domain
SQLITE_LICENSE_FILES	:= file://sqlite3.c;startline=29;endline=30;md5=43af35cab122fd0eed4d5469d0507788

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# sqlite has an unusual config system where some defines are set by the
# configure script, but others can still be defined on the compiler command
# line. Pin down all configure options so that configure_helper.py is pleased,
# but apart from that, set only the defines that we really want (or otherwise
# our CPPFLAGS would explode).
SQLITE_CONF_ENV := \
	$(CROSS_ENV) \
	ac_cv_header_zlib_h=no \
	CPPFLAGS=" \
	-DSQLITE_ENABLE_COLUMN_METADATA=1 \
	-DSQLITE_ENABLE_FTS3_PARENTHESIS=1 \
	-DSQLITE_ENABLE_UNLOCK_NOTIFY=1 \
	-DSQLITE_DEFAULT_FOREIGN_KEYS=$(call ptx/ifdef, PTXCONF_SQLITE_FOREIGN_KEYS,1,0) \
	-DSQLITE_SOUNDEX=1 \
	"

SQLITE_CONF_TOOL	:= autoconf
SQLITE_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-static \
	--disable-editline \
	--$(call ptx/endis,PTXCONF_SQLITE_READLINE)-readline \
	--$(call ptx/endis,PTXCONF_SQLITE_THREADSAFE)-threadsafe \
	--$(call ptx/endis,PTXCONF_SQLITE_LOAD_EXTENSION)-dynamic-extensions \
	--enable-fts4 \
	--enable-fts3 \
	--disable-fts5 \
	--enable-json1 \
	--enable-rtree \
	--disable-session \
	--disable-debug \
	--disable-static-shell

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sqlite.targetinstall:
	@$(call targetinfo)

	@$(call install_init, sqlite)
	@$(call install_fixup, sqlite,PRIORITY,optional)
	@$(call install_fixup, sqlite,SECTION,base)
	@$(call install_fixup, sqlite,AUTHOR,"Ladislav Michl <ladis@linux-mips.org>")
	@$(call install_fixup, sqlite,DESCRIPTION,missing)

	@$(call install_lib, sqlite, 0, 0, 0644, libsqlite3)

ifdef PTXCONF_SQLITE_TOOL
	@$(call install_copy, sqlite, 0, 0, 0755, -, /usr/bin/sqlite3)
endif

	@$(call install_finish, sqlite)

	@$(call touch)

# vim: syntax=make
