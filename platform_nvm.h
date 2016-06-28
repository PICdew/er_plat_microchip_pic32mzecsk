/*
 * platform_nvm.h
 *
 *  Created on: Jun 14, 2016
 *      Author: zoltanr
 */

#ifndef ER_PLAT_PIC32MZ_EF_STARTER_KIT_PLATFORM_NVM_H_
#define ER_PLAT_PIC32MZ_EF_STARTER_KIT_PLATFORM_NVM_H_

extern int32_t platform_nvm_init(void);
extern int32_t platform_nvm_safe_erase(void);
extern int32_t platform_nvm_erase(void);
extern int32_t platform_nvm_write(void * sourceBuffer, uint32_t length);
extern int32_t platform_nvm_erase_write(void * sourceBuffer, uint32_t length);
extern int32_t platform_nvm_read(void * destBuffer, uint32_t length);

#endif /* ER_PLAT_PIC32MZ_EF_STARTER_KIT_PLATFORM_NVM_H_ */
