# Build wago-custom-install as the very last package as it contains hidden dependencies to other packages.
# For example, /etc/specific/diagnostic.xml is written to by several packages that use xslt transformation.
# The rules have to be placed in rules/post in order to have the variables PACKAGES-y and PACKAGES-m fully populated.
# Testcase: yes|ptxdist clean ; ptxdist targetinstall -q wago-custom-install && ptxdist -q images

$(STATEDIR)/wago-custom-install.targetinstall: \
	$(patsubst %,$(STATEDIR)/%.targetinstall.post,$(filter-out wago-custom-install,$(PACKAGES-y) $(PACKAGES-m)))
