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
# Note:	Do not use the PTXDIST_WORKSPACE variable because of possible occurrence 
#		of circular dependencies.
#


NOPRINT := @

#---------------------------------------------------------------------------------------------------
#	Provided Package:  libmbs
#---------------------------------------------------------------------------------------------------

PACKAGES-$(PTXCONF_LIBMBS) += libmbs


#---------------------------------------------------------------------------------------------------
#	Paths & Symbols
#---------------------------------------------------------------------------------------------------

#---  rsync commonly used otions  ----------------------------------------------
RSYNC_OPTIONS			:= -a --exclude=".*" --exclude="*~" --exclude="*.out" \
                           --exclude=test_output/ --exclude=output/

#---  libmbs package paths  ----------------------------------------------------
LIBMBS_VERSION_MAJOR	:= 0
LIBMBS_VERSION_MINOR	:= 5
LIBMBS_VERSION_BUGFIX	:= 1
LIBMBS_VERSION			:= $(LIBMBS_VERSION_MAJOR).$(LIBMBS_VERSION_MINOR).$(LIBMBS_VERSION_BUGFIX)
LIBMBS 					:= libmbs
LIBMBS_DIR				:= $(BUILDDIR)/$(LIBMBS)
LIBMBS_LIBDIR			:= $(LIBMBS_DIR)/libs
LIBMBS_CONFDIR			:= $(LIBMBS_DIR)/config
LIBMBS_SRC  			:= $(PTXDIST_WORKSPACE)/wago_intern/device/modbus/slave

LIBMBS_PACKAGE_NAME := $(LIBMBS)_$(LIBMBS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBMBS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#---  local paths within the package  ------------------------------------------
LOCAL_LIBMBS_DIR		:= $(LIBMBS_DIR)/$(LIBMBS)
LOCAL_LIBMBS_PROJDIR	:= $(LOCAL_LIBMBS_DIR)/project
LOCAL_LIBMBS_CONFDIR	:= $(LOCAL_LIBMBS_DIR)/config
TESTAPP_VERSION			:= 0.1
TESTAPP					:= testapp
LOCAL_TESTAPP_DIR		:= $(LIBMBS_DIR)/$(TESTAPP)
LOCAL_TESTAPP_PROJDIR	:= $(LOCAL_TESTAPP_DIR)/project

#---------------------------------------------------------------------------------------------------
#   Get				Get common information
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbs.get:
		@$(call targetinfo)
		@echo "ptx_builddir:    $(BUILDDIR)"
		@echo "ptx_srcdir:      $(SRCDIR)"
		@echo "libmbs_source:   $(LIBMBS_SRC)"
		@echo "libmbs:          $(LIBMBS)"
		@echo "libmbs_dir:      $(LIBMBS_DIR)"
		@echo
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Extract			Create libmbs directory in the <build-target> directory and synchronize it with
#					local source directories
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbs.extract:
		@$(call targetinfo)
		@echo "Create build-target dir:  $(LIBMBS_DIR) ..."
		$(NOPRINT)mkdir -p $(LIBMBS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@echo "Copy local project dirs into build-target dir..."
		$(NOPRINT)rsync  $(RSYNC_OPTIONS)  $(LIBMBS_SRC)/build    $(LIBMBS_DIR)
		$(NOPRINT)rsync  $(RSYNC_OPTIONS)  $(LIBMBS_SRC)/libmbs   $(LIBMBS_DIR)
		$(NOPRINT)rsync  $(RSYNC_OPTIONS)  $(LIBMBS_SRC)/testapp  $(LIBMBS_DIR)		
endif
		@echo
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Prepare			Create the directories <libs> and <test> in the proper <build-target> directory
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbs.prepare:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
		@echo "Create additional dirs in the build-target dir..."
		$(NOPRINT)mkdir -p $(LIBMBS_DIR)/libs
		$(NOPRINT)mkdir -p $(LIBMBS_DIR)/test
		$(NOPRINT)mkdir -p $(LIBMBS_DIR)/config
		@echo "Copy configuration file(s) into dir: $(LIBMBS_CONFDIR)..."
		$(NOPRINT)cp -d $(LOCAL_LIBMBS_CONFDIR)/*.conf  $(LIBMBS_CONFDIR)
		$(NOPRINT)cp -d $(LOCAL_LIBMBS_CONFDIR)/*.xml	$(LIBMBS_CONFDIR)		
		@echo "Create diagnostic header file for libmbs in sysroot-target/usr/include/diagnsostic directory by using of XSLT-script"
		$(NOPRINT)$(call xslt_compile, $(LIBMBS_DIR)/config/mbs_diag.xml)
		$(NOPRINT)@echo
endif
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Compile			Build all targets and copy all them into the <libs> directory.
#---------------------------------------------------------------------------------------------------

#-------LIBMBS_PATH	:= PATH=$(CROSS_PATH)
LIBMBS_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/libmbs.compile:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@echo "Build libmbs an the test application <test_app>..."
        ##  $(PARALLELMFLAGS) not used!
		$(NOPRINT)cd $(LOCAL_LIBMBS_PROJDIR)  &&  $(LIBMBS_ENV) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
                     CROSS_COMPILE=$(COMPILER_PREFIX)  $(MAKE)
		$(NOPRINT)cd $(LOCAL_TESTAPP_PROJDIR) &&  $(LIBMBS_ENV) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
                     CROSS_COMPILE=$(COMPILER_PREFIX)  $(MAKE) $(PARALLELMFLAGS)
		@echo
		@echo "Copy created libraries into dir:     $(LIBMBS_LIBDIR)..."
		$(NOPRINT)cp -d $(LOCAL_LIBMBS_PROJDIR)/output/$(LIBMBS).*  $(LIBMBS_LIBDIR)
		@echo
endif
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Install			Copy built targets and mbs API headers into proper directories in the 
#					sysroot-target. Creat a symbolic link onto libmbs versioned shared lib file.
#					The logging mask config file must be copied into the /etc/specific directory
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbs.install:
		@$(call targetinfo)
		$(NOPRINT)mkdir -p $(PTXCONF_SYSROOT_TARGET)/etc/specific/
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		$(NOPRINT)cp -d  $(LIBMBS_LIBDIR)/$(LIBMBS).*          $(PTXCONF_SYSROOT_TARGET)/usr/lib
		$(NOPRINT)cp -d  $(LOCAL_LIBMBS_DIR)/src/mbs_API.h     $(PTXCONF_SYSROOT_TARGET)/usr/include
		$(NOPRINT)cp -d  $(LOCAL_LIBMBS_DIR)/src/mbs_config.h  $(PTXCONF_SYSROOT_TARGET)/usr/include
		$(NOPRINT)cp -d  $(LOCAL_LIBMBS_DIR)/src/mbs_error.h   $(PTXCONF_SYSROOT_TARGET)/usr/include
		$(NOPRINT)cp -d  $(LOCAL_LIBMBS_DIR)/src/mbs_log.h     $(PTXCONF_SYSROOT_TARGET)/usr/include
		$(NOPRINT)ln -sf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBMBS).so.$(LIBMBS_VERSION) \
                         $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBMBS).so
		$(NOPRINT)cp -d  $(LIBMBS_CONFDIR)/mbslog.conf         $(PTXCONF_SYSROOT_TARGET)/etc/specific/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
        ##  Backup header files as archive for later use in configs/@platform@/packages
		cd $(PTXCONF_SYSROOT_TARGET) && \
		tar -czvf $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/$(LIBMBS_PACKAGE_NAME).tgz \
		    usr/lib/$(LIBMBS).* \
		    usr/include/mbs_API.h \
		    usr/include/mbs_config.h \
		    usr/include/mbs_error.h \
		    usr/include/mbs_log.h \
		    etc/specific/mbslog.conf
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  Install header from archive
		tar -xzvf $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/$(LIBMBS_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Target-Install	Install package on target. Copy the libmbs lib into <root> and <root-debug>
#                   directories of the target platform and create corresponding links.
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbs.targetinstall:
		@$(call targetinfo)
		@$(call install_init,   libmbs)
		@$(call install_fixup,  libmbs, VERSION, $(LIBMBS_VERSION))
		@$(call install_fixup,  libmbs, PRIORITY, optional)
		@$(call install_fixup,  libmbs, SECTION, base)
		@$(call install_fixup,  libmbs, AUTHOR, "Wago")
		@$(call install_fixup,  libmbs, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
        ##  Extract precompiled binaries from archive
		rm -rf $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
		cd $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
		ar -xov $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/$(LIBMBS_PACKAGE_NAME).ipk  
		@$(call install_archive, libmbs, 0, 0, $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@$(call install_copy,   libmbs, 0, 0, 0755, $(LIBMBS_LIBDIR)/$(LIBMBS).so.$(LIBMBS_VERSION), /usr/lib/$(LIBMBS).so.$(LIBMBS_VERSION))
		@$(call install_link,   libmbs, $(LIBMBS).so.$(LIBMBS_VERSION), /usr/lib/$(LIBMBS).so)
		@$(call install_link,   libmbs, $(LIBMBS).so.$(LIBMBS_VERSION), /usr/lib/$(LIBMBS).so.$(LIBMBS_VERSION_MAJOR))
		@$(call install_link,   libmbs, ../$(LIBMBS).so.$(LIBMBS_VERSION), /usr/lib/dal/$(LIBMBS).so)
		@$(call install_copy,   libmbs, 0, 0, 0640, $(LIBMBS_CONFDIR)/mbslog.conf, /etc/specific/mbslog.conf)
endif
		@$(call install_finish, libmbs)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
        ##  Backup binaries in configs/@platform@/packages/
		cp $(PKGDIR)/$(LIBMBS_PACKAGE_NAME).ipk $(LIBMBS_PLATFORMCONFIGPACKAGEDIR)/
endif
		@$(call touch)

#---------------------------------------------------------------------------------------------------
# 	Clean			Uninstall package libmbs and remove all created and copied shared objects and
#					link to them.
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbs.clean:
		@echo
		@echo --------------------
		@echo target: clean libmbs
		@echo --------------------
		@echo
		@echo "statedir:  $(STATEDIR)"
		@echo "pkgdir:    $(PKGDIR)"
		@echo
		$(NOPRINT)$(call xslt_clean, $(LIBMBS_DIR)/config/mbs_diag.xml)
		@echo
		rm -rf $(STATEDIR)/libmbs*
		rm -rf $(PKGDIR)/libmbs*
		rm -rf $(LIBMBS_DIR)
		rm -f  $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBMBS).*
		rm -f  $(ROOTDIR)/usr/lib/$(LIBMBS).*
		rm -f  $(ROOTDIR)/usr/lib/dal/$(LIBMBS).*
		rm -f  $(ROOTDIR)/../root-debug/usr/lib/$(LIBMBS).*
		rm -f  $(ROOTDIR)/../root-debug/usr/lib/dal/$(LIBMBS).*
		rm -f  $(PTXCONF_SYSROOT_TARGET)/usr/include/mbs_*.h
		
		@echo
		@echo finished target clean libmbs
		@echo

#--- End of makefile -------------------------------------------------------------------------------
