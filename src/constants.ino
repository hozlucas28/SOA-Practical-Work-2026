
#include <LiquidCrystal_I2C.h>

#include "constants.h"
#include "pins.h"

Button StockBtn = {
	.pin = STOCK_BUTTON_PIN,
	.led = {
		.pin = STOCK_LED_PIN,
	},
	.debounceDelay = BUTTONS_DEBOUNCE_DELAY,
};

Button SecurityBtn = {
	.pin = SECURITY_BUTTON_PIN,
	.led = {
		.pin = SECURITY_LED_PIN,
	},
	.debounceDelay = BUTTONS_DEBOUNCE_DELAY,
};

LiquidCrystal_I2C LCD(0x27, LCD_COLS, LCD_ROWS);

WeightSensor WeightSensor01 = {
	.dtPin = WEIGHT_SENSOR_01_DT_PIN,
	.sckPin = WEIGHT_SENSOR_01_SCK_PIN,
	.led = {
		.pin = WEIGHT_SENSOR_01_LED_PIN,
	},
};

WeightSensor WeightSensor02 = {
	.dtPin = WEIGHT_SENSOR_02_DT_PIN,
	.sckPin = WEIGHT_SENSOR_02_SCK_PIN,
	.led = {
		.pin = WEIGHT_SENSOR_02_LED_PIN,
	},
};
