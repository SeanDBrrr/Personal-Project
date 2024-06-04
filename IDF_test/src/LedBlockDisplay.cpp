#include "LedBlockDisplay.h"

bool LedBlockDisplay::isSpiInitialized = false;

LedBlockDisplay::LedBlockDisplay(int matrixSize, int matrixCount, int brightness)
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

    isSpiInitialized = true; 
    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST, &buscfg, DMA_CHAN));

    ESP_ERROR_CHECK(max7219_init_desc(&dev, SPI_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS));

    // Set the number of cascaded devices (1 for a single 8x8 matrix)
    dev.cascade_size = matrixCount;
    dev.digits = matrixSize;

    // Initialize the MAX7219 device
    ESP_ERROR_CHECK(max7219_init(&dev));

    // Set brightness
    ESP_ERROR_CHECK(max7219_set_brightness(&dev, brightness)); // Set brightness level (0-15)

    // Clear display
    ESP_ERROR_CHECK(max7219_clear(&dev));
}

void LedBlockDisplay::display(const void * image)
{
     ESP_ERROR_CHECK(max7219_draw_image_8x8(&dev, 0, &image));
}

LedBlockDisplay::~LedBlockDisplay()
{
    // Free the MAX7219 device descriptor
    ESP_ERROR_CHECK(max7219_free_desc(&dev));

    // Free the SPI bus
    ESP_ERROR_CHECK(spi_bus_free(SPI_HOST));
}