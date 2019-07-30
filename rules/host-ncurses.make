# -*-makefile-*-
#
# Copyright (C) 2008 by mol@pengutronix.de
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_NCURSES) += host-ncurses

#
# Paths and names
#
HOST_NCURSES_DIR	:= $(BUILDDIR)/$(HOST_NCURSES)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
NCURSES_PATCHDIR=$(PTXDIST_WORKSPACE)/patches/$(NCURSES)

$(STATEDIR)/host-ncurses.extract:
	@$(call targetinfo)
	@$(call clean, $(NCURSES_DIR))
	@$(call extract, HOST_NCURSES)

# Applying upstream patch using their custom patching approach involving a shell script.
	@if [[ -d $(NCURSES_PATCHDIR) ]]; then \
		for script in $(shell ls $(NCURSES_PATCHDIR)/ncurses-*-patch.sh); do\
			cp $$script $(HOST_NCURSES_DIR) && \
			cd $(HOST_NCURSES_DIR) && \
			chmod 755 $$script && \
			./$$(basename $$script); \
		done; \
	fi

	@$(call patchin, HOST_NCURSES)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_NCURSES_CONF_TOOL	:= autoconf
HOST_NCURSES_CONF_OPT	= \
	$(HOST_AUTOCONF) \
	$(NCURSES_AUTOCONF_SHARED) \
	--with-progs \
	--without-shared

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ncurses.install:
	@$(call targetinfo)
	@$(call world/install, HOST_NCURSES)
#	# don't install headers or libs, so packages like the kernel don't use it
	@rm -r $(HOST_NCURSES_PKGDIR)/include
	@rm -r $(HOST_NCURSES_PKGDIR)/lib/*.a
	@$(call touch)

# vim: syntax=make
