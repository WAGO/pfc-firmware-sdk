# -*-makefile-*-
#
# Copyright (C) 2004 by BSP
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
PACKAGES-$(PTXCONF_SUDO) += sudo

#
# Paths and names
#
SUDO_VERSION	:= 1.9.5p2
SUDO_MD5	:= e6bc4c18c06346e6b3431637a2b5f3d5
SUDO		:= sudo-$(SUDO_VERSION)
SUDO_SUFFIX	:= tar.gz
SUDO_URL	:= \
	http://www.sudo.ws/sudo/dist/$(SUDO).$(SUDO_SUFFIX) \
	http://www.sudo.ws/sudo/dist/OLD/$(SUDO).$(SUDO_SUFFIX)
SUDO_SOURCE	:= $(SRCDIR)/$(SUDO).$(SUDO_SUFFIX)
SUDO_DIR	:= $(BUILDDIR)/$(SUDO)
SUDO_LICENSE	:= BSD, ISC

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SUDO_PATH	:= PATH=$(CROSS_PATH)
SUDO_ENV 	:= \
	$(CROSS_ENV) \
	sudo_cv_func_fnmatch=yes \
	sudo_cv_func_unsetenv_void=no \
	ac_cv_have_working_snprintf=yes \
	ac_cv_have_working_vsnprintf=yes

#
# autoconf
#
SUDO_AUTOCONF = \
	$(CROSS_AUTOCONF_USR) \
	--enable-shadow \
	--enable-root-sudo \
	--disable-log-host \
	--enable-noargs-shell \
	--disable-path-info \
	--enable-warnings \
	--disable-werror \
	--enable-hardening \
	--enable-pie \
	--disable-nls \
	--disable-rpath \
	--enable-static-sudoers \
	--disable-shared-libutil \
	--disable-sia \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_SUDO_PAM)-pam-session \
	--without-AFS \
	--without-DCE \
	--without-logincap \
	--without-bsdauth \
	--without-project \
	--without-lecture \
	--with-ignore-dot \
	--$(call ptx/wwo, PTXCONF_SUDO_PAM)-pam

ifdef PTXCONF_SUDO_USE_SENDMAIL
SUDO_AUTOCONF += --with-sendmail
else
SUDO_AUTOCONF += --without-sendmail
endif

ifdef PTXCONF_SUDO_INSTALL_VISUDO
ifneq ($(PTXCONF_SUDO_DEFAULT_EDITOR),"")
SUDO_AUTOCONF += --with-editor=$(PTXCONF_SUDO_DEFAULT_EDITOR)
endif
endif

ifdef PTXCONF_SUDO_USE_ENV_EDITOR
SUDO_AUTOCONF += --with-env-editor
else
SUDO_AUTOCONF += --without-env-editor
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sudo.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  sudo)
	@$(call install_fixup, sudo,PRIORITY,optional)
	@$(call install_fixup, sudo,SECTION,base)
	@$(call install_fixup, sudo,AUTHOR,"Carsten Schlote <c.schlote@konzeptpark.de>")
	@$(call install_fixup, sudo,DESCRIPTION,missing)

	@$(call install_copy, sudo, 0, 0, 7755, -, /usr/bin/sudo)
	@$(call install_link, sudo, sudo, /usr/bin/sudoedit)

	@$(call install_copy, sudo, 0, 0, 0644, -, \
		/usr/libexec/sudo/sudo_noexec.so)
	@$(call install_copy, sudo, 0, 0, 0644, -, \
		/usr/libexec/sudo/group_file.so)
	@$(call install_copy, sudo, 0, 0, 0644, -, \
                /usr/libexec/sudo/system_group.so)

ifdef PTXCONF_SUDO_INSTALL_ETC_SUDOERS
	@$(call install_alternative, sudo, 0, 0, 0440, /etc/sudoers, n)
	@$(call install_copy, sudo, 0, 0, 755, /etc/sudoers.d)
endif

ifdef PTXCONF_SUDO_INSTALL_VISUDO
	@$(call install_copy, sudo, 0, 0, 755, -, /usr/sbin/visudo)
endif

	@$(call install_finish, sudo)
	@$(call touch)

# vim: syntax=make
