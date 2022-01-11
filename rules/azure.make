# -*-makefile-*-
#
# Copyright (C) 2016 by WAGO Kontakttechnik GmbH@AUTHOR@Co.KG <support@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_AZURE) += azure

#
# Paths and names
#

AZURE_VERSION       := 1.6.0
AZURE_SUFFIX        := tar.gz
AZURE_MD5           := 7f1d2c011e11d9979826d0997d00b7d2

AZURE_URL           := https://github.com/Azure/azure-iot-sdk-c/archive/$(AZURE_VERSION).$(AZURE_SUFFIX) 
                    # Placeholder: Url is not working!!! 
                    # Microsoft does not offer a source archive that includes all dependencies.
                    # Therefore this archive has to be downloaded manually including all files cloned with the following command:  
                    # git clone -b <yyyy-mm-dd> --recursive https://github.com/Azure/azure-iot-sdk-c.git
		    		# Before using the SDK it has to be built for the target operation system:
	  	    		# Linux: cd azure-iot-sdk-c - mkdir cmake - cd cmake - cmake .. - cmake --build .
		    		# For further information have a look at: azure-iot-sdk-c/doc/devbox_setup.md

AZURE		    := azure-iot-sdk-c-$(AZURE_VERSION)
AZURE_SOURCE        := $(SRCDIR)/$(AZURE).$(AZURE_SUFFIX)
AZURE_DIR           := $(BUILDDIR)/$(AZURE)
AZURE_LICENSE       := unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(AZURE_SOURCE):
	@$(call targetinfo)
	@$(call get, AZURE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/AZURE.extract:
	@$(call targetinfo)
	@$(call clean, $(AZURE_DIR))
	@$(call extract, AZURE)
	@$(call patchin, AZURE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
AZURE_CONF_ENV	:= $(CROSS_ENV) 
# cmake
AZURE_CONF_TOOL	:= cmake

AZURE_CONF_OPT	:= $(CROSS_CMAKE_USR) -v --jobs=1 \
	-D"run_e2e_tests:BOOL=OFF" \
	-D"run_longhaul_tests:BOOL=OFF" \
	-D"skip_unittests:BOOL=ON" \
    	-D"use_condition:BOOL=OFF" \
	-D"CMAKE_CXX_FLAGS=-std=gnu++11 -fPIC -Wall "  \
    	-D"CMAKE_C_FLAGS=-std=gnu99 -fPIC -Wall "


ifdef PTXCONF_AZURE_HTTP
AZURE_CONF_OPT	+= 	-D"use_http:BOOL=ON" 
else
AZURE_CONF_OPT	+= 	-D"use_http:BOOL=OFF"
endif

ifdef PTXCONF_AZURE_AMQP
AZURE_CONF_OPT	+= 	-D"use_amqp:BOOL=ON" 
else
AZURE_CONF_OPT	+= 	-D"use_amqp:BOOL=OFF"
endif

ifdef PTXCONF_AZURE_MQTT
AZURE_CONF_OPT	+= 	-D"use_mqtt:BOOL=ON" 
else
AZURE_CONF_OPT	+= 	-D"use_mqtt:BOOL=OFF"
endif

ifdef PTXCONF_AZURE_WSIO
AZURE_CONF_OPT	+= 	-D"use_wsio:BOOL=ON" 
else
AZURE_CONF_OPT	+= 	-D"use_wsio:BOOL=OFF"
endif


# ----------------------------------------------------------------------------
# Compile (default)
# ----------------------------------------------------------------------------


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/azure.install:
	@$(call targetinfo)

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/certs
	@install -m 0644 $(AZURE_DIR)/certs/*.h \
		$(PKGDIR)/$(AZURE)/usr/include/azure/certs

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/deps/parson
	@install -m 0644 $(AZURE_DIR)/deps/parson/*.h \
		$(PKGDIR)/$(AZURE)/usr/include/azure/deps/parson

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/deps/azure-macro-utils-c/inc/azure_macro_utils/
	@install -m 0644 $(AZURE_DIR)/deps/azure-macro-utils-c/inc/azure_macro_utils/*.h \
		$(PKGDIR)/$(AZURE)/usr/include/azure/deps/azure-macro-utils-c/inc/azure_macro_utils

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/deps/umock-c/inc/umock_c/
	@install -m 0644 $(AZURE_DIR)/deps/umock-c/inc/umock_c/*.h \
		$(PKGDIR)/$(AZURE)/usr/include/azure/deps/umock-c/inc/umock_c

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/deps/umock-c/inc/umock_c/aux_inc/
	@install -m 0644 $(AZURE_DIR)/deps/umock-c/inc/umock_c/aux_inc/* \
		$(PKGDIR)/$(AZURE)/usr/include/azure/deps/umock-c/inc/umock_c/aux_inc

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/iothub_client/inc
	@install -m 0644 $(AZURE_DIR)/iothub_client/inc/*.h \
		$(PKGDIR)/$(AZURE)/usr/include/azure/iothub_client/inc

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/include/azure/c-utility/inc/azure_c_shared_utility
	@install -m 0644 $(AZURE_DIR)/c-utility/inc/azure_c_shared_utility/*.h \
		$(PKGDIR)/$(AZURE)/usr/include/azure/c-utility/inc/azure_c_shared_utility

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/lib/azure
	@install -m 0644 $(AZURE_DIR)-build/*.a \
		$(PKGDIR)/$(AZURE)/usr/lib/azure

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/lib/azure/umqtt
	@install -m 0644 $(AZURE_DIR)-build/umqtt/*.a \
		$(PKGDIR)/$(AZURE)/usr/lib/azure/umqtt

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/lib/azure/iothub_client
	@install -m 0644 $(AZURE_DIR)-build/iothub_client/*.a \
		$(PKGDIR)/$(AZURE)/usr/lib/azure/iothub_client

	@mkdir -p $(PKGDIR)/$(AZURE)/usr/lib/azure/c-utility
	@install -m 0644 $(AZURE_DIR)-build/c-utility/*.a \
		$(PKGDIR)/$(AZURE)/usr/lib/azure/c-utility

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/azure.targetinstall:
	@$(call targetinfo)
	@$(call install_init, azure)
	@$(call install_fixup, azure, PRIORITY, optional)
	@$(call install_fixup, azure, SECTION, base)
	@$(call install_fixup, azure, AUTHOR, "Microsoft Corporation")
	@$(call install_fixup, azure, DESCRIPTION, missing)
  	
	@$(call install_copy, azure, 0, 0, 0644, $(AZURE_DIR)/LICENSE, /usr/share/licenses/oss/license.azure_$(AZURE_VERSION).txt)
	@$(call install_finish, azure)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/azure.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, AZURE)

# vim: syntax=make
