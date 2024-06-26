// C/C++ Headers
#include "new"
#include "iostream"
// Other Posix Headers
// ESP IDF Headers
#include "driver/gpio.h"
// Component Headers
#include "Blink.h"
// Public Headers
// Private Headers
#include "LedBlockDisplay.h"
#define OOPTEST

#ifdef OOPTEST

extern "C"
{
  void app_main(void)
  {
    // Create a semaphore for synchronizing access to the current image
    xImageMutex = xSemaphoreCreateMutex();
    if (xImageMutex != NULL)
    {
      uint64_t currentImage = 0;
      // Create tasks for generating random images and displaying them
      xTaskCreate(generate_random_image_task, "GenerateRandomImage", STACK_SIZE_MEDIUM, &currentImage, 5, NULL);
      xTaskCreate(display_image_task, "DisplayImage", STACK_SIZE_MEDIUM, &currentImage, 5, NULL);
    }
    else
    {
      ESP_LOGE(TAG, "Failed to create semaphore");
      return;
    }

    while (1)
    {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}

#endif

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
        new_image |= ((uint64_t)esp_random() & 0xFF) << (i * 8);
      }
      currentImage = new_image;
      ledMatrix.display(&currentImage);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}
#endif

#ifdef NONOOPTEST

extern "C"
{
  void app_main(void)
  {
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_DIN,
        .miso_io_num = -1, 
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 32,
        .flags = 0,    
        .intr_flags = 0 
    };

   
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, DMA_CHAN));

    
    max7219_t dev;
    ESP_ERROR_CHECK(max7219_init_desc(&dev, HSPI_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS));

    dev.cascade_size = 1;
    dev.digits = 8;

    ESP_ERROR_CHECK(max7219_init(&dev));
    ESP_ERROR_CHECK(max7219_set_brightness(&dev, 8)); 
    ESP_ERROR_CHECK(max7219_clear(&dev));
    ESP_ERROR_CHECK(max7219_draw_text_7seg(&dev, 0, "HELLO"));
    uint64_t smiley_face = 0x3c42a581a599423c;

    while (1)
    {
      ESP_ERROR_CHECK(max7219_draw_image_8x8(&dev, 0, &smiley_face));
      vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_ERROR_CHECK(max7219_free_desc(&dev));
    ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
  }
}

#endif