# -*-makefile-*-
#
# Copyright (C) 2017 by WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FILE_TRANSFER) += file-transfer

FILE_TRANSFER_VERSION	:= 0.0.1

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/file-transfer.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/file-transfer.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/file-transfer.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/file-transfer.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/file-transfer.install:
	@$(call targetinfo)

#
# TODO:
# If some of the files are required in root filesystem's build process, install
# these files in the install stage. But use proper variables PTXdist supports
# to install files, instead of fixed paths. The following variables can be
# used:
#
# - $(PTXDIST_SYSROOT_TARGET) points to a directory tree
#   all target relevant libraries and header files are installed to. All
#   packages built for the target are searching in this directory tree for
#   header and library files. These files are for compile time only, not for
#   runtime!
#   Paths:
#    - executables: $(PTXDIST_SYSROOT_TARGET)/bin
#    - header files: $(PTXDIST_SYSROOT_TARGET)/include
#    - libraries: $(PTXDIST_SYSROOT_TARGET)/lib
#
# - $(PTXDIST_SYSROOT_HOST) points to a directory tree all host relevant
#   executables, libraries and header files are installed to. All packages
#   built for the host are searching in this directory tree for executables,
#   header and library files.
#   Paths:
#    - executables: $(PTXDIST_SYSROOT_HOST)/bin
#    - header files: $(PTXDIST_SYSROOT_HOST)/include
#    - libraries: $(PTXDIST_SYSROOT_HOST)/lib
#
# - $(PTXDIST_SYSROOT_CROSS) points to a directory tree all cross relevant
#   executables, libraries and header files are installed to. All packages
#   built for the host to create data for the target are searching in this
#   directory tree for executables, header and library files.
#   Paths:
#    - executables: $(PTXDIST_SYSROOT_CROSS)/bin
#    - header files: $(PTXDIST_SYSROOT_CROSS)/include
#    - libraries: $(PTXDIST_SYSROOT_CROSS)/lib
#
#
# If no compile time files are reqired, skip this stage
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/file-transfer.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, file-transfer)
	@$(call install_fixup,file-transfer,PRIORITY,optional)
	@$(call install_fixup,file-transfer,SECTION,base)
	@$(call install_fixup,file-transfer,AUTHOR,"WAGO Kontakttechnik GmbH")
	@$(call install_fixup,file-transfer,DESCRIPTION,"This config-tool supports preparation and cleanup of file transfers.")

	@$(call install_copy, file-transfer, 0, 0, 0755, /etc/config-tools);

# Files that should be copied to the target

	@$(call install_copy, file-transfer, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/file_transfer, /etc/config-tools/file_transfer);

	@$(call install_finish,file-transfer)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/file-transfer.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, FILE_TRANSFER)

# vim: syntax=make
