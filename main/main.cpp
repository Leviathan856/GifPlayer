#include "sdkconfig.h"

#include "GpioRoutines.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"


static constexpr const char *TAG = "main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Configuring GPIO pins.");
    configureGpioPins();

    ESP_LOGI(TAG, "Starting button task.");
    xTaskCreatePinnedToCore(buttonEventTask, "ButtonTask", 2048, nullptr, 4, nullptr, 0);

    ESP_LOGI(TAG, "Starting LCD task.");
    // TODO: Create and start LCD task.
}