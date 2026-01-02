
#ifndef CAN_INDEX_H
#define CAN_INDEX_H

#ifdef STM32

typedef enum {
    hwCAN_Index_0 = 0,
    hwCAN_Index_1,
#if defined (CAN3_BASE)
    hwCAN_Index_2,
#endif
    hwCAN_Index_MAX
} hwCAN_Index;

#endif //STM32

#endif //CAN_INDEX_H