#######################################################################################################################
# This file is part of project nullderef (PTXdist package nullderef).
#
# For further information about the PTXdist project see the README file.
# For further information about license conditions see the LICENSE file.
#######################################################################################################################

obj-m = nullderef.o


ifeq ($(PTX_KERNEL_DIR),)

all:
	@echo "please export PTX_KERNEL_DIR"

host:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd)/ modules

clean:
	rm -rf *.o *.ko *.kmod *.mod.c .*.cmd .*.o.d Module.symvers .tmp_versions modules.order

else

modules modules_install clean:
	$(MAKE) -C $(PTX_KERNEL_DIR) M=$(CURDIR) V=$(V) $@

endif
