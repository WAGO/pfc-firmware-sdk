#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPACKBUS) += libpackbus


# This is the package version and is automatically used by PTXDIST! It will result in a package named
# $(packagename)_$(LIBPACKBUS_VERSION)_arm.ipk. It is also written as meta-information into the .ipk itself. 
LIBPACKBUS_VERSION := 1.0.0


# Libtool versioning information - DO NOT CHANGE THESE VALUES CARELESSLY BY INCREMENTING SOMETHING YOU DON'T UNDERSTAND
# READ THE LIBTOOL DOCUMENTATION WHAT EACH VALUE MEANS BEFORE ANY CHANGES! THIS IS __NOT__ SOMETHING LIKE MAJOR:MINOR:BUGFIX
# This affects the name of the shared library within the package.
LIBPACKBUS_LT_VERSION	:= 1:1:0

# ----------------------------------------------------------------------------
# Paths and names
# ----------------------------------------------------------------------------
LIBPACKBUS				:= libpackbus
LIBPACKBUS_URL			:= file://$(PTXDIST_WORKSPACE)/wago_intern/device/kbus/$(LIBPACKBUS)
LIBPACKBUS_DIR			:= $(BUILDDIR)/$(LIBPACKBUS)
LIBPACKBUS_VERSION_ARG	:= SO_VERSION=$(LIBPACKBUS_LT_VERSION)

ifdef PTXCONF_LIBPACKBUS_TESTAPP
LIBPACKBUS_TESTAPP	:= --enable-test
else
LIBPACKBUS_TESTAPP	:= --disable-test
endif

ifdef PTXCONF_LIBPACKBUS_DEBUG_DD
LIBPACKBUS_CONF_DEBUG_DD := --enable-debug_dd
LIBPACKBUS_BUILD_PARAMS:= \
                          LIBPACKBUS_DEBUG_LEVEL=$(PTXCONF_LIBPACKBUS_BUILD_CFG_DEBUG_LEVEL)\
                          LIBPACKBUS_COMPILER_OPTIMIZATION=$(PTXCONF_LIBPACKBUS_BUILD_CFG_OPTIMIZATION)
else
LIBPACKBUS_CONF_DEBUG_DD := --disable-debug_dd
LIBPACKBUS_BUILD_PARAMS:= \
                          LIBPACKBUS_DEBUG_LEVEL=g\
                          LIBPACKBUS_COMPILER_OPTIMIZATION=O2
endif

LIBPACKBUS_PACKAGE_NAME := $(LIBPACKBUS)_$(LIBPACKBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/libpackbus.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBPACKBUS_DIR))
	@mkdir -p $(LIBPACKBUS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

# generate kbusconf.c from kbusconf.xml
	cd $(PTXDIST_WORKSPACE)/wago_intern/device/kbus/$(LIBPACKBUS)/src/Conf && ./make_kbus_conf_source.sh

	cp -r -d $(PTXDIST_WORKSPACE)/wago_intern/device/kbus/$(LIBPACKBUS)/* $(LIBPACKBUS_DIR)

	cd $(LIBPACKBUS_DIR) && ./autogen.sh
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBPACKBUS_PATH	:= PATH=$(CROSS_PATH)
LIBPACKBUS_ENV 	:= $(CROSS_ENV) $(LIBPACKBUS_VERSION_ARG)

#
# autoconf
#
LIBPACKBUS_AUTOCONF := $(CROSS_AUTOCONF_USR)

ifdef PTXCONF_LIBPACKBUS_UART_INTERFACE
LIBPACKBUS_AUTOCONF += --with-uart=$(PTXCONF_LIBPACKBUS_UART_INTERFACE)
endif

$(STATEDIR)/libpackbus.prepare:
	@$(call targetinfo)
	@$(call clean, $(LIBPACKBUS_DIR)/config.cache)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call xslt_compile, $(LIBPACKBUS_DIR)/src/xml/kbusdiag.xml)
	cd $(LIBPACKBUS_DIR) && \
		$(LIBPACKBUS_PATH) $(LIBPACKBUS_ENV) \
		./configure $(LIBPACKBUS_BUILD_PARAMS) $(LIBPACKBUS_AUTOCONF) $(LIBPACKBUS_TESTAPP) $(LIBPACKBUS_CONF_DEBUG_DD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/libpackbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, LIBPACKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/libpackbus.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, LIBPACKBUS)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(LIBPACKBUS)/ && \
	tar -czvf $(LIBPACKBUS).tgz * && \
	mv $(LIBPACKBUS).tgz $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(LIBPACKBUS)
	tar -xzvf $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/$(LIBPACKBUS).tgz -C $(PKGDIR)/$(LIBPACKBUS)
endif
	@$(call touch) 

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
$(STATEDIR)/libpackbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libpackbus)	
	@$(call install_fixup, libpackbus,PRIORITY,optional)
	@$(call install_fixup, libpackbus,SECTION,base)
	@$(call install_fixup, libpackbus,AUTHOR,"Lars Friedrich : WAGO GmbH \& Co. KG <www.WAGO.com>")
	@$(call install_fixup, libpackbus,DESCRIPTION, "DAL device library for the kbus terminal rail")
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
# Extract precompiled binaries from archive
	rm -rf $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/$(LIBPACKBUS_PACKAGE_NAME).ipk  
	@$(call install_archive, libpackbus, 0, 0, $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	
#--------------------- Copy and link library file
	@$(call install_lib, libpackbus, 0, 0, 0644, libpackbus)
	@$(call install_link, libpackbus, ../libpackbus.so, /usr/lib/dal/libpackbus.so)	
#--------------------- Copy slave firmware		
	@$(call install_copy, libpackbus, 0,0,  0644, $(LIBPACKBUS_DIR)/src/loader/XE164L1.bin, /etc/specific/XE164L1.bin)
	@$(call install_copy, libpackbus, 0,0,  0644, $(LIBPACKBUS_DIR)/src/loader/XE164L2.bin, /etc/specific/XE164L2.bin)
	@$(call install_copy, libpackbus, 0,0,  0644, $(LIBPACKBUS_DIR)/src/loader/XE164L3.bin, /etc/specific/XE164L3.bin)
#--------------------- Copy configuration file
	@$(call install_copy, libpackbus, 0,0,  0644, $(LIBPACKBUS_DIR)/src/Conf/kbusconf.xml, /etc/specific/kbusconf.xml)
#--------------------- Copy test application
ifdef PTXCONF_LIBPACKBUS_TESTAPP
	@$(call install_copy, libpackbus, 0,0, 0750, $(LIBPACKBUS_DIR)/src/testkbus, /usr/bin/testkbus);
endif
#--------------------- Done
endif
	@$(call install_finish, libpackbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBPACKBUS_PACKAGE_NAME).ipk $(LIBPACKBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
libpackbus_clean:
	@$(call xslt_clean, $(LIBPACKBUS_DIR)/src/xml/kbusdiag.xml)
	rm -rf $(STATEDIR)/libpackbus.*
	rm -rf $(PKGDIR)/libpackbus*
	rm -rf $(LIBPACKBUS_DIR)
