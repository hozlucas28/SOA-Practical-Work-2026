
#include <LiquidCrystal_I2C.h>

#include "constants.h"
#include "pins.h"

Button StockBtn = {
    .pin = STOCK_BUTTON_PIN,
    .led = STOCK_LED_PIN,
    .debounceDelay = BUTTONS_DEBOUNCE_DELAY,
};

Button SecurityBtn = {
    .pin = SECURITY_BUTTON_PIN,
    .led = SECURITY_LED_PIN,
    .debounceDelay = BUTTONS_DEBOUNCE_DELAY,
};

LCD16x2 LCD = {
    .device = LiquidCrystal_I2C(0x27, LCD_COLS, LCD_ROWS),
    .line01 = "",
    .line02 = "",
};

WeightSensor WeightSensor01 = {
    .dtPin = WEIGHT_SENSOR_01_DT_PIN,
    .sckPin = WEIGHT_SENSOR_01_SCK_PIN,
    .led = WEIGHT_SENSOR_01_LED_PIN,
};

WeightSensor WeightSensor02 = {
    .dtPin = WEIGHT_SENSOR_02_DT_PIN,
    .sckPin = WEIGHT_SENSOR_02_SCK_PIN,
    .led = WEIGHT_SENSOR_02_LED_PIN,
};
