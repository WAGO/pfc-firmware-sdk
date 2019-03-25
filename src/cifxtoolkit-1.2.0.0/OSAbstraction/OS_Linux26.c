/**************************************************************************************

 Copyright (c) Hilscher GmbH. All Rights Reserved.

 Adaptations for Linux 2.6
 Copyright (c) 2008 Kontron Modular Computers

 **************************************************************************************

 Filename:
 $Workfile: OS_Linux26.c $
 Last Modification:
 $Author: mathdu-cvs $
 $Modtime: 19.10.06 9:10 $
 $Revision: 4056 $

 Targets:
 Win32/ANSI   : yes
 Win32/Unicode: yes (define _UNICODE)
 WinCE        : no

 Description:
 Win32 OS Abstraction Layer implementation. This MUST not be used in kernel mode
 drivers, but can be used in Userland, or Windows CE.

 Changes:

 Version   Date        Author   Description
 ----------------------------------------------------------------------------------
 1        07.08.2006  MT       initial version
 2		   06.12.2007  MathDu	Changed for Linux 2.6.X

 $Log: OS_Linux26.c,v $
 Revision 1.8  2008/06/09 07:10:26  mathdu-cvs
 Changed copyright in source files

 Revision 1.7  2008/05/19 08:46:18  mathdu-cvs
 Some modifications of demo program and interrupt driven mode.

 Revision 1.6  2008/05/06 14:43:20  mathdu-cvs
 Added current firmwares from Hilscher

 Revision 1.5  2008/04/30 09:47:30  mathdu-cvs
 Beta Version


 **************************************************************************************/

/* Kontron Version Identifier */

/*****************************************************************************/
/*! \file OS_Linux26.c
 *    Linux 2.6.X OS Abstraction Layer implementation.                        */
/*****************************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "OS_Dependent.h"
#include "OS_Includes.h"
#include "cifXHWFunctions.h"

#include <assert.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>

#define __need_timespec
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>

#define NSEC_PER_SEC (1000U * 1000U * 1000U)

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
 *    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Memory allocation function
 *   \param ulSize    Length of memory to allocate
 *   \return Pointer to allocated memory                                      */
/*****************************************************************************/
void* OS_Memalloc(uint32_t ulSize)
{
	return malloc(ulSize);
}

/*****************************************************************************/
/*! Memory freeing function
 *   \param pvMem Memory block to free                                        */
/*****************************************************************************/
void OS_Memfree(void* pvMem)
{
	free(pvMem);
}

/*****************************************************************************/
/*! Memory reallocating function (used for resizing dynamic toolkit arrays)
 *   \param pvMem     Memory block to resize
 *   \param ulNewSize new size of the memory block
 *   \return pointer to the resized memory block                              */
/*****************************************************************************/
void* OS_Memrealloc(void* pvMem,
						  uint32_t ulNewSize)
{
	return  (void*)realloc(pvMem, ulNewSize);
}

/*****************************************************************************/
/*! Memory setting
 *   \param pvMem     Memory block
 *   \param bFill     Byte to use for memory initialization
 *   \param ulSize    Memory size for initialization)                         */
/*****************************************************************************/
void OS_Memset(void* pvMem,
					uint8_t bFill,
					size_t ulSize)
{
	memset(pvMem, bFill, ulSize);
}

/*****************************************************************************/
/*! Copy memory from one block to another
 *   \param pvDest    Destination memory block
 *   \param pvSrc     Source memory block
 *   \param ulSize    Copy size in bytes                                      */
/*****************************************************************************/
void OS_Memcpy(void* pvDest,
					const void* pvSrc,
					size_t ulSize)
{
	memcpy(pvDest, pvSrc, ulSize);
}

/*****************************************************************************/
/*! Compare two memory blocks
 *   \param pvBuf1    First memory block
 *   \param pvBuf2    Second memory block
 *   \param ulSize    Compare size in bytes
 *   \return 0 if both buffers are equal                                      */
/*****************************************************************************/
int32_t OS_Memcmp(const void* pvBuf1,
						const void* pvBuf2,
						size_t ulSize)
{
	return memcmp(pvBuf1, pvBuf2, ulSize);
}

/*****************************************************************************/
/*! Move memory
 *   \param pvDest    Destination memory
 *   \param pvSrc     Source memory
 *   \param ulSize    Size in byte to move                                    */
/*****************************************************************************/
void OS_Memmove(void* pvDest,
					 const void* pvSrc,
					 size_t ulSize)
{
	memmove(pvDest, pvSrc, ulSize);
}

/*****************************************************************************/
/*! Sleep for a specific time
 *   \param ulSleepTimeMs  Time in ms to sleep for                            */
/*****************************************************************************/
void OS_Sleep(uint32_t ulSleepTimeMs)
{
	usleep(ulSleepTimeMs * 1000);
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
 *   \return Current counter value (resolution of this value will influence
 *           timeout monitoring in driver/toolkit functions)                  */
/*****************************************************************************/
uint32_t OS_GetMilliSecCounter(void)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	return (uint32_t)( SEC_TO_MSEC(now.tv_sec) + NSEC_TO_MSEC(now.tv_nsec));
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
 *   \return Current counter value (resolution of this value will influence
 *           timeout monitoring in driver/toolkit functions)                  */
/*****************************************************************************/
uint64_t OS_GetMicroSecCounter(void)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	return (uint64_t)( SEC_TO_USEC(now.tv_sec) + NSEC_TO_USEC(now.tv_nsec));
}

/*****************************************************************************/
/*! Create an auto reset event
 *   \return handle to the created event                                      */
/*****************************************************************************/
void* OS_CreateEvent(void)
{
	/*return (void*)CreateEvent(NULL, FALSE, FALSE, NULL);*/
	sem_t *pSem;
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_CreateEvent");

	pSem = calloc(1, sizeof(sem_t));
	if (pSem)
	{
		if (sem_init(pSem, 0, 0) == -1)
		{
			free(pSem);
			pSem = 0;
		}
		else
		{
			// done
		}
	}
	return pSem;
}

/*****************************************************************************/
/*! Set an event
 *   \param pvEvent Handle to event being signalled                           */
/*****************************************************************************/
void OS_SetEvent(void* pvEvent)
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%s (%p)", __func__, pvEvent);

	/*SetEvent((HANDLE)pvEvent);*/
	int32_t value = 0;
	sem_getvalue(pvEvent, &value);
	if (value > 0)
	{
		USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_SetEvent %p already set value=%d", pvEvent, value);
	}
	else
		sem_post(pvEvent);
}

/*****************************************************************************/
/*! Reset an event
 *   \param pvEvent Handle to event being reset                               */
/*****************************************************************************/
void OS_ResetEvent(void* pvEvent)
{
	/*ResetEvent((HANDLE)pvEvent);*/
	UNREFERENCED_PARAMETER( pvEvent);
	/* this is only needed for manual-reset-events which are not available in linux */
	/* -> do nothing */
}

/*****************************************************************************/
/*! Delete an event
 *   \param pvEvent Handle to event being deleted                             */
/*****************************************************************************/
void OS_DeleteEvent(void* pvEvent)
{
	/*CloseHandle((HANDLE)pvEvent);*/
	sem_destroy(pvEvent);
}

/*****************************************************************************/
/*! Wait for the signalling of an event
 *   \param pvEvent   Handle to event being wait for
 *   \param ulTimeout Timeout in ms to wait for event
 *   \return 0 if event was signalled                                         */
/*****************************************************************************/
uint32_t OS_WaitEvent(void* pvEvent,
							 uint32_t ulTimeout)
{
	struct timespec ts;
	int32_t s;

	USER_Trace(NULL, TRACE_LEVEL_DEBUG,"%s (%s=%p, %s=%d)", __func__ , NV(pvEvent), NV(ulTimeout));

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		return CIFX_EVENT_TIMEOUT;
	}

	ts.tv_nsec += ((ulTimeout % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_nsec -= 1000000000;
		ts.tv_sec += 1;
	}
	ts.tv_sec += ulTimeout / 1000;

	while ((s = sem_timedwait(pvEvent, &ts)) == -1 && errno == EINTR)
		continue; /* Restart if interrupted by handler */

	if ( s != 0)
	{

	}

	/* Check what happened */
	if (s == -1)
	{
		if (errno == ETIMEDOUT)
			return CIFX_EVENT_TIMEOUT; /* Timeout */
		else
		   USER_Trace(NULL, TRACE_LEVEL_ERROR, "sem_timedwait = %s", strerror(errno));
	}
	else
	{
		return CIFX_EVENT_SIGNALLED; /* Success */
	}
	return 3; //CIFX_EVENT_TIMEOUT;	/* Error, but there is no other return value */
}

/*****************************************************************************/
/*! Compare two ASCII string
 *   \param pszBuf1   First buffer
 *   \param pszBuf2   Second buffer
 *   \return 0 if strings are equal                                           */
/*****************************************************************************/
int32_t OS_Strcmp(const char* pszBuf1,
						const char* pszBuf2)
{
	return strcmp(pszBuf1, pszBuf2);
}

/*****************************************************************************/
/*! Compare characters of two strings without regard to case
 *   \param pszBuf1   First buffer
 *   \param pszBuf2   Second buffer
 *   \param ulLen     Number of characters to compare
 *   \return 0 if strings are equal                                           */
/*****************************************************************************/
int32_t OS_Strnicmp(const char* pszBuf1,
						  const char* pszBuf2,
						  uint32_t ulLen)
{
	return strncasecmp(pszBuf1, pszBuf2, ulLen);
}

/*****************************************************************************/
/*! Query the length of an ASCII string
 *   \param szText    ASCII string
 *   \return character count of szText                                        */
/*****************************************************************************/
int32_t OS_Strlen(const char* szText)
{
	return (int32_t) strlen(szText);
}

/*****************************************************************************/
/*! Copies one string to another monitoring the maximum length of the target
 *   buffer.
 *   \param szDest    Destination buffer
 *   \param szSource  Source buffer
 *   \param ulLength  Maximum length to copy
 *   \return pointer to szDest                                                */
/*****************************************************************************/
char* OS_Strncpy(char* szDest,
					  const char* szSource,
					  uint32_t ulLength)
{
	char *r;
	r = strncpy(szDest, szSource, ulLength);
/*     USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_Strncpy(szDest=%s, szSource=%s, ulLength=%u)",*/
/*         szDest, szSource, ulLength);*/
	return r;
}

/*****************************************************************************/
/*! Create an interrupt safe locking mechanism (Spinlock/critical section)
 *   \return handle to the locking object                                     */
/*****************************************************************************/
void* OS_CreateLock(void)
{
   /* we use priority inheritance by default,
    * just to make high prio task don't have to wait
    * long than needed. */
   pthread_mutexattr_t attr;
	pthread_mutex_t *pLock = malloc(sizeof(pthread_mutex_t));
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%s ()", __func__);

	if (pLock == 0)
		return 0;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

#ifndef NDEBUG
//	pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST_NP);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#endif

	pthread_mutex_init(pLock, &attr);
	pthread_mutexattr_destroy(&attr);

	return pLock;
}

/*****************************************************************************/
/*! Enter a critical section/spinlock
 *   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_EnterLock(void* pLock)
{
   int32_t error;
   if ((error =  pthread_mutex_lock(pLock)))
      USER_Trace( NULL, TRACE_LEVEL_ERROR,"mutex lock failed with %s", strerror(error));
}

/*****************************************************************************/
/*! Leave a critical section/spinlock
 *   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_LeaveLock(void* pLock)
{
	int32_t error;

	if (( error = pthread_mutex_unlock(pLock)))
	   USER_Trace( NULL, TRACE_LEVEL_ERROR,"mutex unlock failed with %s", strerror(error));

}

/*****************************************************************************/
/*! Delete a critical section/spinlock object
 *   \param pvLock Handle to the locking object being deleted                 */
/*****************************************************************************/
void OS_DeleteLock(void* pLock)
{
	assert(pLock != NULL);
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_DeleteLock");

	pthread_mutex_destroy(pLock);
	free(pLock);
}

/*****************************************************************************/
/*! Create an Mutex object for locking code sections
 *   \return handle to the mutex object                                       */
/*****************************************************************************/
void* OS_CreateMutex(void)
{

   pthread_mutexattr_t attr;
   pthread_mutex_t *pMutex = malloc(sizeof(pthread_mutex_t));

   USER_Trace( NULL, TRACE_LEVEL_DEBUG,  "%s", "OS_CreateLock");

   if (pMutex == 0)
      return 0;

   pthread_mutexattr_init(&attr);
   /* we use prio inheritance by default */
   pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

#ifndef NDEBUG
//   pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST_NP);
   pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#endif

   pthread_mutex_init(pMutex, &attr);
   pthread_mutexattr_destroy(&attr);

   return pMutex;
}

/*****************************************************************************/
/*! Wait for mutex
 *   \param pvMutex    Handle to the Mutex locking object
 *   \param ulTimeout  Wait timeout in milliseconds
 *   \return !=0 on succes                                                    */
/*****************************************************************************/
int32_t OS_WaitMutex(void* pMutex,
							uint32_t ulTimeout)
{
   assert(pMutex != NULL);
	/*return (WaitForSingleObject((HANDLE)pvMutex, ulTimeout) == WAIT_OBJECT_0) ? 1 : 0;*/
	struct timespec abs_timeout;

	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_WaitMutex(timeout=%dms)", ulTimeout);

	clock_gettime(CLOCK_REALTIME, &abs_timeout);

	abs_timeout.tv_sec += ulTimeout / 1000;	// convert to seconds
	abs_timeout.tv_nsec += (ulTimeout % 1000) * 1000000;	// convert to nanoseconds

	if ( abs_timeout.tv_nsec >= NSEC_PER_SEC)
	{
	   abs_timeout.tv_nsec -= NSEC_PER_SEC;
	   abs_timeout.tv_sec++;
	}


	int32_t error;
	if ((error = pthread_mutex_timedlock(pMutex, &abs_timeout)))
	{
	   if ( error != ETIMEDOUT)
	      USER_Trace( NULL, TRACE_LEVEL_ERROR, "pthread_mutex_timedlock failed with %s", strerror(error));
		return 0;
	}

	return 1;
}

/*****************************************************************************/
/*! Release a mutex section section
 *   \param pvMutex Handle to the locking object                              */
/*****************************************************************************/
void OS_ReleaseMutex(void* pMutex)
{
   assert(pMutex != NULL);

	USER_Trace( NULL, TRACE_LEVEL_DEBUG,  "(%s=%p)", NV(pMutex));
	int32_t error;
	error = pthread_mutex_unlock(pMutex);
	if ( error )
	   USER_Trace( NULL, TRACE_LEVEL_ERROR,"pthread_mutex_unlock failed with %s", strerror(error));
}

/*****************************************************************************/
/*! Delete a Mutex object
 *   \param pvMutex Handle to the mutex object being deleted                  */
/*****************************************************************************/
void OS_DeleteMutex(void* pMutex)
{
   assert(pMutex != NULL);

	USER_Trace( NULL, TRACE_LEVEL_DEBUG,  "(%s=%p)", NV(pMutex));
	pthread_mutex_destroy(pMutex);
	free(pMutex);
}

/*****************************************************************************/
/*! Opens a file in binary mode
 *   \param szFile     Full file name of the file to open
 *   \param pulFileLen Returned length of the opened file
 *   \return handle to the file, NULL mean file could not be opened           */
/*****************************************************************************/
void* OS_FileOpen(const char* szFile,
			uint32_t* pulFileLen)
{
   assert(szFile != NULL);
   assert(pulFileLen != NULL);

	char pwd[200];
	getcwd(pwd, sizeof(pwd));
	int32_t iFile;
	void* pvRet = NULL;

	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_FileOpen(%s)", szFile);

	iFile = open(szFile, O_RDONLY);
	if (iFile != -1)
	{
		struct stat tBuffer;
		ZERO_MEMORY(&tBuffer);

		if (fstat(iFile, &tBuffer) != -1)
		{
			*pulFileLen = (uint32_t) tBuffer.st_size;
			pvRet = (void*) iFile;
		}
	}

	return pvRet;
}

/*****************************************************************************/
/*! Closes a previously opened file
 *   \param pvFile Handle to the file being closed                            */
/*****************************************************************************/
void OS_FileClose(void* pvFile)
{
   assert(pvFile);

	int32_t iFile = (int32_t) pvFile;
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_FileClose");

	close(iFile);
}

/*****************************************************************************/
/*! Read a specific amount of bytes from the file
 *   \param pvFile   Handle to the file being read from
 *   \param ulOffset Offset inside the file, where read starts at
 *   \param ulSize   Size in bytes to be read
 *   \param pvBuffer Buffer to place read bytes in
 *   \return number of bytes actually read from file                          */
/*****************************************************************************/
uint32_t OS_FileRead(void* pvFile,
							uint32_t ulOffset,
							uint32_t ulSize,
							void* pvBuffer)
{
   assert(pvFile != NULL);
   assert(pvBuffer != NULL);

	int32_t iFile = (int32_t) pvFile;
	uint32_t ulRet = 0;

	USER_Trace( NULL, TRACE_LEVEL_DEBUG, "%s ()", __func__);

	if (ulOffset == (uint32_t) lseek(iFile, (int32_t) ulOffset, SEEK_SET))
	{
		ulRet = (uint32_t) read(iFile, pvBuffer, ulSize);
	}

	return ulRet;
}

/*****************************************************************************/
/*! O/S specific initialization (if needed), called during cifXTKitInit()    */
/*****************************************************************************/
int32_t OS_Init()
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_Init");

	return 0;
}

/*****************************************************************************/
/*! O/S specific de-initialization (if needed), called during cifXTKitInit() */
/*****************************************************************************/
void OS_Deinit()
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_Deinit");

}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
 * write back all BAR's (Base address registers), Interrupt and Command
 * Register. These registers are invalidated during cifX Reset and need to be
 * re-written after the reset has succeeded
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice
 *   \param pvPCIConfig   Configuration returned by OS_ReadPCIConfig
 *                        (implementation dependent)                          */
/*****************************************************************************/
void OS_WritePCIConfig(void* pvOSDependent,
							  void* pvPCIConfig)
{
//	USER_Trace( NULL, TRACE_LEVEL_DEBUG,  "%s (%s=%p, %s=%p)", __func__, NV(pvOSDependent), NV(pvPCIConfig));
//
//	uint32_t *v;
//	struct pci_access *pacc;
//	struct pci_dev *dev;
//
//	v = pvPCIConfig;
//
//	pacc = pci_alloc();
//	if (pacc == 0)
//	{
//		USER_Trace( NULL, TRACE_LEVEL_ERROR, "%s(): error at pci_alloc");
//	}
//	else
//	{
//		pci_init(pacc); /* Initialize the PCI library */
//		pci_scan_bus(pacc); /* We want to get the list of devices */
//		for (dev = pacc->devices; dev; dev = dev->next) /* Iterate over all devices */
//		{
//			pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES); /* Fill in header info we need */
//			pci_read_word(dev, PCI_CLASS_DEVICE); /* Read config register directly */
//			USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%02x:%02x.%d vendor=%04x device=%04x irq=%d base0=%lx",
//						  dev->bus,
//						  dev->dev, dev->func, dev->vendor_id, dev->device_id,
//						  dev->irq,
//						  dev->base_addr[0]);
//			if (dev->base_addr[0] == USER_GetPhysicalAddress(pvOSDependent))
//			{
//				USER_Trace(NULL, TRACE_LEVEL_DEBUG, "found\n");
//				pci_write_word(dev, PCI_COMMAND,(uint16_t) v[0]);
//				pci_write_long(dev, PCI_BASE_ADDRESS_0, v[1]);
//				pci_write_long(dev, PCI_BASE_ADDRESS_1, v[2]);
//				pci_write_long(dev, PCI_BASE_ADDRESS_2, v[3]);
//				pci_write_byte(dev, PCI_INTERRUPT_LINE, (uint8_t)v[4]);
//				break;
//			}
//		}
//		pci_cleanup(pacc); /* Close everything */
//	}
//	free(v);


	return;
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
 * read all BAR's (Base address registers), Interrupt and Command Register.
 * These registers are invalidated during cifX Reset and need to be
 * re-written after the reset has succeeded
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice
 *   \return pointer to stored register copies (implementation dependent)     */
/*****************************************************************************/
void* OS_ReadPCIConfig(void* pvOSDependent)
{
  // uint32_t *v;
//	struct pci_access *pacc;
//	struct pci_dev *dev;
//
//	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig(%x)", pvOSDependent);
//
//	v = malloc(5 * sizeof(uint32_t));
//	memset(v, 0, 5 * sizeof(uint32_t));
//
//	pacc = pci_alloc();
//	if (pacc == 0)
//	{
//		USER_Trace(NULL, TRACE_LEVEL_ERROR, "error at pci_alloc()\n");
//	}
//	else
//	{
//		pci_init(pacc); /* Initialize the PCI library */
//		pci_scan_bus(pacc); /* We want to get the list of devices */
//		for (dev = pacc->devices; dev; dev = dev->next) /* Iterate over all devices */
//		{
//			pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES); /* Fill in header info we need */
//			pci_read_word(dev, PCI_CLASS_DEVICE); /* Read config register directly */
//			USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%02x:%02x.%d vendor=%04x device=%04x irq=%d base0=%lx",
//						  dev->bus,
//						  dev->dev, dev->func, dev->vendor_id, dev->device_id,
//						  dev->irq,
//						  dev->base_addr[0]);
//
//			if (dev->base_addr[0] == USER_GetPhysicalAddress(pvOSDependent))
//			{
//				USER_Trace(NULL, TRACE_LEVEL_DEBUG, "found\n");
//				v[0] = pci_read_word(dev, PCI_COMMAND);
//				v[1] = pci_read_long(dev, PCI_BASE_ADDRESS_0);
//				v[2] = pci_read_long(dev, PCI_BASE_ADDRESS_1);
//				v[3] = pci_read_long(dev, PCI_BASE_ADDRESS_2);
//				v[4] = pci_read_byte(dev, PCI_INTERRUPT_LINE);
//				break;
//			}
//		}
//		pci_cleanup(pacc); /* Close everything */
//		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: command %08x", v[0]);
//		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: bar0 %08x", v[1]);
//		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: bar1 %08x", v[2]);
//		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: bar2 %08x", v[3]);
//		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: irql %08x", v[4]);
//	}
	//return v;
   return NULL;
}

/*****************************************************************************/
/*! This function Maps a DPM pointer to a user application if needed.
 *   This example just returns the pointer valid inside the driver.
 *   In Win32 Kernel Mode this must be replaced by calls to
 *   \param pvDriverMem  Pointer to be mapped
 *   \param ulMemSize    Size to be mapped
 *   \param ppvMappedMem Returned mapped pointer (usable by application)
 *   \return Handle that is needed for unmapping (can not be NULL)                */
/*****************************************************************************/
void* OS_MapUserPointer(void* pvDriverMem,
								uint32_t ulMemSize,
								void** ppvMappedMem,
								void* pvOSDependent)
{
	/* TODO: Check if we can Map the pointer here instead inside the netxToolkitInit Function*/
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_MapUserPointer");
	/* in linux the DPM is already mapped to user space, so just return this pointer */
	UNREFERENCED_PARAMETER(ulMemSize);
	UNREFERENCED_PARAMETER(pvOSDependent);
	*ppvMappedMem = pvDriverMem;

	return pvDriverMem;
}

/*****************************************************************************/
/*! This function unmaps a previously mapped user application pointer
 *   In Win32 Kernel Mode this function must call
 *   \param phMapping  Handle that was returned by OS_MapUserPointer
 *   \return !=0 on success                                                   */
/*****************************************************************************/
int32_t OS_UnmapUserPointer(void* phMapping,
									 void* pvOSDependent)
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_UnmapUserPointer");
	/* nothing to do for linux */
	UNREFERENCED_PARAMETER(phMapping);
	UNREFERENCED_PARAMETER(pvOSDependent);
	return 1;
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_EnableInterrupts(void* pvOSDependent)
{
	USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_EnableInterrupts");
	UNREFERENCED_PARAMETER(pvOSDependent);
	/*printf("We do not support IRQs in Win32 User mode!!");*/
	/*UNREFERENCED_PARAMETER(pvOSDependent);*/
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_DisableInterrupts(void* pvOSDependent)
{
	USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_DisableInterrupts");
	UNREFERENCED_PARAMETER(pvOSDependent);
	/*printf("We do not support IRQs in Win32 User mode!!");*/
	/*UNREFERENCED_PARAMETER(pvOSDependent);*/
}

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
