// C/C++ Headers
#include "stdio.h"
#include "cstring"
#include "new"
#include "iostream"
// Other Posix Headers
// ESP IDF Headers
#include "esp_log.h"
#include <esp_err.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// Component Headers
#include "Blink.h"
// Public Headers
// Private Headers
#include "LedBlockDisplay.h"
#define OOPTEST
#ifdef NONTHREADOOPTEST
extern "C"
{

  void app_main()
  {
    LedBlockDisplay ledMatrix(HSPI_HOST, DMA_CHAN, PIN_NUM_CS, PIN_NUM_CLK, PIN_NUM_DIN);
    ledMatrix.init();
    uint64_t currentImage = 0;
    while (1)
    {
      uint64_t new_image = 0;
      for (int i = 0; i < 8; i++)
      {
        // Generate a random row of 8 bits
        new_image |= ((uint64_t)esp_random() & 0xFF) << (i * 8);
      }
      currentImage = new_image;
      ledMatrix.display(&currentImage);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}
#endif



#ifdef OOPTEST

SemaphoreHandle_t xImageSemaphore;
const char *TAG = "GENERATE_RAND_IMG";

extern "C"
{
  void generate_random_image_task(void *);
  void display_image_task(void *);

  void app_main(void)
  {
    // Declare the semaphore and the current image variable
    // Create a semaphore for synchronizing access to the current image
    xImageSemaphore = xSemaphoreCreateMutex();
    if (xImageSemaphore == NULL)
    {
      ESP_LOGE(TAG, "Failed to create semaphore");
      return;
    }
    uint64_t currentImage = 0;

    // Create tasks for generating random images and displaying them
    xTaskCreate(generate_random_image_task, "GenerateRandomImage", STACK_SIZE_MEDIUM, &currentImage, 5, NULL);
    xTaskCreate(display_image_task, "DisplayImage", STACK_SIZE_MEDIUM, &currentImage, 5, NULL);

    while (1)
    {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  // Task to generate random 8x8 images
  void generate_random_image_task(void *pvParameters)
  {
    uint64_t *pCurrentImage = (uint64_t *)pvParameters;
    while (1)
    {
      uint64_t new_image = 0;
      for (int i = 0; i < 8; i++)
      {
        // Generate a random row of 8 bits
        new_image |= ((uint64_t)esp_random() & 0xFF) << (i * 8);
      }

      // Safely update the currentImage using a semaphore
      if (xSemaphoreTake(xImageSemaphore, portMAX_DELAY) == pdTRUE)
      {
        *pCurrentImage = new_image;
        xSemaphoreGive(xImageSemaphore);
      }
      uint64_t img = *pCurrentImage;
      ESP_LOGD(TAG, "Generated num: 0x%016llx", img);

      // Generate a new image every second
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  // Task to display the image on the LED matrix
  void display_image_task(void *pvParameters)
  {
    uint64_t *pCurrentImage = (uint64_t *)pvParameters;
    LedBlockDisplay ledMatrix(HSPI_HOST, DMA_CHAN, PIN_NUM_CS, PIN_NUM_CLK, PIN_NUM_DIN);
    ledMatrix.init();
    ESP_LOGD(TAG, "DisplayTask setup");
    while (1)
    {
      if (xSemaphoreTake(xImageSemaphore, portMAX_DELAY) == pdTRUE)
      {
        ESP_LOGD(TAG, "Displaying image");
        uint64_t img = *pCurrentImage;
        ledMatrix.display(&img);
        xSemaphoreGive(xImageSemaphore);
      }

      // Update display every 100 milliseconds
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}

#endif

#ifdef NONOOPTEST

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
        .data4_io_num = -1, // Initialize unused fields to -1
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 32,
        .flags = 0,     // Initialize flags
        .intr_flags = 0 // Initialize intr_flags
    };

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, DMA_CHAN));

    // Initialize the MAX7219 device descriptor
    max7219_t dev;
    ESP_ERROR_CHECK(max7219_init_desc(&dev, HSPI_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS));

    // Set the number of cascaded devices (1 for a single 8x8 matrix)
    dev.cascade_size = 1;
    dev.digits = 8;

    // Initialize the MAX7219 device
    ESP_ERROR_CHECK(max7219_init(&dev));

    // Set brightness
    ESP_ERROR_CHECK(max7219_set_brightness(&dev, 8)); // Set brightness level (0-15)

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

    while (1)
    {
      ESP_ERROR_CHECK(max7219_draw_image_8x8(&dev, 0, &smiley_face));
      vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Free the MAX7219 device descriptor
    ESP_ERROR_CHECK(max7219_free_desc(&dev));

    // Free the SPI bus
    ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
  }
}

#endif