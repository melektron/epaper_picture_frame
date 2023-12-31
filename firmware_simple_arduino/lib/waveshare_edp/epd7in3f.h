/*****************************************************************************
* | File      	:   EPD_7in3f.h
* | Author      :   Waveshare team
* | Function    :   7.3inch e-paper F
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

#ifndef __EPD_7IN3F_H__
#define __EPD_7IN3F_H__

#include "epdif.h"

// Display resolution
#define EPD_WIDTH       800
#define EPD_HEIGHT      480

#define UWORD   unsigned int
#define UBYTE   unsigned char
#define UDOUBLE  unsigned long

/**********************************
Color Index
**********************************/
#define EPD_7IN3F_BLACK   0x0	/// 000
#define EPD_7IN3F_WHITE   0x1	///	001
#define EPD_7IN3F_GREEN   0x2	///	010
#define EPD_7IN3F_BLUE    0x3	///	011
#define EPD_7IN3F_RED     0x4	///	100
#define EPD_7IN3F_YELLOW  0x5	///	101
#define EPD_7IN3F_ORANGE  0x6	///	110
#define EPD_7IN3F_CLEAN   0x7	///	111   unavailable  Afterimage

class Epd : EpdIf {
public:
    Epd(
        uint8_t _reset_pin,
        uint8_t _dc_pin,
        uint8_t _cs_pin,
        uint8_t _busy_pin,
        uint16_t _width = EPD_WIDTH,
        uint16_t _height = EPD_HEIGHT
    );
    ~Epd();
    int  Init(void);
    void EPD_7IN3F_BusyHigh(void);
    void TurnOnDisplay(void);
    void Reset(void);
    void EPD_7IN3F_Display(const UBYTE *image);
    void EPD_7IN3F_Display_part(const UBYTE *image, UWORD xstart, UWORD ystart,
        UWORD image_width, UWORD image_heigh);
    void EPD_7IN3F_Show7Block(void);
    void EPD_7IN3F_ShowAlternatingPixels(void);

    /**
     * @brief initiates a pixel stream with full resolution
     * (sends data start command)
     */
    void frameStreamStart(void);
    /**
     * @brief sends one byte of data to the display containing the color
     * of two pixels. The color must be one of the color definitions.
     * 
     * @param _p1 pixel 1 color
     * @param _p2 pixel 2 color
     */
    void frameStreamTwoPixels(uint8_t _p1, uint8_t _p2);
    /**
     * @brief ends the frame stream and shows the frame
     */
    void frameStreamEnd(void);

    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void Sleep(void);
    void Clear(UBYTE color);

private:
    unsigned long width;
    unsigned long height;
};

#endif /* EPD5IN83B_HD_H */

/* END OF FILE */
