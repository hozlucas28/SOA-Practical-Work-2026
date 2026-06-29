#ifndef USER_FUNCTIONS_H_INCLUDED
#define USER_FUNCTIONS_H_INCLUDED

#include "enums.h"
#include "structs.h"

/**
 * @brief Handle the button state change with debounce logic and status toggling.
 *
 * This function should be called in the button's FreeRTOS task loop. It reads the current state of the button, compares
 * it with the last known state, and if a change is detected, it checks if the debounce delay has passed. If the
 * debounce condition is satisfied, it toggles the button's status (`ON`/`OFF`) and updates the associated LED
 * accordingly.
 *
 * @param btn Pointer to the `Button` struct representing the button being handled.
 */
void switchBtn(Button* btn);

/**
 * @brief Print a single line on the LCD, clearing only what is needed.
 *
 * @param lcd Pointer to the LCD struct.
 * @param line The line to print on the LCD.
 */
void lcdPrint(LCD16x2* lcd, const String line);

/**
 * @brief Print two lines on the LCD, clearing only what is needed.
 *
 * @param lcd Pointer to the LCD struct.
 * @param line01 The first line to print on the LCD.
 * @param line02 The second line to print on the LCD.
 */
void lcdPrint(LCD16x2* lcd, const String line01, const String line02);

/**
 * @brief Clear the LCD and the cached `line01`/`line02`.
 *
 * @param lcd Pointer to the LCD struct.
 */
void lcdClear(LCD16x2* lcd);

/**
 * @brief Request the buzzer FreeRTOS task to start playing the melody.
 *
 * @param buzzer Pointer to the target `Buzzer` struct.
 *
 * @note If the buzzer is muted, this function will have no effect.
 */
void playBuzzer(Buzzer* buzzer);

/**
 * @brief Request the buzzer task to stop playing the melody.
 *
 * @param buzzer Pointer to the target `Buzzer` struct.
 */
void stopBuzzer(Buzzer* buzzer);

/**
 * @brief Mute the buzzer, preventing it from playing any sound.
 *
 * @param buzzer Pointer to the target `Buzzer` struct.
 */
void muteBuzzer(Buzzer* buzzer);

/**
 * @brief Unmute the buzzer, allowing it to play sound again.
 *
 * @param buzzer Pointer to the target `Buzzer` struct.
 */
void unmuteBuzzer(Buzzer* buzzer);

/**
 * @brief Get the valid weight reading from the sensor's sample.
 *
 * @param sensor Pointer to the weight sensor.
 *
 * @return The weight in grams if the sample is valid; otherwise, returns `0`.
 */
const int32_t getOffset(WeightSensor* sensor);

/**
 * @brief Get the valid weight reading from the sensor's sample.
 *
 * @param sensor Pointer to the weight sensor.
 *
 * @return The weight in grams if the sample is valid; otherwise, returns `0`.
 */
const unsigned int getWeight(const WeightSensor* sensor);

/**
 * @brief Get the current stock level based on the weight sensor's sample and the associated product.
 *
 * @param sensor Pointer to the weight sensor.
 *
 * @return The calculated stock level (number of units) if the sample is valid; otherwise, returns `0`.
 */
const unsigned int getStock(const WeightSensor* sensor);

/**
 * @brief Get the product name associated with the given weight sensor.
 *
 * @param sensor Pointer to the weight sensor.
 *
 * @return A pointer to the product name string associated with the weight sensor.
 */
const String* getProductName(WeightSensor* sensor);

/**
 * @brief Get the product minimum acceptable stock associated with the given weight sensor.
 *
 * @param sensor Pointer to the weight sensor.
 *
 * @return The minimum acceptable stock (in units) associated with the weight sensor.
 */
const unsigned int getMinimumAcceptableStock(const WeightSensor* sensor);

/**
 * @brief Check if the weight sensor has an anomaly based on the current weight reading and the baseline weight.
 *
 * @param sensor Pointer to the weight sensor.
 */
bool hasAnomaly(const WeightSensor* sensor);

/**
 * @brief Set the weight sensor offset.
 *
 * It should be called with a valid offset value, which can be obtained from a previous
 * tare operation or from persisted storage. By setting the sensor offset, future weight readings will be adjusted
 * accordingly, allowing for accurate measurements even when there is already a product on the weight sensor at
 * startup or after a reboot.
 *
 * @param sensor Pointer to the weight sensor.
 * @param offset Offset value to set.
 */
void setOffset(WeightSensor* sensor, const int32_t offset);

/**
 * @brief Read the current weight from the sensor.
 *
 * This function reads the weight from the sensor and updates the `sample` field of the `WeightSensor` struct. The
 * weight is stored in grams, and if the reading is invalid (e.g., sensor not ready), the `isValid` flag in the sample
 * will be set to `false` and the weight will be set to `0`. If the reading is valid, the weight will be updated and the
 * `isValid` flag will be set to `true`.
 */
void setWeight(WeightSensor* sensor);

/**
 * @brief Take the current weight sensor weight as the new baseline used by `SECURITY_MODE` anomaly detection.
 *
 * @param sensor Pointer to the weight sensor.
 */
void setBaselineWeight(WeightSensor* sensor);

/**
 * @brief Tare the weight sensor.
 *
 * Performs a tare operation on the given weight sensor, which involves calculating the zero offset based on the current
 * weight reading. This function should be called when the sensor is ready and a valid weight sample is available. The
 * resulting offset can be used to adjust future weight readings, effectively treating the current weight as the new
 * "zero" point.
 *
 * @param sensor Pointer to the weight sensor.
 */
void tare(WeightSensor* sensor);

/**
 * @brief Turn on the LED associated with the given weight sensor.
 *
 * @param sensor Pointer to the weight sensor.
 */
void ledOn(const WeightSensor* sensor);

/**
 * @brief Turn off the LED associated with the given weight sensor.
 *
 * @param sensor Pointer to the weight sensor.
 */
void ledOff(const WeightSensor* sensor);

#endif  // USER_FUNCTIONS_H_INCLUDED
