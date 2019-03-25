# -*-makefile-*-
#
# Copyright (C) 2016 by  WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGOCRYPT) += wagocrypt

#
# Paths and names
#
WAGOCRYPT_VERSION	:= 1.0
WAGOCRYPT_MD5		:=
WAGOCRYPT		    := wagocrypt
WAGOCRYPT_URL		:= file://local_src/$(WAGOCRYPT)
WAGOCRYPT_SRC_DIR      := $(PTXDIST_WORKSPACE)/local_src/$(WAGOCRYPT)
WAGOCRYPT_DIR		:= $(BUILDDIR)/$(WAGOCRYPT)
WAGOCRYPT_LICENSE	:= unknown


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/wagocrypt.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wagocrypt.extract:
	@$(call targetinfo)
	mkdir -p $(WAGOCRYPT_DIR)

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(WAGOCRYPT_SRC_DIR)/ $(WAGOCRYPT_DIR)/

	@$(call touch)



# ----------------------------------------------------------------------------
# Prepare - Default, just set conf-tool and environment
# ----------------------------------------------------------------------------
WAGOCRYPT_CONF_TOOL	:= NO
WAGOCRYPT_MAKE_ENV	:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/wagocrypt.compile:
	@$(call targetinfo)
	@cd $(WAGOCRYPT_DIR) && \
		$(WAGOCRYPT_MAKE_ENV) $(WAGOCRYPT_PATH) $(MAKE) 
	@$(call touch)


# ----------------------------------------------------------------------------
# Install - Nothing
# ----------------------------------------------------------------------------
$(STATEDIR)/wagocrypt.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
$(STATEDIR)/wagocrypt.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wagocrypt)
	@$(call install_fixup, wagocrypt, PRIORITY, optional)
	@$(call install_fixup, wagocrypt, SECTION, base)
	@$(call install_fixup, wagocrypt, AUTHOR, "<cm>")
	@$(call install_fixup, wagocrypt, DESCRIPTION, missing)
    # Install lib into default search path
	@$(call install_copy, wagocrypt, 0, 0, 0644, $(WAGOCRYPT_DIR)/src/libwagocrypt.so, /usr/lib/libwagocrypt.so.1.0)
	@$(call install_link, wagocrypt, libwagocrypt.so.1.0, /usr/lib/libwagocrypt.so.1)
	@$(call install_link, wagocrypt, libwagocrypt.so.1.0, /usr/lib/libwagocrypt.so)
    # Link lib into codesys runtime search path 
	@$(call install_link, wagocrypt, ../libwagocrypt.so.1.0, /usr/lib/wide/libwagocrypt.so)

ifdef PTXCONF_WAGOCRYPT_TESTS
	@$(call install_copy, wagocrypt, 0, 0, 0755, $(WAGOCRYPT_DIR)/test/testwagocrypt_aes, /usr/bin/testwagocrypt_aes)
	@$(call install_copy, wagocrypt, 0, 0, 0755, $(WAGOCRYPT_DIR)/test/testwagocrypt_keywrap, /usr/bin/testwagocrypt_keywrap)
	@$(call install_copy, wagocrypt, 0, 0, 0755, $(WAGOCRYPT_DIR)/test/testwagocrypt_hmac, /usr/bin/testwagocrypt_hmac)
	@$(call install_copy, wagocrypt, 0, 0, 0755, $(WAGOCRYPT_DIR)/test/testwagocrypt_gmac, /usr/bin/testwagocrypt_gmac)
endif

	@$(call install_finish, wagocrypt)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/wagocrypt.clean:
	@$(call targetinfo)
	@-cd $(WAGOCRYPT_DIR) && \
		$(WAGOCRYPT_MAKE_ENV) $(MAKE) clean
	@$(call clean, $(WAGOCRYPT_DIR))
	@$(call clean_pkg, WAGOCRYPT)

# vim: syntax=make
