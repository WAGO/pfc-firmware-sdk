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

CBM_MAKE_XML_CONFIG_ENV = PTXDIST_DEP_TARGET="all" \
	                      xmlconf=$(PTXCONF_SYSROOT_HOST)/bin/make-xml-config/make-xml-config


CBM_PATH	:= PATH=$(CROSS_PATH)
CBM_ENV 	:= $(CROSS_ENV)

# use host-python if some other package needs it.
# fall back to the changeroot version otherwise.
ifdef PTXCONF_HOST_PYTHON
$(STATEDIR)/cbm.prepare: $(STATEDIR)/host-python.install.post
CBM_MAKE_XML_CONFIG_ENV += PYTHONHOME="$(HOST_PYTHON_PKGDIR)" 
endif

$(STATEDIR)/cbm.prepare: $(STATEDIR)/host-ct-make-xml-config.install.post 
	@$(call targetinfo)

# generate xml configuration file for cbm using a patched libptxdist.sh
# in $(SYSROOT_HOST) which is part of host-ct-make-xml-config:
	
	@source $(PTXDIST_SYSROOT_HOST)/bin/make-xml-config/libptxdist+xml.sh && \
    $(CBM_MAKE_XML_CONFIG_ENV) \
    ptxd_kconfig CT_MENU_CBM_ $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE) 
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
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.install: $(cbm_install_deps_default)
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cbm)
	@$(call install_fixup,cbm,PRIORITY,optional)
	@$(call install_fixup,cbm,SECTION,base)
	@$(call install_fixup,cbm,AUTHOR,"WAGO")
	@$(call install_fixup,cbm,DESCRIPTION,missing)

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
		$(call install_alternative, cbm, 0, 0, 0640, /etc/config-tools/cbm-script-modules/$$script); \
  done && \
  state_submodules=$$(xmlstarlet sel -t -m "//simple_level" -v "@dyn_state_ressource" -n  $(CBM_DIR)/$(CBM_XML_CONFIG_FILE_TEMPLATE)) && \
  for script in $${state_submodules}; do \
		$(call install_alternative, cbm, 0, 0, 0640, /etc/config-tools/cbm-script-modules/$$script); \
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
