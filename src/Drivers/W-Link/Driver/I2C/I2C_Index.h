
#ifndef I2C_INDEX_H
#define I2C_INDEX_H

#ifdef STM32F207xx
#undef DEFAULT_I2C_INDEX
typedef enum hwI2C_Index_t
{
  hwI2C_Index_0 = 0,
  hwI2C_Index_1,
  hwI2C_Index_2,
  hwI2C_Index_3,
  hwI2C_Index_MAX,
}hwI2C_Index;
#endif //STM32F207xx

#ifdef STM32F767xx
#undef DEFAULT_I2C_INDEX
typedef enum hwI2C_Index_t
{
  hwI2C_Index_0 = 0,
  hwI2C_Index_1,
  hwI2C_Index_2,
  hwI2C_Index_3,
  hwI2C_Index_MAX,
}hwI2C_Index;
#endif //STM32F767xx

#ifdef STM32H723xx
#undef DEFAULT_I2C_INDEX
typedef enum hwI2C_Index_t
{
  hwI2C_Index_0 = 0,
  hwI2C_Index_1,
  hwI2C_Index_2,
  hwI2C_Index_3,
  hwI2C_Index_4,
  hwI2C_Index_5,
  hwI2C_Index_MAX,
}hwI2C_Index;
#endif //STM32H723xx

#endif //I2C_INDEX_H