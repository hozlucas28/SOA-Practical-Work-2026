
#include "sync.h"

SemaphoreHandle_t ButtonsMutex = nullptr;
SemaphoreHandle_t WeightSensorsMutex = nullptr;
SemaphoreHandle_t BuzzerMutex = nullptr;

void initSyncObjects() {
    ButtonsMutex = xSemaphoreCreateMutex();
    WeightSensorsMutex = xSemaphoreCreateMutex();
    BuzzerMutex = xSemaphoreCreateMutex();
}

void lockButtons() {
    if (ButtonsMutex) xSemaphoreTake(ButtonsMutex, portMAX_DELAY);
}

void unlockButtons() {
    if (ButtonsMutex) xSemaphoreGive(ButtonsMutex);
}

void lockWeightSensors() {
    if (WeightSensorsMutex) xSemaphoreTake(WeightSensorsMutex, portMAX_DELAY);
}

void unlockWeightSensors() {
    if (WeightSensorsMutex) xSemaphoreGive(WeightSensorsMutex);
}

void lockBuzzer() {
    if (BuzzerMutex) xSemaphoreTake(BuzzerMutex, portMAX_DELAY);
}

void unlockBuzzer() {
    if (BuzzerMutex) xSemaphoreGive(BuzzerMutex);
}
