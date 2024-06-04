#ifndef LEDBLOCKDISPLAY_H
#define LEDBLOCKDISPLAY_H
#include "stdio.h"
#include "cstring"
#include <esp_err.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "IDisplay.h"
#include "max7219.h"
#include "max7219_priv.h"

const gpio_num_t PIN_NUM_CS = GPIO_NUM_23;
const gpio_num_t PIN_NUM_CLK = GPIO_NUM_5;
const gpio_num_t PIN_NUM_DIN = GPIO_NUM_19;
const int DMA_CHAN = 2;

#ifdef __cplusplus
extern "C" {
#endif

class LedBlockDisplay : IDisplay
{
private:
    max7219_t dev;
    static bool isSpiInitialized;
   
public:
    LedBlockDisplay(int matrixSize, int matrixCount = 1, int brightness = 8);
    void display(const void *image);
    ~LedBlockDisplay();
};


#ifdef __cplusplus
}
#endif
#endif