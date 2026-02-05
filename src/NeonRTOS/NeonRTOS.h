
#ifndef __NEONRT__H__
#define	__NEONRT__H__

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define NEONRT_WAIT_FOREVER   			(0xFFFFFFFF)

#define NEONRT_NO_WAIT        			(0)

#ifndef SPAWN_TASK_STACK
#define STACK_LEN          (2048) /*Stack Size*/
#else
#define STACK_LEN          (SPAWN_TASK_STACK)
#endif

#define NEONRT_LOOP_FOREVER()                      while(1);

typedef enum
{
  NeonRTOS_OK = 0,
  NeonRTOS_FAILURE = -1,
  NeonRTOS_OPERATION_FAILED = -2,
  NeonRTOS_ABORTED = -3,
  NeonRTOS_INVALID_PARAMS = -4,
  NeonRTOS_MEMORY_ALLOCATION_FAILURE = -5,
  NeonRTOS_TIMEOUT = -6,
  NeonRTOS_EVENTS_IN_USE = -7,
  NeonRTOS_EVENT_OPEARTION_FAILURE = -8
}NeonRTOS_ReturnVal_e;

typedef void* NeonRTOS_MsgQ_t;

typedef unsigned int NeonRTOS_Time_t;

typedef void * NeonRTOS_SyncObj_t;

typedef void * NeonRTOS_CountingObj_t;

typedef void * NeonRTOS_LockObj_t;

typedef short (*P_NeonRTOS_SPAWN_ENTRY)(void* pValue);

typedef void (*P_NeonRTOS_EVENT_HANDLER)(void* pValue);

typedef void (*P_NeonRTOS_TASK_ENTRY)(void* pValue);

typedef void (*P_NeonRTOS_INTR_ENTRY)(void);

typedef void* NeonRTOS_TaskHandle;

typedef void* NeonRTOS_TimerHandle;

typedef void (*NeonRTOS_TimerCallbackFunction_t)( NeonRTOS_TimerHandle osiTimer );
typedef void (*NeonRTOS_PendingFunction_t)( void * p1, unsigned int p2 );

NeonRTOS_ReturnVal_e NeonRTOS_SyncObjCreate(NeonRTOS_SyncObj_t* pSyncObj);
NeonRTOS_ReturnVal_e NeonRTOS_SyncObjDelete(NeonRTOS_SyncObj_t* pSyncObj);
NeonRTOS_ReturnVal_e NeonRTOS_SyncObjSignal(NeonRTOS_SyncObj_t* pSyncObj);
NeonRTOS_ReturnVal_e NeonRTOS_SyncObjSignalFromISR(NeonRTOS_SyncObj_t* pSyncObj);
NeonRTOS_ReturnVal_e NeonRTOS_SyncObjWait(NeonRTOS_SyncObj_t* pSyncObj , NeonRTOS_Time_t Timeout);
NeonRTOS_ReturnVal_e NeonRTOS_SyncObjClear(NeonRTOS_SyncObj_t* pSyncObj);

NeonRTOS_ReturnVal_e NeonRTOS_CountingObjCreate(NeonRTOS_CountingObj_t* pCountingObj, unsigned long uxMaxCount, unsigned long uxInitialCount);
NeonRTOS_ReturnVal_e NeonRTOS_CountingObjDelete(NeonRTOS_CountingObj_t* pCountingObj);
NeonRTOS_ReturnVal_e NeonRTOS_CountingObjIncrease(NeonRTOS_CountingObj_t* pCountingObj);
NeonRTOS_ReturnVal_e NeonRTOS_CounterObjIncreaseFromISR(NeonRTOS_CountingObj_t* pCountingObj);
NeonRTOS_ReturnVal_e NeonRTOS_CountingObjWait(NeonRTOS_CountingObj_t* pCountingObj , NeonRTOS_Time_t Timeout);
NeonRTOS_ReturnVal_e NeonRTOS_CountingObjGetCount(NeonRTOS_CountingObj_t* pCountingObj, unsigned int* pCountVal);
NeonRTOS_ReturnVal_e NeonRTOS_CountingObjClear(NeonRTOS_CountingObj_t* pCountingObj);

NeonRTOS_ReturnVal_e NeonRTOS_LockObjCreate(NeonRTOS_LockObj_t* pLockObj);
NeonRTOS_ReturnVal_e NeonRTOS_LockObjDelete(NeonRTOS_LockObj_t* pLockObj);
NeonRTOS_ReturnVal_e NeonRTOS_LockObjLock(NeonRTOS_LockObj_t* pLockObj , NeonRTOS_Time_t Timeout);
NeonRTOS_ReturnVal_e NeonRTOS_LockObjUnlock(NeonRTOS_LockObj_t* pLockObj);

NeonRTOS_ReturnVal_e NeonRTOS_MsgQCreate(NeonRTOS_MsgQ_t* 		pMsgQ , 
							  char*				pMsgQName,
							  unsigned long 		MsgSize,
							  unsigned long 		MaxMsgs);
NeonRTOS_ReturnVal_e NeonRTOS_MsgQDelete(NeonRTOS_MsgQ_t* pMsgQ);
NeonRTOS_ReturnVal_e NeonRTOS_MsgQWrite(NeonRTOS_MsgQ_t* pMsgQ, void* pMsg , NeonRTOS_Time_t Timeout);
NeonRTOS_ReturnVal_e NeonRTOS_MsgQRead(NeonRTOS_MsgQ_t* pMsgQ, void* pMsg , NeonRTOS_Time_t Timeout);

NeonRTOS_ReturnVal_e NeonRTOS_TaskCreate(P_NeonRTOS_TASK_ENTRY pEntry,const signed char * const pcName,unsigned short usStackDepth,void *pvParameters,unsigned long uxPriority,NeonRTOS_TaskHandle *pTaskHandle);
void NeonRTOS_TaskDelete(NeonRTOS_TaskHandle* pTaskHandle);
void NeonRTOS_TaskSuspend(NeonRTOS_TaskHandle* pTaskHandle);
void NeonRTOS_TaskResume(NeonRTOS_TaskHandle* pTaskHandle);
void NeonRTOS_TaskSetPriority(NeonRTOS_TaskHandle* pTaskHandle, unsigned long uxPriority);
NeonRTOS_Time_t NeonRTOS_TaskGetTickCount();

NeonRTOS_ReturnVal_e NeonRTOS_TimerCreate(NeonRTOS_TimerHandle* pTimerHandle, const signed char * const pcTimerName, NeonRTOS_Time_t periodMs, unsigned int autoLoad, unsigned int uTimerID, NeonRTOS_TimerCallbackFunction_t pCallbackFunction);
void NeonRTOS_TimerDelete(NeonRTOS_TimerHandle* pTimerHandle);
NeonRTOS_ReturnVal_e NeonRTOS_TimerChangePeriod(NeonRTOS_TimerHandle* pTimerHandle, NeonRTOS_Time_t newPeriodMs);
NeonRTOS_ReturnVal_e NeonRTOS_TimerStart(NeonRTOS_TimerHandle* pTimerHandle);
NeonRTOS_ReturnVal_e NeonRTOS_TimerStop(NeonRTOS_TimerHandle* pTimerHandle);
NeonRTOS_ReturnVal_e NeonRTOS_TimerReStart(NeonRTOS_TimerHandle* pTimerHandle);
NeonRTOS_ReturnVal_e NeonRTOS_GetTimerID(NeonRTOS_TimerHandle* pTimerHandle, unsigned int* pxTimerID);
NeonRTOS_ReturnVal_e NeonRTOS_PendingFunctionCall(NeonRTOS_PendingFunction_t pPendFunc, void* pParameter1, unsigned int uParameter2);

const char* NeonRTOS_GetOsVersion();
void NeonRTOS_start();

size_t NeonRTOS_GetFreeHeapSize();

void * mem_Malloc(unsigned long Size);
void mem_Free(void *pMem);
void *mem_Realloc(void *pMem, unsigned long newSize);
void mem_set(void *pBuf,int Val,size_t Size);
void mem_copy(void *pDst, void *pSrc,size_t Size);

unsigned long NeonRTOS_DisableAllInterrupts(void);
void NeonRTOS_EnableAllInterrupts();
unsigned long NeonRTOS_EnterCritical(void);
void NeonRTOS_ExitCritical(unsigned long ulKey);

void NeonRTOS_Sleep(unsigned int MilliSecs);
unsigned long NeonRTOS_TaskDisable(void);
void NeonRTOS_TaskEnable(unsigned long);
  
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif
