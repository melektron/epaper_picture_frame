
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

    void dump_file(const char *_filename);

    el::retcode stream_bitmap(const char *_filename, uint16_t _width, uint16_t _height, pixel_callback _pixel_stream = nullptr);

} // namespace sd_image
