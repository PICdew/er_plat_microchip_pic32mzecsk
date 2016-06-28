/*
 * platform_freertos.c
 *
 *  Created on: Sep 30, 2015
 *      Author: zoltanr
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#include <system_utils.h>
#include <lib/type.h>
#include <lib/error.h>
#include <lib/debug.h>
#include <platform_freertos.h>

static char sn[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

/**
 * Return the serial number of the platform
 *
 * @return Serial number as a string
 **/
const char* platform_get_sn(void) {

    TCPIP_NET_HANDLE    netH;
    TCPIP_MAC_ADDR*     pAdd;

    if(sn[0] == 0) {
        netH = TCPIP_STACK_IndexToNet(0);
        pAdd = (TCPIP_MAC_ADDR*)TCPIP_STACK_NetAddressMac(netH);
        sprintf(sn, "%.2x%.2x%.2x%.2x%.2x%.2x", pAdd->v[0],pAdd->v[1],pAdd->v[2],pAdd->v[3],pAdd->v[4],pAdd->v[5]);
        info_log(DEBUG_PLATFORM, ("   ** Device serial: %s\n", sn));
    }

    return sn;
}

/**
 * Returns the CIK stored in the non-volatile storage of this platform
 *
 * @param[out] cik The CIK stored in non-volatile storage
 * @param[in] len  The length of the output buffer for CIK
 *
 * @return ERR_SUCCESS on success ERR_FAILURE otherwise
 **/
int platform_get_cik(char *cik, size_t len) {

    uint16_t status = 0;

    status = platform_nvm_read(cik, len-1);
    if (status != ERR_SUCCESS) {
        error_log(DEBUG_PLATFORM, ("CIK can not be read\n"), ERR_FAILURE);
        return status;
    }

    cik[len-1]=0;
    if (strlen(cik) != len && strlen(cik) != CIK_LENGTH) {
        info_log(DEBUG_PLATFORM, ("There is no stored CIK\n"));
        return ERR_FAILURE;
    }

    info_log(DEBUG_PLATFORM, ("Stored CIK: %s\n", cik));
    return ERR_SUCCESS;
}

/**
 * Return the CIK stored in the non-volatile storage of this platform
 * param[in] cik The cik in C-string format
 *
 * @return ERR_SUCCESS on success ERR_FAILURE otherwise
 **/
int platform_set_cik(const char *cik) {

    int32_t status;
    char tmp[CIK_LENGTH + 1];
    int i;

    info_log(DEBUG_PLATFORM, ("Save CIK: %s\n", (cik) ? cik : "blank"));

    //status = platform_nvm_erase();
    //if (status != ERR_SUCCESS)
    //    return status;

    if (cik)
        /* Save the CIK */
        status = platform_nvm_erase_write(cik, CIK_LENGTH);
    else {
        char term = 0;
        /* Clear the CIK */
        status = platform_nvm_write(&term, 1);
    }
    if (status != ERR_SUCCESS) {
        error_log(DEBUG_PLATFORM, ("CIK can not be %s\n", cik ? "saved" : "cleared"), status);
        ASSERT(0);
        return ERR_FAILURE;
    }

    /* Verification */
    status = platform_nvm_read(tmp, CIK_LENGTH);
    if (status != ERR_SUCCESS) {
        error_log(DEBUG_PLATFORM, ("CIK is probably not %s\n", cik ? "saved" : "cleared"), status);
        ASSERT(0);
        return ERR_FAILURE;
    }

    if(memcmp ( cik, tmp, CIK_LENGTH)) {
        tmp[CIK_LENGTH] = 0;
        error_log(DEBUG_PLATFORM, ("Saved CIK is corrupt: %s\n", tmp), ERR_FAILURE);
        ASSERT(0);
        return ERR_FAILURE;
    }

    info_log(DEBUG_PLATFORM, ("CIK %s\n", cik ? "saved" : "cleared"));
    return ERR_SUCCESS;
}

/**
 * Reset the board
 * */
void platform_reset(void) {

    // TODO
}
