# -*-makefile-*-
#
# Copyright (C) 2018 by <oleg.karfich@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_BLSPEC_PFC) += blspec-pfc

BLSPEC_PFC_VERSION		:= 0.0.1
BLSPEC_PFC_KERNEL_VERSION	:= $(call remove_quotes,$(PTXCONF_KERNEL_VERSION))
BLSPEC_PFC_ROOTARG_SD_ROOTFS	:= $(call remove_quotes,$(PTXCONF_BLSPEC_PFC_SD_ROOTARG_ROOTFS))
BLSPEC_PFC_ROOTARG_EMMC_ROOTFS0	:= $(call remove_quotes,$(PTXCONF_BLSPEC_PFC_EMMC_ROOTARG_ROOTFS0))
BLSPEC_PFC_ROOTARG_EMMC_ROOTFS1	:= $(call remove_quotes,$(PTXCONF_BLSPEC_PFC_EMMC_ROOTARG_ROOTFS1))
BLSPEC_PFC_ROOTARG_NAND_ROOTFS0	:= $(call remove_quotes,$(PTXCONF_BLSPEC_PFC_NAND_ROOTARG_ROOTFS0))
BLSPEC_PFC_ROOTARG_NAND_ROOTFS1	:= $(call remove_quotes,$(PTXCONF_BLSPEC_PFC_NAND_ROOTARG_ROOTFS1))
BLSPEC_PFC_TARGET		:= $(call remove_quotes,$(PTXCONF_BLSPEC_PFC_TARGET_PREFIX))

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/blspec-pfc.targetinstall: $(STATEDIR)/kernel.targetinstall.post
	@$(call targetinfo)

	@$(call install_init,  blspec-pfc)
	@$(call install_fixup, blspec-pfc, PRIORITY, optional)
	@$(call install_fixup, blspec-pfc, SECTION, base)
	@$(call install_fixup, blspec-pfc, AUTHOR, "<oleg.karfich@wago.com>")
	@$(call install_fixup, blspec-pfc, DESCRIPTION, missing)

ifdef PTXCONF_BLSPEC_PFC_SD

# SD-Entrys
	@$(foreach file, $(shell find $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/boot/sd/loader/entries/$(BLSPEC_PFC_TARGET)* -type f), \
		@$(call install_alternative, blspec-pfc, 0, 0, 0755, /boot/sd/loader/entries/$(notdir $(file))) \
		@$(call install_replace, blspec-pfc, /boot/sd/loader/entries/$(notdir $(file)), @KERNEL_VERSION@, "$(BLSPEC_PFC_KERNEL_VERSION)") \
		@$(call install_replace, blspec-pfc, /boot/sd/loader/entries/$(notdir $(file)), @ROOTARG@, "$(BLSPEC_PFC_ROOTARG_SD_ROOTFS)"))

endif

ifdef PTXCONF_BLSPEC_PFC_EMMC

# EMMC-Entrys
	@$(foreach file, $(shell find $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/boot/emmc0/loader/entries/$(BLSPEC_PFC_TARGET)* -type f), \
		@$(call install_alternative, blspec-pfc, 0, 0, 0755, /boot/emmc0/loader/entries/$(notdir $(file))) \
		@$(call install_replace, blspec-pfc, /boot/emmc0/loader/entries/$(notdir $(file)), @KERNEL_VERSION@, "$(BLSPEC_PFC_KERNEL_VERSION)") \
		@$(call install_replace, blspec-pfc, /boot/emmc0/loader/entries/$(notdir $(file)), @ROOTARG@, "$(BLSPEC_PFC_ROOTARG_EMMC_ROOTFS0)"))

	@$(foreach file, $(shell find $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/boot/emmc1/loader/entries/$(BLSPEC_PFC_TARGET)* -type f), \
		@$(call install_alternative, blspec-pfc, 0, 0, 0755, /boot/emmc1/loader/entries/$(notdir $(file))) \
		@$(call install_replace, blspec-pfc, /boot/emmc1/loader/entries/$(notdir $(file)), @KERNEL_VERSION@, "$(BLSPEC_PFC_KERNEL_VERSION)") \
		@$(call install_replace, blspec-pfc, /boot/emmc1/loader/entries/$(notdir $(file)), @ROOTARG@, "$(BLSPEC_PFC_ROOTARG_EMMC_ROOTFS1)"))

endif


ifdef PTXCONF_BLSPEC_PFC_NAND

# NAND-Entrys
	@$(foreach file, $(shell find $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/boot/nand0/loader/entries/$(BLSPEC_PFC_TARGET)* -type f), \
		@$(call install_alternative, blspec-pfc, 0, 0, 0755, /boot/nand0/loader/entries/$(notdir $(file))) \
		@$(call install_replace, blspec-pfc, /boot/nand0/loader/entries/$(notdir $(file)), @KERNEL_VERSION@, "$(BLSPEC_PFC_KERNEL_VERSION)") \
		@$(call install_replace, blspec-pfc, /boot/nand0/loader/entries/$(notdir $(file)), @ROOTARG@, "$(BLSPEC_PFC_ROOTARG_NAND_ROOTFS0)"))

	@$(foreach file, $(shell find $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/boot/nand1/loader/entries/$(BLSPEC_PFC_TARGET)* -type f), \
		@$(call install_alternative, blspec-pfc, 0, 0, 0755, /boot/nand1/loader/entries/$(notdir $(file))) \
		@$(call install_replace, blspec-pfc, /boot/nand1/loader/entries/$(notdir $(file)), @KERNEL_VERSION@, "$(BLSPEC_PFC_KERNEL_VERSION)") \
		@$(call install_replace, blspec-pfc, /boot/nand1/loader/entries/$(notdir $(file)), @ROOTARG@, "$(BLSPEC_PFC_ROOTARG_NAND_ROOTFS1)"))

endif

	@$(call install_finish,blspec-pfc)

	@$(call touch)

# vim: syntax=make
