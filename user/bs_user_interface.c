/**
 * @file bs_user_func.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "bs_user_interface.h"
#include "bs_type.h"

bs_user_interface bs_user_func;

/**
 * @brief 用户需要注册的函数
 *
 * @param user_func 用户创建一个结构体然后将功能依次注册
 * @return 0成功 1失败
 */
int bs_user_func_register(bs_user_interface *user_func)
{
    if (user_func == NULL || user_func->bs_flash_write == NULL
            || user_func->bs_malloc == NULL
            || user_func->bs_free == NULL
       )
    {
        return 1;
    }
    bs_user_func.bs_flash_write = user_func->bs_flash_write;
    bs_user_func.bs_malloc = user_func->bs_malloc;
    bs_user_func.bs_free = user_func->bs_free;
    return 0;
}





