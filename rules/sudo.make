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
SUDO_VERSION	:= 1.7.10p9
SUDO_MD5	:= 6c9288b816414695c8e2a94cab6b3f58
SUDO		:= sudo-$(SUDO_VERSION)
SUDO_SUFFIX	:= tar.gz
SUDO_URL	:= \
	http://www.sudo.ws/sudo/dist/$(SUDO).$(SUDO_SUFFIX) \
	http://www.sudo.ws/sudo/dist/OLD/$(SUDO).$(SUDO_SUFFIX) \
	ftp://ftp.sudo.ws/pub/sudo/$(SUDO).$(SUDO_SUFFIX)
SUDO_SOURCE	:= $(SRCDIR)/$(SUDO).$(SUDO_SUFFIX)
SUDO_DIR	:= $(BUILDDIR)/$(SUDO)
SUDO_LICENSE	:= Multiple Licenses

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SUDO_PATH	:= PATH=$(CROSS_PATH)
SUDO_ENV 	:= \
	$(CROSS_ENV) \
	sudo_cv_func_fnmatch=yes \
	sudo_cv_func_unsetenv_void=no \
	sudo_cv_sock_sa_len=yes \
	sudo_cv_uid_t_len=10

#
# autoconf
#
SUDO_AUTOCONF = \
	$(CROSS_AUTOCONF_USR) \
	--disable-shadow \
	--enable-root-sudo \
	--disable-log-host \
	--enable-noargs-shell \
	--disable-path-info \
	--disable-sia \
	--without-AFS \
	--without-DCE \
	--without-logincap \
	--without-bsdauth \
	--without-project \
	--without-lecture \
	--with-ignore-dot \
	--without-pam

ifdef PTXCONF_SUDO_USE_SENDMAIL
SUDO_AUTOCONF += --with-sendmail
else
SUDO_AUTOCONF += --without-sendmail
endif

ifneq ($(PTXCONF_SUDO_DEFAULT_EDITOR),"")
SUDO_AUTOCONF += --with-editor=$(PTXCONF_SUDO_DEFAULT_EDITOR)
endif

ifdef PTXCONF_SUDO_USE_ENV_EDITOR
SUDO_AUTOCONF += --with-env-editor
else
SUDO_AUTOCONF += --without-env-editor
endif

#  --disable-root-mailer   Don't run the mailer as root, run as the user
#  --disable-setreuid      Don't try to use the setreuid() function
#  --disable-setresuid     Don't try to use the setresuid() function
#  --enable-shell-sets-home set $HOME to target user in shell mode
#  --with-AFS              enable AFS support
#  --with-logging          log via syslog, file, or both
#  --with-logfac           syslog facility to log with (default is "local2")
#  --with-goodpri          syslog priority for commands (def is "notice")
#  --with-badpri           syslog priority for failures (def is "alert")
#  --with-logpath          path to the sudo log file
#  --with-loglen           maximum length of a log file line (default is 80)
#  --without-mail-if-no-user do not send mail if user not in sudoers
#  --with-mail-if-no-host  send mail if user in sudoers but not for this host
#  --with-mail-if-noperms  send mail if user not allowed to run command
#  --with-mailto           who should get sudo mail (default is "root")
#  --with-mailsubject      subject of sudo mail
#  --with-passprompt       default password prompt
#  --with-badpass-message  message the user sees when the password is wrong
#  --with-fqdn             expect fully qualified hosts in sudoers
#  --with-timedir          path to the sudo timestamp dir
#  --with-sudoers-mode     mode of sudoers file (defaults to 0440)
#  --with-sudoers-uid      uid that owns sudoers file (defaults to 0)
#  --with-sudoers-gid      gid that owns sudoers file (defaults to 0)
#  --with-umask            umask with which the prog should run (default is 022)
#  --without-umask         Preserves the umask of the user invoking sudo.
#  --with-runas-default    User to run commands as (default is "root")
#  --with-exempt=group     no passwd needed for users in this group
#  --with-editor=path      Default editor for visudo (defaults to vi)
#  --with-env-editor       Use the environment variable EDITOR for visudo
#  --with-passwd-tries     number of tries to enter password (default is 3)
#  --with-timeout          minutes before sudo asks for passwd again (def is 5 minutes)
#  --with-password-timeout passwd prompt timeout in minutes (default is 5 minutes)
#  --with-tty-tickets      use a different ticket file for each tty
#  --with-insults          insult the user for entering an incorrect password
#  --with-all-insults      include all the sudo insult sets
#  --with-classic-insults  include the insults from the "classic" sudo
#  --with-csops-insults    include CSOps insults
#  --with-hal-insults      include 2001-like insults
#  --with-goons-insults    include the insults from the "Goon Show"
#  --with-ldap[=DIR]       enable LDAP support
#  --with-ldap-conf-file   path to LDAP configuration file
#  --with-ldap-secret-file path to LDAP secret pasdword file
#  --with-pc-insults       replace politically incorrect insults with less offensive ones
#  --with-secure-path      override the user's path with a built-in one
#  --without-interfaces    don't try to read the ip addr of ether interfaces
#  --with-stow             properly handle GNU stow packaging
#  --with-pic              try to use only PIC/non-PIC objects [default=use

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
		/usr/libexec/sudo_noexec.so)

ifdef PTXCONF_SUDO_INSTALL_ETC_SUDOERS
	@$(call install_alternative, sudo, 0, 0, 0440, /etc/sudoers, n)
endif

ifdef PTXCONF_SUDO_INSTALL_VISUDO
	@$(call install_copy, sudo, 0, 0, 750, -, /usr/sbin/visudo)
endif

	@$(call install_finish, sudo)
	@$(call touch)

# vim: syntax=make
