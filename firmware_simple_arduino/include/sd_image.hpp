
/*
ELEKTRON © 2023 - now
Written by melektron
www.elektron.work
28.09.23, 19:58

functions to read image data from SD-card
*/

#include <el/retcode.hpp>
#include <SD.h>


#define SD_CS_PIN 4
#define SD_BITMAP_FOLDER "epd_imgs"
#define SD_RNG_CONTEXT "rng"

namespace sd_image
{
    /**
     * @brief function type for the pixel stream callback
     */
    using pixel_callback = void (*)(uint32_t);

    /**
     * @brief initializes the SD card
     * 
     * @retval ok - success
     * @retval err - couldn't initialize SD card
     */
    el::retcode setup();

    /**
     * @brief dumps the file content to the serial port
     * 
     * @param _filename name of file to dump
     */
    void dump_file(const char *_filename);

    /**
     * @brief generates a random number with the pseudo RNG context
     * being stored on the SD card, so the number stays "random" even
     * throughout power cycles.
     * 
     * @param _max upper boundary of numbers (exclusive)
     * @return int32_t random number between 0 and _max
     */
    uint32_t static_random_number(uint32_t _max);

    /**
     * @brief selects a random image file from the image folder
     * 
     * @param _filename_buffer buffer to write the file name to
     * @param _buffer_len length of the buffer (total length, including null termination)
     * @return el::retcode 
     */
    el::retcode select_random_image_file(char *_filename_buffer, size_t _buffer_len);

    /**
     * @brief reads a bitmap and checks if the format is correct. Then it calls
     * the provided stream handler for every pixel from the top down, left to right.
     * 
     * @param _filename name of bitmap to read
     * @param _width width of target screen (must match bitmap)
     * @param _height height of target screen (must match bitmap)
     * @param _pixel_stream callback stream handler
     * @return el::retcode 
     */
    el::retcode stream_bitmap(const char *_filename, uint16_t _width, uint16_t _height, pixel_callback _pixel_stream = nullptr);

} // namespace sd_image
