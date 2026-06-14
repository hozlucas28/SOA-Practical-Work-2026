
#include "sync.h"

SemaphoreHandle_t buttonsMutex = nullptr;
SemaphoreHandle_t buzzerMutex = nullptr;
SemaphoreHandle_t weightSensorMutex = nullptr;

void initMutexs() {
    buttonsMutex = xSemaphoreCreateMutex();
    buzzerMutex = xSemaphoreCreateMutex();
    weightSensorMutex = xSemaphoreCreateMutex();
}

void lockButtons() {
    if (!buttonsMutex) return;
    xSemaphoreTake(buttonsMutex, portMAX_DELAY);
}

void unlockButtons() {
    if (!buttonsMutex) return;
    xSemaphoreGive(buttonsMutex);
}

void lockBuzzer() {
    if (!buzzerMutex) return;
    xSemaphoreTake(buzzerMutex, portMAX_DELAY);
}

void unlockBuzzer() {
    if (!buzzerMutex) return;
    xSemaphoreGive(buzzerMutex);
}

void lockWeightSensor() {
    if (!weightSensorMutex) return;
    xSemaphoreTake(weightSensorMutex, portMAX_DELAY);
}

void unlockWeightSensor() {
    if (!weightSensorMutex) return;
    xSemaphoreGive(weightSensorMutex);
}
