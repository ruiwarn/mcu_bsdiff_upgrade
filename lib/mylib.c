/**
  ******************************************************************************
  * @file    mylib.c
  * @author  eming
  * @version V1.0.0
  * @date    2022-03-21
  * @brief   定义一些公用函数
  ******************************************************************************
  */
#include "stdint.h"
// #include "ustdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "bs_type.h"

/**
 * @brief 和校验
 * @details 字节数组的和校验函数
 *
 * @param *p 数组指针
 * @param size 数组长度
 * @return 求和结果
 *   @retval 无
 */
uint32_t mylib_sum(const uint8_t *p, uint16_t size)
{
    uint32_t sum = 0;

    while(size--)
    {
        sum += *(p++);
    }
    return(sum);
}

/**
 * @brief 数据比较
 * @details 字节数组的数据比较
 *
 * @param *p1 数组1指针
 * @param *p2 数组2指针
 * @param size 比较的数据块大小
 * @return 比较结果
 *   @retval TRUE 相等
 *   @retval FALSE 不相等
 */
int mylib_memcmp(const void *p1, const void *p2, int size)
{
    uint8_t *bp1, *bp2;
    
    bp1 = (uint8_t*)p1;
    bp2 = (uint8_t*)p2;
    while(size--)
    {
        if(*(bp1++) != *(bp2++))return(FALSE);
    }
    return(TRUE);
}

/**
 * @brief 数据比较
 * @details 字节数组与指定数据比较
 *
 * @param *p1 数组指针
 * @param value 被比较的数值
 * @param size 比较的数据块大小
 * @return 比较结果
 *   @retval TRUE 相等
 *   @retval FALSE 不相等
 */
int mylib_memcmp_b(uint8_t *p1, uint8_t value, int size)
{
    while(size--)
    {
        if(*(p1++) != value)return(FALSE);
    }
    return(TRUE);
}

/**
 * @brief 延时函数
 * @details 大概为1uS的延时函数
 *
 * @param us 需要延时的微秒数
 * @return 无
 *   @retval 无
 */
void mylib_uDelay(uint32_t us)
{
    int i;
    
    for(; us > 0; us--)
    {
        for(i = 0; i < 40; i++);
    }
}

/**
 * @brief CRC16函数
 * @details 计算字节数组的CRC16
 *
 * @param *p 数组指针
 * @param size 数组长度
 * @return 计算结果
 *   @retval uint16_t CRC16值
 */
uint16_t mylib_crc16(uint8_t *p, uint16_t size)
{
    uint16_t crc;                        //校验码
    uint16_t i;
    uint16_t j;
    crc = 0xFFFF;
    for (i = 0; i < size; i++)
    {
        crc ^= *p;
        for (j = 0; j < 8; j++)
        {
            if ((crc & 1) == 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        p++;
    }
    return(crc);
}

/**
 * @brief 16位大小端转换
 * @details 数据大端小段互相转换函数
 *
 * @param *frm 源数组指针
 * @param *to 目标数组指针
 * @param size 转换的数据个数(半字)
 * @return 无
 *   @retval 无
 */
void mylib_BigtoLittle16(const void *frm, void *to, uint16_t size)
{
    uint8_t  buff[2];
    uint8_t 	*fp, *tp;

    fp = (uint8_t *)frm;
    tp = (uint8_t *)to;
    while(size--)
    {
        buff[0] = fp[0];
        buff[1] = fp[1];
        tp[0] 	= buff[1];
        tp[1] 	= buff[0];

        tp  += 2;
        fp += 2;
    }
}

/**
 * @brief 32位大小端转换
 * @details 数据大端小段互相转换函数
 *
 * @param *frm 源数组指针
 * @param *to 目标数组指针
 * @param size 转换的数据个数(字)
 * @return 无
 *   @retval 无
 */
void mylib_BigtoLittle32(const void *frm, void *to, uint16_t size)
{
    uint8_t buff[4];
    uint8_t *fp, *tp;

    fp = (uint8_t *)frm;
    tp = (uint8_t *)to;
    while(size--)
    {
        buff[0] = fp[0];
        buff[1] = fp[1];
        buff[2] = fp[2];
        buff[3] = fp[3];
        tp[0] 	= buff[3];
        tp[1] 	= buff[2];
        tp[2] 	= buff[1];
        tp[3] 	= buff[0];

        tp  += 4;
        fp += 4;
    }
}

/**
 * @brief 64位浮点数大小端转换
 * @details 数据大端小段互相转换函数
 *
 * @param dat 浮点数
 * @return 转换结果
 *   @retval 转换后的浮点数
 */
double mylib_BigtoLittle_fp64(double dat)
{
    F64_U8 tmp1, tmp2;

    tmp1.f64 = dat;
    tmp2.d8[0] = tmp1.d8[7];
    tmp2.d8[1] = tmp1.d8[6];
    tmp2.d8[2] = tmp1.d8[5];
    tmp2.d8[3] = tmp1.d8[4];
    tmp2.d8[4] = tmp1.d8[3];
    tmp2.d8[5] = tmp1.d8[2];
    tmp2.d8[6] = tmp1.d8[1];
    tmp2.d8[7] = tmp1.d8[0];
    
    return(tmp2.f64);
}

/**
 * @brief BCD编码函数
 * @details 二进制数据转BCD码
 *
 * @param hex 二进制数据
 * @return 转换结果
 *   @retval BCD码
 */
uint8_t mylib_HEXtoBCD(uint8_t hex)
{
    uint8_t bcd = 0;

    bcd = hex / 10;
    bcd <<= 4;
    bcd += hex % 10;

    return(bcd);
}

/**
 * @brief BCD解码函数
 * @details BCD码转二进制数据
 *
 * @param bcd BCD码数据
 * @return 转换结果
 *   @retval 二进制数据
 */
uint8_t mylib_BCDtoHEX(uint8_t bcd)
{
    uint8_t hex = 0;

    hex = ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);
    return(hex);
}

/**
 * @brief 数值转字符串
 * @details 整形数值转字符串
 *
 * @param value 整形数值
 * @param *str 转换结果字符串指针
 * @param radix 进制
 * @return 换结果字符串指针
 */
char *my_itoa(int value, char *str, uint8_t radix)
{
    static char szMap[] =
    {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };  // 字符映射表
    int nCount = -1, nIndex;
    char *pStr = str, nTemp;
    unsigned int nValue = *(unsigned *)&value;

    if(radix >= 2 && radix <= 36 )          //限制radix必须在2到36之间
    {
        if(value < 0 && radix == 10)        //如果是负数就在首位添加负号，并将字符串前移
        {
            *pStr++ = '-';
            value = -value;                 //转为正数，
        }
        do                                  //循环转换每一个数字，直到结束
        {
            pStr[++nCount] = szMap[nValue % radix];
            nValue /= radix;
        }
        while(nValue > 0);                  //转换结束后字符串是翻的
        nIndex = (nCount + 1) / 2;          //计算出一半的长度
        while(nIndex-- > 0)                 //将字符串的字符序翻转
        {
            nTemp = pStr[nIndex];
            pStr[nIndex] = pStr[nCount - nIndex];
            pStr[nCount - nIndex] = nTemp;
        }
    }
    pStr[nCount + 1] = '\0';                // 置结束符
    return str;
}

/**
 * @brief 数组转字符串
 * @details 字节数组转字符串
 *
 * @param *str 转换后的字符串
 * @param bytes 被转换的字节数组
 * @param size 被转换的字节数组长度
 * @return 转换出的字符串长度
 */
int mylib_bytes_to_string(char *str, const uint8_t *bytes, int size)
{
    int i;

    char hexArray[] = "0123456789ABCDEF";

    for (i = 0; i < size; i++)
    {
        int v = bytes[i] & 0xFF;
        str[i * 2] = hexArray[v >> 4];
        str[i * 2 + 1] = hexArray[v & 0x0F];
    }
    str[size * 2] = '\0';

    return (size * 2);
}

/**
 * @brief 字符串转数组
 * @details 字符串转字节数组
 *
 * @param *str 被转换的字符串
 * @param bytes 转换出的字节数组
 * @return 转换出的字节数组长度
 */
int mylib_string_to_bytes(char *str, uint8_t *bytes)
{
    uint8_t 	tmp, tmp1;
    uint16_t len;
    uint16_t dat_len;

    len = strlen(str);
    if(len % 2)				  			//检查是否为偶数
    {
        str[len] = '0';					//补0
        len++;
        str[len] = '\0';
    }
    dat_len = len / 2;
    while(len)
    {
        if((*str > 47) && (*str < 58))	  	//0~9
        {
            tmp = *str - 48;
        }
        else if((*str > 64) && (*str < 71))	//A~F
        {
            tmp = *str - 55;
        }
        else if((*str > 96) && (*str < 103))	//a~f
        {
            tmp = *str - 87;
        }
        else
        {
            tmp = 32;
        }
        tmp <<= 4;
        str++;
        if((*str > 47) && (*str < 58))	  	//0~9
        {
            tmp1 = *str - 48;
        }
        else if((*str > 64) && (*str < 71))	//A~F
        {
            tmp1 = *str - 55;
        }
        else if((*str > 96) && (*str < 103))	//a~f
        {
            tmp1 = *str - 87;
        }
        else
        {
            tmp1 = 32;
        }
        tmp += tmp1;
        *(bytes++) = tmp;
        str++;
        len -= 2;
    }
    
    return(dat_len);
}

/**
 * @brief 大小写转换
 * @details 将字符串中的大写字符变为小写字符
 *
 * @param *str 被转换的字符串
 * @return 转换出的字符串
 */
char *strlwr(char *str)
{
    uint16_t len;
    char   *p;

    len = strlen(str);
    p = str;
    while(len--)
    {
        if((*p > 64) && (*p < 91))
        {
            *p += 32;
        }
        p++;
    }
    return(str);
}

/**
 * @brief 字符串提取数字
 * @details 字符串提取数字字符,去除其他符号
 *
 * @param *str 被处理的字符串
 * @param *num 结果字符串
 * @return 提取出的字符串长度
 */
int strval(char *str, char *num)
{
    int len = 0;

    while(*str != '\0')
    {
        if((*str > 47) && (*str < 58))
        {
            *num = *str;
            num++;
            len++;
        }
        str++;
    }
    *num = '\0';
    
    return(len);
}

/**
 * @brief 打印16进制数据
 * @details 由于调试时输出二进制数据
 *
 * @param *p 二进制数据指针
 * @param size 数据长度
 * @return 无
 */
void printk_hex(const uint8_t *p, int size)
{
//    int i;

//    for(i = 0; i < size; i++)
//    {
//        printk(" %02X", *(p++));
//    }
}

/**
 * @brief utc秒转时间
 * @details 包含时区参数
 *
 * @param utc_s utc秒
 * @param *t 转换结果
 * @param tz 时区
 * @return 无
 */
void utc_to_time(uint32_t utc_s, struct tm *t, int tz)
{
    #define SECOND_OF_DAY     86400      		    //1天的秒数    
    uint16_t i, j, iDay;
    uint32_t lDay;
    const uint8_t DayOfMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    utc_s += 3600 * tz;    //时区

    lDay = utc_s / SECOND_OF_DAY;
    utc_s = utc_s % SECOND_OF_DAY;

    i = 1970;										//星期取值为0-6
    t->tm_wday = (lDay + 3) % 7;					//1970年1月1号是星期4
    while(lDay > 365)
    {
        if(((i % 4 == 0) && (i % 100 != 0)) || (i % 400 == 0))    	// 闰年
            lDay -= 366;
        else
            lDay -= 365;
        i++;
    }
    if((lDay == 365) && !(((i % 4 == 0) && (i % 100 != 0)) || (i % 400 == 0))) //平年
    {
        lDay -= 365;
        i++;
    }
    t->tm_year = i - 1900;     					//得到年份
    for(j = 0; j < 12; j++)     		        //计算月份
    {
        if((j == 1) && (((i % 4 == 0) && (i % 100 != 0)) || (i % 400 == 0)))
            iDay = 29;
        else
            iDay = DayOfMon[j];
        if(lDay >= iDay) lDay -= iDay;
        else break;
    }
    t->tm_mon  = j;
    t->tm_mday = lDay + 1;
    t->tm_hour = utc_s / 3600;
    t->tm_min  = (utc_s % 3600) / 60;
    t->tm_sec  = (utc_s % 3600) % 60;
}

/**
 * @brief 计算日期是一年中的那一天
 * @details 计算日期的年索引,2月当做29天算
 *
 * @param utc_s utc秒
 * @param *t 转换结果
 * @param tz 时区
 * @return 无
 */
int mylib_day_index(uint8_t mon, uint8_t day)
{
    const  uint8_t  Line_Mon_Len[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int index = 0;
    int  i;

    if((mon < 1) || (mon > 12))return(0); //参数判断
    if((day < 1) || (day > 31))return(0);

    for(i = 1; i < mon; i++)
    {
        index += Line_Mon_Len[i - 1];
    }
    index += day - 1;

    return(index);
}

/**
 * @brief 16进制字符串转数据
 * @details 16进制字符串转数据,可以包含"0x"头
 *
 * @param *s 字符串
 * @return 提取的数值
 */
int tolower(int c)  
{  
    if (c >= 'A' && c <= 'Z')  
    {  
        return c + 'a' - 'A';  
    }  
    else  
    {  
        return c;  
    }  
}

uint32_t htoi(const char *s)  
{  
    int i;  
    uint32_t n = 0; 

    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
        if (tolower(s[i]) > '9')  
        {  
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        }  
        else  
        {  
            n = 16 * n + (tolower(s[i]) - '0');  
        }  
    }  
    return n;  
} 

/**
 * @brief 返回堆内存的大小
 * @details 返回堆内存的大小
 *
 * @param *mp 堆内存指针
 * @return 堆内存的大小
 */
uint32_t _msize(const void *mp)
{
    return(*((int*)((uint32_t)mp - 4)));
}

/*******************************************************************************************************
**                            End Of File
********************************************************************************************************/


