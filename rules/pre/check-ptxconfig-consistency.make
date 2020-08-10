#$(STATEDIR)/base.get: | $(STATEDIR)/consistency-check
#
#IN_FILES := $(wildcard $(PTXDIST_WORKSPACE)/rules/*.in $(PTXDIST_PLATFORMCONFIGDIR)/rules/*.in)
#
#$(STATEDIR)/consistency-check: $(PTXDIST_WORKSPACE)/selected_ptxconfig $(STATEDIR)/host-ct-make-xml-config.install $(IN_FILES)
#	@echo "*******************************************************"; \
#	echo "*                                                     *"; \
#	echo "* Checking ptxconfig consistency: not yet implemented *"; \
#	echo "*                                                     *"; \
#	echo "*******************************************************"; \
#    touch $@

#$(STATEDIR)/consistency-check: $(PTXDIST_WORKSPACE)/selected_ptxconfig $(STATEDIR)/host-ct-make-xml-config.install $(IN_FILES)
#	@echo "**********************************"; \
#	echo "*                                *"; \
#	echo "* Checking ptxconfig consistency *"; \
#	echo "*                                *"; \
#	echo "**********************************"; \
#	source $(HOST_CT_MAKE_XML_CONFIG_DIR)/libptxdist+xml.sh && \
#      xmlconf=$(HOST_CT_MAKE_XML_CONFIG_DIR)/make-xml-config && \
#      PTXDIST_DEP_TARGET="all" ptxd_kconfig CT_MENU_CBM_ /dev/null && \
#    touch $@
