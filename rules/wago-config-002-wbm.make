## -*-makefile-*-
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
PACKAGES-$(PTXCONF_WBM) += wbm

#
# Paths and names
#
WBM_VERSION 	:= 1.0.1
WBM_DIR		:= $(BUILDDIR)/wbm-$(WBM_VERSION)
WBM_SOURCE_DIR = $(call ptx/get_alternative, projectroot, var/www/wbm)
WBM_XML_CONFIG_FILE_TEMPLATE := frontend_config_wbm_template.xml

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(WBM_SOURCE):
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#
# Dummy extract target - nothing to extract here
# Just create the build dir (i.e. to store the config file - see Prepare)
#

$(STATEDIR)/wbm.extract:
	@$(call targetinfo)
	@mkdir -p "$(WBM_DIR)"
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WBM_MAKE_XML_CONFIG_ENV = PTXDIST_DEP_TARGET="all" \
		                   xmlconf=$(PTXCONF_SYSROOT_HOST)/bin/make-xml-config/make-xml-config

# use host-python if some other package needs it.
# fall back to the changeroot version otherwise.
ifdef PTXCONF_HOST_PYTHON
$(STATEDIR)/wbm.prepare: $(STATEDIR)/host-python.install.post
WBM_MAKE_XML_CONFIG_ENV += PYTHONHOME="$(HOST_PYTHON_PKGDIR)" 
endif

$(STATEDIR)/wbm.prepare: $(STATEDIR)/host-ct-make-xml-config.install.post 
	@$(call targetinfo)

	# generate xml configuration file for wbm using a patched libptxdist.sh
	# in $(SYSROOT_HOST) which is part of host-ct-make-xml-config:
	source $(PTXDIST_SYSROOT_HOST)/bin/make-xml-config/libptxdist+xml.sh && \
		    $(WBM_MAKE_XML_CONFIG_ENV) \
            ptxd_kconfig CT_MENU_WBM_ $(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE)

#	# patch generated *.xml to adjust user levels
	test -e $(PTXDIST_PLATFORMCONFIGDIR)/products/user_level.json && \
		$(PTXCONF_SYSROOT_HOST)/bin/make-xml-config/set_user_level.py \
			$(PTXDIST_PLATFORMCONFIGDIR)/products/user_level.json \
			$(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE) \
			1>$(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE).tmp && \
		mv $(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE).tmp \
			$(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE) || true

	# check output for conformity with XSD definition.
	@$(PTXDIST_SYSROOT_HOST)/bin/xmllint --noout --schema \
		$(SRCDIR)/ct-make-xml-config/xsd/menu.xsd \
		$(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE) || exit 1

ifdef PTXCONF_CT_MULTI_PRODUCT_CONFIG
	# now generate an xml file for each hw variant (continue on errors: products folder is optional)
	test -e $(PTXDIST_PLATFORMCONFIGDIR)/products && \
		for file in $(PTXDIST_PLATFORMCONFIGDIR)/products/*.xsl ; do \
			$(PTXDIST_SYSROOT_HOST)/bin/xsltproc $$file $(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE) \
			> $(WBM_DIR)/$$(basename $$file .xsl)_wbm.xml; \
		done
endif # PTXCONF_CT_MULTI_PRODUCT_CONFIG

	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm.compile:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm.install: $(wbm_install_deps_default)
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm)
	@$(call install_fixup,wbm,PRIORITY,optional)
	@$(call install_fixup,wbm,SECTION,base)
	@$(call install_fixup,wbm,AUTHOR,"Wago Kontakttechnik")
	@$(call install_fixup,wbm,DESCRIPTION,missing)

ifdef PTXCONF_WBM
	# create a directory owned by user and group "www"
	@$(call install_copy, wbm, 12, 102, 0750, /var/downloads/update-script)

	# copy all wbm related files
	@$(call install_alternative, wbm, 0, 0, 0644, /var/www/index.html)

	# instead of using copy_alternative* macro, copy files with shell
	# copy_alternative* macro does not support setting permissions
	# for files and folders
	@cd $(WBM_SOURCE_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm, 0, 0, 0644, $(WBM_SOURCE_DIR)/$$object, /var/www/wbm/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm, 0, 0, 0755, /var/www/wbm/$$object); \
		fi; \
	done;

	# create a link to the LED configuration file
	@$(call install_link, wbm, /etc/specific/led.xml, /var/www/wbm/led.xml)

# XML configuration
# If products folder exists, copy the product-specific configurations to the target
# otherwise use the maximal confiuration (i.e. the template)
ifdef PTXCONF_CT_MULTI_PRODUCT_CONFIG
	@$(call install_glob, wbm, 0, 0, $(WBM_DIR), /var/www/wbm/xml, *_wbm.xml)
else
	@$(call install_copy, wbm, 0, 0, 0644, $(WBM_DIR)/$(WBM_XML_CONFIG_FILE_TEMPLATE), /var/www/wbm/frontend_config_wbm.xml, n);
endif # PTXCONF_CT_MULTI_PRODUCT_CONFIG
endif # PTXCONF_WBM

	@$(call install_finish, wbm)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/wbm.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, wbm)

# vim: syntax=make
