#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "SysCtrl/SysCtrl.h"

#include "NeonRTOS.h"
#include "NeonTCPIP.h"
#include "NeonAppInterface.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // 堆疊溢出處理
    while (1);
}

void vApplicationIdleHook(void) {
    // MCU 進入低功耗模式，等待中斷
    __WFI(); // Wait For Interrupt
}

void vApplicationTickHook(void) {
    HAL_IncTick(); // 增加 HAL 的滴答計數
}

void SystemClock_Config(void) {
    SysCtrl_Init();
}

int main(void) {
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_WWDG_CLK_DISABLE();  // 禁用窗口看門狗
    //__HAL_RCC_IWDG_CLK_DISABLE();  // 禁用獨立看門狗

    Neon_App_Init();

    // 啟動 NeonRTOS 調度器
    NeonRTOS_start();

    while (1);
}
//