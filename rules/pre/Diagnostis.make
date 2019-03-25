# -*-makefile-*-
#
# This file contains global macro and environment definitions.
#

#
# xslt_compile
#
# Compile the given xml file
#
xslt_compile =               \
  $(HOST_WAGO_PFC_DIAGNOSTIC_DIAG_DIR)/xslt_make.sh compile $(1) $(HOST_WAGO_PFC_DIAGNOSTIC_DIR) $(SYSROOT) $(PTXDIST_WORKSPACE)


#
# xslt_clean
#
# Compile the given xml file
#
xslt_clean =               \
  $(HOST_WAGO_PFC_DIAGNOSTIC_DIAG_DIR)/xslt_make.sh clean $(1) $(HOST_WAGO_PFC_DIAGNOSTIC_DIR) $(SYSROOT) $(PTXDIST_WORKSPACE)
  
#
# xslt_word
#
# this file creates a word document from diagnostic.xml
#
xslt_word =               \
  $(PTXDIST_WORKSPACE)/rules/pre/scripts/xslt.sh $(PTXDIST_WORKSPACE)/rules/pre/scripts/ $(PTXDIST_SYSROOT_TARGET)/etc/specific/ $(PTXDIST_PLATFORMDIR)/