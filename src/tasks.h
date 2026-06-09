#ifndef SRC_TASKS_H_INCLUDED
#define SRC_TASKS_H_INCLUDED

/**
 * Polls both push buttons and applies debounce. Param: unused (NULL).
 * Period: `BUTTONS_TASK_PERIOD_MS`.
 */
void xButtonsTask(void* parameters);

/**
 * Refreshes the cached `WeightSample` for both load cells. Param: unused (NULL).
 * Period: `WEIGHT_SAMPLE_TASK_PERIOD_MS`.
 */
void xWeightSampleTask(void* parameters);

/**
 * Drives the buzzer through its melody steps while `Buzzer.playing` is
 * true; silences the GPIO when it transitions to false. Param: pointer to
 * the target `Buzzer`.
 */
void xBuzzerTask(void* parameters);

#endif  // SRC_TASKS_H_INCLUDED
