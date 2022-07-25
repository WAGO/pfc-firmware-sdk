# -*-makefile-*-
# todo: header
#
#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_PRODUCTION_ARTIFACTS_ZIP) += image-production-artifacts-zip
# note: name has to start with "image-" otherwise PTXdist will ignore it

#
# Paths and names
#
IMAGE_PRODUCTION_ARTIFACTS_MODEM_FW_MD5 := 293e9d0aa27fccd875d7d5b119805702
IMAGE_PRODUCTION_ARTIFACTS_ZIP		:= image-production-artifacts-zip
IMAGE_PRODUCTION_ARTIFACTS_ZIP_PATH	:= $(IMAGEDIR)/$(IMAGE_PRODUCTION_ARTIFACTS_ZIP)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

IMAGE_PRODUCTION_ARTIFACTS_HEX_RAW	:= firmware.tar
SVNREV = $(call assert,$(shell ptxd_get_svn_revision | sed 's/svn/r/g'),"Error: cannot determine SVN revision. Aborting!")
FWVER = $(call assert,$(shell cat "$(PTXDIST_WORKSPACE)/projectroot/etc/REVISIONS" | sed 's/FIRMWARE=/V/g' | sed 's/\.//g' | sed 's/\x28/_/g' | sed 's/\x29//g'), "Error: cannot determine FW version. Aborting!")

ifdef PTXCONF_PFC_200_G2
	NAME_PTXCONF := PFC-G2

	ARTIFACTORY_BASE_URL			:= $(call remove_quotes, ${PTXCONF_ARTIFACTORY_BASE_URL})
	PROD_ARTIFACTS_MODEM_FW_IMAGE_DE := ${ARTIFACTORY_BASE_URL}/modem-firmware-generic-prod/quectel/EC25E/EC25EFAR06A06M4G_01.006.01.006.mfw
	PROD_ARTIFACTS_MODEM_FW_IMAGE_GLOBAL := ${ARTIFACTORY_BASE_URL}/modem-firmware-generic-prod/quectel/EG25G/EG25GGBR07A08M2G_01.002.01.002.mfw
	
	IMAGE_PRODUCTION_ARTIFACTS_EMMC_HDIMG	:= emmc-wago-production-pfc200v3.hdimg	

	PROD_ARTIFACTS_MODEM_FW_MD5_FILE	:= ${IMAGE_PRODUCTION_ARTIFACTS_ZIP_PATH}/$(PROD_ARTIFACTS_MODEM_FW_IMAGE).md5
	PROD_ARTIFACTS_MODEM_FW_MD5	= $(shell [ -f "$(PROD_ARTIFACTS_MODEM_FW_MD5_FILE)" ] && cat "$(PROD_ARTIFACTS_MODEM_FW_MD5_FILE)")
else
	NAME_PTXCONF := "PFC-XXX"
#	IMAGE_PRODUCTION_ARTIFACTS_ZIP_FILES	+= $(IMAGEDIR)/nand-wago-PROD-pfc200_V$(TODO).hdimg
endif

IMAGE_PRODUCTION_ARTIFACTS_ZIP_ARCHIVE = $(IMAGEDIR)/production-artifacts_$(NAME_PTXCONF)_$(FWVER)_$(SVNREV).zip
IMAGE_PRODUCTION_ARTIFACTS_HEX_NAME = firmware_$(FWVER)_$(SVNREV)_development.hex
IMAGE_PRODUCTION_ARTIFACTS_ZIP_IMAGE = production-artifacts_$(NAME_PTXCONF)_*.zip

$(IMAGE_PRODUCTION_ARTIFACTS_ZIP_IMAGE):
	@$(call targetinfo)
	
ifdef PTXCONF_PFC_200_G2	
	# prepare: copy all necessary production artifacts to images/production-artifacts-zip
	@mkdir -p $(IMAGEDIR)/$(IMAGE_PRODUCTION_ARTIFACTS_ZIP)
	${PTXDIST_WORKSPACE}/scripts/production-helpers/download_modem_fw.sh \
		'${PTXDIST_WORKSPACE}' \
		'$(IMAGE_PRODUCTION_ARTIFACTS_ZIP_PATH)' \
		'$(PROD_ARTIFACTS_MODEM_FW_IMAGE_DE)' \
		'$(PROD_ARTIFACTS_MODEM_FW_IMAGE_GLOBAL)'
	cp $(IMAGEDIR)/$(IMAGE_PRODUCTION_ARTIFACTS_HEX_RAW) $(IMAGE_PRODUCTION_ARTIFACTS_ZIP_PATH)/$(IMAGE_PRODUCTION_ARTIFACTS_HEX_NAME)
	cp $(IMAGEDIR)/$(IMAGE_PRODUCTION_ARTIFACTS_EMMC_HDIMG) $(IMAGE_PRODUCTION_ARTIFACTS_ZIP_PATH)/emmc-wago-production-pfc200v3_$(FWVER)_$(SVNREV)_development.img
	
	# zip artifacts
	cd $(IMAGE_PRODUCTION_ARTIFACTS_ZIP_PATH) && zip -r $(IMAGE_PRODUCTION_ARTIFACTS_ZIP_ARCHIVE) *
	@rm -rf $(IMAGEDIR)/$(IMAGE_PRODUCTION_ARTIFACTS_ZIP)
else
	# TODO
endif

	@$(call finish)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/image-production-artifacts-zip.clean:
	@$(call targetinfo)
	@$(call clean_pkg, IMAGE_PRODUCTION_ARTIFACTS_ZIP)
	@rm -rf $(IMAGEDIR)/$(IMAGE_PRODUCTION_ARTIFACTS_ZIP)
	@rm -rf $(IMAGEDIR)/production-artifacts_*.zip

endif # PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

# vim: syntax=make
