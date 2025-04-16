#include <gtest/gtest.h>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <stdio.h>

// Task for running tests on ESP32
void run_tests_task(void *pvParameters) {
    printf("Running GoogleTest on ESP32\n");
    
    int argc = 1;
    char *argv[] = {(char*)"test_runner"};
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    
    printf("Tests complete with return code: %d\n", ret);
    fflush(stdout);
    
    // Delay to allow output to be fully sent before potential restart
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    #ifdef QEMU_TESTS
    // For QEMU, just end the task
    printf("Test execution complete\n");
    #else
    // On real hardware, restart after tests
    if (ret != 0) {
        printf("Tests failed, restarting...\n");
        esp_restart();
    }
    #endif
    
    vTaskDelete(NULL);
}

extern "C" void app_main() {
    // Higher stack size as GoogleTest requires more memory
    xTaskCreate(run_tests_task, "tests", 8192, NULL, 5, NULL);
}

#else
// Regular main for native platforms
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif