# This file contains additional dependencies which fix the build order of packages that may have 
# different behaviour depending on whether other packages have been built or not.
#
# It's aim is to make builds reproducible and to be able to use PTXdist's option "-j".
#
# WARNING: At the time of writing, it is not yet proven that all necessary dependencies have been fixed
#          or if there are any hidden data races in build scripts.
#
# If you want to build your workspace using "-j", do at your own risk.
#
PTX_PACKAGES_UNSELECTED	:= \
	$(PACKAGES-) \
	$(CROSS_PACKAGES-) \
	$(HOST_PACKAGES-) \
	$(LAZY_PACKAGES-)

PTX_PACKAGES_SELECTED += autogen-tools

define validate_existence
	$(if $(filter $(PTX_PACKAGES_SELECTED) $(PTX_PACKAGES_UNSELECTED),$(1)), \
		, \
		$(warning A package named $(1) does not exist) \
	)
endef

# function add-dependency(from, to)
# Adds a PTXdist package dependency from $(1).extract to $(2).install.post IF both packages exist and are selected.
define add-order
	$(call validate_existence,$(1)) \
	$(call validate_existence,$(2)) \
	$(if $(filter-out $(PTX_PACKAGES_UNSELECTED),$(1)), \
		$(if $(filter-out $(PTX_PACKAGES_UNSELECTED),$(2)), \
			$(if $(findstring host-,$(2)), \
				$(STATEDIR)/$(1).extract: $(STATEDIR)/$(2).install.post \
			, \
				$(STATEDIR)/$(1).extract: $(STATEDIR)/$(2).targetinstall.post \
			) \
		, \
			$(if $(filter $(2),autogen-tools), \
				$(STATEDIR)/$(1).extract: $(STATEDIR)/autogen-tools \
			) \
		) \
	)
endef


## Dependencies to make autotools use PTXdist's host tools instead of the build host's build tools
$(call add-order,host-flex,autogen-tools)
$(call add-order,host-pkg-config,autogen-tools)
$(call add-order,dataagent,autogen-tools)
$(call add-order,memedit,autogen-tools)

$(call add-order,host-libkmod,host-libxslt)
$(call add-order,libkmod,host-libxslt)
$(call add-order,host-util-linux-ng,host-libxslt)
$(call add-order,util-linux-ng,host-libxslt)

$(call add-order,host-flex,host-m4)
$(call add-order,flex,host-flex)
$(call add-order,glib,host-pkg-config)
$(call add-order,host-glib,host-libxml2)



## Dependencies that change build behaviour
$(call add-order,host-libkmod,host-glib)
$(call add-order,host-cmake,host-acl)
$(call add-order,pureftpd,libcap)
$(call add-order,host-e2fsprogs,host-attr)
$(call add-order,host-e2fsprogs,host-acl)


## Other (yet to be sorted) dependencies

#$(call add-order,host-pkg-config,host-libarchive)

$(call add-order,bash,termcap)
$(call add-order,checksystem,flex)
$(call add-order,coreutils,libgmp)
$(call add-order,dataagent,libcap)
$(call add-order,libelf,host-flex)
$(call add-order,libelf,flex)
$(call add-order,glib,host-dbus)
$(call add-order,glib,host-libxml2)
$(call add-order,host-glib,host-dbus)
$(call add-order,googletest,host-acl)
$(call add-order,inetutils,termcap)
$(call add-order,inetutils,readline)
$(call add-order,libarchive,e2fsprogs)
$(call add-order,host-libarchive,host-e2fsprogs)
$(call add-order,libconfig,flex)
$(call add-order,libelf,flex)
$(call add-order,libgmp,flex) #??
#$(call add-order,libpcap,autogen-tools)  ## Tests if YYTEXT is provided with autogen
$(call add-order,libpcap,host-flex)  ## Tests if YYTEXT is provided with host-flex
$(call add-order,libpcap,flex)  ## Tests if YYTEXT is provided with host-flex
$(call add-order,libpcre,host-zlib)
$(call add-order,libpcre,bzip2)
$(call add-order,libpcre,zlib)
#$(call add-order,libxml2,host-libxml2)
$(call add-order,libxml2,host-libxslt)
###$(call add-order,lighttpd,host-libuuid)
$(call add-order,lighttpd,libuuid)
$(call add-order,lighttpd,zlib)
$(call add-order,memedit,flex)
$(call add-order,memedit,host-libarchive)
#$(call add-order,nfsutils,host-e2fsprogs)
$(call add-order,nfsutils,e2fsprogs)
#$(call add-order,ntfs-3g,host-libuuid)
$(call add-order,ntfs-3g,libuuid)
$(call add-order,openssh,host-libcap)
#$(call add-order,pureftpd,host-libcap)
$(call add-order,pureftpd,libcap)
$(call add-order,rauc,host-glib)
$(call add-order,sqlite,zlib)
$(call add-order,strongswan,host-gperf)
$(call add-order,sudo,host-flex)
$(call add-order,syslogng,flex)
$(call add-order,syslogng,libcap)
$(call add-order,host-util-linux-ng,host-ncurses)

# Avoid cbm and wbm being built parallelly, to avoid clashes in .in file inclusion.
$(call add-order,wbm,cbm)

# Todo'
# host-libxslt depends on host-libxml2, but host-libxml2 uses host-libxslt's xsltproc. Is it a problem?



$(call add-order,ebtables,kernel)
$(call add-order,swconfig,kernel)
$(call add-order,libcgroup,flex)
$(call add-order,host-cmake,flex)
$(call add-order,host-cmake,host-e2fsprogs)
$(call add-order,net-snmp,e2fsprogs)
$(call add-order,net-snmp,libelf)
$(call add-order,net-snmp,openssl)
$(call add-order,net-snmp,valgrind)


#??
$(call add-order,zip,bzip2)
$(call add-order,zip,gzip)

# When building data-pfc100.ubi and data-pfc200.ubi in parallel, sometimes the build fails with errors like
# /ptxproj/configs/wago-pfcXXX/rules/image-ubi.make:43: recipe for target '/ptxproj/platform-wago-pfcXXX/images/data-pfc200.ubi' failed
# ubinize: error!: empty file "/ptxproj.2013.03.0/platform-wago-pfcXXX/images/ubi_boot.ubifs" referred from section "boot"
# As a workaround we build the images one after another.
# This will break as soon as PTXCONF_IMAGE_DATA_UBI_BOOT_FILES contains references to more than five files.
$(word 1,$(IMAGE_DATA_UBI_IMAGE)): $(word 2,$(IMAGE_DATA_UBI_IMAGE))
$(word 2,$(IMAGE_DATA_UBI_IMAGE)): $(word 3,$(IMAGE_DATA_UBI_IMAGE))
$(word 3,$(IMAGE_DATA_UBI_IMAGE)): $(word 4,$(IMAGE_DATA_UBI_IMAGE))
$(word 4,$(IMAGE_DATA_UBI_IMAGE)): $(word 5,$(IMAGE_DATA_UBI_IMAGE))
