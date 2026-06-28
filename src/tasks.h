#ifndef TASKS_H_INCLUDED
#define TASKS_H_INCLUDED

/**
 * @brief FreeRTOS task responsible for managing interaction with the buttons.
 *
 * @param parameters Generic FreeRTOS parameter for the task.
 *
 * @note `parameters` is not used, button controls is done through the global `stockBtn` and `securityBtn` variables.
 */
void xButtonsTask(void* parameters);

/**
 * @brief FreeRTOS task responsible for controlling the buzzer.
 *
 * @param parameters Generic FreeRTOS parameter for the task.
 *
 * @note `parameters` is not used, buzzer control is done through the global `buzzer` variable.
 */
void xBuzzerTask(void* parameters);

/**
 * @brief FreeRTOS task responsible for performing weight sampling.
 *
 * @param parameters Generic FreeRTOS parameter for the task.
 *
 * @note `parameters` is not used, weight sensor sampling is done through the global `weightSensor`'s variables.
 */
void xWeightSampleTask(void* parameters);

#endif  // TASKS_H_INCLUDED
