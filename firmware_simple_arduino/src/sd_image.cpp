/*
ELEKTRON © 2023 - now
Written by melektron
www.elektron.work
28.09.23, 19:58

functions to read image data from SD-card
*/

#include <Arduino.h>

#include "sd_image.hpp"

File current_fileb;

el::retcode sd_image::setup()
{
    Serial.println("Initializing SD card...");

    if (!SD.begin(SD_CS_PIN))
    {
        Serial.println("initialization failed!");
        return el::retcode::err;
    }

    Serial.println("initialization done.");
    return el::retcode::ok;
}

void sd_image::dump_file(const char *_filename)
{
    File file = SD.open(_filename, FILE_READ);

    if (file.isDirectory())
    {
        Serial.println("Is directory, cannot dump!");
        file.close();
        return;
    }

    Serial.println("===== START OF FILE =====");

    while (file.available())
    {
        Serial.write(file.read());
    }

    Serial.println("\n====== END OF FILE ======");

    file.close();
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

el::retcode sd_image::stream_bitmap(const char *_filename, uint16_t _width, uint16_t _height, pixel_callback _pixel_stream)
{
    // This function opens a Windows Bitmap (BMP) file and
    // displays it at the given coordinates.  It's sped up
    // by reading many pixels worth of data at a time
    // (rather than pixel by pixel).  Increasing the buffer
    // size takes more of the Arduino's precious RAM but
    // makes loading a little faster.  20 pixels seems a
    // good balance.

#define BUFFPIXEL 20

    File bmpFile;
    int bmpWidth, bmpHeight;            // W+H in pixels
    uint8_t bmpDepth;                   // Bit depth (currently must be 24)
    uint32_t bmpImageoffset;            // Start of image data in file
    uint32_t rowSize;                   // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel buffer (R+G+B per pixel)
    uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
    boolean goodBmp = false;            // Set to true on valid header parse
    boolean flip = true;                // BMP is stored bottom-to-top
    int w, h, row, col, x2, y2, bx1, by1;
    uint8_t r, g, b;
    uint32_t pos = 0, startTime = millis();

    Serial.println();
    Serial.print(F("Loading image '"));
    Serial.print(_filename);
    Serial.println('\'');

    // Open requested file on SD card
    if ((bmpFile = SD.open(_filename, FILE_READ)) == false)
    {
        Serial.print(F("File not found"));
        return el::retcode::notfound;
    }

    // Parse BMP header
    if (read16(bmpFile) == 0x4D42)
    { // BMP signature
        Serial.print(F("File size: "));
        Serial.println(read32(bmpFile));
        (void)read32(bmpFile);            // Read & ignore creator bytes
        bmpImageoffset = read32(bmpFile); // Start of image data
        Serial.print(F("Image Offset: "));
        Serial.println(bmpImageoffset, DEC);
        // Read DIB header
        Serial.print(F("Header size: "));
        Serial.println(read32(bmpFile));
        bmpWidth = read32(bmpFile);
        bmpHeight = read32(bmpFile);
        if (read16(bmpFile) != 1) // # planes -- must be '1'
        {
            Serial.println("Invalid number of planes in BMP!");
            return el::retcode::invalid;
        }
   
        bmpDepth = read16(bmpFile); // bits per pixel
        Serial.print(F("Bit Depth: "));
        Serial.println(bmpDepth);

        if (bmpDepth != 24) // must be 24 bits/pixel
        {
            Serial.println("Invalid bit-depth!");
            return el::retcode::invalid;
        }

        if (read32(bmpFile) != 0) // 0 = uncompressed
        {
            Serial.println("Compressed BMP not supported!");
            return el::retcode::invalid;
        }

        goodBmp = true; // Supported BMP format -- proceed!

        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0)
        {
            bmpHeight = -bmpHeight;
            flip = false;
        }

        // Crop area to be loaded
        x2 = bmpWidth - 1; // Lower-right corner
        y2 = bmpHeight - 1;
        if ((x2 >= 0) && (y2 >= 0))
        { // On screen?
            w = bmpWidth;               // Width/height of section to load/display
            h = bmpHeight;
            bx1 = by1 = 0; // UL coordinate in BMP file

            for (row = 0; row < h; row++)
            { // For each scanline...

                // Seek to start of scan line.  It might seem labor-
                // intensive to be doing this on every line, but this
                // method covers a lot of gritty details like cropping
                // and scanline padding.  Also, the seek only takes
                // place if the file position actually needs to change
                // (avoids a lot of cluster math in SD library).
                if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                    pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
                else // Bitmap is stored top-to-bottom
                    pos = bmpImageoffset + (row + by1) * rowSize;
                pos += bx1 * 3;                  // Factor in starting column (bx1)
                if (bmpFile.position() != pos)
                { // Need seek?
                    bmpFile.seek(pos);
                    buffidx = sizeof(sdbuffer); // Force buffer reload
                }
                for (col = 0; col < w; col++)
                { // For each pixel...
                    // Time to read more pixel data?
                    if (buffidx >= sizeof(sdbuffer))
                    { // Indeed
                        bmpFile.read(sdbuffer, sizeof(sdbuffer));
                        buffidx = 0; // Set index to beginning
                    }
                    // Convert pixel from BMP to EPD format, push to display
                    b = sdbuffer[buffidx++];
                    g = sdbuffer[buffidx++];
                    r = sdbuffer[buffidx++];
                    uint32_t color = r;
                    color <<= 8;
                    color |= g;
                    color <<= 8;
                    color |= b;

                    (*_pixel_stream)(color);
                } // end pixel
            }   // end scanline
        }     // end onscreen

        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
    }

    bmpFile.close();

    if (!goodBmp)
    {
        Serial.println(F("BMP format not recognized."));
        return el::retcode::err;
    }

    return el::retcode::ok;
}