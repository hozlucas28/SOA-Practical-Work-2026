
#include "sync.h"

SemaphoreHandle_t buttonsMutex = nullptr;
SemaphoreHandle_t weightSensorMutex = nullptr;
SemaphoreHandle_t buzzerMutex = nullptr;

void initMutexs() {
    buttonsMutex = xSemaphoreCreateMutex();
    weightSensorMutex = xSemaphoreCreateMutex();
    buzzerMutex = xSemaphoreCreateMutex();
}

void lockButtons() {
    if (buttonsMutex) xSemaphoreTake(buttonsMutex, portMAX_DELAY);
}

void unlockButtons() {
    if (buttonsMutex) xSemaphoreGive(buttonsMutex);
}

void lockWeightSensor() {
    if (weightSensorMutex) xSemaphoreTake(weightSensorMutex, portMAX_DELAY);
}

void unlockWeightSensor() {
    if (weightSensorMutex) xSemaphoreGive(weightSensorMutex);
}

void lockBuzzer() {
    if (buzzerMutex) xSemaphoreTake(buzzerMutex, portMAX_DELAY);
}

void unlockBuzzer() {
    if (buzzerMutex) xSemaphoreGive(buzzerMutex);
}
