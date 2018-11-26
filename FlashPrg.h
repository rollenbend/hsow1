#ifndef __FLASH__
#define __FLASH__
#ifdef __cplusplus
extern "C" void FlashInit ();
#endif

#include "stm32f30x_flash.h"

#define FLASH_PAGE_SIZE         ((uint32_t)0x00000800)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08020000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08034000)   /* End @ of user Flash area */

#define FlashSectorSize	FLASH_PAGE_SIZE
#define ProgBufArea FLASH_USER_START_ADDR
#define FlashStart  0x08000000

#endif
