# -*-makefile-*-
#
# Copyright (C) 2020 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

ifdef PTXCONF_HOST_SYSTEM_PYTHON
SYSTEMPYTHON := python2
else
SYSTEMPYTHON := host-system-python-is-disabled
endif

ifdef PTXCONF_HOST_SYSTEM_PYTHON3
SYSTEMPYTHON3 := python3
else
SYSTEMPYTHON3 := host-system-python3-is-disabled
endif

# vim: syntax=make
