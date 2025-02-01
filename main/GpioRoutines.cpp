#include "GpioRoutines.h"

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"

#include <utility>

using enum LogicLevel;

static inline constexpr int msInSecond = 1000;

static constexpr const char *TAG = "Button";


template <std::integral T>
inline bool operator==(T lhs, LogicLevel rhs)
{
    return lhs == std::to_underlying(rhs);
}

inline esp_err_t gpio_set_level(gpio_num_t gpio_num, LogicLevel level)
{
    return gpio_set_level(gpio_num, std::to_underlying(level));
}

void [[noreturn]] configureGpioPins()
{
    gpio_config_t gpioPinConf = { };

    gpioPinConf.intr_type = GPIO_INTR_DISABLE;
    gpioPinConf.mode = GPIO_MODE_OUTPUT;
    gpioPinConf.pin_bit_mask = (1ULL << BACKLIGHT_PIN);
    gpioPinConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioPinConf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpioPinConf);

    gpioPinConf.pin_bit_mask = (1ULL << BATTERY_PIN);
    gpioPinConf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&gpioPinConf);

    gpioPinConf.mode = GPIO_MODE_INPUT;
    gpioPinConf.pin_bit_mask = (1ULL << BUTTON_PIN);
    gpioPinConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&gpioPinConf);
}

void [[noreturn]] enterDeepSleep()
{
    gpio_set_level(BATTERY_PIN, LOW);

    ESP_LOGI(TAG, "Going to deep sleep...");
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup((1ULL << BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_LOW));
    esp_deep_sleep_start();
}

void [[noreturn]] buttonEventTask(void *pvParameters)
{
    gpio_set_level(BATTERY_PIN, HIGH);
    gpio_set_level(BACKLIGHT_PIN, HIGH);

    while (true)
    {
        /*
           TODO: Monitor state using interrupt, use queue with press/release time to determine
           event (press, double press, hold), notify LCD task
        */

        ESP_LOGI(TAG, "Waiting for button hold.");

        if (int isButtonPressed = gpio_get_level(BUTTON_PIN); isButtonPressed == LOW)
        {
            ESP_LOGI(TAG, "Button pressed, waiting for release.");

            while (gpio_get_level(BUTTON_PIN) == LOW)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            ESP_LOGI(TAG, "Button released.");
            enterDeepSleep();
        }

        vTaskDelay(5 * msInSecond / portTICK_PERIOD_MS);
    }
}