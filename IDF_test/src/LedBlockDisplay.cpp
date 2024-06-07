#include "LedBlockDisplay.h"

const char *TAG = "GENERATE_RAND_IMG"; 
SemaphoreHandle_t xImageSemaphore;

LedBlockDisplay::LedBlockDisplay(spi_host_device_t host, int dma_chan, int pin_cs, int pin_clk, int pin_din)
        : host(host), dma_chan(dma_chan), pin_cs(pin_cs), pin_clk(pin_clk), pin_din(pin_din) 
{}

void LedBlockDisplay::init(int matrixSize, int matrixCount, uint8_t brightness)
{
    const char *TAG = "INITIALIZATION";
    // Log the start of the init function
    ESP_LOGI(TAG, "Initializing LED matrix display...");

    // Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_DIN,
        .miso_io_num = -1, // MISO is not used
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

    // Initialize the SPI bus
    ESP_LOGI(TAG, "Initializing SPI bus...");
    esp_err_t ret = spi_bus_initialize(HSPI_HOST, &buscfg, DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "SPI bus initialized successfully.");

    ESP_LOGI(TAG, "Initializing MAX7219 descriptor...");
    ret = max7219_init_desc(&dev, HSPI_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MAX7219 descriptor: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "MAX7219 descriptor initialized successfully.");

    // Set the number of cascaded devices (1 for a single 8x8 matrix)
    dev.cascade_size = matrixCount;
    dev.digits = matrixSize;

    // Initialize the MAX7219 device
    ESP_LOGI(TAG, "Initializing MAX7219 device...");
    ret = max7219_init(&dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MAX7219 device: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "MAX7219 device initialized successfully.");

    // Set brightness
    ESP_LOGI(TAG, "Setting brightness...");
    ret = max7219_set_brightness(&dev, brightness);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set brightness: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Brightness set successfully.");

    // Clear display
    ESP_LOGI(TAG, "Clearing display...");
    ret = max7219_clear(&dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear display: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Display cleared successfully.");
    ESP_LOGI(TAG, "LED matrix display initialized.");
}


void LedBlockDisplay::display(void* image)
{
    printf("LedBlockDisplay::display called with image: 0x%016llx\n", *(uint64_t*)image);
    uint64_t img = *(uint64_t*)image;
    ESP_ERROR_CHECK(max7219_draw_image_8x8(&dev, 0, &img));
}

LedBlockDisplay::~LedBlockDisplay()
{
    // Free the MAX7219 device descriptor
    ESP_ERROR_CHECK(max7219_free_desc(&dev));

    // Free the SPI bus
    ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
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

      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }