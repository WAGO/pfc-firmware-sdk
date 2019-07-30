# $(call subst_template,template,name/value...)
subst_template = $(shell echo '$1' | sed $(foreach KV,$2,-e 's+${KV}+g'))

# $(call version_major,version)
version_major = $(shell echo '$1' | cut -d'.' -f1)

# Attention: sed in subst_template uses + as a delimiter!
# $(call jfrog_template_to_url, PACKAGE)
jfrog_template_to_url = \
	$(shell echo \
		$(call subst_template,$(PTXCONF_$(strip $(1))_URL_TEMPLATE),\
            %ARTIFACTORY_BASE_URL%+$(call remove_quotes, $(PTXCONF_ARTIFACTORY_BASE_URL))\
            %VERSION%+$($(strip $(1))_VERSION)\
            %VERSION_MAJOR%+$(call version_major,$($(strip $(1))_VERSION))))

# vim: syntax=make
