/*-
 * Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "bspatch.h"
#include "vFile.h"
#include "bs_user_interface.h"

#define WRITE_BLOCK_SIZE    1024

int64_t offtin(uint8_t *buf)
{
    int64_t y;
    
    y = buf[7] & 0x7F;
    y = y * 256;
    y += buf[6];
    y = y * 256;
    y += buf[5];
    y = y * 256;
    y += buf[4];
    y = y * 256;
    y += buf[3];
    y = y * 256;
    y += buf[2];
    y = y * 256;
    y += buf[1];
    y = y * 256;
    y += buf[0];
    
    if (buf[7] & 0x80)
    {
        y = -y;
    }
    
    return y;
}

int bspatch(const uint8_t *old, int64_t oldsize, int64_t newsize, struct bspatch_stream *stream)
{
    uint8_t buf[8];
    uint8_t *buf_data;
    int64_t oldpos, newpos, len;
    int64_t ctrl[3];
    int64_t i;
    
    buf_data = (uint8_t *)vmalloc(WRITE_BLOCK_SIZE + 1);
    
    if (buf_data == NULL)
    {
        return -1;
    }
    
    oldpos = 0;
    newpos = 0;
    
    while (newpos < newsize)
    {
        /* Read control data */
        for (i = 0; i <= 2; i++)
        {
            if (stream->read(stream, buf, 8))
            {
                bs_printf("err%d", __LINE__);
                return -1;
            }
            
            ctrl[i] = offtin(buf);
        };
        
        /* Sanity-check */
        if (ctrl[0] < 0 || ctrl[0] > INT_MAX ||
                ctrl[1] < 0 || ctrl[1] > INT_MAX ||
                newpos + ctrl[0] > newsize)
        {
            bs_printf("err%d", __LINE__);
            return -1;
        }
        
        /* Read diff string */
        while (ctrl[0] > 0)
        {
            if (ctrl[0] > WRITE_BLOCK_SIZE)
            {
                len = WRITE_BLOCK_SIZE;
            }
            else
            {
                len = ctrl[0];
            }
            
            if (stream->read(stream, buf_data, len))
            {
                return -1;
            }
            
            for (i = 0; i < len; i++)
            {
                if ((oldpos + i >= 0) && (oldpos + i < oldsize))
                {
                    buf_data[i] += old[oldpos + i];
                }
            }
            
            stream->write(stream, buf_data, len);
            ctrl[0] -= len;
            oldpos += len;
            newpos += len;
        }
        
        /* Sanity-check */
        if (newpos + ctrl[1] > newsize)
        {
            bs_printf("err%d", __LINE__);
            return -1;
        }
        
        /* Read extra string */
        while (ctrl[1] > 0)
        {
            if (ctrl[1] > WRITE_BLOCK_SIZE)
            {
                len = WRITE_BLOCK_SIZE;
            }
            else
            {
                len = ctrl[1];
            }
            
            if (stream->read(stream, buf_data, len))
            {
                return -1;
            }
            
            stream->write(stream, buf_data, len);
            ctrl[1] -= len;
            newpos += len;
        }
        
        /* Adjust pointers */
        oldpos += ctrl[2];
        //printk("newpos = %d, ctrl[0] = %ld, ctrl[1] = %ld, ctrl[2] = %ld\n", newpos, ctrl[0], ctrl[1], ctrl[2]);
    };
    
    if (buf_data != NULL)
    {
        vfree(buf_data);
    }
    
    return 0;
}

//===========================================================
#include "lzma_decompress.h"
#include "vFile.h"
#include "bs_user_interface.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define DCOMPRESS_BUFFER_SIZE   1024
static uint8_t *diff_data_buff;
static int diff_data_len, diff_data_fp;
static int new_data_fp;

static int patch_data_read(const struct bspatch_stream *stream, void *buffer, int length)
{
    uint8_t *dp = (uint8_t *)buffer;
    vFile *pf;
    
    pf = (vFile *) stream->opaque_r;
    
    for (int i = 0; i < length; i++)
    {
        if (diff_data_len == 0)
        {
            diff_data_len = lzma_decompress_read(pf, diff_data_buff, DCOMPRESS_BUFFER_SIZE);
            
            if (diff_data_len > 0)
            {
                diff_data_fp = 0;
            }
            else
            {
                bs_printf("err%d", __LINE__);
                return -1;
            }
        }
        
        if (diff_data_len > 0)
        {
            *(dp++) = diff_data_buff[diff_data_fp++];
            diff_data_len--;
        }
    }
    
    return 0;
}

static int new_data_write(const struct bspatch_stream *stream, void *buffer, int length)
{
    uint32_t file_addr;
    
    if (bs_user_func.bs_flash_write == NULL)
    {
        bs_printf("err");
        return (FALSE);
    }
    
    file_addr = *((uint32_t *)stream->opaque_w);
    
    if (0 != bs_user_func.bs_flash_write(file_addr + new_data_fp, buffer, length))
    {
        bs_printf("err");
    }
    
    new_data_fp += length;
    
    return (TRUE);
}

static void patch_data_read_finish(void)
{
    lzma_decompress_finish();
    
}

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
 * @param newfile 还原后的bin写入的地址，用户提供
 * @return int 还原的文件大小
 */
int iap_patch(const uint8_t *old, uint32_t oldsize, const uint8_t *patch, uint32_t patchsize, uint32_t newfile_addr)
{
    vFile *patch_fp;
    struct bspatch_stream stream;
    uint8_t header[24];
    int64_t newsize;
    
    //初始化全局变量
    diff_data_len = 0;
    diff_data_fp = 0;
    new_data_fp = 0;
    patch_fp = vfopen(patch, patchsize);
    
    if (patch_fp == NULL)
    {
        bs_printf("Line 224 vmalloc err");
        return (0);
    }
    
    //读取差分文件头
    vfread(patch_fp, header, sizeof(header));
    bs_printf("patch_fp->offset:%d", patch_fp->offset);
    
    if (memcmp(header, "ENDSLEY/BSDIFF43", 16) != 0)
    {
        bs_printf("ENDSLEY/BSDIFF43 err: %s", header);
        
        for (uint32_t i = 0; i < sizeof(header); i++)
        {
            bs_printf("%02X ", header[i]);
        }
        
        return (0);
    }
    
    //计算新固件长度
    newsize = offtin(header + 16);
    
    if (newsize < 0)
    {
        bs_printf("newsize err");
        return (0);
    }
    
    //分配内存
    diff_data_buff = vmalloc(DCOMPRESS_BUFFER_SIZE);
    
    if (diff_data_buff == NULL)
    {
        bs_printf("\r\nmalloc err");
        return (0);
    }
    
    //准备合并文件
    stream.read = patch_data_read;
    stream.opaque_r = (void *)patch_fp;
    stream.write = new_data_write;
    stream.opaque_w = &newfile_addr;
    
    int res = bspatch(old, oldsize, newsize, &stream);
    (void)res;
    bs_printf("bspatch res:%d", res);
    
    //释放内存
    patch_data_read_finish();
    vfree(diff_data_buff);
    vfclose(patch_fp);
    
    return ((int)newsize);
}
