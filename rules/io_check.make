# -*-makefile-*-
#
# Copyright (C) 2012 by Hans Florian Scholz <hans-florian.scholz@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_IO_CHECK) += io_check

#
# Paths and names
#
ifdef PTXCONF_IO_CHECK_TRUNK
IO_CHECK_VERSION	:= trunk
else
IO_CHECK_VERSION	:= 1.3
endif
IO_CHECK		:= io_check
IO_CHECK_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/$(IO_CHECK)
IO_CHECK_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/$(IO_CHECK)
IO_CHECK_DIR		:= $(BUILDDIR)/$(IO_CHECK)
IO_CHECK_BUILD_OOT	:= NO
IO_CHECK_LICENSE	:= unknown

IO_CHECK_PACKAGE_NAME := io-check_$(IO_CHECK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
IO_CHECK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/io_check.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(IO_CHECK_DIR))
	@mkdir -p $(IO_CHECK_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IO_CHECK_SRC) $(BUILDDIR)
	@$(call patchin, IO_CHECK)
	cd $(IO_CHECK_DIR) && sh autogen.sh
endif
	@$(call touch)

$(STATEDIR)/io_check.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
IO_CHECK_CONF_TOOL	:= autoconf
IO_CHECK_CONF_OPT	:= $(CROSS_AUTOCONF_USR)
ifndef PTXCONF_IO_CHECK_NET
	IO_CHECK_CONF_OPT	+=  --disable-ethernet
endif
ifndef PTXCONF_IO_CHECK_RS232
	IO_CHECK_CONF_OPT	+=  --disable-rs232
endif
ifdef PTXCONF_IO_CHECK_DEBUGGING
	IO_CHECK_CONF_OPT += --enable-debug 
endif

#Tables
ifndef PTXCONF_IO_CHECK_TABLE_0
	IO_CHECK_CONF_OPT	+=  --disable-table0
endif
ifndef PTXCONF_IO_CHECK_TABLE_8
	IO_CHECK_CONF_OPT	+=  --disable-table8
endif
ifndef PTXCONF_IO_CHECK_TABLE_9
	IO_CHECK_CONF_OPT	+=  --disable-table9
endif
ifndef PTXCONF_IO_CHECK_TABLE_90
	IO_CHECK_CONF_OPT	+=  --disable-table90
endif
ifndef PTXCONF_IO_CHECK_TABLE_92
	IO_CHECK_CONF_OPT	+=  --disable-table92
endif
ifndef PTXCONF_IO_CHECK_TABLE_93
	IO_CHECK_CONF_OPT	+=  --disable-table93
endif
ifndef PTXCONF_IO_CHECK_TABLE_94
	IO_CHECK_CONF_OPT	+=  --disable-table94
endif
ifndef PTXCONF_IO_CHECK_TABLE_95
	IO_CHECK_CONF_OPT	+=  --disable-table95
endif
ifndef PTXCONF_IO_CHECK_TABLE_99
	IO_CHECK_CONF_OPT	+=  --disable-table99
endif
ifndef PTXCONF_IO_CHECK_TABLE_100
	IO_CHECK_CONF_OPT	+=  --disable-table100
endif
ifndef PTXCONF_IO_CHECK_TABLE_110
	IO_CHECK_CONF_OPT	+=  --disable-table110
endif
ifndef PTXCONF_IO_CHECK_TABLE_111
	IO_CHECK_CONF_OPT	+=  --disable-table111
endif
ifndef PTXCONF_IO_CHECK_TABLE_112
	IO_CHECK_CONF_OPT	+=  --disable-table112
endif
ifndef PTXCONF_IO_CHECK_TABLE_113
	IO_CHECK_CONF_OPT	+=  --disable-table113
endif
ifndef PTXCONF_IO_CHECK_TABLE_114
	IO_CHECK_CONF_OPT	+=  --disable-table114
endif
ifndef PTXCONF_IO_CHECK_TABLE_115
	IO_CHECK_CONF_OPT	+=  --disable-table115
endif
ifndef PTXCONF_IO_CHECK_TABLE_116
	IO_CHECK_CONF_OPT	+=  --disable-table116
endif
ifndef PTXCONF_IO_CHECK_TABLE_117
	IO_CHECK_CONF_OPT	+=  --disable-table117
endif
ifndef PTXCONF_IO_CHECK_TABLE_124
	IO_CHECK_CONF_OPT	+=  --disable-table124
endif
ifndef PTXCONF_IO_CHECK_TABLE_126
	IO_CHECK_CONF_OPT	+=  --disable-table126
endif
ifndef PTXCONF_IO_CHECK_TABLE_127
	IO_CHECK_CONF_OPT	+=  --disable-table127
endif
ifndef PTXCONF_IO_CHECK_TABLE_TERM
	IO_CHECK_CONF_OPT	+=  --disable-termaccess
endif
ifndef PTXCONF_IO_CHECK_PROT_0001
        IO_CHECK_CONF_OPT       +=  --disable-prot0001
endif
ifndef PTXCONF_IO_CHECK_PROT_1279
        IO_CHECK_CONF_OPT       +=  --disable-prot1279
endif
ifndef PTXCONF_IO_CHECK_PROT_1288
        IO_CHECK_CONF_OPT       +=  --disable-prot1288
endif
ifndef PTXCONF_IO_CHECK_PROT_2442
	IO_CHECK_CONF_OPT       +=  --disable-prot2442
endif
ifneq ($(PTXCONF_TYPELABEL_EEPROM_PATH),)
	IO_CHECK_CONF_OPT += --with-eeprom-path=$(PTXCONF_TYPELABEL_EEPROM_PATH)
endif
ifdef PTXCONF_TYPELABEL_EEPROM_HAS_WP
	IO_CHECK_CONF_OPT += --enable-eeprom-wp=yes
else
	IO_CHECK_CONF_OPT += --enable-eeprom-wp=no
endif
	IO_CHECK_CONF_OPT	+=  --with-description=$(PTXCONF_IO_CHECK_DESCRIPTION)
endif

$(STATEDIR)/io_check.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, IO_CHECK)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/io_check.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IO_CHECK_SRC) $(BUILDDIR)
ifdef PTXCONF_IO_CHECK_PROT_1288
	@$(call xslt_compile, $(IO_CHECK_DIR)/src/xml/iocheck.xml)	
endif	
	@$(call world/compile, IO_CHECK)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/io_check.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, IO_CHECK)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(IO_CHECK)/ && \
	tar -czvf $(IO_CHECK).tgz * && \
	mv $(IO_CHECK).tgz $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(IO_CHECK)
	tar -xzvf $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/$(IO_CHECK).tgz -C $(PKGDIR)/$(IO_CHECK)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/io_check.targetinstall:
	@$(call targetinfo)
	@$(call install_init, io_check)
	@$(call install_fixup, io_check, PRIORITY, optional)
	@$(call install_fixup, io_check, SECTION, base)
	@$(call install_fixup, io_check, AUTHOR, "Hans Florian Scholz <hans-florian.scholz@wago.com>")
	@$(call install_fixup, io_check, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/$(IO_CHECK_PACKAGE_NAME).ipk  
	@$(call install_archive, io_check, 0, 0, $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

	@$(call install_alternative, io_check, 0, 0, 0644, /etc/specific/iocheck.conf)
	@$(call install_alternative, io_check, 0, 0, 0750, /etc/specific/iocheck_factory_test.sh)
	@$(call install_alternative, io_check, 0, 0, 0755, /usr/bin/iocheckd)
endif
ifdef PTXCONF_IO_CHECK_STARTSCRIPT
	@$(call install_alternative, io_check, 0, 0, 0644, /etc/inetd.conf.d/iocheck)
	@$(call install_replace, io_check, \
							/etc/inetd.conf.d/iocheck, \
							@ETHERNET@, \
							$(PTXCONF_IO_CHECK_NET_STARTLINE) )
	@$(call install_replace, io_check, \
							/etc/inetd.conf.d/iocheck, \
							@SERIAL@, \
							$(PTXCONF_IO_CHECK_RS232_STARTLINE) )

endif
	@$(call install_finish, io_check)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(IO_CHECK_PACKAGE_NAME).ipk $(IO_CHECK_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/io_check.clean:
ifdef PTXCONF_LED_SERVER
	@$(call xslt_clean, $(IO_CHECK_DIR)/src/xml/iocheck.xml)
endif
	@$(call targetinfo)
	#rm -f $(IO_CHECK_DIR)/{configure,config.sub,config.guess}
	@$(call clean_pkg, IO_CHECK)
	@-rm -rf $(IO_CHECK_DIR)

# vim: syntax=make
