#
#	Get
#
#

#
# Case building from source: order-only dependency kept from initial solution
# Case binary artefacts:	 explicit dependency needed to prevent get stage from failing
#							 and to force a rebuild if wcts-bin-artefacts-revision was
#							 altered.

wcts_get_deps = \
	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
		| $($(strip $(1))_SVN_CO_DIR))

#
# wcts_get:
#
#	$1: package name in capital letters (i.e. CDS3_OSLAYER)
#

#
# Disabled because the binary artefacts are hosted on the src file server and saved to the workspace src directory.
#
#wcts_get = \
#	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
#		,\
#		$(call wcts_clean_bin,$(strip $(1))) && \
#		mkdir -p $($(strip $(1))_DIR) && \
#		revision=$$( [[ "$($(strip $(1))_REV_NR)" =~ [0-9]+ ]] && \
#						echo $($(strip $(1))_REV_NR) || \
#						cat $(PTXCONF_SYSROOT_TARGET)/etc/wcts-revision) && \
#		echo $$revision > $($(strip $(1))_DIR)/svn_revision.txt && \
#		fileserver_path=$($(strip $(1))_REV_TAG)_Rev$$revision/$(WCTS_PLATFORMCONFIG) && \
#		$(if $(filter y,$(PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES)),\
#			cp -v $(PTXCONF_BSP_PACKAGES_DIR)/$($(strip $(1))_BIN_BASENAME)_$().tar.xz $($(strip $(1))_DIR),\
#			wget --timeout 5 \
#				'$(call remove_quotes,$(PTXCONF_BIN_ARTEFACTS_FILESRV_URL))/'$$fileserver_path/'$($(strip $(1))_BIN_BASENAME).tar.xz' \
#				-P $($(strip $(1))_DIR)))
#
#
#	Extract
#
#

wcts_extract_src = \
	@ECLIPSE_HOME=$(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless $(MAKE) -r -f $(WCTS_MAKEFILE) $($(1)_MAKE_VARS) fetch; \
	$(call patchin, $(1))

wcts_extract_bin = \
			@mkdir -p $($(strip $(1))_DIR) && \
			tar xvJf $(SRCDIR)/$($(1)_BIN_BASENAME).tar.xz -C $($(1)_DIR)/


wcts_extract = \
	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
			$(call wcts_extract_src,$(strip $(1))),\
			$(call wcts_extract_bin,$(strip $(1))))

#
#	Compile
#

wcts_compile_src = \
	@ECLIPSE_HOME=$(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless $(MAKE) -r -f $(WCTS_MAKEFILE) \
	PTXCONF_TARGET_EXTRA_CPPFLAGS='$(PTXCONF_TARGET_EXTRA_CPPFLAGS)' \
	PTXCONF_TARGET_EXTRA_CFLAGS='$(PTXCONF_TARGET_EXTRA_CFLAGS)' \
	PTXCONF_TARGET_EXTRA_CXXFLAGS='$(PTXCONF_TARGET_EXTRA_CXXFLAGS)' \
	PTXCONF_TARGET_EXTRA_LDFLAGS='$(PTXCONF_TARGET_EXTRA_LDFLAGS)' \
	$($(1)_MAKE_VARS) build

wcts_compile = \
	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
			$(call wcts_compile_src,$(strip $(1))))

#
#	Install
#

#############
#
# Reference counting
#
#############

WCTS_ENTRY_DELIM := '@'

# $1: entry key
# $2: entry value (i.e. package name)
# $3: storage file

wcts_add_entry = \
	touch $(3); \
	if grep -q $1\: $(3); then \
		if ! grep -q $1\:'.*'$2 $(3); then \
		sed -i 's+^\('$1'.*\)+\1 '$2'+' $(3); \
		fi; \
	else \
		echo $1: $2 >> $(3); \
	fi

# $1 object key to remove
wcts_remove_from_sysroot_inc = \
	inc_dir_basename=$$(echo $1 | awk -F$(WCTS_ENTRY_DELIM) '{print $$1}'); \
	inc_dir_version=$$(echo $1 | awk -F$(WCTS_ENTRY_DELIM) '{print $$2}'); \
	if [[ -n "$$inc_dir_basename" && -n "$$inc_dir_version" ]]; then \
		for link in $$(find $(PTXCONF_SYSROOT_TARGET)/usr/include/$$inc_dir_basename -type l); do \
			if [[ "$$(readlink $$link)" == "$$inc_dir_version" ]]; then \
				rm -vf $$link; \
			fi ; \
		done; \
		rm -vrf $(PTXCONF_SYSROOT_TARGET)/usr/include/$$inc_dir_basename/$$inc_dir_version; \
		rmdir -v --ignore-fail-on-non-empty $(PTXCONF_SYSROOT_TARGET)/usr/include/$$inc_dir_basename; \
	fi


# $1 object key to remove
wcts_remove_from_sysroot_lib = \
	for link in $$(find $(PTXCONF_SYSROOT_TARGET)/usr/lib/ -type l); do \
		if [[ "$$(readlink $$link)" == "$1" ]]; then \
			rm -vf $$link; \
		fi ; \
	done; \
	rm -vrf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$1

# $1: entry key
# $2: entry value (i.e. package name)
# $3: storage file
# $4: files deletion callback
wcts_remove_entry = \
	if grep -q $1\: $(3); then \
		if grep -q $1'\:.*'$2 $(3); then \
			sed -i 's+^\('$1'\:.*\)'$2'\(.*\)$$+\1\2+' $(3); \
			if grep -q '^'$1'\: *$$' $(3); then \
				$(call $(4),$(strip $(1))) && \
				sed -i '/^'$1'\: *$$/d' $(3); \
			fi; \
		fi; \
	fi 

#############
#
# Helper functions
#
#############

lib_path_to_version = \
	$$(echo $1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+$$')
	
newest_installed_version_inc = $$(ls -rv $(PTXCONF_SYSROOT_TARGET)/usr/include/$1 | grep -v current | sed -n 1p)
newest_installed_version_lib = $(call lib_path_to_version,$$(ls -rv $(PTXCONF_SYSROOT_TARGET)/usr/lib/$1.* | sed -n '/[0-9]\+\.[0-9]\+\.[0-9]\+/{p;q}'))

get_library_fqn_list = \
	$$(find $($(1)_DIR)/usr/lib/ -type f -and \( -name "*.a.*" -or -name "*.so.*" \) -exec basename {} \;)

get_library_basename = \
	$$(echo $1 | awk -F'.' '{print $$1"."$$2}')


get_includes_list = \
	$$(find $($(1)_DIR)/usr/include/ -mindepth 1 -maxdepth 1 -exec basename {} \;)

get_include_version_list = \
	$$(find $($(1)_DIR)/usr/include/$2 -mindepth 1 -maxdepth 1 -type d -exec basename {} \;)
	
#############

wcts_install_src = \
	export DESTDIR=$($(1)_DIR)/.package; \
	mkdir -p $$DESTDIR; \
	ECLIPSE_HOME=$(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless $(MAKE) -r -f $(WCTS_MAKEFILE) $($(1)_MAKE_VARS) install

wcts_install_create_link = \
	[[ -e "$$(dirname $2)/$(1)" ]] && ln -sfvn $(1) $(2)

wcts_install_lib_links = \
	echo;echo "Updating version links in sysroot lib:";echo; \
	for library_fqn in $(call get_library_fqn_list,$1); do \
		library=$(call get_library_basename,$$library_fqn); \
		newest_version_full=$(call newest_installed_version_lib,$$library); \
		newest_version_major=$$(echo $$newest_version_full | awk -F'.' '{print $$1}'); \
		$(call wcts_install_create_link,$$library.$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/lib/$$library); \
		$(call wcts_install_create_link,$$library.$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/lib/$$library.$$newest_version_major); \
		$(call wcts_add_entry,$$library_fqn,$1,$(STATEDIR)/wcts_lib_provided_by_package.txt); \
	done

wcts_install_include_links = \
	echo;echo "Updating version links in sysroot include:";echo; \
	for project in $(call get_includes_list,$1); do \
		project_version=$(call get_include_version_list,$1,$$project); \
		newest_version_full=$(call newest_installed_version_inc,$$project); \
		newest_version_major=$$(echo $$newest_version_full | awk -F'.' '{print $$1}'); \
		$(call wcts_install_create_link,$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/include/$$project/current); \
		$(call wcts_install_create_link,$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/include/$$project/$$newest_version_major); \
		$(call wcts_add_entry,$$project$(WCTS_ENTRY_DELIM)$$project_version,$1,$(STATEDIR)/wcts_inc_provided_by_package.txt); \
	done

dir_not_empty = [[ -n "$$(ls -A $1)" ]]

wcts_install_bin = \
	$(call dir_not_empty,$($(1)_DIR)/usr/lib) && \
	rsync -av --exclude ="*/*.d" --exclude="*libgmock_main.a*" --no-links $($(1)_DIR)/usr/lib/* ${PTXCONF_SYSROOT_TARGET}/usr/lib/ && \
	$(call wcts_install_lib_links,$(strip $(1))); \
	$(call dir_not_empty,$($(1)_DIR)/usr/include) && \
	rsync -av --exclude ="*/*.d" --no-links $($(1)_DIR)/usr/include/* ${PTXCONF_SYSROOT_TARGET}/usr/include/ && \
	$(call wcts_install_include_links,$(strip $(1)))

wcts_install = \
	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
			$(call wcts_install_src,$(strip $(1))),\
			$(call wcts_install_bin,$(strip $(1))))

#
#	Targetinstall
#

targetinstall_artefacts_src = \
	$(strip $(shell ECLIPSE_HOME=$(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless $(MAKE) --no-print-directory -r -f $(WCTS_MAKEFILE) $($(1)_MAKE_VARS) get-target-artefacts))
	
targetinstall_artefacts_bin = \
	$(strip $(shell find $($(1)_DIR)/usr/{bin,lib,resources-target} -type f) $($(1)_DIR)/svnInfo.txt)

wcts_targetinstall_loop = \
	for file in $(2); do \
		ARTEFACT_VERSION=$$(echo $$file | sed -ne 's_.*\(\.[0-9]\+\.[0-9]\+\.[0-9]\+\)$$_\1_p'); \
		case $$file in \
			*resources-target*) \
				$(call install_copy, $(PTX_MAP_TO_package_$(1)), 0, 0, 0644, $$file, $${file\#\#*resources-target}); \
			;; \
			$($(1)_ARTEFACTS_SO)) \
				fname=$$(basename $$file); \
				$(call install_copy, $(PTX_MAP_TO_package_$(1)), 0, 0, 644, $$file, /usr/lib/$$fname); \
			;; \
			$($(1)_ARTEFACTS_ELF)) \
				fname=$$(basename $$file $$ARTEFACT_VERSION); \
				$(call install_copy, $(PTX_MAP_TO_package_$(1)), 0, 0, 0755, $$file, /usr/bin/$$fname); \
			;; \
			*svnInfo.txt) \
				fname=$$(basename $$file); \
				$(call install_copy, $(PTX_MAP_TO_package_$(1)), 0, 0, 0644, $$file, /usr/local/lib/wcts/$(PTX_MAP_TO_package_$(1)).svnrev); \
			;; \
		esac; \
	done

wcts_targetinstall = \
	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
			$(call wcts_targetinstall_loop,$(strip $(1)),$(call targetinstall_artefacts_src,$(strip $(1)))),\
			$(call wcts_targetinstall_loop,$(strip $(1)),$(call targetinstall_artefacts_bin,$(strip $(1)))))
#
#	Clean
#

# Get information about installed libs and headers from files created in install stage
wcts_get_installed_inc_list = $$(sed 's+'$(WCTS_ENTRY_DELIM)'.*++' $(STATEDIR)/wcts_inc_provided_by_package.txt | paste -s)
wcts_get_installed_lib_list = $$(sed 's+\:.*++' $(STATEDIR)/wcts_lib_provided_by_package.txt | paste -s)

wcts_clean_update_lib_links = \
	echo;echo "Updating version links in sysroot lib:";echo; \
	for library_fqn in $(call wcts_get_installed_lib_list); do \
		library=$(call get_library_basename,$$library_fqn); \
		newest_version_full=$(call newest_installed_version_lib,$$library); \
		newest_version_major=$$(echo $$newest_version_full | awk -F'.' '{print $$1}'); \
		$(call wcts_install_create_link,$$library.$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/lib/$$library); \
		$(call wcts_install_create_link,$$library.$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/lib/$$library.$$newest_version_major); \
	done

wcts_clean_update_include_links = \
	echo;echo "Updating version links in sysroot include:";echo; \
	for project in $(call wcts_get_installed_inc_list); do \
		newest_version_full=$(call newest_installed_version_inc,$$project); \
		newest_version_major=$$(echo $$newest_version_full | awk -F'.' '{print $$1}'); \
		$(call wcts_install_create_link,$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/include/$$project/current); \
		$(call wcts_install_create_link,$$newest_version_full,$(PTXCONF_SYSROOT_TARGET)/usr/include/$$project/$$newest_version_major); \
	done

wcts_clean_bin = \
	if [[ -d $($(1)_DIR) ]]; then \
		for library_fqn in $(call get_library_fqn_list,$1); do \
			$(call wcts_remove_entry,$$library_fqn,$1,$(STATEDIR)/wcts_lib_provided_by_package.txt,wcts_remove_from_sysroot_lib); \
		done; \
		for project in $(call get_includes_list,$1); do \
			project_version=$(call get_include_version_list,$1,$$project); \
			$(call wcts_remove_entry,$$project$(WCTS_ENTRY_DELIM)$$project_version,$1,$(STATEDIR)/wcts_inc_provided_by_package.txt,wcts_remove_from_sysroot_inc); \
		done; \
		$(call wcts_clean_update_include_links); \
		$(call wcts_clean_update_lib_links); \
		rm -rfv $($(strip $(1))_DIR); \
	fi 


# NB: clean is currently a dummy target in wcts-rules.mk
wcts_clean_src = \
	[ ! -d $(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless ] || ECLIPSE_HOME=$(PTXCONF_SYSROOT_HOST)/lib/eclipse-headless $(MAKE) -r -f $(WCTS_MAKEFILE) $($(strip $(1))_MAKE_VARS) clean || true

wcts_clean = \
	$(if $(filter y,$(PTXCONF_$(strip $(1))_BUILD_FROM_SOURCE)),\
			$(call wcts_clean_src,$(strip $(1))), \
			$(call wcts_clean_bin,$(strip $(1))))

# vim: syntax=make
