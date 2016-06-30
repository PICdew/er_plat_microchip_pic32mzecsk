/*
 * platform_nvm.c
 *
 *  Created on: Jun 14, 2016
 *      Author: zoltanr
 */

#include "system_config.h"
#include "system/int/sys_int.h"
#include "driver/nvm/drv_nvm.h"
#include <system_utils.h>
#include <lib/type.h>
#include <lib/error.h>
#include <lib/debug.h>
#include <porting/thread_port.h>

/* The size of the media used for this demo. */
#define APP_NVM_MEMORY_AREA_SIZE (DRV_NVM_MEDIA_SIZE * 1024)

/* NVM Driver Handle */
static DRV_HANDLE  nvmHandle;

/* Pointer to the NVM Media Geometry */
static SYS_FS_MEDIA_GEOMETRY *gAppNVMMediaGeometry;

/* gAppFlashReserveArea refers to the 32KB of memory starting from address
   DRV_NVM_MEDIA_START_ADDRESS. This memory area is reserved for NVM read/write
   operations. The attribute keep instructs the linker not to remove the section
   even if it is not refered anywhere in the code.
 */
const uint8_t gAppFlashReserveArea[APP_NVM_MEMORY_AREA_SIZE] __attribute__ ((keep)) __attribute__((address(DRV_NVM_MEDIA_START_ADDRESS))) = {0};

/* Buffer used for reading data from the NVM */
uint8_t gAppReadBuffer [DRV_NVM_PAGE_SIZE] __attribute__((coherent, aligned(16)));

/* Buffer used for writing data onto the NVM */
uint8_t gAppWriteBuffer[DRV_NVM_PAGE_SIZE] __attribute__((coherent, aligned(16)));

/* Enumeration of the Read, write and erase region geometry indices */
typedef enum {

    /* Read region index */
    APP_NVM_READ_REGION_INDEX = 0,

    /* Write region index */
    APP_NVM_WRITE_REGION_INDEX,

    /* Erase region index */
    APP_NVM_ERASE_REGION_INDEX

} APP_NVM_REGION_INDEX;

/* Flag to track the number of successful command
complete events */
uint8_t                     eventCount;

/* Counter to track the number of unsuccessful command
complete events */
uint8_t                     errorEventCount;

static system_mutex_t nvm_event_mutex;
static system_mutex_t nvm_lock_mutex;

/**********************************************************
 * This function issues command to erase the entire memory
 * area and then reads back the memory to ensure that the
 * erase operation has set the bits to '1'
 ***********************************************************/
/* Enumeration listing the Erase operation sub-states */
typedef enum {

    /* Initialize variables for Erase operation */
    APP_ERASE_STATE_INIT = 0,

    /* Issue Erase command */
    APP_ERASE_STATE_ERASE_CMD,

    /* Check if the erase command operation is complete */
    APP_ERASE_STATE_ERASE_CMD_STATUS,

    /* Issue Read command */
    APP_ERASE_STATE_READ_CMD,

    /* Check if the read command operation is complete */
    APP_ERASE_STATE_READ_CMD_STATUS,

    /* Verify that the data is indeed erased */
    APP_ERASE_STATE_VERIFY_DATA,

    /* Erase and verification of data is successful */
    APP_ERASE_STATE_IDLE,

    /* Erase or verification of data is unsuccessful */
    APP_ERASE_STATE_ERROR

} APP_ERASE_STATES;

static void _nvm_event_handler(DRV_NVM_EVENT event,
                               DRV_NVM_COMMAND_HANDLE commandHandle,
                               uintptr_t context);

int32_t platform_nvm_init(void)
{
    int32_t status = ERR_SUCCESS;
    info_log(DEBUG_PLATFORM, ("NVM init\n"));

    /** NVP Initialization */
    /* Initialize the state variables to default values. */

    eventCount      = 0;
    errorEventCount = 0;

    nvmHandle = DRV_NVM_Open(0, DRV_IO_INTENT_READWRITE);
    if(DRV_HANDLE_INVALID == nvmHandle)
        return ERR_FAILURE;

    /* Register for NVM driver events */
    DRV_NVM_EventHandlerSet (nvmHandle, _nvm_event_handler, 1);

    /* Read the NVM Media Geometry. */
    gAppNVMMediaGeometry = DRV_NVM_GeometryGet(nvmHandle);
    if(NULL == gAppNVMMediaGeometry)
        return ERR_FAILURE;

    /*
    printf("APP_NVM_ERASE_REGION_INDEX: %d\n", gAppNVMMediaGeometry->geometryTable[APP_NVM_ERASE_REGION_INDEX].numBlocks);
    printf("APP_NVM_READ_REGION_INDEX: %d\n", gAppNVMMediaGeometry->geometryTable[APP_NVM_READ_REGION_INDEX].numBlocks);
    printf("APP_NVM_WRITE_REGION_INDEX: %d\n", gAppNVMMediaGeometry->geometryTable[APP_NVM_WRITE_REGION_INDEX].numBlocks);
    */

    status = system_mutex_create(&nvm_event_mutex);
    if (status != ERR_SUCCESS) {
        error_log(DEBUG_PLATFORM, ("Mutex can not be created\n"), status);
        return status;
    }
    system_mutex_lock(nvm_event_mutex);

    status = system_mutex_create(&nvm_lock_mutex);
    if (status != ERR_SUCCESS) {
        error_log(DEBUG_PLATFORM, ("Mutex can not be created\n"), status);
        system_mutex_destroy(nvm_event_mutex);
        return status;
    }

    return ERR_SUCCESS;
}

int32_t platform_nvm_safe_erase( void )
{
    uint32_t i = 0;
    /* NVM Command Handles */
    DRV_NVM_COMMAND_HANDLE nvmCommandHandle;
    DRV_NVM_COMMAND_STATUS commandStatus = DRV_NVM_COMMAND_IN_PROGRESS;

    uint32_t readBlockAddr = 0;                    /* Read block address */
    uint32_t numReadBlocks = DRV_NVM_PAGE_SIZE;    /* Number of blocks to be read */

    system_mutex_lock(nvm_lock_mutex);

    debug_log(DEBUG_PLATFORM, ("Erase permanent storage\n"));

    /* Erase the entire 32 KB of memory area */
    DRV_NVM_Erase(nvmHandle, &nvmCommandHandle, 0,
                  gAppNVMMediaGeometry->geometryTable[APP_NVM_ERASE_REGION_INDEX].numBlocks);

    if(nvmCommandHandle == DRV_NVM_COMMAND_HANDLE_INVALID) {
        error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_HANDLE_INVALID);
        ASSERT(0);
        return ERR_FAILURE;
    }

    /* Check if the Erase operation has been completed successfully. */
    do {
        commandStatus = DRV_NVM_CommandStatus(nvmHandle, nvmCommandHandle);
        if (DRV_NVM_COMMAND_ERROR_UNKNOWN == commandStatus) {
            error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_ERROR_UNKNOWN);
            ASSERT(0);
            return ERR_FAILURE;
        }
    } while (DRV_NVM_COMMAND_COMPLETED != commandStatus);

    debug_log(DEBUG_PLATFORM, ("Permanent storage verification\n"));
    while (readBlockAddr < gAppNVMMediaGeometry->geometryTable[APP_NVM_READ_REGION_INDEX].numBlocks) {

        /* Read data */
        DRV_NVM_Read(nvmHandle, &nvmCommandHandle, gAppReadBuffer, readBlockAddr, numReadBlocks);
        if (nvmCommandHandle == DRV_NVM_COMMAND_HANDLE_INVALID) {
            error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_HANDLE_INVALID);
            ASSERT(0);
            return ERR_FAILURE;
        }

        do {
            commandStatus = DRV_NVM_CommandStatus(nvmHandle, nvmCommandHandle);
            if (DRV_NVM_COMMAND_ERROR_UNKNOWN == commandStatus) {
                error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_ERROR_UNKNOWN);
                ASSERT(0);
                return ERR_FAILURE;
            }
        } while (DRV_NVM_COMMAND_COMPLETED != commandStatus);

        /* Update the read block address */
        readBlockAddr += numReadBlocks;

        /* Completed verifying the erased memory. */
        for (i = 0; i < numReadBlocks; i++)
            if (gAppReadBuffer[i] != 0xFF) {
                error_log(DEBUG_PLATFORM, ("Permanent memory corruption\n"), ERR_FAILURE);
                ASSERT(0);
                return ERR_FAILURE;
            }
    }

    debug_log(DEBUG_PLATFORM, ("Permanent storage is erased!\n"));

    system_mutex_unlock(nvm_lock_mutex);

    return ERR_SUCCESS;
}

int32_t platform_nvm_erase( void )
{
    uint32_t i = 0;
    /* NVM Command Handles */
    DRV_NVM_COMMAND_HANDLE nvmCommandHandle;
    DRV_NVM_COMMAND_STATUS commandStatus = DRV_NVM_COMMAND_IN_PROGRESS;

    system_mutex_lock(nvm_lock_mutex);

    debug_log(DEBUG_PLATFORM, ("Erase permanent storage\n"));

    eventCount      = 0;
    errorEventCount = 0;

    /* Erase the entire 32 KB of memory area */
    DRV_NVM_Erase(nvmHandle, &nvmCommandHandle, 0,
                  gAppNVMMediaGeometry->geometryTable[APP_NVM_ERASE_REGION_INDEX].numBlocks);
    if(nvmCommandHandle == DRV_NVM_COMMAND_HANDLE_INVALID) {
        error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_HANDLE_INVALID);
        ASSERT(0);
        return ERR_FAILURE;
    }

    /* Waiting for the event handler to unlock the mutex */
    system_mutex_lock(nvm_event_mutex);

    ASSERT(DRV_NVM_COMMAND_COMPLETED == DRV_NVM_CommandStatus(nvmHandle, nvmCommandHandle));
    debug_log(DEBUG_PLATFORM, ("Memory erase success\n"));

    return ERR_SUCCESS;
}

int32_t platform_nvm_write(void * sourceBuffer, uint32_t length)
{
    DRV_NVM_COMMAND_HANDLE nvmCommandHandle;
    int32_t status = ERR_SUCCESS;

    system_mutex_lock(nvm_lock_mutex);

    debug_log(DEBUG_PLATFORM, ("Write %d bytes to the permanent storage!\n", length));

    eventCount      = 0;
    errorEventCount = 0;

    memcpy(gAppWriteBuffer, sourceBuffer, (sizeof(gAppWriteBuffer) >= length) ? length : sizeof(gAppWriteBuffer));

    DRV_NVM_Write(nvmHandle, &nvmCommandHandle, gAppWriteBuffer, 0, 1);
    if (nvmCommandHandle == DRV_NVM_COMMAND_HANDLE_INVALID) {
       error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_HANDLE_INVALID);
       ASSERT(0);
       return ERR_FAILURE;
    }

    /* Waiting for the event handler to unlock the mutex */
    system_mutex_lock(nvm_event_mutex);

    if(!eventCount || errorEventCount) {
        error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), ERR_FAILURE);
        ASSERT(0);
        status = ERR_FAILURE;
    }

    ASSERT(DRV_NVM_COMMAND_COMPLETED == DRV_NVM_CommandStatus(nvmHandle, nvmCommandHandle));
    debug_log(DEBUG_PLATFORM, ("Memory write %s!\n", status ? "failure" : "success"));
    system_mutex_unlock(nvm_lock_mutex);

    return status;
}

int32_t platform_nvm_erase_write(void * sourceBuffer, uint32_t length)
{
    DRV_NVM_COMMAND_HANDLE nvmCommandHandle;
    int32_t status = ERR_SUCCESS;

    system_mutex_lock(nvm_lock_mutex);

    debug_log(DEBUG_PLATFORM, ("Write %d bytes to the permanent storage!\n", length));

    eventCount      = 0;
    errorEventCount = 0;

    memcpy(gAppWriteBuffer, sourceBuffer, (sizeof(gAppWriteBuffer) >= length) ? length : sizeof(gAppWriteBuffer));

    DRV_NVM_EraseWrite(nvmHandle, &nvmCommandHandle, gAppWriteBuffer, 0, 1);
    if (nvmCommandHandle == DRV_NVM_COMMAND_HANDLE_INVALID) {
       error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_HANDLE_INVALID);
       ASSERT(0);
       return ERR_FAILURE;
    }

    /* Waiting for the event handler to unlock the mutex */
    system_mutex_lock(nvm_event_mutex);

    if(!eventCount || errorEventCount) {
        error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), ERR_FAILURE);
        ASSERT(0);
        status = ERR_FAILURE;
    }

    ASSERT(DRV_NVM_COMMAND_COMPLETED == DRV_NVM_CommandStatus(nvmHandle, nvmCommandHandle));
    debug_log(DEBUG_PLATFORM, ("Memory write %s!\n", status ? "failure" : "success"));
    system_mutex_unlock(nvm_lock_mutex);

    return status;
}

int32_t platform_nvm_read(void * destBuffer, uint32_t length)
{
    DRV_NVM_COMMAND_HANDLE nvmCommandHandle;
    int32_t status = ERR_SUCCESS;

    system_mutex_lock(nvm_lock_mutex);

    ASSERT(sizeof(gAppReadBuffer) >= length);
    ASSERT(destBuffer);
    debug_log(DEBUG_PLATFORM, ("Read %d bytes from permanent storage\n", length));

    eventCount      = 0;
    errorEventCount = 0;

    /* Read data */
    DRV_NVM_Read(nvmHandle, &nvmCommandHandle, gAppReadBuffer, 0, DRV_NVM_PAGE_SIZE);
    if (nvmCommandHandle == DRV_NVM_COMMAND_HANDLE_INVALID) {
       error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), DRV_NVM_COMMAND_HANDLE_INVALID);
       ASSERT(0);
       return ERR_FAILURE;
    }

    /* Waiting for the event handler to unlock the mutex */
    system_mutex_lock(nvm_event_mutex);

    if(!eventCount || errorEventCount) {
        error_log(DEBUG_PLATFORM, ("Permanent memory failure\n"), ERR_FAILURE);
        ASSERT(0);
        status = ERR_FAILURE;
    } else {
        ASSERT(DRV_NVM_COMMAND_COMPLETED == DRV_NVM_CommandStatus(nvmHandle, nvmCommandHandle));
        memcpy(destBuffer, gAppReadBuffer, (sizeof(gAppReadBuffer) >= length) ? length : sizeof(gAppWriteBuffer));
    }

    debug_log(DEBUG_PLATFORM, ("Memory read %s!\n", status ? "failure" : "success"));

    system_mutex_unlock(nvm_lock_mutex);

    return status;
}

/********************************************************
 * NVM Driver Events handler
 ********************************************************/
static void _nvm_event_handler (DRV_NVM_EVENT event,
                                DRV_NVM_COMMAND_HANDLE commandHandle,
                                uintptr_t context ) {

    switch (event)
    {
        case DRV_NVM_EVENT_COMMAND_COMPLETE:
            eventCount++;
            break;

        case DRV_NVM_EVENT_COMMAND_ERROR:
            errorEventCount++;
            break;

        default:
            ASSERT(0);
            break;
    }

    system_mutex_unlock(nvm_event_mutex);
}
