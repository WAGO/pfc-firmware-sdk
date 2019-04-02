#ifndef __CONF_H__
#define __CONF_H__

typedef enum {

	oldaskconfig,
	silentoldconfig,
	oldconfig,
	allnoconfig,
	allyesconfig,
	allmodconfig,
	alldefconfig,
	randconfig,
	defconfig,
	savedefconfig,
	listnewconfig,
	oldnoconfig,
	writedepend,
} input_mode_t;

void check_conf(struct menu *menu);

#endif
