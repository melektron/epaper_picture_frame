/*****************************************************************************
* | File      	:   EPD_7in3f.c
* | Author      :   Waveshare team
* | Function    :   7.3inch e-Paper (F)
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2022-10-21
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

#include <stdlib.h>
#include "epd7in3f.h"

Epd::~Epd()
{};

Epd::Epd(
    uint8_t _reset_pin,
    uint8_t _dc_pin,
    uint8_t _cs_pin,
    uint8_t _busy_pin,
    uint16_t _width,
    uint16_t _height
) : EpdIf(_reset_pin, _dc_pin, _cs_pin, _busy_pin),
    width(_width),
    height(_height)
{};

/******************************************************************************
function :  Initialize the e-Paper register
parameter:
******************************************************************************/
int Epd::Init(void)
{
    if (IfInit() != 0)
    {
        return -1;
    }
    Reset();
    DelayMs(20);
    EPD_7IN3F_BusyHigh();

    SendCommand(0xAA);    // CMDH
    SendData(0x49);
    SendData(0x55);
    SendData(0x20);
    SendData(0x08);
    SendData(0x09);
    SendData(0x18);

    SendCommand(0x01);
    SendData(0x3F);
    SendData(0x00);
    SendData(0x32);
    SendData(0x2A);
    SendData(0x0E);
    SendData(0x2A);

    SendCommand(0x00);
    SendData(0x5F);
    SendData(0x69);

    SendCommand(0x03);
    SendData(0x00);
    SendData(0x54);
    SendData(0x00);
    SendData(0x44);

    SendCommand(0x05);
    SendData(0x40);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x2C);

    SendCommand(0x06);
    SendData(0x6F);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x22);

    SendCommand(0x08);
    SendData(0x6F);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x22);

    SendCommand(0x13);    // IPC
    SendData(0x00);
    SendData(0x04);

    SendCommand(0x30);
    SendData(0x3C);

    SendCommand(0x41);     // TSE
    SendData(0x00);

    SendCommand(0x50);
    SendData(0x3F);

    SendCommand(0x60);
    SendData(0x02);
    SendData(0x00);

    SendCommand(0x61);
    SendData(0x03);
    SendData(0x20);
    SendData(0x01);
    SendData(0xE0);

    SendCommand(0x82);
    SendData(0x1E);

    SendCommand(0x84);
    SendData(0x00);

    SendCommand(0x86);    // AGID
    SendData(0x00);

    SendCommand(0xE3);
    SendData(0x2F);

    SendCommand(0xE0);   // CCSET
    SendData(0x00);

    SendCommand(0xE6);   // TSSET
    SendData(0x00);

    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

void Epd::EPD_7IN3F_BusyHigh(void)// If BUSYN=0 then waiting
{
    while (!DigitalRead(busy_pin))
    {
        DelayMs(1);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void)
{
    DigitalWrite(reset_pin, HIGH);
    DelayMs(20);
    DigitalWrite(reset_pin, LOW);                //module reset    
    DelayMs(1);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(20);
}

void Epd::TurnOnDisplay(void)
{
    SendCommand(0x04);  // POWER_ON
    EPD_7IN3F_BusyHigh();

    SendCommand(0x12);  // DISPLAY_REFRESH
    SendData(0x00);
    EPD_7IN3F_BusyHigh();

    SendCommand(0x02);  // POWER_OFF
    SendData(0x00);
    EPD_7IN3F_BusyHigh();
}

/******************************************************************************
function :  Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void Epd::EPD_7IN3F_Display(const UBYTE *image)
{
    unsigned long i, j;

    SendCommand(0x10);
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width / 2; j++)
        {
            SendData(image[j + width / 2 * i]);
        }
    }

    TurnOnDisplay();
}

/******************************************************************************
function :  Sends the part image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void Epd::EPD_7IN3F_Display_part(const UBYTE *image, UWORD xstart, UWORD ystart,
    UWORD image_width, UWORD image_heigh)
{
    unsigned long i, j;

    SendCommand(0x10);
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width / 2; j++)
        {
            if (i < image_heigh + ystart && i >= ystart && j < (image_width + xstart) / 2 && j >= xstart / 2)
            {
                SendData(pgm_read_byte(&image[(j - xstart / 2) + (image_width / 2 * (i - ystart))]));
            }
            else
            {
                SendData(0x11);
            }
        }
    }

    TurnOnDisplay();
}

/******************************************************************************
function :  show 7 kind of color block
parameter:
******************************************************************************/
void Epd::EPD_7IN3F_Show7Block(void)
{
    unsigned long i, j, k;
    unsigned char const Color_seven[8] =
    { EPD_7IN3F_BLACK, EPD_7IN3F_BLUE, EPD_7IN3F_GREEN, EPD_7IN3F_ORANGE,
    EPD_7IN3F_RED, EPD_7IN3F_YELLOW, EPD_7IN3F_WHITE, EPD_7IN3F_CLEAN };

    SendCommand(0x10);
    for (i = 0; i < 240; i++)
    {
        for (k = 0; k < 4; k++)
        {
            for (j = 0; j < 100; j++)
            {
                SendData((Color_seven[k] << 4) | Color_seven[k]);
            }
        }
    }

    for (i = 0; i < 240; i++)
    {
        for (k = 4; k < 8; k++)
        {
            for (j = 0; j < 100; j++)
            {
                SendData((Color_seven[k] << 4) | Color_seven[k]);
            }
        }
    }
    TurnOnDisplay();
}

void Epd::EPD_7IN3F_ShowAlternatingPixels(void)
{
    uint32_t bytes = width * (height << 1);
    int8_t color = EPD_7IN3F_BLACK;

    SendCommand(0x10);
    for (uint32_t i = 0; i < bytes; i ++)
    {
        SendData((color++ << 4) | color++);
        if (color > EPD_7IN3F_CLEAN)
            color = EPD_7IN3F_BLACK;
    }

    TurnOnDisplay();
}

/******************************************************************************
function :
      Clear screen
******************************************************************************/
void Epd::Clear(UBYTE color)
{
    SendCommand(0x10);
    for (int i = 0; i < width / 2; i++)
    {
        for (int j = 0; j < height; j++)
        {
            SendData((color << 4) | color);
        }
    }

    TurnOnDisplay();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          The only one parameter is a check code, the command would be
 *          You can use EPD_Reset() to awaken
 */
void Epd::Sleep(void)
{
    SendCommand(0x07);
    SendData(0xA5);
    DelayMs(10);
    DigitalWrite(reset_pin, 0); // Reset
}



/* END OF FILE */
