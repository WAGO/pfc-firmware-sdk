# -*-makefile-*-
#
# Copyright (C) 2020 by Ladislav Michl <ladis@linux-mips.org>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SHADOW) += shadow

#
# Paths and names
#
SHADOW_VERSION	:= 4.8.1
SHADOW_MD5	:= 4b05eff8a427cf50e615bda324b5bc45
SHADOW		:= shadow-$(SHADOW_VERSION)
SHADOW_SUFFIX	:= tar.xz
SHADOW_URL	:= https://github.com/shadow-maint/shadow/releases/download/$(SHADOW_VERSION)/$(SHADOW).$(SHADOW_SUFFIX)
SHADOW_SOURCE	:= $(SRCDIR)/$(SHADOW).$(SHADOW_SUFFIX)
SHADOW_DIR	:= $(BUILDDIR)/$(SHADOW)
SHADOW_LICENSE	:= BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
SHADOW_CONF_TOOL	:= autoconf
SHADOW_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
	--bindir=/usr/bin \
	--sbindir=/usr/sbin \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-shadowgrp \
	--disable-man \
	--$(call ptx/endis, PTXCONF_SHADOW_PAM)-account-tools-setuid \
	--disable-utmpx \
	--enable-subordinate-ids \
	--disable-nls \
	--disable-rpath \
	--without-audit \
	--$(call ptx/wwo, PTXCONF_SHADOW_PAM)-libpam \
	--without-btrfs \
	--$(call ptx/wwo, PTXCONF_GLOBAL_SELINUX)-selinux \
	--without-acl \
  	--without-attr \
	--without-skey \
	--without-tcb \
	--without-libcrack \
	--with-sha-crypt \
	--without-nscd \
	--without-sssd \
	--with-su \
	--without-fcaps

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

SHADOW_BIN_PROGS-$(PTXCONF_SHADOW_FAILLOG) += faillog
SHADOW_BIN_PROGS-$(PTXCONF_SHADOW_GROUPS)  += groups
SHADOW_BIN_PROGS-$(PTXCONF_SHADOW_LASTLOG) += lastlog
SHADOW_BIN_PROGS-$(PTXCONF_SHADOW_LOGIN)   += login

SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_CHAGE)     += chage
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_CHFN)      += chfn
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_CHSH)      += chsh
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_EXPIRY)    += expiry
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_GPASSWD)   += gpasswd
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_NEWGIDMAP) += newgidmap
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_NEWGRP)    += newgrp
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_NEWUIDMAP) += newuidmap
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_PASSWD)    += passwd
SHADOW_SUID_BIN_PROGS-$(PTXCONF_SHADOW_SU)        += su

SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_CHGPASSWD) += chgpasswd
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_CHPASSWD)  += chpasswd
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GROUPADD)  += groupadd
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GROUPDEL)  += groupdel
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GROUPMEMS) += groupmems
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GROUPMOD)  += groupmod
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GRPCK)     += grpck
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GRPCONV)   += grpconv
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_GRPUNCONV) += grpunconv
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_NEWUSERS)  += newusers
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_NOLOGIN)   += nologin
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_PWCK)      += pwck
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_PWCONV)    += pwconv
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_PWUNCONV)  += pwunconv
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_USERADD)   += useradd
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_USERDEL)   += userdel
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_USERMOD)   += usermod
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_VIPW)      += vipw
SHADOW_SBIN_PROGS-$(PTXCONF_SHADOW_SULOGIN)   += sulogin

ifdef PTXCONF_SHADOW_PAM
SHADOW_PAMD-$(PTXCONF_SHADOW_CHAGE)   += chage
SHADOW_PAMD-$(PTXCONF_SHADOW_LOGIN)   += login
SHADOW_PAMD-$(PTXCONF_SHADOW_PASSWD)  += passwd
SHADOW_PAMD-$(PTXCONF_SHADOW_SU)      += su
SHADOW_PAMD-$(PTXCONF_SHADOW_USERMOD) += groupadd
SHADOW_PAMD-$(PTXCONF_SHADOW_USERMOD) += groupdel
SHADOW_PAMD-$(PTXCONF_SHADOW_USERMOD) += useradd
SHADOW_PAMD-$(PTXCONF_SHADOW_USERMOD) += userdel
SHADOW_PAMD-$(PTXCONF_SHADOW_USERMOD) += usermod
endif # PTXCONF_SHADOW_PAM

$(STATEDIR)/shadow.targetinstall:
	@$(call targetinfo)

	@$(call install_init, shadow)
	@$(call install_fixup, shadow,PRIORITY,optional)
	@$(call install_fixup, shadow,SECTION,base)
	@$(call install_fixup, shadow,AUTHOR,"Ladislav Michl <ladis@linux-mips.org>")
	@$(call install_fixup, shadow,DESCRIPTION,missing)

	#
	# install executables and links/aliases
	#
	@$(foreach prog, $(SHADOW_BIN_PROGS-y), \
		$(call install_copy, shadow, 0, 0, 0755, -, \
			/usr/bin/$(prog))$(ptx/nl))

	@$(foreach prog, $(SHADOW_SUID_BIN_PROGS-y), \
		$(call install_copy, shadow, 0, 0, 4755, -, \
			/usr/bin/$(prog))$(ptx/nl))

ifdef PTXCONF_SHADOW_NEWGRP
	@$(call install_link, shadow, newgrp, /usr/bin/sg)
endif # PTXCONF_SHADOW_NEWGRP

	@$(foreach prog, $(SHADOW_SBIN_PROGS-y), \
		$(call install_copy, shadow, 0, 0, 0755, -, \
			/usr/sbin/$(prog))$(ptx/nl))

ifdef PTXCONF_SHADOW_VIPW
	@$(call install_link, shadow, vipw, /usr/sbin/vigr)
endif # PTXCONF_SHADOW_VIPW

	#
	# install configuration files
	#
ifneq "$(or $(PTXCONF_SHADOW_CHFN),$(PTXCONF_SHADOW_CHGPASSWD),$(PTXCONF_SHADOW_GPASSWD),\
$(PTXCONF_SHADOW_GROUPADD),$(PTXCONF_SHADOW_GROUPDEL),$(PTXCONF_SHADOW_GROUPMEMS),\
$(PTXCONF_SHADOW_GROUPMOD),$(PTXCONF_SHADOW_GRPCK),$(PTXCONF_SHADOW_GRPCONV),\
$(PTXCONF_SHADOW_GRPUNCONV),$(PTXCONF_SHADOW_GRPUNCONV),$(PTXCONF_SHADOW_LASTLOG),\
$(PTXCONF_SHADOW_LOGIN),$(PTXCONF_SHADOW_NEWGRP),$(PTXCONF_SHADOW_NEWUSERS),\
$(PTXCONF_SHADOW_PWCK),$(PTXCONF_SHADOW_PWCONV),$(PTXCONF_SHADOW_SU),\
$(PTXCONF_SHADOW_USERADD),$(PTXCONF_SHADOW_USERDEL),$(PTXCONF_SHADOW_USERMOD))" ""
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/login.defs)
endif # ifneq

ifdef PTXCONF_SHADOW_NEWUIDMAP
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/subuid)
endif # PTXCONF_SHADOW_NEWUIDMAP

ifdef PTXCONF_SHADOW_NEWGIDMAP
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/subgid)
endif # PTXCONF_SHADOW_NEWGIDMAP

ifdef PTXCONF_SHADOW_GROUPADD
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/pam.d/groupadd)
endif # PTXCONF_SHADOW_GROUPADD

ifdef PTXCONF_SHADOW_GROUPDEL
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/pam.d/groupdel)
endif # PTXCONF_SHADOW_GROUPDEL

ifdef PTXCONF_SHADOW_USERADD
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/pam.d/useradd)
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/default/useradd)
endif # PTXCONF_SHADOW_USERADD

ifdef PTXCONF_SHADOW_USERDEL
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/pam.d/userdel)
endif # PTXCONF_SHADOW_USERDEL

ifdef PTXCONF_SHADOW_USERMOD
	@$(call install_alternative, shadow, 0, 0, 0644, /etc/pam.d/usermod)
endif # PTXCONF_SHADOW_USERMOD

	#
	# install Linux-PAM configuration files
	#
	@$(foreach cfg, $(SHADOW_PAMD-y), \
		$(call install_alternative, shadow, 0, 0, 0644, \
			/etc/pam.d/$(cfg))$(ptx/nl))

	# install license file
	@$(call install_copy, shadow, 0, 0, 0644, $(SHADOW_DIR)/COPYING, /usr/share/licenses/oss/license.shadow_$(SHADOW_VERSION).txt)

	@$(call install_finish, shadow)

	@$(call touch)

# vim: syntax=make
