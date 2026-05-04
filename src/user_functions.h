#ifndef SRC_USER_FUNCTIONS_H_INCLUDED
#define SRC_USER_FUNCTIONS_H_INCLUDED

#include "enums.h"
#include "structs.h"

/**
 * Read the button GPIO, apply debounce, and toggle `btn->status` on press.
 * Drives the paired LED from the resulting status. Called from
 * `xButtonsTask`.
 */
void switchBtnState(Button* btn);

/**
 * Read 10 raw HX711 samples and write the averaged weight (in grams) into
 * `weightSensor->sample`. Sets `sample.valid = false` while the device is
 * unready; sets it `true` after a successful read. Called from
 * `xWeightSampleTask` and once during `setup()` to prime the cache.
 */
void sampleWeight(WeightSensor* weightSensor);

/** Clear the LCD and the cached `line01`/`line02`. */
void lcdClear(LCD16x2* lcd);

/** Print a single line on the LCD, clearing only what is needed. */
void lcdPrint(LCD16x2* lcd, const String line);

/** Print two lines on the LCD, clearing only what is needed. */
void lcdPrint(LCD16x2* lcd, const String line01, const String line02);

/** Drive the buzzer GPIO with a tone or silence (frequency == 0). */
void applyTone(Buzzer* buzzer, unsigned int frequency);

/** Request the buzzer task to start playing the configured melody. */
void playBuzzer(Buzzer* buzzer);

/** Request the buzzer task to stop playing. The task owns silencing the GPIO. */
void stopBuzzer(Buzzer* buzzer);

/** Cached weight in grams (0 if no valid sample yet). */
unsigned int getWeight(WeightSensor* weightSensor);

/** Cached stock count derived from the cached weight. 0 if no valid sample. */
unsigned int getStock(WeightSensor* weightSensor);

/**
 * Snapshot the cached weight into `*outWeight`. Returns `false` (and leaves
 * the out-param untouched) when the cache is invalid, so callers can skip
 * the tick instead of treating an unready sensor as `0 g`.
 */
bool tryGetWeight(WeightSensor* weightSensor, unsigned int* outWeight);

/** Snapshot the cached stock count. Same skip-on-invalid contract as `tryGetWeight`. */
bool tryGetStock(WeightSensor* weightSensor, unsigned int* outStock);

/**
 * Take the current cached weight as the new baseline used by Security-mode
 * anomaly detection. No-op when the cache is invalid.
 */
void setBaselineWeight(WeightSensor* weightSensor);

/** Light the per-shelf LED if it is currently off. */
void ledOn(WeightSensor* weightSensor);

/** Turn off the per-shelf LED if it is currently on. */
void ledOff(WeightSensor* weightSensor);

#endif  // SRC_USER_FUNCTIONS_H_INCLUDED
