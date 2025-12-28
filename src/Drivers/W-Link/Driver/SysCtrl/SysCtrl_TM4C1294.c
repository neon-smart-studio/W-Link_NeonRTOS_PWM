
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#ifdef DEVICE_TM4C1294

#define TM4C1294_CLOCK_HZ 120000000

void SysCtrl_Init()
{
    MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                            SYSCTL_OSC_MAIN |
                            SYSCTL_USE_PLL |
                            SYSCTL_CFG_VCO_240), TM4C1294_CLOCK_HZ);
                                             
}

#endif //DEVICE_TM4C1294
