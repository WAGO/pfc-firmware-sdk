## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCWEBSOCKETSERVER) += cds3-tscwebsocketserver

CDS3_TSCWEBSOCKETSERVER_VERSION      := 2.0.4
CDS3_TSCWEBSOCKETSERVER_MD5          := b975d30a668db19f04ef4020991ec851
CDS3_TSCWEBSOCKETSERVER_SUFFIX       := tgz
CDS3_TSCWEBSOCKETSERVER              := cds3_cmp_wsserver_2_0_4_wsfrontend_1_4_0
CDS3_TSCWEBSOCKETSERVER_DIR          := $(BUILDDIR)/$(CDS3_TSCWEBSOCKETSERVER)
CDS3_TSCWEBSOCKETSERVER_URL          := http://svsv01003/svn/repo14/$(CDS3_TSCWEBSOCKETSERVER).$(CDS3_TSCWEBSOCKETSERVER_SUFFIX)

CDS3_TSCWEBSOCKETSERVER_CONFIGDIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/TscWebsocketServer
CDS3_TSCWEBSOCKETSERVER_CONFIGFILE   := CmpWSServer.cfg

CDS3_TSCWEBSOCKETSERVER_SOURCE       := $(SRCDIR)/$(CDS3_TSCWEBSOCKETSERVER).$(CDS3_TSCWEBSOCKETSERVER_SUFFIX)

WEBSOCKET_FRONTEND                   := WebSocketFrontend
WEBSOCKET_FRONTEND_DIR               := $(BUILDDIR)/$(WEBSOCKET_FRONTEND)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# Not needed using world/get here
#$(STATEDIR)/cds3-tscwebsocketserver.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/cds3-tscwebsocketserver.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# Nothing just copy the config file to the build dir. 
# Must be done here because extract stage will override build directory.
$(STATEDIR)/cds3-tscwebsocketserver.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebsocketserver.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebsocketserver.install:
	@$(call targetinfo)
# Clean all libCmpWSServer.so files and install the proper files and links
	@rm -f $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so*
	@install -D -m755 $(CDS3_TSCWEBSOCKETSERVER_DIR)/libCmpWSServer.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION)
	@ln -s $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so.2.0
	@ln -s $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so.2
	@ln -s $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libCmpWSServer.so
# Clean all libSysUtil.so files and install the proper files and links
	@rm -f $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so*
	@install -D -m755 $(CDS3_TSCWEBSOCKETSERVER_DIR)/libSysUtil.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION)
	@ln -s $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so.2.0
	@ln -s $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so.2
	@ln -s $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so.$(CDS3_TSCWEBSOCKETSERVER_VERSION) $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/usr/lib/libSysUtil.so	
# Clean eWS.js file and install proper one
	@rm -f $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/eWS.js
	@install -D -m755 $(CDS3_TSCWEBSOCKETSERVER_DIR)/eWS.js $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/eWS.js
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebsocketserver.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwebsocketserver)
	@$(call install_fixup, cds3-tscwebsocketserver,PRIORITY,optional)
	@$(call install_fixup, cds3-tscwebsocketserver,SECTION,base)
	@$(call install_fixup, cds3-tscwebsocketserver,AUTHOR,"S.Schwalowsky und S.Borcherding")
	@$(call install_fixup, cds3-tscwebsocketserver,DESCRIPTION,"WebSocket server library for codesys3 (elrest)")

	@$(call install_lib, cds3-tscwebsocketserver, 0, 0, 0755, libCmpWSServer)
	@$(call install_lib, cds3-tscwebsocketserver, 0, 0, 0755, libSysUtil)
	@$(call install_link, cds3-tscwebsocketserver, ../libCmpWSServer.so, /usr/lib/cds3-custom-components/libCmpWSServer.so)
	@$(call install_link, cds3-tscwebsocketserver, ../libSysUtil.so, /usr/lib/cds3-custom-components/libSysUtil.so)

	@$(call install_alternative, cds3-tscwebsocketserver , 0, 0, 0644, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCWEBSOCKETSERVER_CONFIGFILE))

ifdef PTXCONF_WEBSOCKETFRONTEND
	@$(call install_copy, cds3-tscwebsocketserver, 0, 0, 0755, /var/www/ws)
	@$(call install_copy, cds3-tscwebsocketserver, 0, 0, 0644, $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)/eWS.js, /var/www/ws/eWS.js)
endif
	
	@$(call install_finish, cds3-tscwebsocketserver)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebsocketserver.clean:
	@$(call targetinfo)
	@$(call clean_pkg, CDS3_TSCWEBSOCKETSERVER)
	rm -rf $(CDS3_TSCWEBSOCKETSERVER_DIR)
	rm -rf $(PKGDIR)/$(CDS3_TSCWEBSOCKETSERVER)
	rm -f $(CDS3_TSCWEBSOCKETSERVER_SOURCE)

