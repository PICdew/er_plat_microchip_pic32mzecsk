/*
 * display.c
 *
 *  Created on: Jun 20, 2016
 *      Author: zoltanr
 */

#ifndef NO_DISPLAY

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <lib/error.h>
#include <lib/type.h>
#include <lib/debug.h>
#include "system_config.h"
#include "system_definitions.h"
#include <display/SSD1306.h>

#define DISPLAY_IP_PREFIX               "ip: "
#define DISPLAY_TEMPERATURE_PREFIX      "temp: "
#define DISPLAY_HUMINITY_PREFIX         "hum: "
#define DISPLAY_SN_PREFIX               "sn: "
#define DISPLAY_REMOTE_MESSAGE_PREFIX   ""

#define DISPLAY_LINE_IP               LINE1
#define DISPLAY_LINE_SN               LINE1
#define DISPLAY_LINE_TEMPERATURE      LINE1
#define DISPLAY_LINE_HUMINITY         LINE1
#define DISPLAY_LINE_REMOTE_MSG       LINE2


struct _DISPLAY_DATA
{
    unsigned char huminity_str[32];
    unsigned char temperature_str[32];
    unsigned char ip_str[32];
    unsigned char sn_str[32];
    unsigned char remote_msg_str[32];
    unsigned char* huminity;
    unsigned char* temperature;
    unsigned char* ip;
    unsigned char* sn;
    unsigned char* remote_msg;
} gDisplayData;

DISPLAY_CTRL_DATA display_ctrlData;

int32_t display_init() {

  sprintf(gDisplayData.ip_str, "%s Unknown", DISPLAY_IP_PREFIX);
  gDisplayData.ip = &gDisplayData.ip_str[strlen(DISPLAY_IP_PREFIX)];

  sprintf(gDisplayData.temperature_str, "%s Unknown", DISPLAY_TEMPERATURE_PREFIX);
  gDisplayData.temperature = &gDisplayData.temperature_str[strlen(DISPLAY_TEMPERATURE_PREFIX)];

  sprintf(gDisplayData.huminity_str, "%s Unknown", DISPLAY_HUMINITY_PREFIX);
  gDisplayData.huminity = &gDisplayData.huminity_str[strlen(DISPLAY_HUMINITY_PREFIX)];

  sprintf(gDisplayData.sn_str, "%s Unknown", DISPLAY_SN_PREFIX);
  gDisplayData.sn = &gDisplayData.sn_str[strlen(DISPLAY_SN_PREFIX)];
  
  sprintf(gDisplayData.remote_msg_str, "%s", DISPLAY_REMOTE_MESSAGE_PREFIX);
  gDisplayData.remote_msg = &gDisplayData.remote_msg_str[strlen(DISPLAY_REMOTE_MESSAGE_PREFIX)];

  return ERR_SUCCESS;
}

BOOL is_display_ready() {
    return(display_ctrlData.isDisplayHWInitialized);
}

int32_t display_print_temperature(const int temperature) {

    if(display_ctrlData.isDisplayHWInitialized != TRUE)
        return ERR_FAILURE;

    sprintf(gDisplayData.temperature, "%d", temperature);
    SSD1306_clearLine(DISPLAY_LINE_TEMPERATURE);
    SSD1306_drawString(0, DISPLAY_LINE_TEMPERATURE, gDisplayData.temperature_str);

    return ERR_SUCCESS;
}
int32_t display_print_huminity(const int huminity) {

    if(display_ctrlData.isDisplayHWInitialized != TRUE)
        return ERR_FAILURE;

    sprintf(gDisplayData.huminity, "%d", huminity);
    SSD1306_clearLine(DISPLAY_LINE_HUMINITY);
    SSD1306_drawString(0, DISPLAY_LINE_HUMINITY, gDisplayData.huminity_str);

    return ERR_SUCCESS;
}

int32_t display_print_sn() {

    if(display_ctrlData.isDisplayHWInitialized != TRUE)
        return ERR_FAILURE;
    const char* sn = platform_get_sn();
    strcpy(gDisplayData.sn, sn);
    SSD1306_clearLine(DISPLAY_LINE_SN);
    SSD1306_drawString(0, DISPLAY_LINE_SN, gDisplayData.sn_str);

    return ERR_SUCCESS;
}

int32_t display_print_ip(const IPV4_ADDR* ipAddr) {

    if(display_ctrlData.isDisplayHWInitialized != TRUE)
        return ERR_FAILURE;

    sprintf(gDisplayData.ip, "%d.%d.%d.%d", ipAddr->v[0], ipAddr->v[1], ipAddr->v[2], ipAddr->v[3]);
    SSD1306_clearLine(DISPLAY_LINE_IP);
    SSD1306_drawString(0, DISPLAY_LINE_IP, gDisplayData.ip_str);

    return ERR_SUCCESS;
}

int32_t display_print_remote_msg(const char* msg) {

    if(display_ctrlData.isDisplayHWInitialized != TRUE)
        return ERR_FAILURE;

    strcpy(gDisplayData.remote_msg, msg);
    SSD1306_clearLine(DISPLAY_LINE_REMOTE_MSG);
    SSD1306_drawString(0, DISPLAY_LINE_REMOTE_MSG, gDisplayData.remote_msg_str);

    return ERR_SUCCESS;
}

#endif