# -*-makefile-*-
#
# Copyright (C) 2017 by Enrico Joerns <e.joerns@pengutronix.de>
# Copyright (C) 2016 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_RAUC) += image-rauc

#
# Paths and names
#
IMAGE_RAUC		:= image-rauc
IMAGE_RAUC_DIR		:= $(BUILDDIR)/$(IMAGE_RAUC)
IMAGE_RAUC_IMAGE	:= $(IMAGEDIR)/update.raucb
IMAGE_RAUC_FILES	:= $(BUILDDIR)/rauc_bundle_conf/rauc_bundle_conf.tar.gz
IMAGE_RAUC_CONFIG	:= $(call remove_quotes, $(PTXCONF_IMAGE_RAUC_CONFIG))

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

IMAGE_RAUC_KEY = $(PTXDIST_WORKSPACE)/$(call remove_quotes,$(PTXCONF_RAUC_DEVELOPMENT_KEY))
IMAGE_RAUC_CERT = $(PTXDIST_WORKSPACE)/$(call remove_quotes,$(PTXCONF_RAUC_DEVELOPMENT_CERT))

# TEMPORARY WORKAROUND TODO: reimplement properly (s. WAT26585)
#
# By setting IMAGE_RAUC_FILES to rauc_bundle_conf.tar.gz ptxdist generates a
# make dependency from this file for update.raucb.  This archive file is
# provided by a target package rauc_bundle_conf. In image-rauc.in a
# ptxdist-style dependency "IMAGE_RAUC selects RAUC_BUNDLE_CONF" is set.
#
# During parallel build, this dependency is not evaluated because make aborts
# earlier when no target to build rauc_bunlde_conf.tar.gz is found.
#
# Defining an empty target makes GNU make happy and it begins making the
# target, triggering the ptxdist dependency.
#
# This workaround shall be removed after the image creation logic is fixed.

$(BUILDDIR)/rauc_bundle_conf/rauc_bundle_conf.tar.gz:

IMAGE_RAUC_ENV	:= \
	RAUC_BUNDLE_COMPATIBLE="$(call remove_quotes,$(PTXCONF_RAUC_COMPATIBLE))" \
	RAUC_BUNDLE_VERSION=$(PTXDIST_BSP_AUTOVERSION) \
	RAUC_BUNDLE_BUILD=$(shell date +%FT%T%z) \
	RAUC_BUNDLE_DESCRIPTION=$(PTXCONF_IMAGE_RAUC_DESCRIPTION) \
	RAUC_KEY=$(IMAGE_RAUC_KEY) \
	RAUC_CERT=$(IMAGE_RAUC_CERT)

$(IMAGE_RAUC_IMAGE): $(IMAGE_RAUC_KEY) $(IMAGE_RAUC_CERT)
	@$(call targetinfo)
	@$(call image/genimage, IMAGE_RAUC)
	@$(call finish)

$(IMAGE_RAUC_KEY):
	@echo
	@echo "****************************************************************************"
	@echo "******** Please configure your signing key path in                  ********"
	@echo "******** PTXCONF_RAUC_DEVELOPMENT_KEY                               ********"
	@echo "*                                                                          *"
	@echo "* Note: For test-purpose you can create one by running rauc-gen-certs.sh   *"
	@echo "*       from the scripts/ folder of your PTXdist installation              *"
	@echo "****************************************************************************"
	@echo
	@echo
	@exit 1

$(IMAGE_RAUC_CERT):
	@echo
	@echo "****************************************************************************"
	@echo "**** Please configure your signing certificate path in                  ****"
	@echo "**** PTXCONF_RAUC_DEVELOPMENT_CERT                                      ****"
	@echo "*                                                                          *"
	@echo "* Note: For test-purpose you can create one by running rauc-gen-certs.sh   *"
	@echo "*       from the scripts/ folder of your PTXdist installation              *"
	@echo "****************************************************************************"
	@echo
	@echo
	@exit 1

# vim: syntax=make
