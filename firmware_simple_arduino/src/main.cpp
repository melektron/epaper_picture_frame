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


#define FN_BUF_LEN 50
char fn_buffer[FN_BUF_LEN];

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
    /* Pallette experimenting:

    // original test pallette (pal1)
    case 0x0006c5: return EPD_7IN3F_BLACK;
    case 0xffffff: return EPD_7IN3F_WHITE;
    case 0x0052f7: return EPD_7IN3F_BLUE;
    case 0x34bd00: return EPD_7IN3F_GREEN;
    case 0xc10000: return EPD_7IN3F_RED;
    case 0xdfca00: return EPD_7IN3F_YELLOW;
    case 0xdd5f00: return EPD_7IN3F_ORANGE;
    case 0xe8c7b4: return EPD_7IN3F_CLEAN;
    
    // pallette according to datasheet (pal2)
    case 0x302637: return EPD_7IN3F_BLACK;
    case 0xaeada8: return EPD_7IN3F_WHITE;
    case 0x393f68: return EPD_7IN3F_BLUE;
    case 0x306544: return EPD_7IN3F_GREEN;
    case 0x923d3e: return EPD_7IN3F_RED;
    case 0xada049: return EPD_7IN3F_YELLOW;
    case 0xa05343: return EPD_7IN3F_ORANGE;
    case 0xba8560: return EPD_7IN3F_CLEAN;

    // black and white modified for better looks (pal3)
    //case 0x302637: return EPD_7IN3F_BLACK;    // duplicate (C doesn't like that)
    case 0xe9e9e7: return EPD_7IN3F_WHITE;
    case 0x8a98f9: return EPD_7IN3F_BLUE;
    //case 0x306544: return EPD_7IN3F_GREEN;
    //case 0x923d3e: return EPD_7IN3F_RED;
    //case 0xada049: return EPD_7IN3F_YELLOW;
    //case 0xa05343: return EPD_7IN3F_ORANGE;
    //case 0xba8560: return EPD_7IN3F_CLEAN;

    // some other colors also modified (pal4)
    //case 0x302637: return EPD_7IN3F_BLACK;
    //case 0xe9e9e7: return EPD_7IN3F_WHITE;
    //case 0x8a98f9: return EPD_7IN3F_BLUE;
    case 0x458f4a: return EPD_7IN3F_GREEN;
    case 0xc74c4d: return EPD_7IN3F_RED;
    case 0xefdb54: return EPD_7IN3F_YELLOW;
    //case 0xa05343: return EPD_7IN3F_ORANGE;
    case 0xfda76c: return EPD_7IN3F_CLEAN;

    // pal5/6/7/8
    //case 0x302637: return EPD_7IN3F_BLACK;
    case 0xfafafa: return EPD_7IN3F_WHITE;
    //case 0x0052f7: return EPD_7IN3F_BLUE;
    case 0x2ea102: return EPD_7IN3F_GREEN;
    //case 0xc10000: return EPD_7IN3F_RED;
    //case 0xdfca00: return EPD_7IN3F_YELLOW;
    case 0xc97249: return EPD_7IN3F_ORANGE;
    //case 0xe8c7b4: return EPD_7IN3F_CLEAN;

    */

    // Final pallette
    case 0x302637: return EPD_7IN3F_BLACK;
    case 0xfafafa: return EPD_7IN3F_WHITE;
    case 0x0052f7: return EPD_7IN3F_BLUE;
    case 0x2ea102: return EPD_7IN3F_GREEN;
    case 0x923d3e: return EPD_7IN3F_RED;
    case 0xdfca00: return EPD_7IN3F_YELLOW;
    case 0xc97249: return EPD_7IN3F_ORANGE;
    case 0xe8c7b4: return EPD_7IN3F_CLEAN;
    
    default:
        Serial.write('i'); Serial.println(_rgb, HEX);
        return EPD_7IN3F_WHITE;
    }
}

bool pixel_sign = false;
uint8_t first_pixel = 0;
void epd_pixel_stream_receiver(uint32_t pixel)
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
    //Serial.println("Successfully initialized SD Card!");

    Serial.println("Initializing EPD...");
    if (epd.Init() != 0)
    {
        Serial.println("Error during e-Paper init");
        for (;;);
    }
    //Serial.println("Successfully initialized EPD!");

    //Serial.println("Updating screen in 5sec ...");
    //delay(5000);

    //epd.frameStreamStart();
    //if (sd_image::stream_bitmap("pal5.bmp", epd_pixel_stream_receiver) == el::retcode::ok){
    //    epd.frameStreamEnd();
    //}
    //epd.Sleep();
    //
    //Serial.println("Done temp!");
    //for (;;);

    //Serial.println("Updating EPD...");

    if (sd_image::select_random_image_file(fn_buffer, FN_BUF_LEN) != el::retcode::ok)
    {
        Serial.println("Error while selecting image file");
        for (;;);
    }

    Serial.print("Showing image: "); Serial.println(fn_buffer);

    epd.frameStreamStart();
    if (sd_image::stream_bitmap(fn_buffer, epd_pixel_stream_receiver) == el::retcode::ok){
        epd.frameStreamEnd();
    }

    Serial.println("Done!");

    // sleep the display
    epd.Sleep();

    for (;;);   // remove this to enter interactive mode
}

void loop()
{
    while (!Serial.available());
    char c = Serial.read();

    snprintf(fn_buffer, FN_BUF_LEN, "pal%1d.bmp", c - '0');

    epd.frameStreamStart();
    if (sd_image::stream_bitmap(fn_buffer, epd_pixel_stream_receiver) == el::retcode::ok){
        epd.frameStreamEnd();
    }

}
