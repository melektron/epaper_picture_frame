/**
 *  @filename   :   epdif.cpp
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "epdif.h"
#include <SPI.h>

EpdIf::EpdIf(
    uint8_t _reset_pin,
    uint8_t _dc_pin,
    uint8_t _cs_pin,
    uint8_t _busy_pin)
    : reset_pin(_reset_pin),
    dc_pin(_dc_pin),
    cs_pin(_cs_pin),
    busy_pin(_busy_pin)
{};

EpdIf::~EpdIf()
{};

void EpdIf::DigitalWrite(int pin, int value)
{
    digitalWrite(pin, value);
}

int EpdIf::DigitalRead(int pin)
{
    return digitalRead(pin);
}

void EpdIf::DelayMs(unsigned int delaytime)
{
    delay(delaytime);
}

void EpdIf::SpiTransfer(unsigned char data)
{
    digitalWrite(cs_pin, LOW);
    SPI.transfer(data);
    digitalWrite(cs_pin, HIGH);
}

int EpdIf::IfInit(void)
{
    pinMode(cs_pin, OUTPUT);
    pinMode(reset_pin, OUTPUT);
    pinMode(dc_pin, OUTPUT);
    pinMode(busy_pin, INPUT);
    SPI.begin();
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

    return 0;
}

