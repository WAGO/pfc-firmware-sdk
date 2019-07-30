# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by Stefanie Meihöfer
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# TODO: install modules according to xml configuration
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CBM) += cbm

#
# Paths and names
#
CBM_VERSION 	:= 2.01.00
CBM		:= cbm
CBM_URL		:= /$(CBM)
CBM_SOURCE	:= 
CBM_DIR		:= $(BUILDDIR)/$(CBM)

CBM_XML_CONFIG_FILE_TEMPLATE := "frontend_config_cbm_template.xml"

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#
# Dummy get target - nothing to fetch here
# $(STATEDIR)/cbm.get is rebuilt implicitely
#

$(CBM_SOURCE):
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#
# Dummy extract target - nothing to extract here
# Just create the build dir (i.e. to store the config file - see Prepare)
#

$(STATEDIR)/cbm.extract:
	@$(call targetinfo)
	@mkdir -p $(CBM_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CBM_PATH	:= PATH=$(CROSS_PATH)
CBM_ENV 	:= $(CROSS_ENV)

#
# The XML configuration's integrity considering the other configure options is
# of crucial importance: otherwise the configuration frontend would contain
# erroneous menu entries. That's why we have to recreate the config file each
# time the configuration has been changed and use the dependency from the
# main configuration here.
#

$(STATEDIR)/cbm.prepare: $(STATEDIR)/host-ct-make-xml-config.install.post $(PTXDIST_PLATFORMDIR)/selected_ptxconfig
	@$(call targetinfo)

# generate xml configuration file for cbm using a patched libptxdist.sh
# in $(SYSROOT_HOST) which is part of host-ct-make-xml-config:
	
	@source $(PTXDIST_SYSROOT_HOST)/bin/make-xml-config/libptxdist+xml.sh && \
  export PYTHONHOME=$(HOST_PYTHON_PKGDIR) && \
  xmlconf=$(PTXCONF_SYSROOT_HOST)/bin/make-xml-config/make-xml-config &&\
  PTXDIST_DEP_TARGET="all" ptxd_kconfig CT_MENU_CBM_ $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE) 
# check output for conformity with XSD definition.
	@$(PTXDIST_SYSROOT_HOST)/bin/xmllint --noout --schema \
	$(SRCDIR)/ct-make-xml-config/xsd/menu.xsd \
  $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE) || exit 1

ifdef PTXCONF_CT_MULTI_PRODUCT_CONFIG
# now generate an xml file for each hw variant (continue on errors: products folder is optional)
	@for file in $(PTXDIST_PLATFORMCONFIGDIR)/products/*.xsl ; do \
        if test -e $$file; then \
            $(PTXDIST_SYSROOT_HOST)/bin/xsltproc $$file $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE) \
            > $(CBM_DIR)/$$(basename $$file .xsl)_cbm.xml; \
        fi; \
    done
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.compile:
	@$(call targetinfo)

#	@if [ "$(PTXCONF_PLATFORM)" = "IO-IPC-Codesys-Tele" ] || [ "$(PTXCONF_PLATFORM)" = "IO-IPC-P-Codesys-Tele" ]; then \
#		cd $(CBM_DIR) && echo $$PWD && $(CBM_ENV) $(CBM_PATH) $(MAKE) CROSS_COMPILE=$(COMPILER_PREFIX) EXTRAS=TELECONTROL $(PARALLELMFLAGS); \
#	else \
#		cd $(CBM_DIR) && echo $$PWD && $(CBM_ENV) $(CBM_PATH) $(MAKE) CROSS_COMPILE=$(COMPILER_PREFIX) EXTRAS=NO $(PARALLELMFLAGS); \
#	fi;

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.install: $(cbm_install_deps_default)
	@$(call targetinfo)
# FIXME: rsc: this needs a proper SYSROOT description!
#
# TODO:	For files that are required at compiletime (headers, libs to link against)
#	you can copy these files to the sysroot directory.
#	Use macro $(PTXCONF_SYSROOT_TARGET) for host files and $(PTXCONF_GNU_TARGET)
#	for target files
#
#	Example for a host header file:
#		@cp friesel.h $(PTXCONF_SYSROOT_TARGET)/include
#	Example for a host library file:
#		@cp friesel.so $(PTXCONF_SYSROOT_TARGET)/lib
#	Example for a target file:
#		@cp frasel.h  $(PTXCONF_GNU_TARGET)/include
#	Example for a target library file:
#		@cp frasel.so $(PTXCONF_GNU_TARGET)/lib

# Ought to be done by config-tools
#	cp $(SRCDIR)/$(CBM)/get_rts_info.h $(PTXDIST_PLATFORMDIR)/sysroot-target/usr/include/
#	cp $(SRCDIR)/$(CBM)/ipc_msg_com.h $(PTXDIST_PLATFORMDIR)/sysroot-target/usr/include/

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cbm)
	@$(call install_fixup,cbm,PRIORITY,optional)
	@$(call install_fixup,cbm,SECTION,base)
	@$(call install_fixup,cbm,AUTHOR,"Wago Kontakttechnik")
	@$(call install_fixup,cbm,DESCRIPTION,missing)

	@$(call install_copy, cbm, 0, 0, 0755, /etc/config-tools);
	

ifdef PTXCONF_CT_WDIALOG
	@$(call install_copy, cbm, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/wdialog, /etc/config-tools/wdialog)
endif

ifdef PTXCONF_CBM
	@$(call install_copy, cbm, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/cbm, /etc/init.d/cbm, n)
	@$(call install_copy, cbm, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/usr/sbin/cbm, /usr/sbin/cbm) 

	# Main Script
	@$(call install_copy, cbm, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/cbm, /etc/config-tools/cbm, n) 
  
	# Additional Modules

	@$(call install_copy, cbm, 0, 0, 0750, /etc/config-tools/cbm-script-modules);

	@$(call install_copy, cbm, 0, 0, 0640, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/cbm-script-modules/port_interfaces.sh, /etc/config-tools/cbm-script-modules/port_interfaces.sh, n)


	# Only install script modules selected via XML configuration
	@script_modules=$$(xmlstarlet sel -t -m "//simple_level" -v "@ressource" -n  $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE)) && \
  for script in $${script_modules}; do \
		$(call install_copy, cbm, 0, 0, 0640, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/cbm-script-modules/$$script, \
    /etc/config-tools/cbm-script-modules/$$script, n); \
  done && \
  state_submodules=$$(xmlstarlet sel -t -m "//simple_level" -v "@dyn_state_ressource" -n  $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE)) && \
  for script in $${state_submodules}; do \
		$(call install_copy, cbm, 0, 0, 0640, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/cbm-script-modules/$$script, \
    /etc/config-tools/cbm-script-modules/$$script, n); \
  done


	# XML configuration
	# If products folder exists, copy the product-specific configurations to the target
  # otherwise use the maximal confiuration (i.e. the template)
ifdef PTXCONF_CT_MULTI_PRODUCT_CONFIG
	@cd $(CBM_DIR) && for file in *_cbm.xml; do \
    $(call install_copy, cbm, 0, 0, 0644, $(CBM_DIR)/$$file, /etc/config-tools/xml/$$file, n); \
  done
else
	@$(call install_copy, cbm, 0, 0, 0644, $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE), /etc/config-tools/frontend_config_cbm.xml, n)
endif

endif # PTXCONF_CBM

	@$(call install_finish,cbm)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

cbm_clean:
	rm -rf $(CBM_DIR)
	rm -rf $(STATEDIR)/cbm.*


# vim: syntax=make
