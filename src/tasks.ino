
#include "constants.h"
#include "sync.h"
#include "tasks.h"
#include "user_functions.h"

void xButtonsTask(void* parameters) {
    uint32_t delay = pdMS_TO_TICKS(5);

    while (true) {
        lockButtons();
        switchBtn(&stockBtn);
        switchBtn(&securityBtn);
        unlockButtons();

        vTaskDelay(delay);
    }
}

void xWeightSampleTask(void* parameters) {
    uint32_t delay = pdMS_TO_TICKS(200);

    while (true) {
        lockWeightSensor();
        setWeight(&weightSensor);
        unlockWeightSensor();

        vTaskDelay(delay);
    }
}

void xBuzzerTask(void* parameters) {
    bool wasPlaying = false;
    size_t currentStep = 0;

    while (true) {
        lockBuzzer();
        bool isPlaying = buzzer.playing;
        unlockBuzzer();

        if (isPlaying) {
            tone(buzzer.pin, buzzer.steps[currentStep].frequency);
            vTaskDelay(pdMS_TO_TICKS(buzzer.steps[currentStep].duration));

            wasPlaying = true;
            currentStep = (currentStep + 1) % buzzer.stepsLength;
        } else {
            if (wasPlaying) {
                wasPlaying = false;
                noTone(buzzer.pin);
            }

            currentStep = 0;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}
