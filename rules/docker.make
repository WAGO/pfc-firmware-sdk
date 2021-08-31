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
DOCKER_VERSION	:= 19.03.8
DOCKER_MD5			:= 328f19218783795cb902450e5891a520
DOCKER					:= docker-$(DOCKER_VERSION)
DOCKER_SUFFIX		:= tgz
DOCKER_URL			:= https://download.docker.com/linux/static/stable/armhf/$(DOCKER).$(DOCKER_SUFFIX)
DOCKER_SOURCE		:= $(SRCDIR)/$(DOCKER).$(DOCKER_SUFFIX)
DOCKER_DIR			:= $(BUILDDIR)/$(DOCKER)
DOCKER_LICENSE	:= Apache 2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.prepare:
	@$(call targetinfo)
		
	# create archive with docker binaries
	@cd $(DOCKER_DIR) \
    && tar cvf docker-binaries.tar.xz --use-compress-program='xz -9' * \
    && find . -type f -not -name "*.tar.xz" -delete
	
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

	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/docker-binaries.tar.xz, /home/wago-docker/docker-binaries.tar.xz)

	@$(call install_alternative, docker, 0, 0, 0755, /etc/init.d/dockerd)
	@$(call install_alternative, docker, 0, 0, 0755, /etc/docker/daemon.json)
	@$(call install_alternative, docker, 0, 0, 0755, /etc/config-tools/events/firewall/iptables/docker)
	@$(call install_alternative, docker, 0, 0, 0755, /opt/wago-docker/sbin/iptables)
	
	@$(call install_link, docker, /home/wago-docker/containerd, /usr/bin/containerd)
	@$(call install_link, docker, /home/wago-docker/containerd-shim, /usr/bin/containerd-shim)
	@$(call install_link, docker, /home/wago-docker/ctr, /usr/bin/ctr)
	@$(call install_link, docker, /home/wago-docker/docker, /usr/bin/docker)
	@$(call install_link, docker, /home/wago-docker/dockerd, /usr/bin/dockerd)
	@$(call install_link, docker, /home/wago-docker/docker-init, /usr/bin/docker-init)
	@$(call install_link, docker, /home/wago-docker/docker-proxy, /usr/bin/docker-proxy)
	@$(call install_link, docker, /home/wago-docker/runc, /usr/bin/runc)

	@$(call install_link, docker, /etc/init.d/dockerd, /etc/rc.d/disabled/S99_docker)
	
	@$(call install_finish, docker)

	@$(call touch)

# vim: syntax=make
