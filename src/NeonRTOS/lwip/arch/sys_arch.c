/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#if !NO_SYS

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <task.h>

#if (defined (__CC_ARM) || defined (__ARMCC_VERSION) || defined (__ICCARM__))
int errno;
#endif

/*-----------------------------------------------------------------------------------*/
/**
 * @brief 创建一个空的消息邮箱 (mailbox)。
 * @param mbox: 指向消息邮箱的指针
 * @param size: 邮箱容量
 * @return ERR_OK 表示成功，ERR_MEM 表示内存不足
 */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  *mbox = xQueueCreate(size, sizeof(void *));
  if (*mbox == NULL)
  {
#if SYS_STATS
    ++lwip_stats.sys.mbox.err;
#endif /* SYS_STATS */
    return ERR_MEM;
  }

#if SYS_STATS
  ++lwip_stats.sys.mbox.used;
  if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used)
  {
    lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
  }
#endif /* SYS_STATS */

  return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/**
 * @brief 释放消息邮箱。
 * @param mbox: 指向消息邮箱的指针
 */
void sys_mbox_free(sys_mbox_t *mbox)
{
  if (uxQueueMessagesWaiting(*mbox) > 0)
  {
    /* 如果邮箱中仍然有消息，可能是编程错误 */
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
  }

  vQueueDelete(*mbox);

#if SYS_STATS
  --lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
}

/*-----------------------------------------------------------------------------------*/
/**
 * @brief 将消息 `msg` 发布到消息邮箱 (mailbox)，如果邮箱满，则会一直阻塞。
 * @param mbox: 指向消息邮箱的指针
 * @param data: 要发布的消息
 */
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
  while (xQueueSend(*mbox, &data, portMAX_DELAY) != pdPASS);
}

/*-----------------------------------------------------------------------------------*/
/**
 * @brief 尝试将消息 `msg` 发布到消息邮箱 (mailbox)。
 *        如果邮箱已满，不会阻塞。
 * @param mbox: 指向消息邮箱的指针
 * @param msg: 要发布的消息
 * @return ERR_OK 表示成功，ERR_MEM 表示邮箱已满
 */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  if (xQueueSend(*mbox, &msg, 0) == pdPASS)
  {
    return ERR_OK;
  }
  else
  {
    // 无法发布消息，邮箱可能已满
#if SYS_STATS
    lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    return ERR_MEM;
  }
}

/*-----------------------------------------------------------------------------------*/
//   Try to post the "msg" to the mailbox.
err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
  return sys_mbox_trypost(mbox, msg);
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/

/*-----------------------------------------------------------------------------------*/
/**
 * @brief 从消息邮箱 (mailbox) 中获取一条消息。如果消息已经准备好，则立即返回消息。
 *        如果设置了超时时间，在超时后返回 SYS_ARCH_TIMEOUT。
 * @param mbox: 指向消息邮箱的指针
 * @param msg: 用于存储消息的指针
 * @param timeout: 超时时间（以毫秒为单位）；如果设置为 0，则无限等待
 * @return 消息等待时间（毫秒），或 SYS_ARCH_TIMEOUT
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  BaseType_t status;
  void *received_msg = NULL;
  uint32_t starttime = xTaskGetTickCount();
  uint32_t wait_time_ticks = timeout / portTICK_PERIOD_MS;

  if (timeout != 0)
  {
    // 尝试获取消息，超时后返回
    status = xQueueReceive(*mbox, &received_msg, wait_time_ticks);
    if (status == pdPASS)
    {
      if (msg != NULL)
      {
        *msg = received_msg;
      }
      return (xTaskGetTickCount() - starttime) * portTICK_PERIOD_MS;
    }
    else
    {
      return SYS_ARCH_TIMEOUT;
    }
  }
  else
  {
    // 无限等待获取消息
    xQueueReceive(*mbox, &received_msg, portMAX_DELAY);
    if (msg != NULL)
    {
      *msg = received_msg;
    }
    return (xTaskGetTickCount() - starttime) * portTICK_PERIOD_MS;
  }
}

/*-----------------------------------------------------------------------------------*/
/**
 * @brief 尝试从消息邮箱 (mailbox) 中获取一条消息。如果消息不在队列中立即可用，则返回 SYS_MBOX_EMPTY。
 * @param mbox: 指向消息邮箱的指针
 * @param msg: 用于存储消息的指针
 * @return ERR_OK 表示成功，SYS_MBOX_EMPTY 表示邮箱为空
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  void *received_msg = NULL;

  // 非阻塞尝试获取消息
  if (xQueueReceive(*mbox, &received_msg, 0) == pdPASS)
  {
    if (msg != NULL)
    {
      *msg = received_msg;
    }
    return ERR_OK;
  }
  else
  {
    return SYS_MBOX_EMPTY;
  }
}

/*----------------------------------------------------------------------------------*/
int sys_mbox_valid(sys_mbox_t *mbox)
{
  if (*mbox == SYS_MBOX_NULL)
    return 0;
  else
    return 1;
}
/*-----------------------------------------------------------------------------------*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  *mbox = SYS_MBOX_NULL;
}

/**
 * @brief 创建一个新信号量。
 *        `count` 参数指定信号量的初始状态。
 * @param sem: 指向信号量句柄的指针
 * @param count: 信号量初始计数值
 * @return ERR_OK: 成功
 *         ERR_MEM: 分配失败
 */
err_t sys_sem_new(sys_sem_t *sem, uint8_t count)
{
    // 创建一个二值信号量
    *sem = xSemaphoreCreateCounting(UINT16_MAX, count);

    if (*sem == NULL)
    {
#if SYS_STATS
        ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
        return ERR_MEM;
    }

    // 如果初始计数为 0，则立即获取信号量，使其无法被其他任务获取
    if (count == 0)
    {
        xSemaphoreTake(*sem, 0);
    }

#if SYS_STATS
    ++lwip_stats.sys.sem.used;
    if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
    {
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
    }
#endif /* SYS_STATS */

    return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
/**
 * @brief 等待信号量 (semaphore)。如果信号量可用，则立即返回。
 *        如果设置了超时时间且在超时前未获取信号量，则返回 `SYS_ARCH_TIMEOUT`。
 * @param sem: 指向信号量的指针
 * @param timeout: 超时时间（以毫秒为单位）；如果设置为 0，则无限等待
 * @return 等待信号量的时间（毫秒），或 `SYS_ARCH_TIMEOUT`
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    uint32_t starttime = xTaskGetTickCount();
    uint32_t wait_time_ticks = timeout / portTICK_PERIOD_MS;

    if (timeout != 0)
    {
        // 尝试在指定时间内获取信号量
        if (xSemaphoreTake(*sem, wait_time_ticks) == pdTRUE)
        {
            return (xTaskGetTickCount() - starttime) * portTICK_PERIOD_MS;
        }
        else
        {
            // 超时未获取信号量
            return SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        // 无限等待获取信号量
        while (xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE)
        {
            // 理论上此处不应该触发
        }
        return (xTaskGetTickCount() - starttime) * portTICK_PERIOD_MS;
    }
}

/*-----------------------------------------------------------------------------------*/
// 释放信号量，唤醒等待任务
void sys_sem_signal(sys_sem_t *sem)
{
    xSemaphoreGive(*sem);
}

/*-----------------------------------------------------------------------------------*/
// 释放并销毁信号量
void sys_sem_free(sys_sem_t *sem)
{
#if SYS_STATS
    // 更新统计数据
    --lwip_stats.sys.sem.used;
#endif /* SYS_STATS */

    // 删除信号量
    vSemaphoreDelete(*sem);
}
/*-----------------------------------------------------------------------------------*/
int sys_sem_valid(sys_sem_t *sem)
{
  if (*sem == SYS_SEM_NULL)
    return 0;
  else
    return 1;
}

/*-----------------------------------------------------------------------------------*/
void sys_sem_set_invalid(sys_sem_t *sem)
{
  *sem = SYS_SEM_NULL;
}

SemaphoreHandle_t lwip_sys_mutex;

/*-----------------------------------------------------------------------------------*/
// 初始化系统架构
void sys_init(void)
{
    lwip_sys_mutex = xSemaphoreCreateMutex();
    configASSERT(lwip_sys_mutex != NULL); // 确保互斥锁成功创建
}
/*-----------------------------------------------------------------------------------*/
                                      /* Mutexes*/
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
#if LWIP_COMPAT_MUTEX == 0
/* Create a new mutex*/
err_t sys_mutex_new(sys_mutex_t *mutex) {

  *mutex = xSemaphoreCreateMutex();

  if (*mutex == NULL) {
#if SYS_STATS
    ++lwip_stats.sys.mutex.err;
#endif /* SYS_STATS */
    return ERR_MEM;
  }

#if SYS_STATS
  ++lwip_stats.sys.mutex.used;
  if (lwip_stats.sys.mutex.max < lwip_stats.sys.mutex.used) {
    lwip_stats.sys.mutex.max = lwip_stats.sys.mutex.used;
  }
#endif /* SYS_STATS */
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/* Deallocate a mutex*/
void sys_mutex_free(sys_mutex_t *mutex)
{
#if SYS_STATS
      --lwip_stats.sys.mutex.used;
#endif /* SYS_STATS */

  vSemaphoreDelete(*mutex);
}
/*-----------------------------------------------------------------------------------*/
/* Lock a mutex*/
void sys_mutex_lock(sys_mutex_t *mutex)
{
    // 檢查互斥鎖是否存在
    if (*mutex == NULL) {
        // 創建新互斥鎖
        *mutex = xSemaphoreCreateMutex();
        if (*mutex == NULL) {
            // 互斥鎖創建失敗，處理錯誤
            printf("Failed to create mutex\n");
            return;
        }
    }

    // 嘗試獲取互斥鎖
    xSemaphoreTake(*mutex, portMAX_DELAY);
}

/*-----------------------------------------------------------------------------------*/
/* Unlock a mutex*/
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    // 檢查互斥鎖是否存在
    if (*mutex == NULL) {
        // 創建新互斥鎖
        *mutex = xSemaphoreCreateMutex();
        if (*mutex == NULL) {
            // 互斥鎖創建失敗，處理錯誤
            printf("Failed to create mutex\n");
            return;
        }
    }

    // 嘗試獲取互斥鎖
    xSemaphoreGive(*mutex);
}
#endif /*LWIP_COMPAT_MUTEX*/

/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    BaseType_t result;
    TaskHandle_t taskHandle;

    /* Create a new FreeRTOS task */
    result = xTaskCreate(
        thread,           /* Task function */
        name,             /* Task name */
        stacksize / sizeof(StackType_t), /* Stack size (in words) */
        arg,              /* Task argument */
        prio,             /* Task priority */
        &taskHandle       /* Task handle */
    );

    if (result == pdPASS)
    {
        return taskHandle;
    }
    else
    {
        return NULL; /* Task creation failed */
    }
}

#endif /* !NO_SYS */
