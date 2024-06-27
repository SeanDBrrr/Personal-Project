// C/C++ Headers
// Other Posix Headers

// ESP IDF Headers


// Component Headers

// Public Headers

// Private Headers
#include "Blink.h"


#define RTOS
const char* BLINK_TAG = "BLINK";

#ifdef NORTOS
#include "Blink.h"


static void blink_internal(gpio_num_t gpio_num, int delay_ms) {
  
  int counter = 0;
        while (true)
        {
            if (counter > delay_ms)
            {
                ESP_ERROR_CHECK(gpio_set_level(gpio_num, 1));
                ESP_LOGD(BLINK_TAG, "LED ON");
            }
            else
            {
                ESP_ERROR_CHECK(gpio_set_level(gpio_num, 0));
                ESP_LOGD(BLINK_TAG, "LED OFF");
            }
            if (counter > (delay_ms * 2))
            {
                counter = 0;
            }
            ++counter;
        }
}

void blink(gpio_num_t gpio_num, int delay_ms)
{
    gpio_pad_select_gpio(gpio_num);
    ESP_ERROR_CHECK(gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(gpio_num, 1));
    blink_internal(gpio_num, delay_ms);
}
#endif

#ifdef RTOS

struct BlinkParams {
    gpio_num_t gpio_num;
    int delay_ms;
};

static void blink_task(void *param) {
    BlinkParams *blinkParams = (BlinkParams *)param;
    gpio_num_t gpio_num = blinkParams->gpio_num;
    int delay_ms = blinkParams->delay_ms;

    while (true) {
        ESP_ERROR_CHECK(gpio_set_level(gpio_num, 1));
        ESP_LOGD(BLINK_TAG, "LED ON");
        vTaskDelay(pdMS_TO_TICKS(delay_ms));

        ESP_ERROR_CHECK(gpio_set_level(gpio_num, 0));
        ESP_LOGD(BLINK_TAG, "LED OFF");
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
    // Free the allocated memory before deleting the task
    free(param);
    vTaskDelete(NULL);
}

void blink(gpio_num_t gpio_num, int delay_ms) {
    gpio_pad_select_gpio(gpio_num);
    ESP_ERROR_CHECK(gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT));

    // Allocate memory for the parameters and assign values
    BlinkParams *params = (BlinkParams *)malloc(sizeof(BlinkParams));
    params->gpio_num = gpio_num;
    params->delay_ms = delay_ms;

    xTaskCreate(blink_task, "blink_task", STACK_SIZE_MEDIUM, params, 5, NULL);
}
#endif