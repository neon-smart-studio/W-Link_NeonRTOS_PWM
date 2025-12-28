
#ifndef I2C_INDEX_H
#define I2C_INDEX_H

#define DEFAULT_I2C_INDEX

#ifdef DEVICE_RTL8195AM
#undef DEFAULT_I2C_INDEX
typedef enum hwI2C_Index_t
{
  hwI2C_Index_0 = 0,
  hwI2C_Index_1,
  hwI2C_Index_3,
  hwI2C_Index_MAX,
}hwI2C_Index;
#endif //DEVICE_RTL8195AM

#ifdef DEVICE_RTL8710AF
#undef DEFAULT_I2C_INDEX
typedef enum hwI2C_Index_t
{
  hwI2C_Index_0 = 0,
  hwI2C_Index_MAX,
}hwI2C_Index;
#endif //DEVICE_RTL8710AF

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

#ifdef DEFAULT_I2C_INDEX
typedef enum hwI2C_Index_t
{
  hwI2C_Index_0 = 0,
  hwI2C_Index_MAX = 1
}hwI2C_Index;
#endif //DEFAULT_I2C_INDEX

#endif //I2C_INDEX_H