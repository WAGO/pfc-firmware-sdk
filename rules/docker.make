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
DOCKER_VERSION				:= 20_10_8
DOCKER_VERSION_SUFFIX := wago_static-linux_arm_v7
DOCKER_MD5						:= 72eb023440b66764a5b97a8477b94313
DOCKER								:= docker-$(DOCKER_VERSION)-$(DOCKER_VERSION_SUFFIX)
DOCKER_SUFFIX					:= tar.gz
DOCKER_URL						:= https://github.com/WAGO/docker-engine/releases/download/v$(DOCKER_VERSION)-wago/docker-$(subst .,_,$(DOCKER_VERSION))-$(DOCKER_VERSION_SUFFIX).tar.gz
DOCKER_SOURCE					:= $(SRCDIR)/$(DOCKER).$(DOCKER_SUFFIX)
DOCKER_DIR						:= $(BUILDDIR)/$(DOCKER)
DOCKER_STRIP_LEVEL 		:= 0
DOCKER_LICENSE				:= Apache 2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.prepare:
	@$(call targetinfo)
		
	# create archive with docker binaries
	@cd $(DOCKER_DIR) \
    && tar --exclude='*.md5' --exclude='*.sha256' -cvf docker-binaries.tar.xz --use-compress-program='xz -9' * \
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
	
	@$(call install_alternative, docker, 0, 0, 0750, /usr/sbin/settings_backup_docker_status)

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
