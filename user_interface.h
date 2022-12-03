/**
 * @file user_interface.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __USER_INTERFACE_H_
#define __USER_INTERFACE_H_

#include <stdint.h>
#include <stdio.h>
#include "common_utils.h"
#include "bs_type.h"
#include "bspatch.h"
#include "crc32.h"


#define IH_NMLEN   (32 - 4)     /* Image Name Length        */

/* 差分包制作时自带的文件头信息，用户只需要关心中文注释的部分 */
typedef struct image_header
{
    uint32_t ih_magic;  /* Image Header Magic Number */
    uint32_t ih_hcrc;   /* Image Header CRC Checksum 差分包包头校验 */
    uint32_t ih_time;   /* Image Creation Timestamp */
    uint32_t ih_size;   /* Image Data Size 差分包的大小 */
    uint32_t ih_load;   /* Data Load Address 上一版本旧文件的大小 */
    uint32_t ih_ep;     /* Entry Point Address 要升级的新文件的大小 */
    uint32_t ih_dcrc;   /* Image Data CRC Checksum 新文件的CRC */
    uint8_t  ih_os;     /* Operating System */
    uint8_t  ih_arch;   /* CPU architecture */
    uint8_t  ih_type;   /* Image Type */
    uint8_t  ih_comp;   /* Compression Type */
    uint8_t  ih_name[IH_NMLEN]; /* Image Name */
    uint32_t ih_ocrc;   /* Old Image Data CRC Checksum 上一版本旧文件的CRC */
} image_header_t; 
/* 差分包制作时自带的文件头信息，用户只需要关心中文注释的部分 */

/* 以下由用户实现 */
#define bs_printf(...)
// #define bs_printf(...) APP_PRINT( __VA_ARGS__)
#define bs_malloc(...) pvPortMalloc(__VA_ARGS__)
#define bs_free(...) vPortFree(__VA_ARGS__)

/* 将差分文件写入用户的flash */
extern int bs_flash_write(uint32_t addr, const void *p, uint32_t len);


/**
 * @brief 用户使用差分升级时唯一需要关心的接口
 * 
 * @param old 设备中执行区代码所在的地址，用户可指定flash执行区的地址，方便算法读出来当前
 *            运行中的代码
 * @param oldsize 设备中执行区代码的长度，用户可在差分包bin头获取
 * @param patch 设备中已经下载的差分包所在的flash地址，或者ram地址，只要能让算法读出来即可
 *              注意，下载的差分包自带image_header_t格式的文件头，真正的差分包需要偏
 *              移sizeof(image_header_t)的长度
 * @param patchsize 设备中已经下载的差分包的长度，用户可在差分包bin头获取
 * @param newfile 新文件的大小，用户需填入新版本bin的长度，用户亦可以差分包bin头获取
 * @return int 然后错误码，0成功，1失败
 */
extern int iap_patch(const uint8_t* old, uint32_t oldsize, const uint8_t* patch,
                     uint32_t patchsize, uint32_t newfile);



#endif // !__USER_INTERFACE_H_

