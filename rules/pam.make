# -*-makefile-*-
#
# Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PAM) += pam

#
# Paths and names
#
PAM_VERSION       := 1.4.0
PAM_MD5           := 39fca0523bccec6af4b63b5322276c84
PAM               := Linux-PAM-$(PAM_VERSION)
PAM_SUFFIX        := tar.xz
PAM_URL           := https://github.com/linux-pam/linux-pam/releases/download/v$(PAM_VERSION)/$(PAM).$(PAM_SUFFIX)
PAM_SOURCE        := $(SRCDIR)/$(PAM).$(PAM_SUFFIX)
PAM_DIR           := $(BUILDDIR)/$(PAM)
PAM_LICENSE       := GPLv2

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/pam.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/pam.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
PAM_AUTOCONF := \
        $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/pam.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/pam.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/pam.install:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

#
# Linux-PAM modules WITHOUT any (default) configuration files
#
PAM_MODULES-$(PTXCONF_PAM_PAM_DEBUG)      += pam_debug.so
PAM_MODULES-$(PTXCONF_PAM_PAM_DENY)       += pam_deny.so
PAM_MODULES-$(PTXCONF_PAM_PAM_ECHO)       += pam_echo.so
PAM_MODULES-$(PTXCONF_PAM_PAM_EXEC)       += pam_exec.so
PAM_MODULES-$(PTXCONF_PAM_PAM_FAILDELAY)  += pam_faildelay.so
PAM_MODULES-$(PTXCONF_PAM_PAM_FTP)        += pam_ftp.so
PAM_MODULES-$(PTXCONF_PAM_PAM_ISSUE)      += pam_issue.so
PAM_MODULES-$(PTXCONF_PAM_PAM_KEYINIT)    += pam_keyinit.so
PAM_MODULES-$(PTXCONF_PAM_PAM_LASTLOG)    += pam_lastlog.so
PAM_MODULES-$(PTXCONF_PAM_PAM_LISTFILE)   += pam_listfile.so
PAM_MODULES-$(PTXCONF_PAM_PAM_LOCALUSER)  += pam_localuser.so
PAM_MODULES-$(PTXCONF_PAM_PAM_LOGINUID)   += pam_loginuid.so
PAM_MODULES-$(PTXCONF_PAM_PAM_MAIL)       += pam_mail.so
PAM_MODULES-$(PTXCONF_PAM_PAM_MOTD)       += pam_motd.so
PAM_MODULES-$(PTXCONF_PAM_PAM_NOLOGIN)    += pam_nologin.so
PAM_MODULES-$(PTXCONF_PAM_PAM_PERMIT)     += pam_permit.so
PAM_MODULES-$(PTXCONF_PAM_PAM_PWHISTORY)  += pam_pwhistory.so
PAM_MODULES-$(PTXCONF_PAM_PAM_RHOSTS)     += pam_rhosts.so
PAM_MODULES-$(PTXCONF_PAM_PAM_ROOTOK)     += pam_rootok.so
PAM_MODULES-$(PTXCONF_PAM_PAM_SECURETTY)  += pam_securetty.so
PAM_MODULES-$(PTXCONF_PAM_PAM_SHELLS)     += pam_shells.so
PAM_MODULES-$(PTXCONF_PAM_PAM_STRESS)     += pam_stress.so
PAM_MODULES-$(PTXCONF_PAM_PAM_SUCCEED_IF) += pam_succeed_if.so
PAM_MODULES-$(PTXCONF_PAM_PAM_UMASK)      += pam_umask.so
PAM_MODULES-$(PTXCONF_PAM_PAM_USERDB)     += pam_userdb.so
PAM_MODULES-$(PTXCONF_PAM_PAM_WARN)       += pam_warn.so
PAM_MODULES-$(PTXCONF_PAM_PAM_WHEEL)      += pam_wheel.so
PAM_MODULES-$(PTXCONF_PAM_PAM_XAUTH)      += pam_xauth.so

$(STATEDIR)/pam.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  pam)
	@$(call install_fixup, pam, PRIORITY, optional)
	@$(call install_fixup, pam, SECTION, base)
	@$(call install_fixup, pam, AUTHOR, "WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, pam, DESCRIPTION, missing)

	#
	# install basic Linux-PAM libraries
	#
	@$(call install_lib, pam, 0, 0, 0644, libpam)
	@$(call install_lib, pam, 0, 0, 0644, libpam_misc)
	@$(call install_lib, pam, 0, 0, 0644, libpamc)

	#
	# Linux-PAM configuration files for services
	#

	# NOTE: Do not install blindly all configuration files from '/etc/pam.d/',
	#       instead install them by the package using actually the configuration!
	#       Here we only install the basic configuration files "manually"
	#       ony-by-one.
#	#@$(call install_alternative_tree, linux-pam, 0, 0, /etc/pam.d)

	@$(call install_copy, pam, 0, 0, 755, /etc/pam.d)
	@$(call install_alternative, pam, 0, 0, 644, /etc/pam.d/common-auth)
	@$(call install_alternative, pam, 0, 0, 644, /etc/pam.d/common-account)
	@$(call install_alternative, pam, 0, 0, 644, /etc/pam.d/common-session)
	@$(call install_alternative, pam, 0, 0, 644, /etc/pam.d/common-session-noninteractive)
	@$(call install_alternative, pam, 0, 0, 644, /etc/pam.d/common-password)
	@$(call install_alternative, pam, 0, 0, 644, /etc/pam.d/other)

	#
	# configuration dir for Linux-PAM modules
	#
	@$(call install_copy, pam, 0, 0, 0755, /etc/security)

	#
	# install modules and file(s) (i.e. scripts and helper binaries):
	#
ifdef PTXCONF_PAM_PAM_ACCESS
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_access.so)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/security/access.conf)
endif

ifdef PTXCONF_PAM_PAM_ENV
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_env.so)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/security/pam_env.conf)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/environment)
endif

ifdef PTXCONF_PAM_PAM_FILTER
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_filter.so)
	@$(call install_alternative_tree, pam, 0, 0, /usr/lib/security/pam_filter)
endif

ifdef PTXCONF_PAM_PAM_GROUP
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_group.so)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/security/group.conf)
endif
ifdef PTXCONF_PAM_PAM_MKHOMEDIR
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_mkhomedir.so)
	@$(call install_copy, pam, 0, 0, 0755, -, /sbin/mkhomedir_helper)
endif
ifdef PTXCONF_PAM_PAM_LIMITS
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_limits.so)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/security/limits.conf)
	@$(call install_copy, pam, 0, 0, 0755, /etc/security/limits.conf.d)
endif
ifdef PTXCONF_PAM_PAM_NAMESPACE
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_namespace.so)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/security/namespace.conf)
	@$(call install_alternative, pam, 0, 0, 0755, /etc/security/namespace.init)
endif
ifdef PTXCONF_PAM_PAM_TALLY
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_tally.so)
	@$(call install_copy, pam, 0, 0, 0755, -, /sbin/pam_tally)
endif
ifdef PTXCONF_PAM_PAM_TALLY2
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_tally2.so)
	@$(call install_copy, pam, 0, 0, 0755, -, /sbin/pam_tally2)
endif
ifdef PTXCONF_PAM_PAM_TIME
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_time.so)
	@$(call install_alternative, pam, 0, 0, 0644, /etc/security/time.conf)
endif

ifdef PTXCONF_PAM_PAM_TIMESTAMP
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_timestamp.so)
	@$(call install_copy, pam, 0, 0, 0755, -, /sbin/pam_timestamp_check)
endif

ifdef PTXCONF_PAM_PAM_UNIX
	@$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/pam_unix.so)
	@$(call install_copy, pam, 0, 42, 0750, -, /sbin/unix_chkpwd)
	@$(call install_copy, pam, 0, 42, 4750, -, /sbin/unix_update)
endif

	#
	# all other modules without additional files to *.so
	#
	@$(foreach mod, $(PAM_MODULES-y), \
		$(call install_copy, pam, 0, 0, 0755, -, /usr/lib/security/$(mod));)

	@$(call install_finish, pam)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/pam.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, PAM)

# vim: syntax=make
