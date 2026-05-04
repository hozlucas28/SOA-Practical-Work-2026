
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

LiquidCrystal_I2C lcdDevice(0x27, LCD_COLS, LCD_ROWS);

LCD16x2 LCD = {
    .device = &lcdDevice,
    .line01 = "",
    .line02 = "",
};

WeightSensor WeightSensor01 = {
    .dtPin = WEIGHT_SENSOR_01_DT_PIN,
    .sckPin = WEIGHT_SENSOR_01_SCK_PIN,
    .led = WEIGHT_SENSOR_01_LED_PIN,
    .product = {
        .name = WEIGHT_SENSOR_01_PRODUCT_NAME,
        .weight = WEIGHT_SENSOR_01_PRODUCT_WEIGHT,
    },
    .minimumAcceptableStock = WEIGHT_SENSOR_01_MINIMUM_ACCEPTABLE_STOCK,
};

WeightSensor WeightSensor02 = {
    .dtPin = WEIGHT_SENSOR_02_DT_PIN,
    .sckPin = WEIGHT_SENSOR_02_SCK_PIN,
    .led = WEIGHT_SENSOR_02_LED_PIN,
    .product = {
        .name = WEIGHT_SENSOR_02_PRODUCT_NAME,
        .weight = WEIGHT_SENSOR_02_PRODUCT_WEIGHT,
    },
    .minimumAcceptableStock = WEIGHT_SENSOR_02_MINIMUM_ACCEPTABLE_STOCK,
};
