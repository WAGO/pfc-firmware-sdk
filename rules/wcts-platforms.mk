WCTS_PLATFORMCONFIG             := __UNKNOWN_PLATFORM__

ifeq ($(PTXCONF_PLATFORM), wago-pfc-adv)
WCTS_PLATFORMCONFIG             := Lin32PTXdistPac100
endif

ifeq ($(PTXCONF_PLATFORM), wago-pac100-evb)
WCTS_PLATFORMCONFIG             := Lin32PTXdistPac100EVB
endif

ifeq ($(PTXCONF_PLATFORM), wago-pfc200)
WCTS_PLATFORMCONFIG             := Lin32PTXdistPfc200
endif

ifeq ($(PTXCONF_PLATFORM), wago-pfcXXX)
WCTS_PLATFORMCONFIG             := Lin64PTXdistPfcXXX_PTXdist201711_Linaro_5_5
endif

ifeq ($(PTXCONF_PLATFORM), wago-pn-rfc)
WCTS_PLATFORMCONFIG             := Lin32PTXdistPac100
endif

ifeq ($(PTXCONF_PLATFORM), wago-src)
WCTS_PLATFORMCONFIG             := Lin32PTXdistPac100
endif


ifeq ($(PTXCONF_PLATFORM), vtp-ctp)
WCTS_PLATFORMCONFIG             := Lin32PTXdistPfcXXX
endif

