# -*-makefile-*-
#
# Copyright (C) 2016 by Falk Werner/Jobst Wellensiek
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OPCUACSDK) += opcuacsdk

#
# Paths and names
#
OPCUACSDK_VERSION	:= 1.9.2
OPCUACSDK_MD5		:=
OPCUACSDK		:= opcuacsdk-$(OPCUACSDK_VERSION)
# Setting _URL to make PTXdist happy and not skip our patch(es)
OPCUACSDK_URL       := http://shut-up-and-patch-my-files-ptxdist/
OPCUACSDK_SRCDIR	:= $(PTXDIST_WORKSPACE)/wago_intern/opcua/csdk
OPCUACSDK_DIR		:= $(BUILDDIR)/$(OPCUACSDK)
OPCUACSDK_LICENSE	:= unknown

OPCUACSDK_WITH_XMLCONFIG:=ON
OPCUACSDK_WITH_OPENSSL:=ON
OPCUACSDK_ENABLE_EXPERIMENTAL_OPTIONS:=OFF
OPCUACSDK_WITH_HTTPS:=OFF

OPCUACSDK_BUILD_TYPE:=MinSizeRel
#OPCUACSDK_BUILD_TYPE:=RelWithDebInfo

OPCUACSDK_CFLAGS_MINSIZEREL:=-DNDEBUG -Os -ffunction-sections -fdata-sections -flto
OPCUACSDK_CXXFLAGS_MINSIZEREL:=-DNDEBUG -Os -ffunction-sections -fdata-sections -flto
OPCUACSDK_LINKER_FLAGS_MINSIZEREL:=-Wl,--gc-sections -flto -Os

OPCUACSDK_CFLAGS_RELWITHDEBINFO:=-O0 -g
OPCUACSDK_CXXFLAGS_RELWITHDEBINFO:=-O0 -g
OPCUACSDK_LINKER_FLAGS_RELWITHDEBINFO:=-O0

OPCUACSDK_CMAKE_OPTS = \
-DCMAKE_BUILD_TYPE:STRING=$(OPCUACSDK_BUILD_TYPE) \
-DCMAKE_CXX_FLAGS_MINSIZEREL:STRING="$(OPCUACSDK_CXXFLAGS_MINSIZEREL)" \
-DCMAKE_C_FLAGS_MINSIZEREL:STRING="$(OPCUACSDK_CFLAGS_MINSIZEREL)" \
-DCMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:STRING="$(OPCUACSDK_LINKER_FLAGS_MINSIZEREL)" \
-DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING="$(OPCUACSDK_CXXFLAGS_RELWITHDEBINFO)" \
-DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING="$(OPCUACSDK_CFLAGS_RELWITHDEBINFO)" \
-DCMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:STRING="$(OPCUACSDK_LINKER_FLAGS_RELWITHDEBINFO)" \
-DCMAKE_COLOR_MAKEFILE:BOOL=OFF \
-DCMAKE_C_FLAGS:STRING="-fexceptions" \
-DCMAKE_CXX_FLAGS:STRING="-fexceptions" \
-DCMAKE_AR:STRING="$(PTXDIST_COMPILER_PREFIX)gcc-ar" \
-DCMAKE_RANLIB:STRING="$(PTXDIST_COMPILER_PREFIX)gcc-ranlib"

OPCUACSDK_CMAKE_SDK_OPTS := \
-DBUILD_SHARED_STACK:BOOL=ON \
-DBUILD_SHARED_LIBS:BOOL=OFF \
-DBUILD_UABASEC:BOOL=ON \
-DUABASE_ENABLE_ERROR_LOOKUP:BOOL=ON \
-DUABASE_SETTINGS_USE_FILEBACKEND:BOOL=ON \
-DUABASE_USE_FILESYSTEM:BOOL=ON \
-DBUILD_UASERVERC:BOOL=ON \
-DBUILD_AUTH_INTERNAL:BOOL=ON \
-DBUILD_AUTH_WIN32:BOOL=OFF \
-DBUILD_AUTH_PAM:BOOL=ON \
-DBUILD_AUTH_SASL:BOOL=OFF \
-DBUILD_AUTH_USER:BOOL=ON \
-DBUILD_AUTHORIZATION:BOOL=ON \
-DBUILD_DI_PROVIDER:BOOL=ON \
-DBUILD_PLCOPEN_PROVIDER:BOOL=ON \
-DUASERVER_CALL:BOOL=ON \
-DUASERVER_EVENTS:BOOL=ON \
-DUASERVER_HISTORYREAD:BOOL=ON \
-DUASERVER_HISTORYUPDATE:BOOL=ON \
-DUASERVER_NODEMANAGEMENT:BOOL=OFF \
-DUASERVER_QUERY:BOOL=OFF \
-DBUILD_WITH_DISCOVERY:BOOL=OFF \
-DBUILD_WITH_UNITS:BOOL=ON \
-DUASERVER_GDS_PUSH:BOOL=ON \
-DUASERVER_AUDITING:BOOL=OFF \
-DUASERVER_ENABLE_DESCRIPTIONS:BOOL=OFF \
-DBUILD_UACLIENTC:BOOL=ON \
-DUASTACK_USE_SYNCHRONIZATION:BOOL=ON \
-DUASTACK_USE_TRACE_RAW:BOOL=ON \
-DUASTACK_CLIENTAPI_ENABLED:BOOL=ON \
-DUASTACK_SERVERAPI_ENABLED:BOOL=ON \
-DUASTACK_SUPPORT_SECURITYPOLICY_NONE:BOOL=ON \
-DUASTACK_SUPPORT_SECURITYPOLICY_BASIC128RSA15:BOOL=ON \
-DUASTACK_SUPPORT_SECURITYPOLICY_BASIC256:BOOL=ON \
-DUASTACK_SUPPORT_SECURITYPOLICY_BASIC256SHA256:BOOL=ON \
-DUASTACK_WITH_OPENSSL:BOOL=ON \
-DUASTACK_WITH_PKI_WIN32:BOOL=OFF \
-DUASTACK_MINIMAL_DISABLE_TRACE:BOOL=OFF \
-DUASTACK_MINIMAL_OMIT_TYPE_NAME:BOOL=OFF \
-DUASTACK_MINIMAL_SMALL_DATAVALUE:BOOL=OFF \
-DUASTACK_TCPLISTENER_MAX_CONNECTIONS:=9 \
-DUASTACK_SOCKETMANAGER_NUMBER_OF_SOCKETS:=32 \
-DUASTACK_NUMBER_OF_AVAILABLE_TIMERS:=32 \
-DENABLE_STATIC_CRT:BOOL=OFF \
-DUASTACK_WITH_HTTPS:BOOL=OFF \
-DUASTACK_WITH_TLS:BOOL=OFF \
-DBUILD_DATA_LOGGER:BOOL=ON \
-DBUILD_DATA_LOGGER_FILE_BACKEND:BOOL=ON \
-DDATA_LOGGER_MAX_DATA_ITEMS:BOOL=1000 \
-DDATA_LOGGER_MAX_EVENT_ITEMS:BOOL=16 \
-DDATA_LOGGER_MAX_STRINGNODEID_LENGTH:=128 \
-DSDK_PLATFORM_NAME=Linux


OPCUACSDK_CONF_TOOL	:= cmake
OPCUACSDK_CONF_OPT	= $(CROSS_CMAKE_USR) \
$(OPCUACSDK_CMAKE_OPTS) \
$(OPCUACSDK_CMAKE_SDK_OPTS)

OPCUACSDK_PACKAGE_NAME := opcuacsdk_$(OPCUACSDK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
OPCUACSDK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
#
$(STATEDIR)/opcuacsdk.extract:
	@$(call targetinfo)
	@mkdir -p $(OPCUACSDK_DIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call clean, $(OPCUACSDK_DIR))
	cp -r $(OPCUACSDK_SRCDIR) $(OPCUACSDK_DIR)
	@$(call patchin, OPCUACSDK, $(OPCUACSDK_DIR))
endif	

	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/opcuacsdk.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuacsdk.prepare:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/prepare, OPCUACSDK)

	# move examples out of the way to prevent the build system from
	# automagically building them
	rm -rf $(OPCUACSDK_DIR)/examples
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuacsdk.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, OPCUACSDK)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuacsdk.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(OPCUACSDK_PKGDIR) && \
	tar xvzf $(OPCUACSDK_PLATFORMCONFIGPACKAGEDIR)/$(OPCUACSDK_PACKAGE_NAME).tgz -C $(OPCUACSDK_PKGDIR)
else

	@$(call world/install, OPCUACSDK)
	cd $(OPCUACSDK_PKGDIR)/usr ; mv include ua ; mkdir -p include ; mv ua include
	mkdir $(OPCUACSDK_PKGDIR)/usr/lib/pkgconfig; cp $(OPCUACSDK_DIR)/uacsdk.pc $(OPCUACSDK_PKGDIR)/usr/lib/pkgconfig/

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@mkdir -p $(OPCUACSDK_PLATFORMCONFIGPACKAGEDIR)
#	@cd $(OPCUACSDK_PKGDIR) && tar cvzf $(OPCUACSDK_PLATFORMCONFIGPACKAGEDIR)/$(OPCUACSDK_PACKAGE_NAME).tgz *
	@cd $(OPCUACSDK_PKGDIR) && tar cvzf $(OPCUACSDK_PLATFORMCONFIGPACKAGEDIR)/$(OPCUACSDK_PACKAGE_NAME).tgz * --exclude=ua
endif

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuacsdk.targetinstall:
	@$(call targetinfo)

	@$(call install_init, opcuacsdk)
	@$(call install_fixup, opcuacsdk, PRIORITY, optional)
	@$(call install_fixup, opcuacsdk, SECTION, base)
	@$(call install_fixup, opcuacsdk, AUTHOR, "Falk Werner/Jobst Wellensiek")
	@$(call install_fixup, opcuacsdk, DESCRIPTION, missing)

#	#
#	# example code:; copy all binaries
#	#

#	@for i in $(shell cd $(OPCUACSDK_PKGDIR) && find usr/lib usr/include -type f); do \
#		$(call install_copy, opcuacsdk, 0, 0, 0755, $(OPCUACSDK_PKGDIR)/$$i, $(PTXDIST_SYSROOT_TARGET)/$$i); \
#	done
	$(call install_copy, opcuacsdk, 0, 0, 0755, -, /usr/lib/libuastack.so)

#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, opcuacsdk)

	@$(call touch)

# vim: syntax=make
