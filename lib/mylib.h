#ifndef __MYLIB_H
#define __MYLIB_H
/*********************************************************************************************************/
#include "stdint.h"
#include "time.h"
#include "bs_type.h"


uint32_t mylib_sum(const uint8_t *p, uint16_t size);
int mylib_memcmp(const void *p1, const void *p2, int size);
int mylib_memcmp_b(uint8_t *p1, uint8_t value, int size);
void mylib_uDelay(uint32_t us);
uint16_t mylib_crc16(uint8_t *p, uint16_t size);
void mylib_BigtoLittle16(const void *frm, void *to, uint16_t size);
void mylib_BigtoLittle32(const void *frm, void *to, uint16_t size);
double mylib_BigtoLittle_fp64(double dat);
uint8_t mylib_HEXtoBCD(uint8_t hex);
uint8_t mylib_BCDtoHEX(uint8_t bcd);
char *my_itoa(int value, char *str, uint8_t radix);
int mylib_bytes_to_string(char *str, const uint8_t *bytes, int size);
int mylib_string_to_bytes(char *str, uint8_t *bytes);
char *strlwr(char *str);
int strval(char *str, char *num);
void printf_hex(const uint8_t *p, int size);
void utc_to_time(uint32_t utc_s, struct tm *t, int tz);
int mylib_day_index(uint8_t mon, uint8_t day);
uint32_t htoi(const char *s);
uint32_t _msize(const void *mp);


/*********************************************************************************************************/
#endif
/*******************************************************************************************************
**                            End Of File
********************************************************************************************************/

