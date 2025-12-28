
#ifndef DELAY_H
#define DELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

void Delay_uS(uint32_t us);
void Delay_mS(uint32_t ms);
void Delay(float sec);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //DELAY_H