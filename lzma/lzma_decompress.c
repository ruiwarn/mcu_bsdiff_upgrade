// #include <ustdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "user_interface.h"
#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "7zFile.h"
#include "vFile.h"
#include "mylib.h"

//#define LZMA_RAM_USE_DEBUG

#ifdef LZMA_RAM_USE_DEBUG
static int ram_used_size = 0;
static int ram_used_max = 0;
#endif

void *lzma_alloc(ISzAllocPtr p, size_t size)
{
    void *mp;

    if (size == 0)
    {
        return NULL;
    }

    mp = bs_malloc(size);

#ifdef LZMA_RAM_USE_DEBUG
    ram_used_size += _msize(mp);
    if (ram_used_max < ram_used_size)ram_used_max = ram_used_size;
    printk("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);
#endif

    return mp;
}

void lzma_free(ISzAllocPtr p, void *address)
{
    if (address != NULL)
    {
#ifdef LZMA_RAM_USE_DEBUG
        ram_used_size -= _msize(address);
        printk("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);
#endif
        bs_free(address);
    }
}

ISzAlloc allocator = {lzma_alloc, lzma_free};
static CLzmaDec *lz_state = NULL;

static int lzma_decompress_init(vFile *pf)
{
    if (lz_state != NULL) return 0;

    extern ISzAlloc allocator;
    UInt64 unpack_size = 0;
    uint8_t header[LZMA_PROPS_SIZE + 8];;
    size_t headerSize = sizeof(header);

    vfread(pf, header, headerSize);
    // debug_array(header, headerSize);
    // for (char i = 0; i < 8; i++)
    // {
    //     unpack_size |= (UInt64)(header[LZMA_PROPS_SIZE + i] << (i * 8));
    // }
    /* 以下为过静态检查写法 */
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 0]) << 0;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 1]) << 8;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 2]) << 16;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 3]) << 24;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 4]) << 32;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 5]) << 40;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 6]) << 48;
    unpack_size |= ((uint64_t)header[LZMA_PROPS_SIZE + 7]) << 56;
    // bs_printf("unpack_size:0X%08X", unpack_size);

    //分配解码内存
    lz_state = (CLzmaDec *)lzma_alloc(NULL, sizeof(CLzmaDec));

    if (lz_state != NULL)
    {
        LzmaDec_Construct(lz_state);
        LzmaDec_Allocate(lz_state, header, LZMA_PROPS_SIZE, &allocator);
        LzmaDec_Init(lz_state);

        return 0;
    }
    else
    {
        bs_printf("lzma_alloc err");
    }

    return -1;
}

int lzma_decompress_read(vFile *pf, uint8_t *buffer, int size)
{
    /* Start to decompress file  */
    uint32_t position, file_size;
    size_t dcmprs_size = 0;
    uint8_t *inBuf;
    SizeT inProcessed;
    SizeT outProcessed = size;
    ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
    ELzmaStatus status;
    SRes res;

    res = lzma_decompress_init(pf);
    if (res != 0)
    {
        bs_printf("err%d", __LINE__);
    }

    //获取当前文件读指针
    inBuf = vfgetpos(pf, &position);
    //检查文件还剩下多少字节
    file_size = vfgetlen(pf);
    // bs_printf("file_size:%d", file_size);
    if ((position + size) > file_size)
    {
        inProcessed = file_size - position;
    }
    else
    {
        inProcessed = size;
    }
    // bs_printf("inProcessed:%d", inProcessed);

    //解压数据
    if (inProcessed > 0 || inProcessed == 0)
    {
        res = LzmaDec_DecodeToBuf(lz_state, buffer, &outProcessed, inBuf, &inProcessed, finishMode, &status);
        // debug_array((uint8_t *)lz_state, 200);
        // debug_array(buffer, 255);
        // bs_printf("outProcessed: %d", outProcessed);
        // debug_array(inBuf, 255);
        // bs_printf("inProcessed: %d", inProcessed);
        // bs_printf("finishMode: %d", finishMode);
        // bs_printf("status: %d", status);
        // bs_printf("LzmaDec_DecodeToBuf res: %d", res);
        if (res)
        {
            // bs_printf("err%d,res: %d", __LINE__, res);
        }
        dcmprs_size = outProcessed;

        //重新设置文件读指针
        position += inProcessed;
        vfsetpos(pf, position);
    }
    // bs_printf("dcmprs_size: %d", dcmprs_size);
    return dcmprs_size;
}

void lzma_decompress_finish(void)
{
    LzmaDec_Free(lz_state, &allocator);

    if (lz_state != NULL)
    {
        lzma_free(NULL, lz_state);
        lz_state = NULL;
    }
}
