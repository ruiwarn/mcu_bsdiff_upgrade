#ifndef __LZMA_SAMPLE_H__
#define __LZMA_SAMPLE_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "vFile.h"

int lzma_decompress_read(vFile* pf, uint8_t *buffer, int size);
void lzma_decompress_finish(void);

#ifdef __cplusplus
}
#endif

#endif
