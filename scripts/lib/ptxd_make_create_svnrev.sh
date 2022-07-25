#!/bin/bash
#
# Copyright (C) 2013 by Heinrich Toews <heinrich.toews@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#


#
# Generates a string with SVN revision information
# 
# $1    suffix placed after 'svn<REV>' string
#

ptxd_get_svn_revision() {
    local suffix="$1"
    if rev=`svn info .. 2>/dev/null | grep '^Last Changed Rev'`; then
	rev=`echo $rev | awk '{print $NF}'`
	printf -- 'svn%s%s' "$rev" "$suffix" 
    elif rev=`git svn info .. 2>/dev/null | grep '^Last Changed Rev'`; then
	rev=`echo $rev | awk '{print $NF}'`
	printf -- 'svn%s%s' "$rev" "$suffix"
    else
	ptxd_bailout "Not a svn repository! Can't create '${filepath}'"
    fi
}

export -f ptxd_get_svn_revision

ptxd_get_svn_build_url() {
    local suffix="$1"
    local rev=0
    local build_date=""
    local build_url=""

    if rev=`svn info .. 2>/dev/null | grep '^Last Changed Rev'`; then
	build_url=`svn info .. 2>/dev/null | grep '^URL:'`
	build_url=`echo $build_url | awk '{print $NF}'`
	printf -- '%s' "$build_url" 
    elif
	rev=`git svn info .. 2>/dev/null | grep '^Last Changed Rev'`; then
	build_url=`git svn info .. 2>/dev/null | grep '^URL:'`
	build_url=`echo $build_url | awk '{print $NF}'`
	printf -- '%s' "$build_url"
    else
	ptxd_bailout "Not a svn repository! Can't create '${filepath}'"
    fi
}

export -f ptxd_get_svn_build_url

ptxd_get_jenkins_env() {
  local build_tag="$BUILD_TAG"
  local docker_reg="$DOCKER_REGISTRY"
  
  if [ -n "$build_tag" ]; then
    printf -- '%s' "$build_tag"
  elif [ -n "$docker_reg" ]; then
    printf -- '%s' "Unknown (Make sure that the BUILD_TAG variable is included in the envWhitelist in the build section of the jenkinsfile)"
  else
    printf -- '%s' "Localbuild"
  fi
}

export -f ptxd_get_jenkins_env
