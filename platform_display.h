/*
 * display.h
 *
 *  Created on: Jun 20, 2016
 *      Author: zoltanr
 */

#ifndef ER_PLAT_PIC32MZ_EF_STARTER_KIT_PLATFORM_DISPLAY_H_
#define ER_PLAT_PIC32MZ_EF_STARTER_KIT_PLATFORM_DISPLAY_H_

#include "system_config.h"
#include "system_definitions.h"

int32_t display_init(void);
BOOL    is_display_ready(void);
int32_t display_print_temperature(int temperature);
int32_t display_print_huminity(int huminity);
int32_t display_print_sn(void);
int32_t display_print_ip(const IPV4_ADDR* ip);
int32_t display_print_remote_msg(const char* msg);

#endif /* ER_PLAT_PIC32MZ_EF_STARTER_KIT_PLATFORM_DISPLAY_H_ */
