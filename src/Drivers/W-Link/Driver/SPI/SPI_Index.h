
#ifndef SPI_MASTER_INDEX_H
#define SPI_MASTER_INDEX_H

#define SPI_DEFAULT_INDEX

#ifdef STM32F207xx
#undef SPI_DEFAULT_INDEX
typedef enum hwSPI_Index_t
{
  hwSPI_Index_0 = 0,
  hwSPI_Index_1,
  hwSPI_Index_2,
  hwSPI_Index_MAX,
}hwSPI_Index;
#endif //STM32F207xx

#ifdef STM32F767xx
#undef SPI_DEFAULT_INDEX
typedef enum hwSPI_Index_t
{
  hwSPI_Index_0 = 0,
  hwSPI_Index_1,
  hwSPI_Index_2,
  hwSPI_Index_3,
  hwSPI_Index_4,
  hwSPI_Index_5,
  hwSPI_Index_MAX,
}hwSPI_Index;
#endif //STM32F767xx

#ifdef STM32H723xx
#undef SPI_DEFAULT_INDEX
typedef enum hwSPI_Index_t
{
  hwSPI_Index_0 = 0,
  hwSPI_Index_1,
  hwSPI_Index_2,
  hwSPI_Index_3,
  hwSPI_Index_4,
  hwSPI_Index_5,
  hwSPI_Index_MAX,
}hwSPI_Index;
#endif //STM32H723xx

#ifdef SPI_DEFAULT_INDEX
typedef enum hwSPI_Index_t
{
  hwSPI_Index_0 = 0,
  hwSPI_Index_MAX = 1,
}hwSPI_Index;
#endif //SPI_DEFAULT_INDEX

#endif //SPI_MASTER_INDEX_H