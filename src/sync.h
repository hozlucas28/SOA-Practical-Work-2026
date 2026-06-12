#ifndef SYNC_H_INCLUDED
#define SYNC_H_INCLUDED

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t buttonsMutex;
extern SemaphoreHandle_t buzzerMutex;
extern SemaphoreHandle_t weightSensorMutex;

void initMutexs();

void lockButtons();
void unlockButtons();

void lockBuzzer();
void unlockBuzzer();

void lockWeightSensor();
void unlockWeightSensor();

#endif  // SYNC_H_INCLUDED
