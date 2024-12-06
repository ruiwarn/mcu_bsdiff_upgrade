/**
 * @file bs_user_func.h
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
#include "bs_type.h"
#include "bspatch.h"
#include "crc32.h"

#ifdef __DEBUG
#define bs_printf(...) printf("\r\n"format, ##__VA_ARGS__)
#else
#define bs_printf(...)
#endif

#define IH_NMLEN   (32 - 4)     /* Image Name Length        */
typedef struct image_header
{
    uint32_t ih_magic;  /* Image Header Magic Number */
    uint32_t ih_hcrc;   /* Image Header CRC Checksum 差分包包头校验 大端 */
    uint32_t ih_time;   /* Image Creation Timestamp */
    uint32_t ih_size;   /* Image Data Size 差分包的大小 大端 */
    uint32_t ih_load;   /* Data Load Address 上一版本旧文件的大小 */
    uint32_t ih_ep;     /* Entry Point Address 要升级的新文件的大小 */
    uint32_t ih_dcrc;   /* Image Data CRC Checksum 新文件的CRC 大端 */
    uint8_t  ih_os;     /* Operating System */
    uint8_t  ih_arch;   /* CPU architecture */
    uint8_t  ih_type;   /* Image Type */
    uint8_t  ih_comp;   /* Compression Type */
    uint8_t  ih_name[IH_NMLEN]; /* Image Name */
    uint32_t ih_ocrc;   /* Old Image Data CRC Checksum 上一版本旧文件的CRC 大端*/
} image_header_t;

/**
 * @brief 用户需注册的flash写入功能函数，bs会调用该函数写入用户的flash，特别注意在写入之前
 * 用户需要擦除，也就是该函数需自带擦除功能
 * @param addr bs将要写入的flash地址
 * @param p bs传入要写入的内容
 * @param len bs传入要写的长度
 * @return int 0代表成功，其他为错误码
 */
typedef int (*bs_flash_write_func)(uint32_t addr, const unsigned char *p, uint32_t len);

/**
 * @brief 用户需注册的内存申请函数，bs会调用该函数申请内存，请注意，最大可能会申请20k左右堆空间
 * @param size 要申请的内存大小
 * @return void* 返回非NULL代表成功
 */
typedef void *(*bs_malloc_func)(uint32_t size);

/**
 * @brief 用户需注册的内存释放函数，bs会调用该函数释放内存
 * @param ptr 要释放的内存地址
 */
typedef void (*bs_free_func)(void *ptr);

typedef struct
{
    bs_flash_write_func bs_flash_write;
    bs_malloc_func bs_malloc;
    bs_free_func bs_free;
} bs_user_interface;
extern bs_user_interface bs_user_func;



/******************************************************************************/
/************************************ 用户提供 *********************************/
/**
 * @brief 用户需要注册的函数
 *
 * @param user_func 用户创建一个结构体然后将功能依次注册
 * @return 0成功 1失败
 */
int bs_user_func_register(bs_user_interface *user_func);

/******************************************************************************/
/************************************ 用户接口 *********************************/
/**
 * @brief 解压并还原文件，用户使用差分升级时唯一需要关心的接口
 *
 * @param old 设备中执行区代码所在的地址，用户可指定flash执行区的地址，方便算法读出来当前
 *            运行中的代码，用户提供
 * @param oldsize 设备中执行区代码的长度，用户可在差分包bin头获取，用户提供
 * @param patch 设备中已经下载的差分包所在的flash地址，或者ram地址，只要能让算法读出来即可
 *              注意，下载的差分包自带image_header_t格式的文件头，真正的差分包需要偏
 *              移sizeof(image_header_t)的长度，用户提供
 * @param patchsize 设备中已经下载的差分包的长度，用户提供，可在差分包bin头获取
 * @param newfile_addr 还原后的bin写入的地址，用户提供
 * @return int 还原的文件大小
 */
extern int iap_patch(const uint8_t *old, uint32_t oldsize, const uint8_t *patch,
                     uint32_t patchsize, uint32_t newfile_addr);

/* bs所使用到的crc32校验函数，用户可酌情使用 */
extern unsigned int crc32( const unsigned char *buf, unsigned int size);

#endif // !__USER_INTERFACE_H_

