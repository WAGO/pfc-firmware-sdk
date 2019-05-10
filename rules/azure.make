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

AZURE_VERSION       := 1.2.9
AZURE_SUFFIX        := tar.gz
AZURE_MD5           := c7d81ddf737c01405309e0214ff76533

AZURE_URL           := https://github.com/Azure/azure-iot-sdk-c/archive/$(AZURE_VERSION).$(AZURE_SUFFIX) 
                    # Placeholder: Url is not working!!! 
                    # Microsoft does not offer a source archive that includes all dependencies.
                    # Therefore this archive has to be downloaded manually including all files cloned with the following command:  
                    # git clone -b <yyyy-mm-dd> --recursive https://github.com/Azure/azure-iot-sdk-c.git
		    		# Before using the SDK it has to be built for the target operation system:
	  	    		# Linux: cd azure-iot-sdk-c - mkdir cmake - cd cmake - cmake .. - cmake --build .
		    		# For further information have a look at: azure-iot-sdk-c/doc/devbox_setup.md

AZURE		    	:= azure-iot-sdk-c-$(AZURE_VERSION)
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
    
#	make -C $(AZURE_DIR)-build  DESTDIR=$(PTXCONF_SYSROOT_TARGET) install

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/azure.targetinstall:
	@$(call targetinfo)

	@$(call install_init, azure)
	@$(call install_fixup, azure, PRIORITY, optional)
	@$(call install_fixup, azure, SECTION, base)
	@$(call install_fixup, azure, AUTHOR, "WAGO Kontakttechnik GmbH@AUTHOR@Co.KG <support@wago.com>")
	@$(call install_fixup, azure, DESCRIPTION, missing)

ifdef PTXCONF_AZURE_SAMPLES

ifdef PTXCONF_AZURE_HTTP
	@$(call install_copy, azure, 0, 0, 0755, $(AZURE_DIR)-build/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http, /usr/bin/iothub_client_sample_http)
endif

ifdef PTXCONF_AZURE_AMQP
	@$(call install_copy, azure, 0, 0, 0755, $(AZURE_DIR)-build/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp, /usr/bin/iothub_client_sample_amqp)
ifdef PTXCONF_AZURE_WSIO
	@$(call install_copy, azure, 0, 0, 0755, $(AZURE_DIR)-build/c/iothub_client/samples/iothub_client_sample_amqp_websockets/iothub_client_sample_amqp_websockets, /usr/bin/#iothub_client_sample_amqp_websockets)
endif
endif

ifdef PTXCONF_AZURE_MQTT
	@$(call install_copy, azure, 0, 0, 0755, $(AZURE_DIR)-build/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt, /usr/bin/iothub_client_sample_mqtt)
endif

endif
	@$(call install_finish, azure)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/azure.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, AZURE)

# vim: syntax=make
