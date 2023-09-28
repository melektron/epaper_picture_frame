/*
ELEKTRON © 2023 - now
Written by melektron
www.elektron.work
28.09.23, 20:13


*/

#include <Arduino.h>
#include <SPI.h>
#include <epd7in3f.h>

#include "sd_image.hpp"



// Pin definition
#define RST_PIN         8
#define DC_PIN          9
#define CS_PIN          10
#define BUSY_PIN        7

Epd epd(RST_PIN, DC_PIN, CS_PIN, BUSY_PIN);

uint8_t rgb_to_epd(uint32_t _rgb)
{
    switch (_rgb)
    {
    case 0x0006c5:
        return EPD_7IN3F_BLACK;
    case 0xffffff:
        return EPD_7IN3F_WHITE;
    case 0x34bd00:
        return EPD_7IN3F_GREEN;
    case 0x0052f7:
        return EPD_7IN3F_BLUE;
    case 0xc10000:
        return EPD_7IN3F_RED;
    case 0xdfca00:
        return EPD_7IN3F_YELLOW;
    case 0xdd5f00:
        return EPD_7IN3F_ORANGE;
    case 0xe8c7b4:
        return EPD_7IN3F_CLEAN;

    default:
        return EPD_7IN3F_WHITE;
        Serial.println("inv color");
    }
}

bool pixel_sign = false;
uint8_t first_pixel = 0;
void streamtest(uint32_t pixel)
{
    if (pixel_sign)
    {
        epd.frameStreamTwoPixels(first_pixel, rgb_to_epd(pixel));
        pixel_sign = false;
    }
    else
    {
        first_pixel = rgb_to_epd(pixel);
        pixel_sign = true;
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Starting...");

    if (sd_image::setup() == el::retcode::err)
    {
        Serial.println("Error during SD-init");
        for (;;);
    };

    if (epd.Init() != 0)
    {
        Serial.println("Error during e-Paper init");
        for (;;);
    }

    sd_image::dump_file("test");

    delay(5000);

    Serial.println("== Rendering to e-Paper");

    epd.frameStreamStart();
    if (sd_image::stream_bitmap("testimg.bmp", 800, 480, streamtest) == el::retcode::ok){
        epd.frameStreamEnd();
    }


    //Serial.println("Black...");
    //epd.Clear(EPD_7IN3F_BLACK);
//
    //delay(5000);

    //epd.Clear(EPD_7IN3F_WHITE);

    //Serial.print("Show pic\r\n ");
    //epd.EPD_7IN3F_Display_part(gImage_7in3f, 250, 150, 300, 180);
    //delay(2000);

    //Serial.println("draw 7 color block");
    //epd.EPD_7IN3F_ShowAlternatingPixels();
    //delay(2000);

    Serial.println("Done!");
    epd.Sleep();
}

void loop()
{}
