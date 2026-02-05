
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "NeonRTOS.h"

static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

// Queue size 
#define slQUEUE_SIZE				( 3 )

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjCreate(NeonRTOS_SyncObj_t* pSyncObj)
{
    //Check for NULL
    if(NULL == pSyncObj)
    {
        return NeonRTOS_INVALID_PARAMS;
    }
    SemaphoreHandle_t *pl_SyncObj = (SemaphoreHandle_t *)pSyncObj;

    *pl_SyncObj = xSemaphoreCreateBinary();

    if((SemaphoreHandle_t)(*pSyncObj) != NULL)
    {
        return NeonRTOS_OK; 
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjDelete(NeonRTOS_SyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
    vSemaphoreDelete(*pSyncObj );
    return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjSignal(NeonRTOS_SyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}

    if(pdTRUE != xSemaphoreGive( *pSyncObj ))
	{
        //In case of Semaphore, you are expected to get this if multiple sem
        // give is called before sem take
        return NeonRTOS_OK;
	}
	
    return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjSignalFromISR(NeonRTOS_SyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
	xHigherPriorityTaskWoken = pdFALSE;
	if(pdTRUE == xSemaphoreGiveFromISR( *pSyncObj, &xHigherPriorityTaskWoken ))
	{
		if( xHigherPriorityTaskWoken )
		{
			taskYIELD ();
		}
		return NeonRTOS_OK;
	}
	else
	{
		//In case of Semaphore, you are expected to get this if multiple sem
		// give is called before sem take
		return NeonRTOS_OK;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjWait(NeonRTOS_SyncObj_t* pSyncObj , NeonRTOS_Time_t Timeout)
{
    //Check for NULL
    if(NULL == pSyncObj)
    {
            return NeonRTOS_INVALID_PARAMS;
    }
    if(pdTRUE == xSemaphoreTake( (SemaphoreHandle_t)*pSyncObj, ( TickType_t )(Timeout/portTICK_PERIOD_MS) ))
    {
        return NeonRTOS_OK;
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjClear(NeonRTOS_SyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}

    if (NeonRTOS_OK == NeonRTOS_SyncObjWait(pSyncObj,0) )
    {
        return NeonRTOS_OK;
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjCreate(NeonRTOS_CountingObj_t* pCountingObj, unsigned long uxMaxCount, unsigned long uxInitialCount)
{
    //Check for NULL
    if(NULL == pCountingObj)
    {
        return NeonRTOS_INVALID_PARAMS;
    }
    SemaphoreHandle_t *pl_CountingObj = (SemaphoreHandle_t *)pCountingObj;

    *pl_CountingObj = xSemaphoreCreateCounting(uxMaxCount, uxInitialCount);

    if((SemaphoreHandle_t)(*pl_CountingObj) != NULL)
    {
        return NeonRTOS_OK; 
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjDelete(NeonRTOS_CountingObj_t* pCountingObj)
{
	//Check for NULL
	if(NULL == pCountingObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
    vSemaphoreDelete(*pCountingObj );
    return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjIncrease(NeonRTOS_CountingObj_t* pCountingObj)
{
	//Check for NULL
	if(NULL == pCountingObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}

    if(pdTRUE != xSemaphoreGive( *pCountingObj ))
	{
        //In case of Semaphore, you are expected to get this if multiple sem
        // give is called before sem take
        return NeonRTOS_OK;
	}
	
    return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_CounterObjIncreaseFromISR(NeonRTOS_CountingObj_t* pCountingObj)
{
	//Check for NULL
	if(NULL == pCountingObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
	xHigherPriorityTaskWoken = pdFALSE;
	if(pdTRUE == xSemaphoreGiveFromISR( *pCountingObj, &xHigherPriorityTaskWoken ))
	{
		if( xHigherPriorityTaskWoken )
		{
			taskYIELD ();
		}
		return NeonRTOS_OK;
	}
	else
	{
		//In case of Semaphore, you are expected to get this if multiple sem
		// give is called before sem take
		return NeonRTOS_OK;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjWait(NeonRTOS_CountingObj_t* pCountingObj , NeonRTOS_Time_t Timeout)
{
	//Check for NULL
	if(NULL == pCountingObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
    if(pdTRUE == xSemaphoreTake( (SemaphoreHandle_t)*pCountingObj, ( TickType_t )(Timeout/portTICK_PERIOD_MS) ))
    {
        return NeonRTOS_OK;
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjGetCount(NeonRTOS_CountingObj_t* pCountingObj, unsigned int* pCountVal)
{
	//Check for NULL
	if(NULL == pCountingObj || NULL == pCountVal)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
        *pCountVal = (unsigned int)uxQueueMessagesWaiting( ( QueueHandle_t ) ( pCountingObj ) );
        
        return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjClear(NeonRTOS_CountingObj_t* pCountingObj)
{
	//Check for NULL
	if(NULL == pCountingObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
        unsigned int countClear;
        
        if(NeonRTOS_OK == NeonRTOS_CountingObjGetCount(pCountingObj, &countClear))
        {
                for( unsigned int i = 0; i<countClear; i++)
                {
                        if (NeonRTOS_OK != NeonRTOS_CountingObjWait(pCountingObj,0) )
                        {
                                return NeonRTOS_OPERATION_FAILED;
                        }
                }
                return NeonRTOS_OK;
        }
        else
        {
                return NeonRTOS_OPERATION_FAILED;
        }
}

NeonRTOS_ReturnVal_e NeonRTOS_LockObjCreate(NeonRTOS_LockObj_t* pLockObj)
{
    //Check for NULL
    if(NULL == pLockObj)
    {
            return NeonRTOS_INVALID_PARAMS;
    }
    *pLockObj = (NeonRTOS_LockObj_t)xSemaphoreCreateMutex();
    if(pLockObj != NULL)
    {  
        return NeonRTOS_OK;
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_TaskCreate(P_NeonRTOS_TASK_ENTRY pEntry,const signed char * const pcName,
                              unsigned short usStackDepth, void *pvParameters,
                              unsigned long uxPriority,NeonRTOS_TaskHandle* pTaskHandle)
{
	if(pdPASS == xTaskCreate( pEntry, (char const*)pcName,
                                (usStackDepth/(sizeof( portSTACK_TYPE ))), 
                                pvParameters,(unsigned portBASE_TYPE)uxPriority,
                                (TaskHandle_t*)pTaskHandle ))
	{
		return NeonRTOS_OK;
	}

	return NeonRTOS_OPERATION_FAILED;	
}

void NeonRTOS_TaskDelete(NeonRTOS_TaskHandle* pTaskHandle)
{
	vTaskDelete((TaskHandle_t)*pTaskHandle);
}

void NeonRTOS_TaskSuspend(NeonRTOS_TaskHandle* pTaskHandle)
{
	vTaskSuspend((TaskHandle_t)*pTaskHandle);
}

void NeonRTOS_TaskResume(NeonRTOS_TaskHandle* pTaskHandle)
{
	vTaskResume((TaskHandle_t)*pTaskHandle);
}

NeonRTOS_Time_t NeonRTOS_TaskGetTickCount()
{
	return xTaskGetTickCount();
}

NeonRTOS_ReturnVal_e NeonRTOS_LockObjDelete(NeonRTOS_LockObj_t* pLockObj)
{
    vSemaphoreDelete((SemaphoreHandle_t)*pLockObj );
    return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_LockObjLock(NeonRTOS_LockObj_t* pLockObj , NeonRTOS_Time_t Timeout)
{
    //Check for NULL
    if(NULL == pLockObj)
    {
            return NeonRTOS_INVALID_PARAMS;
    }
    //Take Semaphore
    if(pdTRUE == xSemaphoreTake( *pLockObj, ( TickType_t ) (Timeout/portTICK_PERIOD_MS) ))
    {
        return NeonRTOS_OK;
    }
    else
    {
        return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_LockObjUnlock(NeonRTOS_LockObj_t* pLockObj)
{
	//Check for NULL
	if(NULL == pLockObj)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
	//Release Semaphore
    if(pdTRUE == xSemaphoreGive( *pLockObj ))
    {
    	return NeonRTOS_OK;
    }
    else
    {
    	return NeonRTOS_OPERATION_FAILED;
    }
}

NeonRTOS_ReturnVal_e NeonRTOS_MsgQCreate(NeonRTOS_MsgQ_t* 		pMsgQ , 
			      char*			pMsgQName,
			      unsigned long 		MsgSize,
			      unsigned long		MaxMsgs)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return NeonRTOS_INVALID_PARAMS;
	}

	QueueHandle_t handle =0;

	//Create Queue
	handle = xQueueCreate( MaxMsgs, MsgSize );
	if (handle==0)
	{
		return NeonRTOS_OPERATION_FAILED;
	}
	
	*pMsgQ = (NeonRTOS_MsgQ_t)handle;

    vQueueAddToRegistry(*pMsgQ, pMsgQName);

	return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_MsgQDelete(NeonRTOS_MsgQ_t* pMsgQ)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
	vQueueDelete((QueueHandle_t) *pMsgQ );
    return NeonRTOS_OK;
}

NeonRTOS_ReturnVal_e NeonRTOS_MsgQWrite(NeonRTOS_MsgQ_t* pMsgQ, void* pMsg , NeonRTOS_Time_t Timeout)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return NeonRTOS_INVALID_PARAMS;
	}

        
        if(pdPASS == xQueueSendFromISR((QueueHandle_t) *pMsgQ, pMsg, &xHigherPriorityTaskWoken ))
        {
                    taskYIELD ();
                    return NeonRTOS_OK;
        }
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_MsgQRead(NeonRTOS_MsgQ_t* pMsgQ, void* pMsg , NeonRTOS_Time_t Timeout)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return NeonRTOS_INVALID_PARAMS;
	}

	if ( Timeout == NEONRT_WAIT_FOREVER )
	{
		Timeout = portMAX_DELAY ;
	}

	//Receive Item from Queue
	if( pdTRUE  == xQueueReceive((QueueHandle_t)*pMsgQ,pMsg,Timeout) )
	{
		return NeonRTOS_OK;
	}
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_TimerCreate(NeonRTOS_TimerHandle* pTimerHandle, const signed char * const pcTimerName, NeonRTOS_Time_t periodMs, unsigned int autoLoad, unsigned int uTimerID, NeonRTOS_TimerCallbackFunction_t pCallbackFunction)
{
	if(NULL == pTimerHandle)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
        UBaseType_t uxAutoReload;
        
        uxAutoReload = pdFALSE;
        if(autoLoad>0){
            uxAutoReload = pdTRUE;
        }
        
        TimerHandle_t handle = NULL;
        
        handle = xTimerCreate( (const char*)pcTimerName, (TickType_t)periodMs/portTICK_RATE_MS, uxAutoReload, ( void * )uTimerID, (TimerCallbackFunction_t)pCallbackFunction );
          
	if (handle==NULL)
	{
		return NeonRTOS_OPERATION_FAILED;
	}

	*pTimerHandle = (NeonRTOS_TimerHandle)handle;
	return NeonRTOS_OK;
}

void NeonRTOS_TimerDelete(NeonRTOS_TimerHandle* pTimerHandle)
{
	xTimerDelete((TimerHandle_t)*pTimerHandle, 0);
}

NeonRTOS_ReturnVal_e NeonRTOS_TimerChangePeriod(NeonRTOS_TimerHandle* pTimerHandle, NeonRTOS_Time_t newPeriodMs)
{
	if(NULL == pTimerHandle)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
	if( pdTRUE  == xTimerChangePeriodFromISR((TimerHandle_t)*pTimerHandle, (TickType_t)newPeriodMs/portTICK_RATE_MS, &xHigherPriorityTaskWoken))
        {
                return NeonRTOS_OK;
        }
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_TimerStart(NeonRTOS_TimerHandle* pTimerHandle)
{
	if(NULL == pTimerHandle)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
	if( pdTRUE  == xTimerStartFromISR((TimerHandle_t)*pTimerHandle, &xHigherPriorityTaskWoken))
        {
                return NeonRTOS_OK;
        }
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_TimerStop(NeonRTOS_TimerHandle* pTimerHandle)
{
	if(NULL == pTimerHandle)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
	if( pdTRUE  == xTimerStopFromISR((TimerHandle_t)*pTimerHandle, &xHigherPriorityTaskWoken))
        {
                return NeonRTOS_OK;
        }
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_TimerReStart(NeonRTOS_TimerHandle* pTimerHandle)
{
	if(NULL == pTimerHandle)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
	if( pdTRUE  == xTimerResetFromISR((TimerHandle_t)*pTimerHandle, &xHigherPriorityTaskWoken))
        {
                return NeonRTOS_OK;
        }
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}

NeonRTOS_ReturnVal_e NeonRTOS_GetTimerID(NeonRTOS_TimerHandle* pTimerHandle, unsigned int* pxTimerID)
{
	if(NULL == pTimerHandle)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
	if(NULL == pxTimerID)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
        unsigned int pTimerID = (uint32_t)pvTimerGetTimerID((TimerHandle_t)*pTimerHandle);
        
	if (pTimerID==NULL)
	{
		return NeonRTOS_OPERATION_FAILED;
	}
        
        *pxTimerID = pTimerID;
        
        return NeonRTOS_OK;
}


NeonRTOS_ReturnVal_e NeonRTOS_PendingFunctionCall(NeonRTOS_PendingFunction_t pPendFunc, void* pParameter1, unsigned int uParameter2)
{
	if(NULL == pPendFunc)
	{
		return NeonRTOS_INVALID_PARAMS;
	}
        
	if( pdTRUE  == xTimerPendFunctionCallFromISR( pPendFunc, pParameter1, uParameter2, &xHigherPriorityTaskWoken ))
        {
                return NeonRTOS_OK;
        }
	else
	{
		return NeonRTOS_OPERATION_FAILED;
	}
}


void * mem_Malloc(unsigned long Size)
{
    return ( void * ) pvPortMalloc( (size_t)Size );
}

void mem_Free(void *pMem)
{
    vPortFree( pMem );
}

void *mem_Realloc(void *pMem, unsigned long newSize)
{
    if (pMem == NULL) {                  /* 相當於 malloc */
        return mem_Malloc(newSize);
    }

    if (newSize == 0) {                  /* 相當於 free */
        mem_Free(pMem);
        return NULL;
    }

    /* 讀出舊大小 */
    uint8_t *oldRaw   = (uint8_t *)pMem - sizeof(size_t);
    size_t   oldSize  = *((size_t *)oldRaw);

    /* 申請新區塊（含表頭）*/
    uint8_t *newRaw = (uint8_t *)pvPortMalloc(newSize + sizeof(size_t));
    if (!newRaw) return NULL;            /* 失敗：舊指標保持有效 */

    /* 搬資料：只搬到兩者較小的範圍 */
    size_t copySize = (newSize < oldSize) ? newSize : oldSize;
    memcpy(newRaw + sizeof(size_t), pMem, copySize);

    /* 更新新表頭並釋放舊區塊 */
    *((size_t *)newRaw) = newSize;
    vPortFree(oldRaw);

    return (void *)(newRaw + sizeof(size_t));
}

size_t NeonRTOS_GetFreeHeapSize()
{
    return xPortGetFreeHeapSize();
}

void mem_set(void *pBuf,int Val,size_t Size)
{
    memset( pBuf,Val,Size);
}

void mem_copy(void *pDst, void *pSrc,size_t Size)
{
    memcpy(pDst,pSrc,Size);
}

unsigned long NeonRTOS_DisableAllInterrupts(void)
{
    portDISABLE_INTERRUPTS();
	return 0;
}

void NeonRTOS_EnableAllInterrupts()
{
    portENABLE_INTERRUPTS();
}

unsigned long NeonRTOS_EnterCritical(void)
{
    vPortEnterCritical();
    return 0;
}

void NeonRTOS_ExitCritical(unsigned long ulKey)
{
    vPortExitCritical();
}

const char* NeonRTOS_GetOsVersion()
{
    return tskKERNEL_VERSION_NUMBER;
}

void NeonRTOS_start()
{
    vTaskStartScheduler();
}

void NeonRTOS_Sleep(unsigned int MilliSecs)
{
	vTaskDelay(MilliSecs / portTICK_PERIOD_MS);
}

unsigned long NeonRTOS_TaskDisable(void)
{
   vTaskSuspendAll();

   return NeonRTOS_OK;
}

void NeonRTOS_TaskEnable(unsigned long key)
{
   xTaskResumeAll();
}

void NeonRTOS_TaskSetPriority(NeonRTOS_TaskHandle* pTaskHandle, unsigned long uxPriority)
{
    vTaskPrioritySet((TaskHandle_t)(*pTaskHandle), uxPriority);
}
