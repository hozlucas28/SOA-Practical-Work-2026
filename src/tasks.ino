
#include "tasks.h"
#include "user_functions.h"

void xBuzzerTask(void* parameters) {
    Buzzer* buzzer = (Buzzer*)parameters;
    size_t currentStep = 0;

    while (true) {
        if (buzzer->playing) {
            applyTone(buzzer, buzzer->steps[currentStep].frequency);
            vTaskDelay(pdMS_TO_TICKS(buzzer->steps[currentStep].duration));
            currentStep = (currentStep + 1) % buzzer->stepsLength;
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
            currentStep = 0;
        }
    }
}
