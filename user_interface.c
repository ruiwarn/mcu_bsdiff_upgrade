/**
 * @file user_interface.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "user_interface.h"
#include "bs_type.h"
#include "flash_ep.h"

/**
 * @brief 将差分文件写入用户的flash,用户自己决定是否在写之前擦除
 *
 * @param addr
 * @param p
 * @param len
 * @return int
 */
int bs_flash_write(uint32_t addr, const void *p, uint32_t len)
{

    if (xmq25qxx_write(addr, (uint8_t *)p, len))
    {
        printf("xmq25qxx_write err");
        return 1;  /* 写入flash失败返回错误码 */
    }
    return 0;
}




