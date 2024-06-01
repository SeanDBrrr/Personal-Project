// C/C++ Headers
#include "stdio.h"
#include "cstring"
// Other Posix Headers
// ESP IDF Headers
#include "esp_log.h"
#include <esp_err.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
// Component Headers
#include "max7219.h"
#include "Blink.h"
// Public Headers
// Private Headers

#ifdef BLINK
extern "C"
{
  void app_main()
  {
    // Start the blink task
    // Example: Blink an LED on GPIO 2 with a 1000ms delay
    blink(GPIO_NUM_2, 1000);
  }
}
#endif

#ifndef BLINK
//#define SPI_HOST
#define DMA_CHAN    2

#define PIN_NUM_CS  GPIO_NUM_23
#define PIN_NUM_CLK GPIO_NUM_5
#define PIN_NUM_DIN GPIO_NUM_19

extern "C"
{
void app_main(void)
{

    // Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_DIN,
        .miso_io_num = -1, // MISO is not used
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1, // Initialize data4_io_num to -1 to avoid the warning
        .data5_io_num = -1, 
        .data6_io_num = -1, 
        .data7_io_num = -1, 
        .max_transfer_sz = 32,
        .flags = 0,
        .intr_flags = 0
    };

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST, &buscfg, DMA_CHAN));

    // Initialize the MAX7219 device descriptor
    max7219_t dev;
    ESP_ERROR_CHECK(max7219_init_desc(&dev, SPI_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS));

    // Set the number of cascaded devices (1 for a single 8x8 matrix)
    dev.cascade_size = 1;
    dev.digits = 8;

    // Initialize the MAX7219 device
    ESP_ERROR_CHECK(max7219_init(&dev));

    // Set brightness
    ESP_ERROR_CHECK(max7219_set_brightness(&dev, 4)); // Set brightness level (0-15)

    // Clear display
    ESP_ERROR_CHECK(max7219_clear(&dev));

    // Draw text on the 8x8 LED matrix
    ESP_ERROR_CHECK(max7219_draw_text_7seg(&dev, 0, "HELLO"));

    // Define a simple 8x8 smiley face pattern
    uint64_t smiley_face = 0x3c42a581a599423c;
    
    // Draw the smiley face on the 8x8 LED matrix
    ESP_ERROR_CHECK(max7219_draw_image_8x8(&dev, 0, &smiley_face));

    // Example of drawing text after a delay
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_ERROR_CHECK(max7219_clear(&dev));
    ESP_ERROR_CHECK(max7219_draw_text_7seg(&dev, 0, "ESP32"));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Free the MAX7219 device descriptor
    ESP_ERROR_CHECK(max7219_free_desc(&dev));

    // Free the SPI bus
    ESP_ERROR_CHECK(spi_bus_free(SPI_HOST));
}
}

#endif