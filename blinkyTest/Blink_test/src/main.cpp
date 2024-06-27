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

extern "C"
{
  void app_main()
  {
    // Start the blink task
    // Example: Blink an LED on GPIO 2 with a 1000ms delay
    blink(GPIO_NUM_2, 1000);
  }
}



