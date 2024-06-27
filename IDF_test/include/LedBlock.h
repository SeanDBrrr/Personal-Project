// #ifndef LEDBLOCK_H
// #define LEDBLOCK_H
// #include "max7219.h"
// #include <stdio.h>
// #include <driver/spi_master.h>
// #include <driver/gpio.h>
// #include <esp_err.h>
// #include <esp_log.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>

// #ifdef __cplusplus
// extern "C" {
// #endif

// class LedBlock {
// private:
//     spi_host_device_t host;
//     int dma_chan;
//     int pin_cs;
//     int pin_clk;
//     int pin_din;
//     max7219_t dev;

// public:
//     LedBlock(spi_host_device_t host, int dma_chan, int pin_cs, int pin_clk, int pin_din)
//         : host(host), dma_chan(dma_chan), pin_cs(pin_cs), pin_clk(pin_clk), pin_din(pin_din) {}

//     void init(int matrix) {
//         spi_bus_config_t buscfg = {
//             .mosi_io_num = pin_din,
//             .miso_io_num = -1,
//             .sclk_io_num = pin_clk,
//             .quadwp_io_num = -1,
//             .quadhd_io_num = -1,
//             .data4_io_num = -1,
//             .data5_io_num = -1,
//             .data6_io_num = -1,
//             .data7_io_num = -1,
//             .max_transfer_sz = 32,
//             .flags = 0,
//             .intr_flags = 0
//         };

//         ESP_ERROR_CHECK(spi_bus_initialize(host, &buscfg, dma_chan));
//         ESP_ERROR_CHECK(max7219_init_desc(&dev, host, MAX7219_MAX_CLOCK_SPEED_HZ, pin_cs));
//         dev.cascade_size = 1;
//         dev.digits = 8;
//         ESP_ERROR_CHECK(max7219_init(&dev));
//     }

//     void setBrightness(uint8_t brightness) {
//         ESP_ERROR_CHECK(max7219_set_brightness(&dev, brightness));
//     }

//     void clear() {
//         ESP_ERROR_CHECK(max7219_clear(&dev));
//     }

//     void drawText(const char* text) {
//         ESP_ERROR_CHECK(max7219_draw_text_7seg(&dev, 0, text));
//     }

//     void drawImage(uint64_t image) {
//         ESP_ERROR_CHECK(max7219_draw_image_8x8(&dev, 0, &image));
//     }

//     ~LedBlock() {
//         ESP_ERROR_CHECK(max7219_free_desc(&dev));
//         ESP_ERROR_CHECK(spi_bus_free(host));
//     }
// };

// // Task to update display periodically
// void displayTask(void *pvParameters) {
//     LedBlock* display = (LedBlock*) pvParameters;

//     // Set initial brightness and clear display
//     display->setBrightness(8);
//     display->clear();

//     // Draw "HELLO" on the display
//     //display->drawText("HELLO");

//     // Define a simple 8x8 smiley face pattern
//     uint64_t smiley_face = 0x3c42a581a599423c;
//     display->drawImage(smiley_face);

//     // Wait for a while before updating the display
//     vTaskDelay(pdMS_TO_TICKS(3000));
//     display->clear();
//     display->drawText("ESP32");

//     while (true) {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

// // extern "C" void app_main(void) {
// //     LedBlock display(HSPI_HOST, DMA_CHAN, PIN_NUM_CS, PIN_NUM_CLK, PIN_NUM_DIN);
// //     display.init();

// //     xTaskCreate(displayTask, "displayTask", 4096, &display, 5, NULL);
// // }

// #ifdef __cplusplus
// }
// #endif

// #endif