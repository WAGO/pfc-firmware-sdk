//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     rmd.c
///
///  \brief    Config tool specific main functionality for
///            reserved_area.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include "rmd/global/fpga_segments.h"
#include "rmd/global/fpga_revision.h"
#include "rmd/sysctrl/sysctrl.h"
#include <libuio.h>
#include <inttypes.h>
#include <stdio.h>
#include "rmd.h"
#include <fcntl.h>
#include <unistd.h>
#include "ctutil/log.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
static const char *RMD_uioDevName = "uio_rmd_mem";
static const char *RESET_FPGA_filename = "/sys/class/psdev/psdev0/reset";
static const char *RESET_FPGA_valueOn = "1";
static const char *RESET_FPGA_valueOff = "0";

static int RMD_uioMapMemBarNum = 0;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
bool reset_fpga(ctutil_Options_t const * const pstOptions)
{
    int fdResetFpga;
    char szReason[MAX_MESSAGE_BUFFER_SIZE];

    if((fdResetFpga = open(RESET_FPGA_filename, O_WRONLY)) < 0)
    {
        sprintf(szReason, "Error opening %s\n", RESET_FPGA_filename);
        goto error;
    }

    // assert reset
    if(-1 == write(fdResetFpga, RESET_FPGA_valueOn, sizeof(RESET_FPGA_valueOn)))
    {
        sprintf(szReason, "Can't write %s to %s.\n", RESET_FPGA_valueOn, RESET_FPGA_filename);
        goto error;
    }

    // ensure minimum reset assertion time, refer RMD data sheet for details
    usleep(1000); // TODO: Create constant value and explain the value (e.g. see data sheet).

    // release reset
    if(-1 == write(fdResetFpga, RESET_FPGA_valueOff, sizeof(RESET_FPGA_valueOff)))
    {
        sprintf(szReason, "Can't write %s to %s.\n", RESET_FPGA_valueOff, RESET_FPGA_filename);
        goto error;
    }

    // wait for RMD internal reset state machine has finished operation
    usleep(500000); // TODO: Create constant value and explain the value (e.g. see data sheet).

    return true;

error:
    CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, szReason);
    return false;
}

bool check_fpga(ctutil_Options_t const * const pstOptions)
{
    uint32_t v[2];
    char *pRMD;
    tSysCtrlInstance* pRMDSysCtr;
    char szReason[MAX_MESSAGE_BUFFER_SIZE];
    struct uio_info_t *pUioHandle = uio_find_by_uio_name((char*)RMD_uioDevName);
    if(pUioHandle == NULL)
    {
        sprintf(szReason, "Not able to find UIO device for RMD (%s)\n", RMD_uioDevName);
        goto error;
    }

    if(uio_open(pUioHandle) != 0)
    {
        sprintf(szReason, "Not able to open UIO device for RMD (%s)\n", RMD_uioDevName);
        goto error;
    }

    pRMD = uio_get_mem_map(pUioHandle, RMD_uioMapMemBarNum);

    if(pRMD == NULL)
    {
        sprintf(szReason, "Not able to get a UIO mapping of RMD's memory (%s)\n", RMD_uioDevName);
        goto error;
    }

#ifdef DEBUGPRINT
    sprintf(szReason,"RMD base address = 0x%.8x\n", (unsigned int) pRMD);
    CTUTIL_LOG_DEBUG(pstOptions->stCommonOptions.quiet,szReason);
#endif

    // Get an pointer to the SysCtrl IP.
    // In the following code, the SysCtrl IP's registers SARC0, SARC1, SARDTF and SARREV will be accessed.
    // An check of the version of the SysCtrl IP via the SysCtrl's ipimpf register is not required here,
    // because any RMD implementation will provide the previously mentioned SFRs at the same address and
    // with the same content.

    pRMDSysCtr = (tSysCtrlInstance*)(pRMD + SYSCTRL_SEGMENT);

    // check connection to RMD
    v[0] = pRMDSysCtr->stRegBankCare.sarc0.reg;
    v[1] = pRMDSysCtr->stRegBankCare.sarc1.reg;
    if((v[0] != 0x12345678UL) || (v[1] != 0xAA5555AAUL))
    {
        sprintf(szReason, "Read unexpected value from RMD SARC0/1:\n"
                "Expected SARC0: 0x12345678, actual SARC0: 0x%.8x\n"
                "Expected SARC1: 0xAA5555AA, actual SARC1: 0x%.8x\n",
                v[0], v[1]);
        goto error;
    }
#ifdef DEBUGPRINT
    sprintf(szReason,"Read value from RMD SARC0/1:\n"
        "Expected SARC0: 0x12345678, actual SARC0: 0x%.8x\n"
        "Expected SARC1: 0xAA5555AA, actual SARC1: 0x%.8x\n", v[0], v[1]);
    CTUTIL_LOG_DEBUG(pstOptions->stCommonOptions.quiet,szReason);
#endif
    for(uint_fast8_t i = 0; i < 32U; i++)
    {
        pRMDSysCtr->stRegBankCare.sarsr0.reg = (1UL << i);
        if(pRMDSysCtr->stRegBankCare.sarsr0.reg != (1UL << i))
        {
            sprintf(szReason, "Walking '1' check on RMD failed (Bit %d)", i);
            goto error;
        }
    }

    // check RMD version for compatibility with generated header files
    v[0] = pRMDSysCtr->stRegBankCare.sardtf.reg;
    v[1] = pRMDSysCtr->stRegBankCare.sarrev.reg;
    if((v[0] != SARDTF_VALUE) || (v[1] != SARREV_VALUE))
    {

        sprintf(szReason, "Unexpected RMD REF/DTF value!\n"
                "Expected SARDTF: 0x%.8x"
                ", actual SARDTF: 0x%.8x\n"
                "Expected SARREF: 0x%.8x"
                ", actual SARREF: 0x%.8x\n",
                SARDTF_VALUE, v[0], SARREV_VALUE, v[1]);

        goto error;
    }

#ifdef DEBUGPRINT
    sprintf(szReason,"RMD REF/DTF value!\n"
        "Expected SARDTF: 0x%.8x"
        ", actual SARDTF: 0x%.8x\n"
        "Expected SARREF: 0x%.8x"
        ", actual SARREF: 0x%.8x\n",
        SARDTF_VALUE, v[0], SARREV_VALUE, v[1]);
    CTUTIL_LOG_DEBUG(pstOptions->stCommonOptions.quiet,szReason);
#endif

    if(uio_close(pUioHandle) < 0)
        CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "config_rmd: error closing handle.");

    uio_free_info(pUioHandle);

#ifdef DEBUGPRINT
    CTUTIL_LOG_DEBUG(pstOptions->stCommonOptions.quiet,"RMC identified\n");
#endif

    return true;
error:
    CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, szReason);
    return false;
}

//---- End of source file ------------------------------------------------------

