# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
#
# Copyright (C) 2012 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CANOPEN) += canopen

#
# Paths and names
#
CANOPEN_VERSION	:= 0.2
CANOPEN			:= canopen
CANOPEN_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/device/can/
CANOPEN_DIR		:= $(BUILDDIR)/$(CANOPEN)

CANOPEN_PACKAGE_NAME := $(CANOPEN)_$(CANOPEN_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CANOPEN_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/canopen.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/canopen.extract:
	@$(call targetinfo, $@)
	@$(call clean, $(CANOPEN_DIR))
	@mkdir -p $(CANOPEN_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cp -rd $(PTXDIST_WORKSPACE)/wago_intern/device/can/* $(CANOPEN_DIR)
endif
	@$(call touch, $@)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CANOPEN_PATH	:= PATH=$(CROSS_PATH)
CANOPEN_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/canopen.prepare:
	@$(call targetinfo, $@)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call xslt_compile, $(CANOPEN_DIR)/xml/canopen.xml)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/canopen.compile:
	@$(call targetinfo, $@)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CANOPEN_DIR)/lib_CANopen && \
		$(CANOPEN_ENV) $(CANOPEN_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@cd $(CANOPEN_DIR)/lib_CANopen/master && \
		$(CANOPEN_ENV) $(CANOPEN_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@cd $(CANOPEN_DIR)/lib_CANopen/slave && \
		$(CANOPEN_ENV) $(CANOPEN_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@cd $(CANOPEN_DIR)/lib_CANopen/layer2 && \
		$(CANOPEN_ENV) $(CANOPEN_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/canopen.install:
	@$(call targetinfo, $@)	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanopen.*
	cp $(CANOPEN_DIR)/lib_CANopen/libcanopen.so.0.2 $(PTXCONF_SYSROOT_TARGET)/usr/lib
	cp $(CANOPEN_DIR)/lib_CANopen/master/libcanopenm.so $(PTXCONF_SYSROOT_TARGET)/usr/lib
	cp $(CANOPEN_DIR)/lib_CANopen/slave/libcanopens.so $(PTXCONF_SYSROOT_TARGET)/usr/lib
	cp $(CANOPEN_DIR)/lib_CANopen/layer2/libcanlayer2.so $(PTXCONF_SYSROOT_TARGET)/usr/lib
	ln $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanopen.so.0.2 $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanopen.so
	cp $(CANOPEN_DIR)/lib_CANopen/canopen_types.h  $(PTXCONF_SYSROOT_TARGET)/usr/include/dal
	cp $(CANOPEN_DIR)/lib_CANopen/libcanopen.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dal/libcanopen.h 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/$(CANOPEN_PACKAGE_NAME).tgz \
		usr/lib/libcanopen.so.0.2 \
		usr/lib/libcanopenm.so \
		usr/lib/libcanopens.so \
		usr/lib/libcanlayer2.so \
		usr/lib/libcanopen.so \
		usr/include/dal/canopen_types.h \
		usr/include/dal/libcanopen.h 
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/$(CANOPEN_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/canopen.targetinstall:
	@$(call targetinfo, $@)
	@$(call install_init, canopen)
	
	@$(call install_fixup,canopen,PRIORITY,optional)
	@$(call install_fixup,canopen,VERSION,$(CANOPEN_VERSION))	
	@$(call install_fixup,canopen,SECTION,base)
	@$(call install_fixup,canopen,AUTHOR,"Axel Wueste")
	@$(call install_fixup,canopen,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
    # Extract precompiled binaries from archive
	rm -rf $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/$(CANOPEN_PACKAGE_NAME).ipk  
	@$(call install_archive, canopen, 0, 0, $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
    # WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, canopen, 0, 0, 0644, $(CANOPEN_DIR)/lib_CANopen/libcanopen.so.0.2,     /usr/lib/libcanopen.so.0.2)
	@$(call install_copy, canopen, 0, 0, 0644, $(CANOPEN_DIR)/lib_CANopen/master/libcanopenm.so,  /usr/lib/libcanopenm.so)
	@$(call install_copy, canopen, 0, 0, 0644, $(CANOPEN_DIR)/lib_CANopen/slave/libcanopens.so,  /usr/lib/libcanopens.so)
	@$(call install_copy, canopen, 0, 0, 0644, $(CANOPEN_DIR)/lib_CANopen/layer2/libcanlayer2.so,/usr/lib/libcanlayer2.so)
	@$(call install_link, canopen, libcanopen.so.0.2 , /usr/lib/libcanopen.so)
	@$(call install_link, canopen, ../libcanopenm.so , /usr/lib/dal/libcanopenm.so)
	@$(call install_link, canopen, ../libcanopens.so , /usr/lib/dal/libcanopens.so)
	@$(call install_link, canopen, ../libcanlayer2.so, /usr/lib/dal/libcanlayer2.so)
endif
	@$(call install_finish,canopen)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CANOPEN_PACKAGE_NAME).ipk $(CANOPEN_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

canopen_clean:
	rm -rf $(STATEDIR)/canopen.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanopen.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/dal/libcanopenm.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/dal/libcanopens.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/dal/libcanlayer2.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanopenm.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanopens.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcanlayer2.*
	rm -rf $(PKGDIR)/canopen*
	rm -rf $(CANOPEN_DIR)
	@$(call xslt_clean, $(CANOPEN_DIR)/xml/canopen.xml)
	

