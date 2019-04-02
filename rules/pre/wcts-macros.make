# The following macro has to be known to make when it parses the package rule files.
# That's why it is has to be placed to the "pre" rules.

wcts_create_svn_co_target = \
	$($(strip $(1))_SVN_CO_DIR): $(STATEDIR)/host-wcts.install ; \
		ECLIPSE_HOME=$(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless $(MAKE) -r -f $(WCTS_MAKEFILE) $($(strip $(1))_MAKE_VARS) checkout

# vim: syntax=make
