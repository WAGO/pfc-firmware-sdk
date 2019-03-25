# -*-makefile-*-
#
# Copyright (C) 2018 by Oleg Karfich <oleg.karfich@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
ifdef PTXCONF_DOCKER_LAZY
LAZY_PACKAGES-$(PTXCONF_DOCKER) += docker
else
PACKAGES-$(PTXCONF_DOCKER) += docker
endif

#
# Paths and names
#
DOCKER_VERSION	:= 18.09.0
DOCKER_MD5	:= ffa897485efd58ed034171ed75fd1a1a
DOCKER		:= docker-$(DOCKER_VERSION)
DOCKER_SUFFIX	:= tgz
DOCKER_URL	:= https://download.docker.com/linux/static/stable/armhf/$(DOCKER).$(DOCKER_SUFFIX)
DOCKER_SOURCE	:= $(SRCDIR)/$(DOCKER).$(DOCKER_SUFFIX)
DOCKER_DIR	:= $(BUILDDIR)/$(DOCKER)
DOCKER_LICENSE	:= Apache 2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.targetinstall:
	@$(call targetinfo)

	@$(call install_init, docker)
	@$(call install_fixup, docker,PRIORITY,optional)
	@$(call install_fixup, docker,SECTION,base)
	@$(call install_fixup, docker,AUTHOR,"Oleg Karfich <oleg.karfich@wago.com>")
	@$(call install_fixup, docker,DESCRIPTION,"Docker daemon $(DOCKER_VERSION)")

	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/docker, /usr/bin/docker)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/containerd, /usr/bin/containerd)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/ctr, /usr/bin/ctr)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/containerd-shim, /usr/bin/containerd-shim)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/docker-init, /usr/bin/docker-init)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/docker-proxy, /usr/bin/docker-proxy)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/runc, /usr/bin/runc)
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/dockerd, /usr/bin/dockerd)

	@$(call install_alternative, docker, 0, 0, 0755, /etc/init.d/dockerd)
	@$(call install_alternative, docker, 0, 0, 0755, /etc/docker/daemon.json)

	@$(call install_link, docker, /etc/init.d/dockerd, /etc/rc.d/S99_docker)

	@$(call install_finish, docker)

	@$(call touch)

# vim: syntax=make
