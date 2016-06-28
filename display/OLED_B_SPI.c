/*
 * Project name:
      OLED B Click (SPI)
 * Copyright:
      (c) mikroElektronika, 2014.
 * Revision History:
      20140918:
      - Initial release (BD);
 * Description:
      OLED B click carries a 96 x 39px blue monochrome passive matrix OLED display.
      The display is bright, has a wide viewing angle and low power consumption.
      To drive the display, OLED B click features an SSD1306 controller.
      It's built-in functionalities include contrast control, normal or inverse image display,
      vertical and horizontal scrolling functions and more.
      OLED B click can communicate with the target board MCU either through SPI or I2C mikroBUS lines.
      You switch between output options by resoldering the onboard SEL COMM jumpers (J1, J2 and J3) to the appropriate position.
      OLED B click uses a 3.3V power supply.

      Description of some terms: OLED M - OLED Monochrome module, OLED B - OLED Blue, OLED W - OLED White, OLED C - OLED Color

 Test configuration:
     MCU:             P32MX795F512L
                      http://ww1.microchip.com/downloads/en/DeviceDoc/61156F.pdf
     Dev.Board:       EasyPIC Fusion v7
                      http://www.mikroe.com/easypic-fusion/
     Oscillator:      XT-PLL, 80.000MHz
     ext. modules:    OLED B click
                      http://www.mikroe.com/click/oled-b/
     SW:              mikroC PRO for PIC32
                      http://www.mikroe.com/mikroc/pic32/
                      
 NOTES:
     - Place OLED B Click board in the mikroBUS socket 1.
     - This code example is for SPI protocol. Set SEL COMM jumpers (J1, J2 and J3) to the appropriate position for SPI.
 */

//#include "Oled_M.h"
#include "SSD1306.h"
#include "Oled_font.h"
#include <lib/type.h>
#include <exosite_api.h>
#include "display_ctrl.h"

uint8_t buffer[96 * 64 / 8];
int myTextAlignment = TEXT_ALIGN_LEFT;
int myColor = WHITE;
unsigned char lastChar;
const char *myFontData = Droid_Serif_Plain_10; //ArialMT_Plain_10;

extern DISPLAY_CTRL_DATA display_ctrlData;

unsigned char pic[] = {
    0xFF, 0xFF, 0x3F, 0x1F, 0x0F, 0x07, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x83, 0xC3, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xE3, 0xE3, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x83, 0xC3, 0xE3, 0x63, 0x63, 0x63, 0x63, 0x63, 0xE3, 0xE3, 0x03, 0x03, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xE3, 0xE3, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0xE3, 0xE3, 0x03, 0x03, 0x03, 0x83, 0xC3, 0xE3, 0x63, 0x63, 0x63, 0x63, 0xC3, 0xC3, 0x83,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x1F, 0x3F, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF3, 0xFF, 0x8C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7F, 0xFF, 0xC1, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x00, 0x00, 0x3E, 0xFF, 0xC1, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC1, 0xFF,
    0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x81, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x83, 0x83, 0x03, 0x00, 0x80, 0x83,
    0x03, 0x03, 0xC3, 0xC3, 0xC3, 0x83, 0x03, 0x03, 0x03, 0x00, 0x00, 0x83, 0xC3, 0xC3, 0xC3, 0x03,
    0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0xFF, 0xFF, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0C, 0x0C, 0x1C, 0xF8, 0xF0, 0x00, 0x0C, 0x0C,
    0x18, 0xFC, 0xFC, 0x00, 0x00, 0x04, 0x8C, 0xF8, 0x70, 0xE0, 0xFF, 0xFF, 0x00, 0x00, 0xFD, 0xFD,
    0x00, 0x00, 0xFF, 0xFF, 0x03, 0x1F, 0xFC, 0xC0, 0x00, 0xC0, 0xFC, 0x1F, 0x03, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0C, 0x3C, 0xF0, 0xC0, 0x00, 0xC0, 0xF0, 0x3C, 0x0C, 0x00, 0xF0, 0xF8,
    0x1C, 0x0C, 0x0C, 0x18, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0xFE, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x80, 0x80, 0x80, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
    0x86, 0x87, 0x87, 0x80, 0x80, 0x81, 0x83, 0x87, 0x86, 0x86, 0x87, 0x83, 0x81, 0x80, 0x80, 0x80,
    0x80, 0x87, 0x87, 0x80, 0x80, 0x86, 0x87, 0x81, 0x80, 0x80, 0x87, 0x87, 0x80, 0x80, 0x87, 0x87,
    0x80, 0x80, 0x87, 0x87, 0x80, 0x80, 0x81, 0x87, 0x86, 0x87, 0x81, 0x80, 0x80, 0x87, 0x87, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x87, 0x9F, 0xBF, 0xB1, 0xB0, 0x80, 0x80, 0x81, 0x83,
    0x87, 0x86, 0x86, 0x83, 0x87, 0x87, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFE, 0xFF
};

int getStringWidth(unsigned char * text)
{
    int value;
    value = (int)(*(myFontData+WIDTH_POS)) * (int)strlen((const char *) text);
    
    return  value;
    
}
void OLED_M_data(unsigned char value);
void OLED_M_command(unsigned char value);
void Set_Page_Address(unsigned char add);
void setFont (const char * font)
{
    myFontData = (const char*) font;
}
void SSD1306_display(void) {
    
    unsigned char i, j;
    for (i = 0; i < 0x08; i++)
    {
        Set_Page_Address(i);
        // Set_Column_Address(0x00);
        OLED_M_command(0x10);
        OLED_M_command(0x40);
        for (j = 0; j < 0x60; j++)
        {
            OLED_M_data(buffer[i * 0x60 + (0x5f-j)]);
        }
    }
    
    
//    OLED_M_command(COLUMNADDR);
//    OLED_M_command(0x0);
//    OLED_M_command(0x0);
////    OLED_M_command(0x7F);
//
//    OLED_M_command(PAGEADDR);
//    OLED_M_command(0x0);
//    OLED_M_command(0x0);
////    OLED_M_command(0x7);
//    uint16_t i;
//    for ( i=0; i<(96*39/8); i++) {
//      // send a bunch of data in one xmission
//      uint8_t x;
//      for ( x=0; x<16; x++) {
//        OLED_M_data(buffer[i]);
//        i++;
//      }
//      i--;
//      
//    }
//

}


void SSD1306_setPixel(int x, int y) {
  if (x >= 0 && x < 96 && y >= 0 && y < 64) {

     switch (myColor) {
      case WHITE:   buffer[x + (y/8)*96] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x + (y/8)*96] &= ~(1 << (y&7)); break;
      case INVERSE: buffer[x + (y/8)*96] ^=  (1 << (y&7)); break;
    }
  }
}

void SSD1306_clearPixel(int x, int y) {
  if (x >= 0 && x < 96 && y >= 0 && y < 64) {

     switch (myColor) {
      case WHITE:   buffer[x + (y/8)*96] &= ~(1 << (y&7)); break;
      case BLACK:   buffer[x + (y/8)*96] |= (1 << (y&7)); break;
      case INVERSE: buffer[x + (y/8)*96] ^=  (1 << (y&7)); break;
    }
  }
}


unsigned char currentByte;
  unsigned int charX, charY;
  unsigned int currentBitCount;
  unsigned int charCode;
  unsigned int currentCharWidth;
  unsigned int currentCharStartPos;
  unsigned int cursorX = 0;
  unsigned char numberOfChars;
  // iterate over string
  //int firstChar = *(myFontData + FIRST_CHAR_POS);
  unsigned char charHeight ;
  unsigned int currentCharByteNum = 0;
  unsigned int startX;
  unsigned int startY;
  
void SSD1306_drawString(int x, int y, unsigned char * text) {
  //text = utf8ascii(text);
    
  //myFontData = ArialMT_Plain_10;

    
numberOfChars = *(myFontData + CHAR_NUM_POS);
charHeight = *(myFontData + HEIGHT_POS);
startX = 0;
startY = y;
cursorX = 0;

  if (myTextAlignment == TEXT_ALIGN_LEFT) {
    startX = x;
  } else if (myTextAlignment == TEXT_ALIGN_CENTER) {
    int width = getStringWidth(text);
    startX = x - width / 2;
  } else if (myTextAlignment == TEXT_ALIGN_RIGHT) {
    int width = getStringWidth(text);
    startX = x - width;
  }
  unsigned int j;
  unsigned int lenght;
  lenght = strlen((const char *)text);
  for ( j=0; j < lenght; j++) {

    charCode = *(text+j)-0x20;

    currentCharWidth = *(myFontData + CHAR_WIDTH_START_POS + charCode);
    // Jump to font data beginning
    currentCharStartPos = CHAR_WIDTH_START_POS + numberOfChars;
    unsigned int m;
    for ( m = 0; m < charCode; m++) {

      currentCharStartPos += *(myFontData + CHAR_WIDTH_START_POS + m)  * charHeight / 8 + 1;
    }

    currentCharByteNum = ((charHeight * currentCharWidth) / 8) + 1;
    // iterate over all bytes of character
    unsigned int i;
    for ( i = 0; i < currentCharByteNum; i++) {

      currentByte = *(myFontData + currentCharStartPos + i);
      //Serial.println(String(charCode) + ", " + String(currentCharWidth) + ", " + String(currentByte));
      // iterate over all bytes of character
      
      unsigned int bite;
      for( bite = 0; bite < 8; bite++) {
         //int currentBit = bitRead(currentByte, bit);

         currentBitCount = i * 8 + bite;

         charX = currentBitCount % currentCharWidth;
         charY = currentBitCount / currentCharWidth;

         if (currentByte & (1<<bite)) {
          SSD1306_setPixel(startX + cursorX + charX, startY + charY);
         }else{
             SSD1306_clearPixel(startX + cursorX + charX, startY + charY);
         }
             

      }
      //Lucyield();
    }
    cursorX += currentCharWidth;

  }
  SSD1306_display();
}

void InitMCU()
{ //  Init MCU function
    // SPI config


}

void Delay_ms(unsigned int delay)
{
    system_sleep_ms(delay);
/*
    unsigned int i;
    for (i = 0; i <= delay*10000; i++);
*/
}

void SPI3_Write(unsigned char value, unsigned char num_of_bytes)
{

    /* Add the buffer to transmit */
    display_ctrlData.drvSPIWRBUFHandle = DRV_SPI_BufferAddWrite(display_ctrlData.drvSPIHandle,
            (SPI_DATA_TYPE *) & value, num_of_bytes, 0, 0);
    while (!(DRV_SPI_BUFFER_EVENT_COMPLETE & DRV_SPI_BufferStatus(display_ctrlData.drvSPIWRBUFHandle)));
}
//Send command to OLED m display

void OLED_M_command(unsigned char value)
{
    OLED_CS(0);
    OLED_DC(0);
    SPI3_Write(value, 1);

    OLED_CS(1);
}
//Send data to OLED m display

void OLED_M_data(unsigned char value)
{
    OLED_CS(0);
    OLED_DC(1);
    SPI3_Write(value, 1);
    OLED_CS(1);
}

void OLED_M_Init()
{
    OLED_RST(0);
    Delay_ms(1000);
    OLED_RST(1);
    Delay_ms(1000);
    OLED_M_command(SSD1306_DISPLAYOFF); //0xAE  Set OLED Display Off
    OLED_M_command(SSD1306_SETDISPLAYCLOCKDIV); //0xD5  Set Display Clock Divide Ratio/Oscillator Frequency
    OLED_M_command(0x80);
    OLED_M_command(SSD1306_SETMULTIPLEX); //0xA8  Set Multiplex Ratio
    OLED_M_command(0x27);
    OLED_M_command(SSD1306_SETDISPLAYOFFSET); //0xD3  Set Display Offset
    OLED_M_command(0x00);
    OLED_M_command(SSD1306_SETSTARTLINE); //0x40  Set Display Start Line
    OLED_M_command(SSD1306_CHARGEPUMP); //0x8D  Set Charge Pump
    OLED_M_command(0x14); //0x14  Enable Charge Pump
    // OLED_M_command(SSD1306_SETSTARTLINE | 0x0);     //A0
    OLED_M_command(SSD1306_COMSCANDEC); //0xC8  Set COM Output Scan Direction
    OLED_M_command(SSD1306_SETCOMPINS); //0xDA  Set COM Pins Hardware Configuration
    OLED_M_command(0x12);
    OLED_M_command(SSD1306_SETCONTRAST); //0x81   Set Contrast Control
    OLED_M_command(0xAF);
    OLED_M_command(SSD1306_SETPRECHARGE); //0xD9   Set Pre-Charge Period
    OLED_M_command(0x25);
    OLED_M_command(SSD1306_SETVCOMDETECT); //0xDB   Set VCOMH Deselect Level
    OLED_M_command(0x20);
    OLED_M_command(SSD1306_DISPLAYALLON_RESUME); //0xA4   Set Entire Display On/Off
    OLED_M_command(SSD1306_NORMALDISPLAY); //0xA6   Set Normal/Inverse Display
    OLED_M_command(SSD1306_DISPLAYON); //0xAF   Set OLED Display On
}

//Set page adress for Page Addressing Mode

void Set_Page_Address(unsigned char add)
{
    add = 0xb0 | add;
    OLED_M_command(add);
}
//Set column adress for Page Addressing Mode

void Set_Column_Address(unsigned char add)
{
    OLED_M_command((0x10 | (add >> 4)));
    OLED_M_command((0x0f & add));
}
//Display picture for Page Addressing Mode

void Display_Picture(unsigned char pic[])
{
    unsigned char i, j;
    for (i = 0; i < 0x05; i++)
    {
        Set_Page_Address(i);
        // Set_Column_Address(0x00);
        OLED_M_command(0x10);
        OLED_M_command(0x40);
        for (j = 0x0; j < 0x60; j++)
        {
            OLED_M_data(pic[i * 0x60 + j]);
        }
    }
}
//Set contrast control

void contrast_control(unsigned char temp)
{
    OLED_M_command(SSD1306_SETCONTRAST); //0x81   Set Contrast Control
    OLED_M_command(temp); // contrast step 1 to 256
}

void startscroll_Right(unsigned char x, unsigned char y)
{
    OLED_M_command(SSD1306_RIGHT_HORIZONTAL_SCROLL); //0x26  Right Horizontal scroll
    OLED_M_command(0X00); //dummy byte
    OLED_M_command(x); //define start page address
    OLED_M_command(0X00); //Set time interval between each scroll
    OLED_M_command(y); //Define end page address
    OLED_M_command(0X00); //dummy byte
    OLED_M_command(0XFF); //dummy byte
    OLED_M_command(SSD1306_ACTIVATE_SCROLL);
}

void startscroll_Left(unsigned char x, unsigned char y)
{
    OLED_M_command(SSD1306_LEFT_HORIZONTAL_SCROLL); //0x27  Right Horizontal scroll
    OLED_M_command(0X00); //dummy byte
    OLED_M_command(x); //define start page address
    OLED_M_command(0X00); //Set time interval between each scroll
    OLED_M_command(y); //Define end page address
    OLED_M_command(0X00); //dummy byte
    OLED_M_command(0XFF); //dummy byte
    OLED_M_command(SSD1306_ACTIVATE_SCROLL); //0x2F Activate scroll
}

void startscroll_DiagRight(unsigned char x, unsigned char y)
{
    OLED_M_command(SSD1306_SET_VERTICAL_SCROLL_AREA); //0xA3 Set Vertical Scroll Area
    OLED_M_command(0X00); //Set No. of rows in top fixed area
    OLED_M_command(SSD1306_LCDHEIGHT); //Set No. of rows in scroll area
    OLED_M_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL); //0x29 Vertical and Right Horizontal Scroll
    OLED_M_command(0X00); //dummy byte
    OLED_M_command(x); //Define start page address
    OLED_M_command(0X00); //Set time interval between each scroll
    OLED_M_command(y); //Define end page address
    OLED_M_command(0X01); //Vertical scrolling offset
    OLED_M_command(SSD1306_ACTIVATE_SCROLL); //0x2F Activate scroll
}

void startscroll_DiagLeft(unsigned char x, unsigned char y)
{
    OLED_M_command(SSD1306_SET_VERTICAL_SCROLL_AREA); //0xA3 Set Vertical Scroll Area
    OLED_M_command(0X00); //Set No. of rows in top fixed area
    OLED_M_command(SSD1306_LCDHEIGHT); //Set No. of rows in scroll area
    OLED_M_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL); //0x2A Vertical and Right Horizontal Scroll
    OLED_M_command(0X00); //dummy byte
    OLED_M_command(x); //Define start page address
    OLED_M_command(0X00); //Set time interval between each scroll
    OLED_M_command(y); //Define end page address
    OLED_M_command(0X01); //Vertical scrolling offset
    OLED_M_command(SSD1306_ACTIVATE_SCROLL); //2F Activate scroll
}

void scroll_STOP()
{
    OLED_M_command(0x2E); //0x2E deactivate scroll
}
unsigned char once = 0;

void SSD1306_clearLine(unsigned char line)
{
    unsigned char str[50];
     
    setFont(Droid_Serif_Plain_10); 
    sprintf (( char *)str,"                                 ");
    SSD1306_drawString (0,line,(unsigned char *)str);
}

void OLED_DisplayBanner()
{
    int i = 0x00;

    SSD1306_display();
    setFont(Droid_Serif_Bold_Bold_14);
    
    SSD1306_drawString (5,0,(unsigned char *)"Microchip");
   
//    unsigned char str[20],val=0;
//     
//    setFont(Droid_Serif_Plain_10); 
//    sprintf (( char *)str,"Allo %x",val++);
//    SSD1306_drawString (0,LINE1,(unsigned char *)str);
//    
//    //sprintf (( char *)str,"Allo %x",val++);
//    //SSD1306_drawString (0,LINE2,(unsigned char *)str);
//    
//    
//    
//    for (val = 10; val <=0; val--)
//    {
//        sprintf (( char *)str,"Allo %d",val);
//        SSD1306_drawString (0,LINE1,(unsigned char *)str);
//        Delay_ms(2000);
//        
//    }
//    
//    sprintf (( char *)str,"                   ");
//        SSD1306_drawString (0,LINE1,(unsigned char *)str);
    while (1)
    {

        Display_Picture(pic);
        Delay_ms(2000);
        OLED_M_command(SSD1306_INVERTDISPLAY);
        Delay_ms(2000);
        OLED_M_command(SSD1306_NORMALDISPLAY);
        Delay_ms(2000);
        return;
        
        
        for (i = 0xAF; i > 0x00; i--)
        {
            contrast_control(i);
            Delay_ms(20);
        }
        for (i = 0x00; i < 0xAF; i++)
        {
            contrast_control(i);
            Delay_ms(20);
        }
        
        startscroll_Right(0x00, 0x05);
        Delay_ms(3000);
        scroll_STOP();
        Display_Picture(pic);


        startscroll_Left(0x00, 0x05);
        Delay_ms(3000);
        scroll_STOP();
        Display_Picture(pic);

        startscroll_DiagRight(0x00, 0x05);
        Delay_ms(3000);
        scroll_STOP();
        Display_Picture(pic);

        startscroll_DiagLeft(0x00, 0x05);
        Delay_ms(3000);
        scroll_STOP();
        return;
    }
}
