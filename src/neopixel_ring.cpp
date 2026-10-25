#include <esp_log.h>
#include <driver/gpio.h>

#include "hal.h"
#include "logger.h" // to set log levels
#include "neopixel.h"
#include "neopixel_ring.h"

#define TAG "RING"

#define NEOPIXEL_ENABLE_SECOND_RING 0 // to enable 2nd ring, also disable the Dotmatrix in main.cpp

#define NEOPIXEL_ENABLE_OUTPUT_EVERY_WRITE 0

#if (1 == 1)
//-----------------
//  9x RGBW ring
//-----------------
NeopixelDriver<PixelType::GRBW_SEQ3> npx;
// NeopixelDriver<PixelType::GRBW_SEQ4> npx;
//   NeopixelDriver<static_cast<PixelType>(666)> npx;

inline constexpr PixelColor neopixelBackgroundColor = {.color = {.b = 0x03, .g = 0, .r = 0, .w = 0}}; // dimmed Blue
                                                                                                      // inline constexpr PixelColor neopixelColored = {.color = {.b = 0, .g = 0, .r = 0, .w = 0x03}};         // dimmed White
inline constexpr PixelColor neopixelColored = {.color = {.b = 0x30, .g = 0x20, .r = 0, .w = 0}};      // fairly brigh cyan, should be dimmed
// inline constexpr PixelColor neopixelColored = {.color = {.b = 0, .g = 0, .r = 0x28, .w = 0}}; // fairly red, should be dimmed

// #define PIXEL_COUNT (1 + 8 + 12 + 16 + 24 + 32 + 40 + 48 + 60) // 1 assembly 9 rings in total
#define PIXEL_COUNT (1 + 8 + 12 + 16 + 24 + 32 + 40 + 48 + 60 - 1) // test: 1 pixel less
// #define PIXEL_COUNT (1)
#else
//-----------------
//  3x RGB ring
//-----------------
NeopixelDriver<PixelType::GRB_SEQ3> npx;
inline constexpr PixelColor neopixelBackgroundColor = {.color = {.b = 0x03, .g = 0, .r = 0, .w = 0}}; // dimmed Blue
inline constexpr PixelColor neopixelColored = {.color = {.b = 0, .g = 0, .r = 0x10, .w = 0}};         // dimmed Red

// #define PIXEL_COUNT (60 + 24 + 1 + 8 + 12 + 16 + 24 + 32) // 1 ring of 60, 1 ring of 24, 1 assembly of 6 rings
#define PIXEL_COUNT (60 + 24 + 1 + 8 + 12 + 16 + 24 + 32 - 1) // test: 1 pixel less
// #define PIXEL_COUNT (60 + 24 + 1 + 8 + 12 + 16 + 24 + 32 + 1) // test: 1 pixel more
// #define PIXEL_COUNT 61
#endif

void killNeopixelRing(void) {
    ESP_LOGI(TAG, "Killing Neopixel ring");
    npx.~NeopixelDriver();
}

void allBlackNeopixelRing(void) {              // for console
    npx.setAllPixels(neopixelBackgroundColor); //@@@TODO: change to neopixelBlack
    npx.show();
}

void rotateLeftRing(void) {
    npx.rotateAllLeft();
    npx.show();
}

void rotateRightRing(void) {
    npx.rotateAllRight();
    npx.show();
}

#if (NEOPIXEL_ENABLE_SECOND_RING)
/*
=========================================================== ========================================
    Second NeoPixel ring
===================================================================================================
*/
NeopixelDriver<PixelType::GRB_SEQ3> npx2;

inline constexpr PixelColor neopixelBackgroundColor2 = {.color = {.b = 0x10, .g = 0, .r = 0, .w = 0}}; // dimmed Blue
inline constexpr PixelColor neopixelColored2 = {.color = {.b = 0, .g = 0, .r = 0x20, .w = 0}};         // fairly bright Red, should be dimmed

// #define PIXEL2_COUNT (60 + 24 + 1 + 8 + 12 + 16 + 24 + 32) // 1 ring of 60, 1 ring of 24, 1 assembly of 6 rings
#define PIXEL2_COUNT (60 + 24 + 1 + 8 + 12 + 16 + 24 + 32 - 1) // test: 1 pixel less
// #define PIXEL2_COUNT (60 + 24 + 1 + 8 + 12 + 16 + 24 + 32 + 1) // test: 1 pixel more
// #define PIXEL2_COUNT 61

bool startNeopixelRing2(void) {
    gpio_num_t dataPin = hal.get_spi_MOSI_pin();

    if (dataPin == GPIO_NUM_NC) {
        ESP_LOGE(TAG, "Neopixel data pin is not configured");
        return (false);
    }

    ESP_LOGI(TAG, "Initializing second NeoPixel ring on pin=%d with %d pixels", dataPin, PIXEL2_COUNT);
    npx2.begin(PIXEL2_COUNT, dataPin);
    //@@@TODO: error handling

#if (NEOPIXEL_ENABLE_OUTPUT_EVERY_WRITE == 0) //@@@TODO: do not enable/disable for every write, will give CONFLICT between ring1 and ring2
    hal.setNeoPixelEnable(true);              // enable the data output
#endif

    npx2.setAllPixels(neopixelBlack); // set all pixels to black
    npx2.show();                      // send the data to the Neopixel ring
    return (true);
}
#endif

bool startNeopixelRing(void) {
    setLogLevel("NPIX", ESP_LOG_DEBUG);
    setLogLevel("I2S_", ESP_LOG_DEBUG);
    setLogLevel("RING", ESP_LOG_DEBUG);

    gpio_num_t dataPin = hal.get_neopixel_data_pin();

    if (dataPin == GPIO_NUM_NC) {
        ESP_LOGE(TAG, "Neopixel data pin is not configured");
        return (false);
    }

    ESP_LOGI(TAG, "Initializing Neopixel ring on pin=%d with %d pixels", dataPin, PIXEL_COUNT);
    npx.begin(PIXEL_COUNT, dataPin);
    //@@@TODO: error handling

    if (npx.isRotatable()) {
        ESP_LOGI(TAG, "Neopixel ring is rotatable");
    } else {
        ESP_LOGI(TAG, "Neopixel ring is NOT rotatable");
    }

#if (NEOPIXEL_ENABLE_OUTPUT_EVERY_WRITE == 0)
    hal.setNeoPixelEnable(true); // enable the data output
#endif

    npx.setAllPixels(neopixelBlack); // set all pixels to black
    npx.show();                      // send the data to the Neopixel ring
#if (NEOPIXEL_ENABLE_SECOND_RING)
    startNeopixelRing2();
#endif
    return (true);
}

void logNeopixelStatistics(void) {
#if (ENABLE_I2S_TASK_VERSION)
    ESP_LOGI(TAG, "minimumFreeStack=%u", npx.stats.minimumFreeStack);
#endif
    ESP_LOGI(TAG, "maxNrChunksSent=%u, maxSendMicros=%lld", npx.stats.maxNrChunksSent, npx.stats.maxSendMicros);
    ESP_LOGI(TAG, "nrTimeouts=%u, nrPreloadDataErrors=%u, nrChannelErrors=%u", npx.stats.nrTimeouts, npx.stats.nrPreloadDataErrors, npx.stats.nrChannelErrors);
}

/*
===============================================================================
    Check and log the Neopixel statistics
===============================================================================
*/
static void statistics(unsigned long currentMillis) {
    static int loopStartMillis = 0;
    static int maxMillisPerLoop = 0;

    //---------------------------------------------------------------------------------------------
    // Ring loop speed
    //---------------------------------------------------------------------------------------------
    if (loopStartMillis != 0) {
        int loopMillis = currentMillis - loopStartMillis;
        if (loopMillis > maxMillisPerLoop) {
            maxMillisPerLoop = loopMillis;
            ESP_LOGI(TAG, "Max millis per ring loop = %d", maxMillisPerLoop);
        }
    }
    loopStartMillis = currentMillis;

    //---------------------------------------------------------------------------------------------
    // Maximum time taken to send data to the Neopixels
    //
    // Initially short (approx 200 us)
    // Will increase when you try to send faster than the Neopixel transmission rate,
    // because the Driver needs to wait for the previous transmission to complete
    //---------------------------------------------------------------------------------------------
    if (esp_log_level_get("I2S_") < ESP_LOG_DEBUG) {
        // Driver is not logging it, do it here
        static int64_t loggedMaxSendMicros = 0;
        if (npx.stats.maxSendMicros > loggedMaxSendMicros) {
            loggedMaxSendMicros = npx.stats.maxSendMicros;
            ESP_LOGI(TAG, "maxSendMicros=%lld", loggedMaxSendMicros);
        }
    } // else: don't log twice

    //---------------------------------------------------------------------------------------------
    // Used chunks
    //
    // Usually same as the configured number of chunks (2 or more)
    // Will increase on certain ESP32xx variants when sending fast to a small number of Neopixels,
    // because the Driver may need to send multiple dummy flush chunks until it is done
    //---------------------------------------------------------------------------------------------
    static int loggedMaxChunksSent = 0;
    if (npx.stats.maxNrChunksSent > loggedMaxChunksSent) {
        loggedMaxChunksSent = npx.stats.maxNrChunksSent;
        ESP_LOGI(TAG, "maxNrChunksSent=%d", loggedMaxChunksSent);
    }

    //---------------------------------------------------------------------------------------------
    // Preload data errors
    //
    // Never seen this happen in practice
    //---------------------------------------------------------------------------------------------
    static int loggedNrPreloadDataErrors = 0;
    if (npx.stats.nrPreloadDataErrors > loggedNrPreloadDataErrors) {
        loggedNrPreloadDataErrors = npx.stats.nrPreloadDataErrors;
        ESP_LOGI(TAG, "nrPreloadDataErrors=%d", loggedNrPreloadDataErrors);
    }
    //---------------------------------------------------------------------------------------------
    // Channel errors
    //
    // Never seen this happen in practice
    //---------------------------------------------------------------------------------------------
    static int loggedNrChannelErrors = 0;
    if (npx.stats.nrChannelErrors > loggedNrChannelErrors) {
        loggedNrChannelErrors = npx.stats.nrChannelErrors;
        ESP_LOGI(TAG, "nrChannelErrors=%d", loggedNrChannelErrors);
    }

    //---------------------------------------------------------------------------------------------
    // Timeouts
    //
    // Only seen this one happen in case the dataPin was not configured correctly
    //---------------------------------------------------------------------------------------------
    static int loggedNrTimeouts = 0;
    if (npx.stats.nrTimeouts > loggedNrTimeouts) {
        loggedNrTimeouts = npx.stats.nrTimeouts;
        ESP_LOGI(TAG, "nrTimeouts=%d", loggedNrTimeouts);
    }

#if (ENABLE_I2S_TASK_VERSION)
    //---------------------------------------------------------------------------------------------
    // Free room in the Task stack
    //
    // Should remain at least 100 bytes, ESP32xx will reboot when the stack is exhausted
    // Depends a lot on the ESP32xx variant, Task on ESP32-S3 uses the most stack
    //---------------------------------------------------------------------------------------------
    static UBaseType_t loggedMinFreeTaskStack = UINT_MAX;
    if (npx.stats.minimumFreeStack < loggedMinFreeTaskStack) {
        loggedMinFreeTaskStack = npx.stats.minimumFreeStack;
        ESP_LOGI(TAG, "minimumFreeStack=%u", loggedMinFreeTaskStack);
    }
#endif
}

void animateSinglePixel(unsigned long currentMillis) {
    static PixelColor pixel = {value : 0};
    static int colorMode = 0;

    switch (colorMode) {
    case 0:
        if (pixel.color.r++ >= 64) {
            pixel.color.r = 0;
            colorMode = 1; // switch to the next color mode
        }
        break;
    case 1:
        if (pixel.color.g++ >= 64) {
            pixel.color.g = 0;
            colorMode = 2; // switch to the next color mode
        }
        break;
    default:
        if (pixel.color.b++ >= 64) {
            pixel.color.b = 0;
            colorMode = 0; // switch to the next color mode
            statistics(currentMillis);
        }
        break;
    }
    npx.setPixel(0, pixel);
    npx.show(); // send the data to the Neopixel ring
}

void movingPixel(unsigned long currentMillis) {
    static int coloredIndex = 0;
    static int blackIndex = PIXEL_COUNT - 1;

    npx.setPixel(blackIndex, neopixelBlack);     // erase previously colored pixel
    npx.setPixel(coloredIndex, neopixelColored); // set new colored pixel
    npx.show();

#if (NEOPIXEL_ENABLE_SECOND_RING)
    npx2.setPixel(blackIndex / 2, neopixelBlack);      // erase previously colored pixel
    npx2.setPixel(coloredIndex / 2, neopixelColored2); // set new colored pixel for the second ring
    npx2.show();
#endif

    // Update the pixel indexes for the next iteration
    blackIndex = coloredIndex;
    if (++coloredIndex >= PIXEL_COUNT) {
        // New loop
        coloredIndex = 0;

        statistics(currentMillis);
    }
}

uint16_t readPoti(void) {
    static uint32_t avg = 0; // 32 bit integer is easily large enough to max 3300 (12 bits) + 4x shift (avg << 4)

    avg = ((avg << 4) - avg + hal.readAnalog()) >> 4; // 0...3300 (mV)

    return (avg);
}

/*
=========================================================================================
    Loop
=========================================================================================
*/
void loopNeopixelRing(unsigned long currentMillis) {
    //---------------------------------------------------------------
    //  Throttle the ring updates for better visibility (if needed)
    //---------------------------------------------------------------
    static unsigned long timeoutMillis = 0;
    if ((long)(currentMillis - timeoutMillis) < 0) {
        return;
    }
    timeoutMillis = currentMillis + 0; // update the timeout for the next iteration

    //---------------------------------------------------------------
    //  Adjust brightness based on potentiometer reading
    //---------------------------------------------------------------
    static unsigned long readPotiTimeoutMillis = 0;

    if ((long)(currentMillis - readPotiTimeoutMillis) >= 0) {
        static uint16_t potiValue = 0;
        uint16_t newPotiValue = readPoti();
        if (newPotiValue != potiValue) {
            npx.brightness = ((unsigned)potiValue * 255) / 3300; // scale 0...3300 to 0...64
#if (NEOPIXEL_ENABLE_SECOND_RING)
            npx2.brightness = npx.brightness;
#endif
            ESP_LOGD("Neopixel", "Poti=%u, brightness=%u", potiValue, npx.brightness);
            potiValue = newPotiValue;
        }
        readPotiTimeoutMillis = currentMillis + 10;
    }

    //---------------------------------------------------------------
    //  Animate the Neopixel ring
    //---------------------------------------------------------------
    if (PIXEL_COUNT == 1) {
        animateSinglePixel(currentMillis);
    } else {
        movingPixel(currentMillis);
    }
}
