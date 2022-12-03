#ifndef __VFILE_H__
#define __VFILE_H__
/*******************************************************************************************************/
#include "stdint.h"

typedef struct
{
    uint8_t *curptr;
    uint32_t offset;
    uint32_t size;
}vFile;

/*******************************************************************************************************/
vFile *vfopen(const uint8_t *dp, uint32_t size);
int vfread(vFile *fp, uint8_t *buff, int len);
uint8_t *vfgetpos(vFile *fp, uint32_t *position);
int vfsetpos(vFile *fp, uint32_t position);
int vfclose(vFile *fp);
uint32_t vfgetlen(vFile *fp);

#endif
/*******************************************************************************************************
**                            End Of File
********************************************************************************************************/
