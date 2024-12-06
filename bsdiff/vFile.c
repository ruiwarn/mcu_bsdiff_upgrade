/**
  ******************************************************************************
  * @file    vFile.c
  * @author  eming
  * @version V1.0.0
  * @date    2022-03-21
  * @brief   虚拟文件接口,将保存在内部Flash的数据虚拟为文件
  ******************************************************************************
  */

#include <stdlib.h>
#include <string.h>
#include "vFile.h"
#include "bs_user_interface.h"


void *vmalloc(size_t size)
{
    if (bs_user_func.bs_malloc == NULL)
    {
        return NULL;
    }
    return bs_user_func.bs_malloc((size_t)size);
}

void vfree(void *ptr)
{
    if (bs_user_func.bs_free == NULL)
    {
        return;
    }
    bs_user_func.bs_free(ptr);
}

vFile *vfopen(const uint8_t *dp, uint32_t size)
{
    vFile *fp = NULL;

    fp = vmalloc(sizeof(vFile));
    if (fp != NULL)
    {
        fp->curptr = (uint8_t *)dp;
        fp->offset = 0;
        fp->size = size;
    }

    return (fp);
}

int vfread(vFile *fp, uint8_t *buff, int len)
{
    if (fp != NULL)
    {
        if ((fp->offset + len) > fp->size)
        {
            len = fp->size - fp->offset;
        }
        memcpy(buff, fp->curptr + fp->offset, len);
        fp->offset += len;

        return (len);
    }

    return (0);
}

uint8_t *vfgetpos(vFile *fp, uint32_t *position)
{
    if (fp != NULL)
    {
        *position = fp->offset;

        return (fp->curptr + fp->offset);
    }

    return (NULL);
}

int vfsetpos(vFile *fp, uint32_t position)
{
    if (fp != NULL)
    {
        fp->offset = position;
        return (fp->offset);
    }
    return -1;
}

int vfclose(vFile *fp)
{
    if (fp != NULL)
    {
        vfree(fp);
    }

    return (0);
}

uint32_t vfgetlen(vFile *fp)
{
    return (fp->size);
}

/*******************************************************************************************************
**                            End Of File
********************************************************************************************************/

