
#include <LiquidCrystal_I2C.h>

#include "pins.h"
#include "global-structs.h"

#ifndef BUTTONS_DEBOUNCE_DELAY
	#define BUTTONS_DEBOUNCE_DELAY  25 // Miliseconds
#endif

Button StockBtn = {
  .pin = STOCK_BUTTON,
  .led = {
    .pin = STOCK_LED
  },
  .debounceDelay = BUTTONS_DEBOUNCE_DELAY
};

Button SecurityBtn = {
  .pin = SECURITY_BUTTON,
  .led = {
    .pin = SECURITY_LED
  },
  .debounceDelay = BUTTONS_DEBOUNCE_DELAY
};

LiquidCrystal_I2C LCD(0x27, LCD_COLS, LCD_ROWS);

WeightSensor WeightSensor01 = {
  .dtPin = WEIGHT_SENSOR_01_DT,
  .sckPin = WEIGHT_SENSOR_01_SCK,
  .led = {
    .pin = WEIGHT_SENSOR_01_LED
  }
};

WeightSensor WeightSensor02 = {
  .dtPin = WEIGHT_SENSOR_02_DT,
  .sckPin = WEIGHT_SENSOR_02_SCK,
  .led = {
    .pin = WEIGHT_SENSOR_02_LED
  }
};
