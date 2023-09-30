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


    Serial.println("Initializing SD Card...");
    if (sd_image::setup() == el::retcode::err)
    {
        Serial.println("Error during SD-init");
        for (;;);
    };

    Serial.println("Initializing EPD...");
    if (epd.Init() != 0)
    {
        Serial.println("Error during e-Paper init");
        for (;;);
    }

    //Serial.println("Dumping test file...");
    //sd_image::dump_file("test");

    //Serial.println("Dumping done, updating screen in 5sec ...");
    //delay(5000);

    Serial.println("Updating EPD...");

#define FN_BUF_LEN 50
    char fn_buffer[FN_BUF_LEN];
    if (sd_image::select_random_image_file(fn_buffer, FN_BUF_LEN) != el::retcode::ok)
    {
        Serial.println("Error while selecting image file");
        for (;;);
    }

    Serial.print("Showing image: "); Serial.println(fn_buffer);

    epd.frameStreamStart();
    if (sd_image::stream_bitmap(fn_buffer, 800, 480, streamtest) == el::retcode::ok){
        epd.frameStreamEnd();
    }

    Serial.println("Done!");

    // sleep the display
    epd.Sleep();
}

void loop()
{}
