#pragma once
#include "sdkconfig.h"
#include "soc/gpio_num.h"

#include <cstdint>
#include <type_traits>
#include <utility>

#define BUTTON_PIN GPIO_NUM_14
#define BATTERY_PIN GPIO_NUM_15
#define BACKLIGHT_PIN GPIO_NUM_38

enum class LogicLevel : uint32_t
{
    LOW = 0,
    HIGH = 1
};

void configureGpioPins();
void buttonEventTask(void *pvParameters);