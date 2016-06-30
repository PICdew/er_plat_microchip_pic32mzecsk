/*
 * platform_demo.c
 *
 *  Created on: Jun 13, 2016
 *      Author: zoltanr
 */

#include <system_utils.h>
#include <lib/type.h>
#include <lib/error.h>
#include <lib/debug.h>
#include <platform_display.h>
#include "platform_nvm.h"

int32_t platform_init(void)
{
    int32_t status = ERR_SUCCESS;
    info_log(DEBUG_PLATFORM, ("Platform init\n"));

#ifndef NO_NVM
    /** NVP Initialization */
    status = platform_nvm_init();
    if (status != ERR_SUCCESS)
        return status;
#endif
    
#ifndef NO_DISPLAY
    status = display_init();
    if (status != ERR_SUCCESS)
        return status;
#endif
    
    info_log(DEBUG_PLATFORM, ("Platform initialized\n"));
    
    return ERR_SUCCESS;
}

