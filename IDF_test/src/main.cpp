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
// #define SPI_HOST
#define DMA_CHAN 2

#define PIN_NUM_CS GPIO_NUM_23
#define PIN_NUM_CLK GPIO_NUM_5
#define PIN_NUM_DIN GPIO_NUM_19

SemaphoreHandle_t xImageSemaphore;
uint16_t currentImage = 0;
LedBlockDisplay *ledMatrix;

const char *TAG = "GENERATED_IMG";

extern "C"
{
  void generate_random_image_task(void *);
  void display_image_task(void *);

  void app_main(void)
  {
    
     ledMatrix = new LedBlockDisplay(8);

    // Create a semaphore for synchronizing access to the current image
    xImageSemaphore = xSemaphoreCreateMutex();

    // Create tasks for generating random images and displaying them
    xTaskCreate(generate_random_image_task, "GenerateRandomImage", 2048, NULL, 5, NULL);
    xTaskCreate(display_image_task, "DisplayImage", 2048, NULL, 5, NULL);

    while (1)
    {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
     delete (ledMatrix);
  }

  // Task to generate random 8x8 images
void generate_random_image_task(void *pvParameters)
{
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
      currentImage = new_image;
      xSemaphoreGive(xImageSemaphore);
    }

    ESP_LOGD(TAG, "Displaying image: 0x%04x", currentImage);

    // Generate a new image every second
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Task to display the image on the LED matrix
void display_image_task(void *pvParameters)
{
  while (1)
  {
    blink(GPIO_NUM_2, 1000);
    // Safely read the currentImage using a semaphore
    if (xSemaphoreTake(xImageSemaphore, portMAX_DELAY) == pdTRUE)
    {
      //ledMatrix->display(&currentImage);
      xSemaphoreGive(xImageSemaphore);
    }

    // Update display every 100 milliseconds
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

}



#endif