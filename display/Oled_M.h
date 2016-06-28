/**************************************************************************************************
* File: SF_Test.h
* File Type: C - Header File
* Company: (c) mikroElektronika, 2011
* Revision History:
* Description:
*     This module contains a set of functions that are used for communication with
*     Serial Flash.
* Test configuration:
*     MCU:             STM32F107VC
*                      http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00220364.pdf
*     Dev.Board:       EasyMx PRO v7 for STM32(R) ARM(R)
*                      http://www.mikroe.com/easymx-pro/stm32/
*                      ac:tft_touchpanel
*                      ac:serial-flash
*
*     Oscillator:      HSE-PLL, 72.000MHz
*     SW:              mikroC PRO for ARM
*                      http://www.mikroe.com/mikroc/arm/
* NOTES:
*     Serial Flash use SPI bus to communicate with MCU.
*     Turn on TFT backlight on SW11.3
*     Turn on SPI lines at SW13.1, SW13.2, sw13.3 and Serial Flash CS at SW13.8
**************************************************************************************************/

#define SSD1306_96_39
#define SSD1306_LCDWIDTH                  96
#define SSD1306_LCDHEIGHT                 39

#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_SETSEGMENTREMAP 0xA1  // ja
#define SSD1306_SEGREMAP 0xA0

#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_DISPLAYON 0xAF



#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2



#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

// Global Variables
unsigned char Refresh_Cmd_Menu = 1;
//unsigned char write_array[_DATA_ARRAY_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//unsigned char read_array[_DATA_ARRAY_SIZE] = {0};

// Functions
void SF_Start(void);
void SF_Test(char *err);

//- Imported from SF_DRIVER.H ---------------------------------------------------------------------
extern void SerialFlash_init();
extern void SerialFlash_WriteEnable();
extern unsigned char SerialFlash_IsWriteBusy();
extern void SerialFlash_WriteByte(unsigned char _data, unsigned long address);
extern void SerialFlash_WriteWord(unsigned int _data, unsigned long address);
extern unsigned char SerialFlash_ReadID(void);
extern unsigned char SerialFlash_ReadByte(unsigned long address);
extern unsigned int SerialFlash_ReadWord(unsigned long address);
extern unsigned char SerialFlash_WriteArray(unsigned long address, unsigned char* pData, unsigned int nCount);
extern void SerialFlash_ReadArray(unsigned long address, unsigned char* pData, unsigned int nCount);
extern void SerialFlash_ChipErase(void);
extern void SerialFlash_ResetWriteProtection();
extern void SerialFlash_SectorErase(unsigned long address);

/**************************************************************************************************
* End of File
**************************************************************************************************/