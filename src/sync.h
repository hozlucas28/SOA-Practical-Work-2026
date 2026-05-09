#ifndef SRC_SYNC_H_INCLUDED
#define SRC_SYNC_H_INCLUDED

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/** TODO: Añadir documentación  */
extern SemaphoreHandle_t ButtonsMutex;

/** TODO: Añadir documentación  */
extern SemaphoreHandle_t WeightSensorsMutex;

/** TODO: Añadir documentación  */
extern SemaphoreHandle_t BuzzerMutex;

/** TODO: Añadir documentación  */
void initSyncObjects();

/** TODO: Añadir documentación  */
void lockButtons();

/** TODO: Añadir documentación  */
void unlockButtons();

/** TODO: Añadir documentación  */
void lockWeightSensors();

/** TODO: Añadir documentación  */
void unlockWeightSensors();

/** TODO: Añadir documentación  */
void lockBuzzer();

/** TODO: Añadir documentación  */
void unlockBuzzer();

#endif  // SRC_SYNC_H_INCLUDED
