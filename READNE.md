# 简介

适用于嵌入式单片机的差分升级库，通用所有单片机，如stm32、华大、复旦微、瑞萨等。适合嵌入式的差分升级又叫增量升级，顾名思义就是通过差分算法将源版本与目标版本之间差异的部分提取出来制作成差分包，然后在设备通过还原算法将差异部分在源版本上进行还原从而升级成目标版本的过程。 差分升级方案不仅可以节省MCU内部的资源空间、还可以节省下载流程及下载和升级过程中的功耗。技术支持vx 18219255930，qq791314247

我将更详细的教程内容放在了我的博客：

[代码说明:https://blog.csdn.net/qq_35333978/article/details/128211763?spm=1001.2014.3001.5501](https://blog.csdn.net/qq_35333978/article/details/128211763?spm=1001.2014.3001.5501)

PS：很多人调不通99%都是因为申请内存那里有问题，要么malloc是自己写的、移植的有问题，要么就是给的堆内存不够。该套源码很多人用了很多工程都没问题的，基本无bug，如果调不通，注意以下几点：

1. 可以先用malloc测试，堆内存给20k以上，如果想缩减也是可以，需要修改源码每次还原的长度小一点，目前每次还原1k。
2. 检查堆、栈，如果调不通，可以先给比较大的值测试，基本不是堆就是栈的问题，源码是没有问题的，大量人测试过的。

# 修改日志

V1.0.1:

1，增加CMakeList.txt和conanfile.py文件，可以直接生成conan库，用不上conan库的不用关心这两个文件，删除即可

2，修改动态内存申请接口，从宏定义的方式升级为注册接口，宏的方式用户始终得修改代码，新版注册的方式，整个代码用户完全不用修改，注册的例子如下:

```
/**
 * @brief Initializes the upgrade module.
 *
 * This function sets up the necessary function pointers and registers them with the
 * bs_user_func_register() function. It assigns the xmq25qxx_write() function to the
 * bs_flash_write function pointer, and assigns the pvPortMalloc() and vPortFree()
 * functions to the bs_malloc and bs_free function pointers respectively.
 */
void UpgradeInit(void)
{
    bs_user_interface bsFunc;
  
    bsFunc.bs_flash_write = xmq25qxxWrite;
    bsFunc.bs_malloc = (bs_malloc_func)pvPortMalloc;
    bsFunc.bs_free = vPortFree;
    bs_user_func_register(&bsFunc);
}
```

3，该版本整体修改没有动任何逻辑，只是优化了接口


# 详细介绍

# 1.    什么是差分/增量升级？

借用网上的介绍：适合嵌入式的差分升级又叫增量升级，顾名思义就是通过差分算法将源版本与目标版本之间差异的部分提取出来制作成差分包，然后在设备通过还原算法将差异部分在源版本上进行还原从而升级成目标版本的过程。 差分升级方案不仅可以节省MCU内部的资源空间、还可以节省下载流程及下载和升级过程中的功耗。

也就是说，差分升级是拿以前旧设备内的bin，和当前新版本的bin用某种算法算出他们的差异部分，然后在借助压缩算法，生产一个极其小的差分包，然后将这个差分包下载到设备中，设备在根据解压算法、差分还原算法，生产一个完整的新版本bin，然后将这个新版本bin刷到执行区执行代码。

差分升级一般来说，可以极大的减少下载量，特别是对于嵌入式STM32等单片机来说，可以极大的减少维护成本，因为嵌入式设备的升级维护一般都是空中ota升级，比如蓝牙、红外等，下载速度受到波特率、包长等限制，更新固件包非常的慢，而差分升级可以让下载的过程极大的缩小。正常的维护版本，即使改的再多，生成的差分包bin理论上在原bin的5%左右，比如一个300k的bin，改的很多的情况下差分包也不过15k左右，而我实际测试，版本维护平均都在5~10k左右。

# 2.    差分升级实现原理

差分升级过程：

1. 使用旧版本bin文件和新版本bin文件制作差分包
2. 将差分包下载到设备内
3. 设备使用差分算法还原出新版本bin
4. 设备将新版本bin进行crc验证后刷到代码执行区
5. 设备重启并以新版本运行

在过程中有2个关键点：

**第一：如何使用旧版本bin****文件和新版本bin****文件制作差分包？**

该过程我们使用稳定的开源差分算法bsdiff+lzma生成差分包，该算法被大量使用，稳定安全，并且我们已在项目中批量使用，经过长时间的验证无任何问题。一般来说，该过程都是使用上位机来完成，嵌入式设备无需关心，我们已经做好了上位机软件，可以供大家随意使用，稍后会进行介绍。

**第二：设备收到差分包后如何还原出新版本的bin****文件？**

    该过程就是我们要讲解的重点过程，相对应的，嵌入式设备中，我们依然使用开源差分算法bsdiff+lzma来还原新版本文件，代码全开源，并且我已做成了库、抽象出了极简的接口，移植起来费不了多少功夫，基本是市场上所有的单片机如stm32、瑞萨、华大、复旦微等都可以使用，但是有内存限制，要求ram至少要10k以上，然后是该库本身的消耗大概是5k的rom。

# 3.    关键点一：差分包制作过程

对于差分包的制作，我已经开发好了上位机软件，界面如下图所示：
![在这里插入图片描述](https://img-blog.csdnimg.cn/8a4a092094774282ab6239c33fd6eebd.png)
 上位机这边主要实现使用开源算法bsdiff制作旧版本bin和新版本bin的差分包，然后在使用lzma压缩算法来压缩差分包，最终生成一个差分bin，使用方法上位机界面提示的很清楚，最终效果如下图所示：
![在这里插入图片描述](https://img-blog.csdnimg.cn/bd6cc76b8f2945278fd74f2b1dd075d5.png)
下载链接：

链接：[https://pan.baidu.com/s/1nBpftim8OCUI3i32sP3eOA?pwd=diff ](https://pan.baidu.com/s/1nBpftim8OCUI3i32sP3eOA?pwd=diff)

提取码：diff

# 4.    关键点二：嵌入式设备中差分算法库的移植(还原差分包)

## 4.1.   移植开关算法库代码

整体代码如下图所示：
![在这里插入图片描述](https://img-blog.csdnimg.cn/c13c11642a1c4e04ba756b4960cc4800.png)
如上图所示，99%的代码用户都不用去关心，用户只需要提供一个flash写入接口即可，也就是该库给定用户flash地址、数据内容指针、数据内容长度，用户将该段数据写入到flash即可，移植起来特别简单，花不了几分钟的功夫，这也是我花大力气抽象接口的原因。

## 4.2.   使用该库的流程

### 4.2.1.   使用库的接口

对于整个库的代码，我们只需要关心一个接口iap_patch，iap_patch在文件”bs_user_func.h”中。
![在这里插入图片描述](https://img-blog.csdnimg.cn/f9748aa6f4684926a05ff4aa7bbd2a1d.png)

该接口介绍也比较清晰，差分包的还原，只需要调用这一个接口即可。

```c
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
```

另外，使用该接口还原时所需要的一些信息可以在差分包文件头中获取，上位机在制作差分包时，会自动在差分包的bin头加上64字节的文件头，以便于告诉嵌入式设备旧/新版本bin文件的CRC校验、长度等信息。所以用户在收到差分包头时，偏移掉这64个字节的文件头的地址才是需要给到iap_patch接口的真正的bin文件。文件头格式如下代码，用户只需要关心中文注释的部分，其余的都是预留的信息。

```c
/* 差分包制作时自带的文件头信息，用户只需要关心中文注释的部分 */
 typedef struct image_header
 {
   uint32_t ih_magic; /* Image Header Magic Number */
   uint32_t ih_hcrc;  /* Image Header CRC Checksum 差分包包头校验 */
   uint32_t ih_time;  /* Image Creation Timestamp */
   uint32_t ih_size;  /* Image Data Size 差分包的大小 */
   uint32_t ih_load;  /* Data Load Address 上一版本旧文件的大小 */
   uint32_t ih_ep;   /* Entry Point Address 要升级的新文件的大小 */
   uint32_t ih_dcrc;  /* Image Data CRC Checksum 新文件的CRC */
   uint8_t ih_os;   /* Operating System */
   uint8_t ih_arch;  /* CPU architecture */
   uint8_t ih_type;  /* Image Type */
   uint8_t ih_comp;  /* Compression Type */
   uint8_t ih_name[IH_NMLEN]; /* Image Name */
   uint32_t ih_ocrc;  /* Old Image Data CRC Checksum 上一版本旧文件的CRC */
 } image_header_t; 
 /* 差分包制作时自带的文件头信息，用户只需要关心中文注释的部分 */
```

### 4.2.2.   接口使用例子

我截取一段我工程中的代码来讲解如何使用该接口还原出新版本bin文件：

```c
  1#ifdef BSDIFF_UPGRADE
  2        image_header_t recv_head;
  3        uint32_t recv_hcrc;  /* 接收到的文件头CRC */
  4        uint32_t calculation_crc;  /* 计算出来的文件头CRC */
  5        uint32_t spi_flash_addr = UPGRADE_PROGRAM_ADDR;
  6
  7        memcpy(&recv_head, (uint8_t *)APPLICATION_A, sizeof(image_header_t));
  8        recv_hcrc = BigtoLittle32(recv_head.ih_hcrc);
  9        recv_head.ih_hcrc = 0;
 10        calculation_crc = crc32((uint8_t *)&recv_head, sizeof(image_header_t));
 11
 12        if (recv_hcrc == calculation_crc)
 13        {
 14            recv_head.ih_hcrc = recv_hcrc;
 15            recv_head.ih_time = BigtoLittle32(recv_head.ih_time);
 16            recv_head.ih_size = BigtoLittle32(recv_head.ih_size);
 17            recv_head.ih_dcrc = BigtoLittle32(recv_head.ih_dcrc);
 18            recv_head.ih_ocrc = BigtoLittle32(recv_head.ih_ocrc);
 19            /* 差分升级包 */
 20            recv_head.ih_hcrc = calculation_crc;
 21            if (crc32((uint8_t *)APPLICATION_RUN, recv_head.ih_load) != recv_head.ih_ocrc)
 22            {
 23                APP_ERR_PRINT("file oldcrc err,calcrc:0X%08X, ih_oldbin_crc:0X%08X,", 
 24                                          crc32((uint8_t *)APPLICATION_RUN, 
 25                                          recv_head.ih_load), recv_head.ih_ocrc);
 26                goto bsdiff_out;
 27            }
 28            RTOS_LOCK();
 29            disable_task_monitoring(ALL_TASK_RUNFLAG_BIT, true);
 30            // flash_erase_sector(UPGRADE_PROGRAM_ADDR, UPGRADE_PROGRAM_PAGE);
 31            recv_hcrc = iap_patch((uint8_t *)APPLICATION_RUN, recv_head.ih_load, 
 32                                 (uint8_t *)(APPLICATION_A + sizeof(image_header_t)),  
 33                                   recv_head.ih_size, UPGRADE_PROGRAM_ADDR);
 34            if (recv_hcrc != recv_head.ih_ep)
 35            {
 36                APP_ERR_PRINT("iap_patch len err.");
 37                APP_ERR_PRINT("iap_patch len: %lu, new_len: %lu", recv_hcrc, recv_head.ih_ep);
 38                goto bsdiff_out;
 39            }
 40            if (erase_program(APPLICATION_A))
 41            {
 42                APP_ERR_PRINT("I erase program failed.");
 43                goto bsdiff_out;
 44            }
 45
 46            current_flash_write_addr = APPLICATION_A;
 47            for (uint32_t i = 0; i < (recv_head.ih_ep / 1024); i++)
 48            {
 49                xmq25qxx_read(spi_flash_addr, spi_read_cache, 1024);
 50                if (xflash_write(current_flash_write_addr, spi_read_cache, 1024))
 51                {
 52                    APP_ERR_PRINT("I write program failed.");
 53                    goto bsdiff_out;
 54                }
 55                spi_flash_addr += 1024;
 56                current_flash_write_addr += 1024;
 57                APP_PRINT("current_flash_write_addr: 0X%08X", current_flash_write_addr);
 58            }
 59            if (recv_head.ih_ep % 1024 != 0)
 60            {
 61                memset(spi_read_cache, 0XFF, 1024);
 62                xmq25qxx_read(spi_flash_addr, spi_read_cache, recv_head.ih_ep % 1024);
 63
 64                if (xflash_write(current_flash_write_addr, spi_read_cache, 1024))
 65                {
 66                    APP_ERR_PRINT("I write program failed.");
 67                    goto bsdiff_out;
 68                }
 69            }
 70            if (crc32((uint8_t *)APPLICATION_A, recv_head.ih_ep) != recv_head.ih_dcrc)
 71            {
 72                APP_ERR_PRINT("file newcrc err,calcrc:0X%08X, newcrc:0X%08X, len: %lu", 
 73                                         crc32((uint8_t *)APPLICATION_A, recv_head.ih_ep), 
 74                                         recv_head.ih_load, recv_head.ih_dcrc);
 75                goto bsdiff_out;
 76            }
 77            /* 下载成功,开始升级 */
 78            if (check_bin_file((bin_info_t *)(APPLICATION_A + BIN_INFO_OFFSET))) /* bin文件非法 */
 79            {
 80                APP_ERR_PRINT("check_bin_file err.");
 81                goto bsdiff_out;
 82            }
 83            recv_head.ih_dcrc = CRT_CRC16_check(0, (uint8_t *)APPLICATION_A, 
 84                                                recv_head.ih_ep);
 85            readwrite_app_run_bin_info(0, &recv_head.ih_ep, (uint16_t *)&recv_head.ih_dcrc);
 86            /* 整体校验成功,确认升级 */
 87            if (switch_program_stage(STAGE_1))
 88            {
 89                APP_ERR_PRINT("I write switch_program_stage STAGE_0 failed.");  /* 置位升级标志写失败 */
 90                goto bsdiff_out;
 91            }
 92            APP_PRINT("upgrade success.");
 93  bsdiff_out:
 94            SYSTEM_RESET();
 95        }

```
