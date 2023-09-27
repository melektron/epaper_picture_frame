#include <Arduino.h>
#include <SPI.h>
//#include "imagedata.h"
#include "epd7in3f.h"



// Pin definition
#define RST_PIN         8
#define DC_PIN          9
#define CS_PIN          10
#define BUSY_PIN        7

Epd epd(RST_PIN, DC_PIN, CS_PIN, BUSY_PIN);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Starting...");

    if (epd.Init() != 0) {
        Serial.println("e-Paper init failed");
        return;
    }

    //Serial.println("Black...");
    //epd.Clear(EPD_7IN3F_BLACK);
//
    //delay(5000);

    Serial.println("e-Paper Clear");
    epd.Clear(EPD_7IN3F_WHITE);

    //Serial.print("Show pic\r\n ");
    //epd.EPD_7IN3F_Display_part(gImage_7in3f, 250, 150, 300, 180);
    //delay(2000);

    //Serial.println("draw 7 color block");
    //epd.EPD_7IN3F_ShowAlternatingPixels();
    //delay(2000);
    
    Serial.println("Done!");
    epd.Sleep();
}

void loop() {
}
