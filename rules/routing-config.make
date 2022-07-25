# -*-makefile-*-
#
# Copyright (C) 2017 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ROUTING_CONFIG) += routing-config

ROUTING_CONFIG_VERSION	:= 1.1.0

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/routing-config.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/routing-config.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/routing-config.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/routing-config.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/routing-config.install:
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

$(STATEDIR)/routing-config.targetinstall:
	@$(call targetinfo)
#
# TODO: To build your own package, if this step requires one
	@$(call install_init, routing-config)
	@$(call install_fixup,routing-config,PRIORITY,optional)
	@$(call install_fixup,routing-config,SECTION,base)
	@$(call install_fixup,routing-config,AUTHOR,"WAGO GmbH")
	@$(call install_fixup,routing-config,DESCRIPTION,"This packet provides config-tools supporting configuration of static routing, IP-masquerading and port-forwarding.")

# Files that should be copied to the target

	@$(call install_copy, routing-config, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_routing_lib, /etc/config-tools/config_routing_lib);
	@$(call install_copy, routing-config, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_routing, /etc/config-tools/config_routing);
	@$(call install_copy, routing-config, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/validate_gateway_config, /etc/init.d/validate_gateway_config);
	@$(call install_copy, routing-config, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/events/networking/update_routing, /etc/config-tools/events/networking/update_routing);
	@$(call install_alternative, routing-config, 0, 0, 0644, /etc/specific/routing.conf);
	@$(call install_copy, routing-config, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/backup-restore/backup_routing, /etc/config-tools/backup-restore/backup_routing);
	@$(call install_link, routing-config, ../init.d/validate_gateway_config, /etc/rc.d/S12_validate_gateway_config);
	@$(call install_finish,routing-config)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/routing-config.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, ROUTING_CONFIG)

# vim: syntax=make
