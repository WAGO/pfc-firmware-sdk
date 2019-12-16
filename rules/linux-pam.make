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
PACKAGES-$(PTXCONF_LINUX_PAM) += linux-pam

#
# Paths and names
#
LINUX_PAM_VERSION       := 1.3.1
LINUX_PAM_MD5           := 558ff53b0fc0563ca97f79e911822165
LINUX_PAM               := Linux-PAM-$(LINUX_PAM_VERSION)
LINUX_PAM_SUFFIX        := tar.xz
LINUX_PAM_URL           := https://github.com/linux-pam/linux-pam/releases/download/v$(LINUX_PAM_VERSION)/$(LINUX_PAM).$(LINUX_PAM_SUFFIX)
LINUX_PAM_SOURCE        := $(SRCDIR)/$(LINUX_PAM).$(LINUX_PAM_SUFFIX)
LINUX_PAM_DIR           := $(BUILDDIR)/$(LINUX_PAM)
LINUX_PAM_LICENSE       := GPLv2

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-pam.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-pam.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-pam.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-pam.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-pam.install:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

#
# Linux-PAM modules WITHOUT any (default) configuration files
#
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_CRACKLIB)   += pam_cracklib.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_DEBUG)      += pam_debug.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_DENY)       += pam_deny.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_ECHO)       += pam_echo.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_EXEC)       += pam_exec.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_FAILDELAY)  += pam_faildelay.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_FTP)        += pam_ftp.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_ISSUE)      += pam_issue.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_KEYINIT)    += pam_keyinit.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_LASTLOG)    += pam_lastlog.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_LISTFILE)   += pam_listfile.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_LOCALUSER)  += pam_localuser.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_LOGINUID)   += pam_loginuid.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_MAIL)       += pam_mail.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_MOTD)       += pam_motd.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_NOLOGIN)    += pam_nologin.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_PERMIT)     += pam_permit.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_PWHISTORY)  += pam_pwhistory.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_RHOSTS)     += pam_rhosts.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_ROOTOK)     += pam_rootok.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_SECURETTY)  += pam_securetty.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_SHELLS)     += pam_shells.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_STRESS)     += pam_stress.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_SUCCEED_IF) += pam_succeed_if.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_UMASK)      += pam_umask.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_USERDB)     += pam_userdb.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_WARN)       += pam_warn.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_WHEEL)      += pam_wheel.so
LINUX_PAM_MODULES-$(PTXCONF_LINUX_PAM_PAM_XAUTH)      += pam_xauth.so

$(STATEDIR)/linux-pam.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  linux-pam)
	@$(call install_fixup, linux-pam, PRIORITY, optional)
	@$(call install_fixup, linux-pam, SECTION, base)
	@$(call install_fixup, linux-pam, AUTHOR, "WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, linux-pam, DESCRIPTION, missing)

	#
	# install basic Linux-PAM libraries
	#
	@$(call install_lib, linux-pam, 0, 0, 0644, libpam)
	@$(call install_lib, linux-pam, 0, 0, 0644, libpam_misc)
	@$(call install_lib, linux-pam, 0, 0, 0644, libpamc)

	#
	# Linux-PAM configuration files for services
	#
#	#@$(call install_alternative_tree, linux-pam, 0, 0, /etc/pam.d)
	@$(call install_copy, linux-pam, 0, 0, 755, /etc/pam.d)
	@$(call install_alternative, linux-pam, 0, 0, 644, /etc/pam.d/common-auth)
	@$(call install_alternative, linux-pam, 0, 0, 644, /etc/pam.d/common-account)
	@$(call install_alternative, linux-pam, 0, 0, 644, /etc/pam.d/common-session)
	@$(call install_alternative, linux-pam, 0, 0, 644, /etc/pam.d/common-session-noninteractive)
	@$(call install_alternative, linux-pam, 0, 0, 644, /etc/pam.d/common-password)
	@$(call install_alternative, linux-pam, 0, 0, 644, /etc/pam.d/other)

	#
	# configuration dir for Linux-PAM modules
	#
	@$(call install_copy, linux-pam, 0, 0, 0755, /etc/security)

# TODO: Check file permissions

	#
	# install modules and file(s) (i.e. scripts and helper binaries):
	#
ifdef PTXCONF_LINUX_PAM_PAM_ACCESS
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_access.so)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/security/access.conf)
endif

ifdef PTXCONF_LINUX_PAM_PAM_ENV
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_env.so)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/security/pam_env.conf)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/environment)
endif

ifdef PTXCONF_LINUX_PAM_PAM_FILTER
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_filter.so)
	@$(call install_alternative_tree, linux-pam, 0, 0, /usr/lib/security/pam_filter)
endif

ifdef PTXCONF_LINUX_PAM_PAM_GROUP
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_group.so)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/security/group.conf)
endif
ifdef PTXCONF_LINUX_PAM_PAM_MKHOMEDIR
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_mkhomedir.so)
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /sbin/mkhomedir_helper)
endif
ifdef PTXCONF_LINUX_PAM_PAM_LIMITS
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_limits.so)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/security/limits.conf)
	@$(call install_copy, linux-pam, 0, 0, 0755, /etc/security/limits.conf.d)
endif
ifdef PTXCONF_LINUX_PAM_PAM_NAMESPACE
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_namespace.so)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/security/namespace.conf)
	@$(call install_alternative, linux-pam, 0, 0, 0755, /etc/security/namespace.init)
endif
ifdef PTXCONF_LINUX_PAM_PAM_TALLY
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_tally.so)
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /sbin/pam_tally)
endif
ifdef PTXCONF_LINUX_PAM_PAM_TALLY2
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_tally2.so)
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /sbin/pam_tally2)
endif
ifdef PTXCONF_LINUX_PAM_PAM_TIME
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_time.so)
	@$(call install_alternative, linux-pam, 0, 0, 0644, /etc/security/time.conf)
endif

ifdef PTXCONF_LINUX_PAM_PAM_TIMESTAMP
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_timestamp.so)
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /sbin/pam_timestamp_check)
endif

ifdef PTXCONF_LINUX_PAM_PAM_UNIX
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/pam_unix.so)
	@$(call install_copy, linux-pam, 0, 0, 2755, -, /sbin/unix_chkpwd)
	@$(call install_copy, linux-pam, 0, 0, 0755, -, /sbin/unix_update)
endif

	#
	# all other modules without additional files to *.so
	#
	@$(foreach mod, $(LINUX_PAM_MODULES-y), \
		$(call install_copy, linux-pam, 0, 0, 0755, -, /usr/lib/security/$(mod));)

	@$(call install_finish, linux-pam)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-pam.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LINUX_PAM)

# vim: syntax=make
