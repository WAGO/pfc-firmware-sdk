// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


#include "cgrules.h"

#include <libcgroup.h>
#include <string.h>

#include "pthread_wrapper.h"
#include "log.h"

#define CGRULES_CONF_FILE "/etc/cgrules.conf"
#define CGCONFIG_CONF_FILE "/etc/cgconfig.conf"

static struct cgroup *default_group;
static char proc_name[4096];
static bool isRuntimeSystem = false;
static bool isCodesysV2 = false;
static bool isCodesysV3= false;

void cgrules_init()
{
	FILE* f = fopen("/proc/self/comm","r");
	if(f){
		size_t size;
		size = fread(proc_name, sizeof(char), sizeof(proc_name), f);
		if(size>0){
			if('\n'==proc_name[size-1])
				proc_name[size-1]='\0';
		}
		fclose(f);
	}

	isCodesysV2 = strcmp("plclinux_rt", proc_name) == 0;
	isCodesysV3 = strcmp("codesys3", proc_name) == 0;

	isRuntimeSystem = isCodesysV2 || isCodesysV3;

	if(!isRuntimeSystem) goto finished;

    int ret;
    if ((ret = cgroup_init()) != 0)
    {
        ERROR("libcgroup initialization failed, %s",
                cgroup_strerror(ret));
        goto finished;
    }

    cgroup_set_loglevel(CGROUP_LOG_ERROR);

    /* Ask libcgroup to load the configuration rules. */
    if ((ret = cgroup_init_rules_cache()) != 0)
    {
        ERROR("libcgroup failed to initialize rules"
                        "cache from %s. %s",
                CGRULES_CONF_FILE, cgroup_strerror(ret));
        goto finished;
    }

    /* ask libcgroup to load template rules as well */
    ret = cgroup_init_templates_cache(CGCONFIG_CONF_FILE);
    if (ret != 0)
    {
        ERROR("libcgroup failed to initialize template"
                        "rules from %s. %s",
                CGCONFIG_CONF_FILE, cgroup_strerror(ret));
        goto finished;
    }

    ret = cgroup_change_all_cgroups();
    if(ret != 0)
    {
        WARN("cgroup_change_all_cgroups() failed: %s", cgroup_strerror(ret));
    }

    default_group = cgroup_new_cgroup("/rts/def");
    ret = cgroup_get_cgroup(default_group);
    if (ret != 0)
    {
        ERROR("failed to get default group: /rts/def"
                        ": %s",
                cgroup_strerror(ret));
        goto finished;
    }

finished:
    return;
}

static void cgrules_get_task_name_by_prio(char *buffer, size_t buffer_size, int prio, int policy)
{
    if (isCodesysV2)
    {
	    snprintf(buffer, buffer_size,"cdsv2_pol%02dprio%03d", policy, prio);
    }
    if (isCodesysV3)
    {
	    snprintf(buffer, buffer_size,"cdsv3_pol%02dprio%03d", policy, prio);
    }
}

bool cgrules_is_rts_proc(void)
{
	return isRuntimeSystem;
}

void cgrules_move_to_default(pid_t pid)
{
	if (!isRuntimeSystem) return;
	DBG("%s", __func__);
	cgroup_attach_task_pid(default_group, pid);
}

void cgrules_apply_rule_by_name(const char *name, pid_t tpid)
{
	if (!isRuntimeSystem) return;
    if (strcmp(name, proc_name))
    {
    	DBG("cgrules_apply_rule_by_name: %s", name);
    	cgroup_change_cgroup_flags(0, 0, name, tpid, CGFLAG_USECACHE);
    }
}

void cgrules_apply_rule_by_prio(pid_t tpid, int prio, int policy)
{
	char prio_name[sizeof("rtsVx::pol00prio000")];
	cgrules_get_task_name_by_prio(prio_name, sizeof(prio_name), prio, policy);
	cgrules_apply_rule_by_name(prio_name, tpid);
}

