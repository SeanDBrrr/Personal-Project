#ifndef LEDBLOCKDISPLAY_H
#define LEDBLOCKDISPLAY_H
#include "stdio.h"
#include "cstring"
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "IDisplay.h"
#include "max7219.h"
#include "max7219_priv.h"

const gpio_num_t PIN_NUM_CS = GPIO_NUM_5;
const gpio_num_t PIN_NUM_CLK = GPIO_NUM_18;
const gpio_num_t PIN_NUM_DIN = GPIO_NUM_23;
const int DMA_CHAN = 2;

extern const char *TAG;
extern SemaphoreHandle_t xImageSemaphore;
#ifdef __cplusplus
extern "C"
{
#endif

    class LedBlockDisplay : IDisplay
    {
    private:
        spi_host_device_t host;
        int dma_chan;
        int pin_cs;
        int pin_clk;
        int pin_din;
        max7219_t dev;

    public:
        LedBlockDisplay(spi_host_device_t host, int dma_chan, int pin_cs, int pin_clk, int pin_din);
        void init(int = 8, int = 1, uint8_t = 8) override;
        void display(void *image) override;
        ~LedBlockDisplay();
    };

    void generate_random_image_task(void *);
    void display_image_task(void *);
#ifdef __cplusplus
}
#endif
#endif