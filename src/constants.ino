
#include <rgb_lcd.h>

#include "constants.h"
#include "pins.h"

Button stockBtn = {
    .pin = STOCK_BUTTON_PIN,
    .led = STOCK_LED_PIN,
    .debounceDelay = BUTTONS_DEBOUNCE_DELAY,
};

Button securityBtn = {
    .pin = SECURITY_BUTTON_PIN,
    .led = SECURITY_LED_PIN,
    .debounceDelay = BUTTONS_DEBOUNCE_DELAY,
};

rgb_lcd lcdDevice;

LCD16x2 LCD = {
    .device = &lcdDevice,
    .line01 = "",
    .line02 = "",
};

const BuzzerStep AlarmSteps[] = {
    { .frequency = 700, .duration = 500 },
    { .frequency = 560, .duration = 500 },
    { .frequency = 700, .duration = 500 },
    { .frequency = 560, .duration = 500 },
};

Buzzer buzzer = {
    .pin = BUZZER_PIN,
    .steps = AlarmSteps,
    .stepsLength = sizeof(AlarmSteps) / sizeof(AlarmSteps[0]),
    .playing = false,
};

WeightSensor weightSensor = {
    .dtPin = WEIGHT_SENSOR_DT_PIN,
    .sckPin = WEIGHT_SENSOR_SCK_PIN,
    .led = WEIGHT_SENSOR_LED_PIN,
    .product = {
        .name = WEIGHT_SENSOR_PRODUCT_NAME,
        .weight = WEIGHT_SENSOR_PRODUCT_WEIGHT,
    },
    .minimumAcceptableStock = WEIGHT_SENSOR_MINIMUM_ACCEPTABLE_STOCK,
    .sample = { .weight = 0, .valid = false },
};
