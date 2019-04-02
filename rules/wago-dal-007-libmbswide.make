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
#	Provided Package:  libmbswide
#---------------------------------------------------------------------------------------------------

PACKAGES-$(PTXCONF_LIBMBSWIDE) += libmbswide


#---------------------------------------------------------------------------------------------------
#	Paths & Symbols
#---------------------------------------------------------------------------------------------------

#---  rsync commonly used otions  ----------------------------------------------
RSYNC_OPTIONS 				:= -a --exclude=".*" --exclude="*~" --exclude="*.out" \
                               --exclude=test_output/ --exclude=output/

#--- libmbswide package paths  -------------------------------------------------
LIBMBSWIDE_VERSION_MAJOR	:= 0
LIBMBSWIDE_VERSION_MINOR	:= 1
LIBMBSWIDE_VERSION_BUGFIX	:= 1
LIBMBSWIDE_VERSION			:= $(LIBMBSWIDE_VERSION_MAJOR).$(LIBMBSWIDE_VERSION_MINOR).$(LIBMBSWIDE_VERSION_BUGFIX)
LIBMBSWIDE					:= libmbswide
LIBMBSWIDE_DIR				:= $(BUILDDIR)/$(LIBMBSWIDE)
LIBMBSWIDE_LIBDIR			:= $(LIBMBSWIDE_DIR)/libs
LIBMBSWIDE_SRCDIR			:= $(PTXDIST_WORKSPACE)/wago_intern/device/modbus/slave

#---  local paths within the package  ------------------------------------------
LOCAL_LIBMBSWIDE_DIR		:= $(LIBMBSWIDE_DIR)/$(LIBMBSWIDE)
LOCAL_LIBMBSWIDE_PROJDIR	:= $(LOCAL_LIBMBSWIDE_DIR)/project

#$(error $(PTXCONF_LIBSMB_SOURCE))

LIBMBSWIDE_PACKAGE_NAME := $(LIBMBSWIDE)_$(LIBMBSWIDE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#---------------------------------------------------------------------------------------------------
#   Get				Get common information
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbswide.get:
		@$(call targetinfo)
		@echo "ptx build dir:         $(BUILDDIR)"
		@echo "ptx source dir:        $(SRCDIR)"
		@echo "libmbswide source dir: $(LIBMBSWIDE_SRCDIR)"
		@echo
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Extract			Create libmbswide directory in the <build-target> directory and synchronize it
#					with local source directories
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbswide.extract:
		@$(call targetinfo)
		@echo "Create build-target dir:  $(LIBMBSWIDE_DIR) ..."
		$(NOPRINT)mkdir -p $(LIBMBSWIDE_DIR)
ifndef	PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@echo "Copy local project dirs into build-target dir..."
		$(NOPRINT)rsync  $(RSYNC_OPTIONS)  $(LIBMBSWIDE_SRCDIR)/build       $(LIBMBSWIDE_DIR)
		$(NOPRINT)rsync  $(RSYNC_OPTIONS)  $(LIBMBSWIDE_SRCDIR)/libmbswide  $(LIBMBSWIDE_DIR)
endif
		@echo
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Prepare			Create the directories <libs> and <test> in the proper <build-target> directory.
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbswide.prepare:
		@$(call targetinfo)
		@echo "Create additional dirs in the build-target dir..."
		$(NOPRINT)mkdir -p $(LIBMBSWIDE_DIR)/libs
		@echo
		@$(call touch)


#---------------------------------------------------------------------------------------------------
#   Compile			Build all targets and copy all them into the <libs> directory.
#---------------------------------------------------------------------------------------------------

# ----  LIBMBSWIDE_PATH	:= PATH=$(CROSS_PATH)
LIBMBSWIDE_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/libmbswide.compile:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@echo Build libmbswide...
		@echo
        ##  $(PARALLELMFLAGS) not used!
		$(NOPRINT)cd $(LOCAL_LIBMBSWIDE_PROJDIR) &&  $(LIBMBSWIDE_ENV)  DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		             CROSS_COMPILE=$(COMPILER_PREFIX)  $(MAKE) $(PARALLELMFLAGS)
		@echo "Copy created libraries into dir:  $(LIBMBSWIDE_LIBDIR)..."
		$(NOPRINT)cp -d $(LOCAL_LIBMBSWIDE_PROJDIR)/output/$(LIBMBSWIDE).*  $(LIBMBSWIDE_LIBDIR)
endif
		@echo
		@$(call touch)
		@echo


#---------------------------------------------------------------------------------------------------
#   Install			Copy built targets and mbs API headers into proper directories in the 
#					sysroot-target. Creat a symbolic link onto libmbswide versioned shared lib file.
#					 
#---------------------------------------------------------------------------------------------------

$(STATEDIR)/libmbswide.install:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ##  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		$(NOPRINT)cp $(LIBMBSWIDE_LIBDIR)/libmbswide.a  $(PTXCONF_SYSROOT_TARGET)/usr/lib/
		$(NOPRINT)cp $(LIBMBSWIDE_LIBDIR)/libmbswide.so.$(LIBMBSWIDE_VERSION)  $(PTXCONF_SYSROOT_TARGET)/usr/lib/
		$(NOPRINT)ln -sf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libmbswide.so.$(LIBMBSWIDE_VERSION) \
		                 $(PTXCONF_SYSROOT_TARGET)/usr/lib/libmbswide.so
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		##  Backup header files as archive for later use in configs/<platform>/includes
		cd $(PTXCONF_SYSROOT_TARGET) && \
		tar -czvf $(LIBMBSWIDE_PACKAGE_NAME).tgz \
		    usr/lib/libmbswide.a \
		    usr/lib/libmbswide.so \
		    usr/lib/libmbswide.so.$(LIBMBSWIDE_VERSION) && \
		    mv $(LIBMBSWIDE_PACKAGE_NAME).tgz $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        ## Install header files from archive
		tar -xzvf $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)/$(LIBMBSWIDE_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
		@echo
		@$(call touch)
		@echo


#-------------------------------------------------------------------------------
#   Target-Install	Install package on target. Copy the libmbswide lib into <root>
#                   and <root-debug> directories of the target platform and
#					create corresponding links.
#-------------------------------------------------------------------------------

$(STATEDIR)/libmbswide.targetinstall:
		@$(call targetinfo)	
		@$(call install_init, libmbswide)
		@$(call install_fixup,  libmbswide, PRIORITY, optional)
		@$(call install_fixup,  libmbswide, VERSION, $(LIBMBSWIDE_VERSION))
		@$(call install_fixup,  libmbswide, SECTION, base)
		@$(call install_fixup,  libmbswide, AUTHOR, "Wago")
		@$(call install_fixup,  libmbswide, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
		#---  Extract precompiled binaries from archive
		rm -rf $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
		cd $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp && \
		ar -xov $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)/$(LIBMBSWIDE_PACKAGE_NAME).ipk  
		@$(call install_archive, libmbswide, 0, 0, $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
		#---  WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@$(call install_copy,   libmbswide, 0, 0, 0755, $(LIBMBSWIDE_LIBDIR)/$(LIBMBSWIDE).so.$(LIBMBSWIDE_VERSION), /usr/lib/$(LIBMBSWIDE).so.$(LIBMBSWIDE_VERSION))
		@$(call install_link,   libmbswide, $(LIBMBSWIDE).so.$(LIBMBSWIDE_VERSION), /usr/lib/$(LIBMBSWIDE).so)
		@$(call install_link,   libmbswide, $(LIBMBSWIDE).so.$(LIBMBSWIDE_VERSION), /usr/lib/$(LIBMBSWIDE).so.$(LIBMBSWIDE_VERSION_MAJOR))		
		@$(call install_link,   libmbswide, ../$(LIBMBSWIDE).so.$(LIBMBSWIDE_VERSION), /usr/lib/wide/$(LIBMBSWIDE).so)		
endif
		@$(call install_finish, libmbswide)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		#---  Backup binaries in configs/@platform@/packages/
		cp $(PKGDIR)/$(LIBMBSWIDE_PACKAGE_NAME).ipk $(LIBMBSWIDE_PLATFORMCONFIGPACKAGEDIR)/
endif
		@echo
		@$(call touch)
		@echo

#-------------------------------------------------------------------------------
# 	Clean			Uninstall package libmbswide and remove all created and copied
#					shared objects and link to them.
#-------------------------------------------------------------------------------

$(STATEDIR)/libmbswide.clean:
		@echo
		@echo ------------------------
		@echo target: clean libmbswide
		@echo ------------------------
		@echo
		@echo "statedir:  $(STATEDIR)"
		@echo "pkgdir:    $(PKGDIR)"
		@echo
		rm -rf $(STATEDIR)/libmbswide*
		rm -rf $(PKGDIR)/libmbswide*
		rm -rf $(LIBMBSWIDE_DIR)
		rm -f  $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBMBSWIDE).*
		rm -f  $(ROOTDIR)/usr/lib/$(LIBMBSWIDE).*
		rm -f  $(ROOTDIR)/usr/lib/wide/$(LIBMBSWIDE).*
		rm -f  $(ROOTDIR)/../root-debug/usr/lib/$(LIBMBSWIDE).*
		rm -f  $(ROOTDIR)/../root-debug/usr/lib/wide/$(LIBMBSWIDE).*
		@echo
		@echo finished target clean libmbswide
		@echo

#--- End of makefile -----------------------------------------------------------
