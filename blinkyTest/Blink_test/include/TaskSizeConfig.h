#ifndef TASKSIZECONFIG_H
#define TASKSIZECONFIG_H

#include "freertos/FreeRTOS.h"  // Include FreeRTOS headers to access configMINIMAL_STACK_SIZE

// Define stack sizes as macros relative to configMINIMAL_STACK_SIZE
#define STACK_SIZE_SMALL   (configMINIMAL_STACK_SIZE * 2)  // Example: twice the minimal stack size
#define STACK_SIZE_MEDIUM  (configMINIMAL_STACK_SIZE * 4)  // Example: four times the minimal stack size
#define STACK_SIZE_LARGE   (configMINIMAL_STACK_SIZE * 8)  // Example: eight times the minimal stack size

#endif // TASK_CONFIG_H