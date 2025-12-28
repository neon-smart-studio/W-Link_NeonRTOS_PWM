#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#include "Delay.h"

#ifdef STM32

static uint32_t s_cycles_per_us = 0;
static bool delay_has_init = false;

static void Delay_Init(void)
{
    // HCLK 週期/微秒
    s_cycles_per_us = HAL_RCC_GetHCLKFreq() / 1000000U;

#if (__CORTEX_M >= 0x03U)   // M3/M4/M7 才有 DWT
    // 啟用 DWT CYCCNT
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif
}

void Delay_uS(uint32_t us)
{
    if(delay_has_init==false)
    {
        Delay_Init();

        delay_has_init = true;
    }
    
#if (__CORTEX_M >= 0x03U)
    // 以 cycle counter busy-wait
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = (uint32_t)((uint64_t)us * (uint64_t)s_cycles_per_us);

    while ((uint32_t)(DWT->CYCCNT - start) < ticks) {
        __NOP();
    }
#else
    // Cortex-M0/M0+：沒有 DWT，這裡先給保底（不精準）
    // 建議改用 TIM 做 us delay（見下方說明）
    uint32_t ms = (us + 999U) / 1000U;
    HAL_Delay(ms);
#endif
}

void Delay_mS(uint32_t ms)
{
    HAL_Delay(ms);
}

void Delay(float sec)
{
    if (sec <= 0.0f) return;

    // 以 us 精度做 float 秒
    uint32_t us = (uint32_t)(sec * 1000000.0f);
    Delay_uS(us);
}

#endif //STM32
