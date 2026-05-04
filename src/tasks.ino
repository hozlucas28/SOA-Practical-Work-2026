
#include "constants.h"
#include "tasks.h"
#include "user_functions.h"

void xButtonsTask(void* parameters) {
    while (true) {
        switchBtnState(&StockBtn);
        switchBtnState(&SecurityBtn);
        vTaskDelay(pdMS_TO_TICKS(BUTTONS_TASK_PERIOD_MS));
    }
}

void xWeightSampleTask(void* parameters) {
    while (true) {
        sampleWeight(&WeightSensor01);
        sampleWeight(&WeightSensor02);
        vTaskDelay(pdMS_TO_TICKS(WEIGHT_SAMPLE_TASK_PERIOD_MS));
    }
}

void xBuzzerTask(void* parameters) {
    Buzzer* buzzer = (Buzzer*)parameters;
    size_t currentStep = 0;
    bool wasPlaying = false;

    while (true) {
        if (buzzer->playing) {
            applyTone(buzzer, buzzer->steps[currentStep].frequency);
            vTaskDelay(pdMS_TO_TICKS(buzzer->steps[currentStep].duration));
            currentStep = (currentStep + 1) % buzzer->stepsLength;
            wasPlaying = true;
        } else {
            if (wasPlaying) {
                applyTone(buzzer, 0);
                wasPlaying = false;
            }
            currentStep = 0;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}
