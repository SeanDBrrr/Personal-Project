#ifndef _BLINK_H_
#define _BLINK_H_

// C/C++ Headers
#include "stdio.h"
// Other Posix Headers

// ESP IDF Headers
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// Component Headers

// Public Headers

// Private Headers
#include "TaskSizeConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void blink(gpio_num_t gpio_num, int delay_ms);

#ifdef __cplusplus
}
#endif

#endif // _BLINK_H_