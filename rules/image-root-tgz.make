# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_ROOT_TGZ) += image-root-tgz

#
# Paths and names
#
IMAGE_ROOT_TGZ		:= image-root-tgz
IMAGE_ROOT_TGZ_DIR	:= $(BUILDDIR)/$(IMAGE_ROOT_TGZ)
IMAGE_ROOT_TGZ_IMAGE	:= $(IMAGEDIR)/root.tgz

IMAGE_ROOT_TGZ_PKGS    = $(PTX_PACKAGES_INSTALL)
ifdef PTXCONF_WAGO_BUILD_COLLECTION
ifneq ($(call remove_quotes, $(PTXCONF_WAGO_BUILD_COLLECTION)),)
IMAGE_ROOT_TGZ_PKGS	= $(call ptx/collection, $(PTXDIST_WORKSPACE)/configs/$(PTXCONF_PLATFORM)/$(call remove_quotes, $(PTXCONF_WAGO_BUILD_COLLECTION)))
endif
endif

IMAGE_ROOT_TGZ_LABEL	:= $(call remove_quotes, $(PTXCONF_IMAGE_ROOT_TGZ_LABEL))

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

ifdef PTXCONF_IMAGE_ROOT_TGZ
$(IMAGE_ROOT_TGZ_IMAGE):
	@$(call targetinfo)
	@$(call image/archive, IMAGE_ROOT_TGZ)
	@$(call finish)
endif

# vim: syntax=make
