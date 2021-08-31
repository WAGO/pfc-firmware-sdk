# -*-makefile-*-
#
# Copyright (C) 2008 by Juergen Beisert
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CPUFREQUTILS) += cpufrequtils

#
# Paths and names
#
CPUFREQUTILS_VERSION	:= 008
CPUFREQUTILS_MD5	:= e0c9f333a9546f71d17fd5a0546db79e
CPUFREQUTILS		:= cpufrequtils-$(CPUFREQUTILS_VERSION)
CPUFREQUTILS_SUFFIX	:= tar.xz
CPUFREQUTILS_URL	:= $(call ptx/mirror, KERNEL, utils/kernel/cpufreq/$(CPUFREQUTILS).$(CPUFREQUTILS_SUFFIX))
CPUFREQUTILS_SOURCE	:= $(SRCDIR)/$(CPUFREQUTILS).$(CPUFREQUTILS_SUFFIX)
CPUFREQUTILS_DIR	:= $(BUILDDIR)/$(CPUFREQUTILS)
CPUFREQUTILS_LICENSE	:= GPL-2.0-only
CPUFREQUTILS_LICENSE_FILES := \
	file://COPYING;md5=94d55d512a9ba36caa9b7df079bae19f \
	file://utils/info.c;startline=1;endline=5;md5=a3504f2818a74b2fffdc8e61bf3ef0f8

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CPUFREQUTILS_MAKE_ENV	:= \
	$(CROSS_ENV) \
	NLS=false \
	V=$(if $(filter 1,$(PTXDIST_VERBOSE)),true,false) \
	STRIP=strip

CPUFREQUTILS_MAKE_OPT	:= \
	CROSS=$(COMPILER_PREFIX)

$(STATEDIR)/cpufrequtils.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cpufrequtils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cpufrequtils)
	@$(call install_fixup, cpufrequtils,PRIORITY,optional)
	@$(call install_fixup, cpufrequtils,SECTION,base)
	@$(call install_fixup, cpufrequtils,AUTHOR,"Juergen Beisert <j.beisert@pengutronix.de>")
	@$(call install_fixup, cpufrequtils,DESCRIPTION,missing)

	@$(call install_copy, cpufrequtils, 0, 0, 0644, -, \
		/usr/lib/libcpufreq.so.0.0.0)
	@$(call install_link, cpufrequtils, \
		libcpufreq.so.0.0.0, \
		/usr/lib/libcpufreq.so.0)
	@$(call install_link, cpufrequtils, \
		libcpufreq.so.0.0.0, \
		/usr/lib/libcpufreq.so)

ifdef PTXCONF_CPUFREQUTILS_FREQ_INFO
	@$(call install_copy, cpufrequtils, 0, 0, 0755, -, /usr/bin/cpufreq-info)
endif
ifdef PTXCONF_CPUFREQUTILS_FREQ_SET
	@$(call install_copy, cpufrequtils, 0, 0, 0755, -, /usr/bin/cpufreq-set)
endif

	@$(call install_finish, cpufrequtils)

	@$(call touch)

# vim: syntax=make
