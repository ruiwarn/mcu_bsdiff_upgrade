#ifndef __TYPE_H__
#define __TYPE_H__
/********************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ERROR
#define ERROR 0xFF
#endif

// typedef unsigned char       bool;
// typedef unsigned char       uint8_t;	    /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
// typedef signed   char       int8;       /* defined for signed 8-bits integer variable		有符号8位整型变量  */
// typedef unsigned short      uint16_t;	    /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
// typedef signed   short      int16; 	    /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
// typedef unsigned long       uint32_t;     /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
// typedef signed   long       int32; 	    /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
// typedef float               fp32;  	    /* single precision floating point variable (32bits)单精度浮点数（32位长度） */
// typedef unsigned long long  uint64;     /* defined for unsigned 64-bits integer variable 	无符号64位整型变量 */
// typedef signed   long long  int64; 	    /* defined for signed 64-bits integer variable 		有符号64位整型变量 */
// typedef double              fp64;       /* double precision floating point variable (64bits)双精度浮点数（64位长度） */
/********************************************************************************************************/
#define BigtoLittle16(x)    ((((uint16_t)(x) & 0xff00) >> 8) | (((uint16_t)(x) & 0x00ff) << 8))
#define BigtoLittle32(x)    ((((uint32_t)(x) & 0xff000000) >> 24) | \
                            (((uint32_t)(x) & 0x00ff0000) >> 8) | \
                            (((uint32_t)(x) & 0x0000ff00) << 8) | \
                            (((uint32_t)(x) & 0x000000ff) << 24))
/********************************************************************************************************/
//定义一个联合，将一个64位数分拆成4个8位数，以便于计算
//M3内核为小端方式,即低字节在低位,高字节在高位
typedef union
{
    uint64_t d64;
    uint8_t  d8[8];
} U64_U8;
//定义一个联合，将一个64位数分拆成4个8位数，以便于计算
//M3内核为小端方式,即低字节在低位,高字节在高位
typedef union
{
    double   f64;
    uint8_t  d8[8];
} F64_U8;
//定义一个联合，将一个32位数分拆成4个8位数，以便于计算
//M3内核为小端方式,即低字节在低位,高字节在高位
typedef union
{
    uint32_t d32;
    uint8_t  d8[4];
} U32_U8;
/********************************************************************************************************/
//定义一个联合，将一个16位数分拆成2个8位数，以便于计算
typedef union
{
    uint16_t d16;
    uint8_t  d8[2];
} U16_U8;
/********************************************************************************************************/
//定义一个联合，将一个16位数分拆成2个8位数，以便于计算
typedef union
{
    int16_t  d16;
    uint8_t  d8[2];
} I16_U8;
/********************************************************************************************************/
#endif
