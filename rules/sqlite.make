# -*-makefile-*-
#
# Copyright (C) 2004 by Ladislav Michl
#               2009 by Juergen Beisert <j.beisert@pengtronix.de>
#               2009 by Erwin Rol <erwin@erwinrol.com>
#               2010, 2012 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2011 by Markus Rathgeb <rathgeb.markus@googlemail.com>
#
# See CREDITS for details about who has contributed to this project.
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
SQLITE_VERSION	:= 3220000
SQLITE_MD5	:= 96b5648d542e8afa6ab7ffb8db8ddc3d
SQLITE		:= sqlite-autoconf-$(SQLITE_VERSION)
SQLITE_SUFFIX	:= tar.gz
SQLITE_URL	:= http://www.sqlite.org/2018/$(SQLITE).$(SQLITE_SUFFIX)
SQLITE_SOURCE	:= $(SRCDIR)/$(SQLITE).$(SQLITE_SUFFIX)
SQLITE_DIR	:= $(BUILDDIR)/$(SQLITE)
SQLITE_LICENSE	:= public_domain

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_SQLITE_WAL_SYNCHRONOUS_NORMAL
SQLITE_CPPFLAGS+= -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1
endif

ifdef PTXCONF_SQLITE_DISABLE_MEMSTATUS
SQLITE_CPPFLAGS+= -DSQLITE_DEFAULT_MEMSTATUS=0
else
SQLITE_CPPFLAGS+= -DSQLITE_DEFAULT_MEMSTATUS=1
endif

ifdef PTXCONF_SQLITE_LIKE_DOESNT_MATCH_BLOBS
SQLITE_CPPFLAGS+= -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
endif

ifdef PTXCONF_SQLITE_MAX_EXPR_DEPTH
SQLITE_CPPFLAGS+= -DSQLITE_MAX_EXPR_DEPTH=0
endif

# The php5 pdo_sqlite extension depends on functions of SQLite3, which are
# deprecated in newer versions of SQLite3. Therefore, we cannot omit
# these deprecated functions as long as we want to support the php5 pdo_sqlite interface.
ifdef PTXCONF_SQLITE_OMIT_DEPRECATED
SQLITE_CPPFLAGS+= -DSQLITE_OMIT_DEPRECATED
endif

ifdef PTXCONF_SQLITE_OMIT_SHARED_CACHE
SQLITE_CPPFLAGS+= -DSQLITE_OMIT_SHARED_CACHE
endif

ifdef PTXCONF_SQLITE_USE_ALLOCA
SQLITE_CPPFLAGS+= -DSQLITE_USE_ALLOCA
endif


SQLITE_CONF_ENV := \
	CPPFLAGS=" \
	$(SQLITE_CPPFLAGS) \
	-DSQLITE_ENABLE_COLUMN_METADATA \
	-DSQLITE_ENABLE_FTS3 \
	-DSQLITE_ENABLE_RTREE \
	-DSQLITE_SOUNDEX=1 \
	"
SQLITE_AUTOCONF	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-static

ifdef PTXCONF_SQLITE_THREADSAFE
SQLITE_AUTOCONF += --enable-threadsafe
else
SQLITE_AUTOCONF += --disable-threadsafe
endif

ifdef PTXCONF_SQLITE_LOAD_EXTENTION
SQLITE_AUTOCONF += --enable-dynamic-extensions
else
SQLITE_AUTOCONF += --disable-dynamic-extensions
endif

ifdef PTXCONF_SQLITE_READLINE
SQLITE_AUTOCONF += --enable-readline
else
SQLITE_AUTOCONF += --disable-readline
endif

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
